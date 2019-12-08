/**
 * @file user_manage.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-10-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#define LOG_TAG "USER_MANAGE"

#include <memory>
#include <utility>
#include <SQLiteCpp/Backup.h>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <SQLiteCpp/Exception.h>
#include <SQLiteCpp/sqlite3.h> // for SQLITE_ERROR, SQLITE_RANGE and SQLITE_DONE
#include <easylogger/easylogger_setup.h>
#include <sole/sole.hpp>
#include "../../base_fun/task_manage/task_manage.h"
#include "../../sys_ctrl/config/sys_capability.h"
#include "../../tools/timer/timer.h"
#include "../session/session_manage.h"
#include "user_manage.h"

using namespace SQLite;
using namespace wotsen;
using namespace insider::sdk;

#define USER_TABLE "sdk_user"

static Database *user_db = nullptr;

class UserSession : public Session {
public:
	std::string token;
	std::string name;
	time_t alive_time;
	time_t now;

	UserSession() {}
};

class UserManager : public SessionManager<UserSession> {
private:
	std::vector<std::shared_ptr<UserSession>> sessions;
	pthread_mutex_t mutex;
public:
	UserManager(const uint32_t max_session) : SessionManager<UserSession>(max_session) {
		pthread_mutex_init(&mutex, NULL);
	}

	~UserManager() {
		pthread_mutex_destroy(&mutex);
	}

	void update_session(void)
	{
		pthread_mutex_lock(&mutex);

		for (auto &item : sessions)
		{
			if (0 == item->now)
			{
				continue;
			}

			item->now--;
		}

		sessions.erase(std::remove_if(sessions.begin(), sessions.end(), [](auto &_session) -> bool {
							   return 0 == _session->now;
                        }),
                        sessions.end());
		
		pthread_mutex_unlock(&mutex);
	}

	bool search_user_session(const std::string &token, UserSession **session) const
	{
		for (auto &item : sessions)
		{
			if (token == item->token)
			{
				*session = &(*item);
				return true;
			}
		}
		return false;
	}

	bool add_user_session(UserSession &session)
	{
		UserSession *_session;
		std::shared_ptr<UserSession> new_session(new UserSession);

		pthread_mutex_lock(&mutex);
		if (sessions.size() >= max_session || search_user_session(session.token, &_session))
		{
			pthread_mutex_unlock(&mutex);
			return false;
		}

		new_session->token = session.token;
		new_session->name = session.name;
		new_session->alive_time = session.alive_time;
		new_session->now = new_session->alive_time;

		sessions.push_back(new_session);
		pthread_mutex_unlock(&mutex);

		return true;
	}

	void delete_user_session(const std::string &token)
	{
		pthread_mutex_lock(&mutex);
		sessions.erase(std::remove_if(sessions.begin(), sessions.end(), [&token](auto &_session) -> bool {
                               return _session->token == token;
                        }),
                        sessions.end());
		pthread_mutex_unlock(&mutex);
	}

	enum UserInfo_Result login(UserInfo &user_info)
	{
		Statement query(*user_db, "SELECT * FROM " USER_TABLE " where name = ? and password = ?");

		query.bind(1, user_info.user().user_name());
		query.bind(2, user_info.user().user_pass());

		query.executeStep();

		if (!query.hasRow())
		{
			log_i("user : %s not exsit\n", user_info.user().user_name().c_str());
			return UserInfo_Result::UserInfo_Result_U_ERROR;
		}

		UserSession session;
		sole::uuid uid = sole::uuid4();

		session.now = user_info.alive_time();
		session.alive_time = user_info.alive_time();
		session.token = uid.str();
		user_info.set_token(uid.str());

		if (add_user_session(session))
		{
			return UserInfo_Result::UserInfo_Result_U_OK;
		}

		return UserInfo_Result::UserInfo_Result_U_ERROR;
	}

	enum UserInfo_Result logout(UserInfo &user_info)
	{
		delete_user_session(user_info.token());
		return UserInfo_Result::UserInfo_Result_U_OK;
	}

	enum UserInfo_Result verify(UserInfo &user_info)
	{
		UserSession *_session;

		pthread_mutex_lock(&mutex);
		if (search_user_session(user_info.token(), &_session))
		{
			pthread_mutex_unlock(&mutex);
			return UserInfo_Result::UserInfo_Result_U_OK;
		}

		pthread_mutex_unlock(&mutex);
		return UserInfo_Result::UserInfo_Result_U_TOKEN_TIMEOUT;
	}

	enum UserInfo_Result user_register(UserInfo &user_info)
	{
		Statement query(*user_db, "SELECT * FROM " USER_TABLE " where name = ?");

		query.bind(1, user_info.user().user_name());
		query.executeStep();
		if (query.hasRow())
		{
			return UserInfo_Result::UserInfo_Result_U_USER_EXIST;
		}

		Statement insert(*user_db, "INSERT INTO " USER_TABLE " VALUES (NULL, ?, ?, ?, ?)");
		insert.bind(1, user_info.user().user_name());
		insert.bind(2, user_info.user().user_pass());
		insert.bind(3, user_info.user().phone());
		insert.bind(4, user_info.user().email());
		insert.exec();

		if (SQLITE_DONE != user_db->getErrorCode())
		{
			return UserInfo_Result::UserInfo_Result_U_ERROR;
		}

		UserSession session;
		sole::uuid uid = sole::uuid4();

		session.now = user_info.alive_time();
		session.alive_time = user_info.alive_time();
		session.token = uid.str();
		user_info.set_token(uid.str());

		if (add_user_session(session))
		{
			return UserInfo_Result::UserInfo_Result_U_OK;
		}

		return UserInfo_Result::UserInfo_Result_U_ERROR;
	}
};

static std::unique_ptr<UserManager> user_manager;

template <typename T>
bool _user_manange_midware_do(const enum sdk_net_data_type type, sdk_package<T> *req, Sdk &sdk_req, Sdk &sdk_res);

template<typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts&&... params)
{
	return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

template <>
bool _user_manange_midware_do(const enum sdk_net_data_type type, sdk_package<uv_stream_s> *req, Sdk &sdk_req, Sdk &sdk_res)
{
	UserInfo user_info = sdk_req.mutable_body()->user();
	enum UserInfo_Result user_ret = UserInfo_Result::UserInfo_Result_U_OK;

	log_i("%s %s\n", user_info.user().user_name().c_str(), user_info.user().user_pass().c_str());

	switch (user_info.user_type())
	{
		case UserInfo_UserType::UserInfo_UserType_U_LOGIN:
			user_ret = user_manager->login(user_info);
			break;
		case UserInfo_UserType::UserInfo_UserType_U_LOGOUT:
			user_ret = user_manager->logout(user_info);
			break;
		case UserInfo_UserType::UserInfo_UserType_U_VERI:
			user_ret = user_manager->verify(user_info);
			break;
		case UserInfo_UserType::UserInfo_UserType_U_REGISTER:
			user_ret = user_manager->user_register(user_info);
			break;
		default:
			user_ret = UserInfo_Result::UserInfo_Result_U_ERROR;
			break;
	}

	sdk_res.mutable_body()->mutable_user()->set_result(user_ret);

	switch (user_ret)
	{
		case UserInfo_Result::UserInfo_Result_U_OK:
			sdk_res.mutable_body()->mutable_user()->set_token(user_info.token());
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_status_code(ResponseResult::OK);
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_code("ok");
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_status_code(ContentResultE::R_CODE_OK);
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_code("ok");
			return true;
		
		case UserInfo_Result::UserInfo_Result_U_USER_EXIST:
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_status_code(ResponseResult::ERROR);
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_code("error");
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_status_code(ContentResultE::R_CODE_USER_EXIST);
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_code("user exist");
			return false;

		case UserInfo_Result::UserInfo_Result_U_TOKEN_TIMEOUT:
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_status_code(ResponseResult::ERROR);
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_code("error");
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_status_code(ContentResultE::R_CODE_USER_TOKEN_TIMEOUT);
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_code("user token timeout");
			return false;

		default:
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_status_code(ResponseResult::ERROR);
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_code("error");
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_status_code(ContentResultE::R_CODE_ERROR);
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_code("unknown error");
			return false;
	}
}

bool user_manange_midware_do(const enum sdk_net_data_type type, void *req, Sdk &sdk_req, Sdk &sdk_res)
{
	if (SDK_TCP_DATA_TYPE == type)
	{
		return _user_manange_midware_do(type, (sdk_package<uv_stream_s> *)req, sdk_req, sdk_res);
	}
	else if (SDK_UDP_DATA_TYPE == type)
	{
		// sdk_package<uv_stream_t> *src_req = req; 
		return false; // TODO:UDP
	}
	else
	{
		return false;
	}
}

bool user_db_init(void)
{
	try {
		user_db = new Database("../data/ai_platform_service_user.db3", OPEN_READWRITE|OPEN_CREATE);
	} catch (std::exception& e) {
		log_e("%s\n", e.what());

		return false;
	}

	if (!user_db->tableExists(USER_TABLE))
	{
		user_db->exec("CREATE TABLE sdk_user (id INTEGER PRIMARY KEY, name CHAR(30), password CHAR(128), phone CHAR(24), email CHAR(128))");
		user_db->exec("INSERT INTO sdk_user VALUES (1, \"admin\", \"admin\", \"15558198512\", \"wotsen@outlook.com\")");
	}

	return true;
}

static void *user_manage_task(void *name)
{
	pthread_t tid = pthread_self();

	for (;;)
    {
        task_alive(tid);          // 自身任务心跳

        ostime_delay(OS_SEC(1)); // 1秒刷新一次

		user_manager->update_session();
    }
}

void user_manager_init(void)
{
	user_db_init();

	user_manager = std::unique_ptr<UserManager>(new UserManager(get_user_manage_max_users()));

	task_create(user_manage_task, STACKSIZE(8000), "user_manage", TASK_NORMAL_ID, OS_MIN(5), E_TASK_RELOAD);
}
