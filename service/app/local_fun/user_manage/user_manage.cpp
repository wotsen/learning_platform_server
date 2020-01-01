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
#include "util_time/util_time.h"
#include "task_manage/task_manage.h"
#include "sys_capability.h"
#include "os_param.h"
#include "../session/session_manage.h"
#include "user_manage.h"

using namespace SQLite;
using namespace wotsen;
using namespace insider::sdk;

#define USER_TABLE SDK_USER_TABLE

class UserSession;
class UserManager;

// 模块状态
static bool _module_state = false;

// 用户数据库
static Database *user_db = nullptr;

// 用户管理
static std::unique_ptr<UserManager> user_manager;

// 用户中间件处理接口
bool _user_manange_midware_do(Sdk &sdk_req, Sdk &sdk_res);

/**
 * @brief 用户会话
 * 
 */
class UserSession : public Session {
public:
	std::string token_;
	std::string name_;
	time_t alive_time_;
	time_t now_;

	UserSession() {}
};

/**
 * @brief 用户管理
 * 
 */
class UserManager : public SessionManager<UserSession> {
private:
	// std::vector<std::shared_ptr<UserSession>> sessions;
	pthread_mutex_t mutex_;
public:
	UserManager(const uint32_t max_session) : SessionManager<UserSession>(max_session) {
		pthread_mutex_init(&mutex_, NULL);
	}

	~UserManager() {
		pthread_mutex_destroy(&mutex_);
	}

	// TODO:修改密码
	bool change_password(UserSessionMsg &user_info)
	{
		return true;
	}

	// TODO:查询用户信息
	std::vector<UserAllInfo> query_user_list(void)
	{
		std::vector<UserAllInfo> user_list;
		// FIXME:无法一次查完?
		return user_list;
	}

	// TODO:用户权限校验中间件
	bool user_permisson_verify(const std::string &username, const UserPermissson &permission)
	{
		return true;
	}

	/**
	 * @brief 更新会话
	 * 
	 */
	void update_session(void)
	{
		pthread_mutex_lock(&mutex_);

		for (auto &item : sessions_)
		{
			if (0 == item->now_)
			{
				continue;
			}

			// 刷新登陆时间
			item->now_--;
		}

		// 移除过期用户
		sessions_.erase(std::remove_if(sessions_.begin(), sessions_.end(), [](auto &_session) -> bool {
							   return 0 == _session->now_;
                        }),
                        sessions_.end());
		
		pthread_mutex_unlock(&mutex_);
	}

	/**
	 * @brief 搜索用户
	 * 
	 * @param token 
	 * @param session 输出，搜索到的用户
	 * @return true 搜索到
	 * @return false 未搜索到
	 */
	bool search_user_session(const std::string &token, UserSession **session) const
	{
		for (auto &item : sessions_)
		{
			if (token == item->token_)
			{
				*session = &(*item);
				return true;
			}
		}
		return false;
	}

	/**
	 * @brief 增加用户会话
	 * 
	 * @param session 会话信息
	 * @return true 
	 * @return false 
	 */
	bool add_session(const UserSession &session)
	{
		UserSession *_session;
		std::shared_ptr<UserSession> new_session(new UserSession);

		pthread_mutex_lock(&mutex_);
		if (sessions_.size() >= max_session_ || search_user_session(session.token_, &_session))
		{
			pthread_mutex_unlock(&mutex_);
			return false;
		}

		new_session->token_ = session.token_;
		new_session->name_ = session.name_;
		new_session->alive_time_ = session.alive_time_;
		new_session->now_ = new_session->alive_time_;

		sessions_.push_back(new_session);
		pthread_mutex_unlock(&mutex_);

		return true;
	}

	/**
	 * @brief 删除用户会话信息
	 * 
	 * @param token 会话token
	 */
	void delete_session(const std::string &token)
	{
		pthread_mutex_lock(&mutex_);
		sessions_.erase(std::remove_if(sessions_.begin(), sessions_.end(), [&token](auto &_session) -> bool {
                               return _session->token_ == token;
                        }),
                        sessions_.end());
		pthread_mutex_unlock(&mutex_);
	}

	/**
	 * @brief 用户登陆
	 * 
	 * @param user_info 用户信息,FIXME:数据库操作未作异常处理
	 * @return enum UserInfo_Result 
	 */
	enum ContentResultE login(UserSessionMsg &user_info)
	{
		Statement query_user(*user_db, "SELECT * FROM " USER_TABLE " where name = ?");

		query_user.bind(1, user_info.user().user_name());

		query_user.executeStep();

		// 用户不存在
		if (!query_user.hasRow())
		{
			log_i("user : %s not exsit\n", user_info.user().user_name().c_str());
			return ContentResultE::R_CODE_USER_NOT_EXIST;
		}

		// 密码错误
		Statement query_user_pass(*user_db, "SELECT * FROM " USER_TABLE " where name = ? and password = ?");
		query_user_pass.bind(1, user_info.user().user_name());
		query_user_pass.bind(2, user_info.user().user_pass());

		query_user_pass.executeStep();

		if (!query_user_pass.hasRow())
		{
			log_i("user : %s password error\n", user_info.user().user_name().c_str());
			return ContentResultE::R_CODE_USER_PASS_ERROR;
		}

		UserSession session;
		sole::uuid uid = sole::uuid4();

		session.now_ = user_info.alive_time();
		session.alive_time_ = user_info.alive_time();
		session.token_ = uid.str();
		user_info.set_token(uid.str());

		return add_session(session) ? ContentResultE::R_CODE_OK : ContentResultE::R_CODE_ERROR;
	}

	/**
	 * @brief 用户登出
	 * 
	 * @param user_info 用户信息
	 * @return enum UserInfo_Result 
	 */
	enum ContentResultE logout(UserSessionMsg &user_info)
	{
		delete_session(user_info.token());
		return ContentResultE::R_CODE_OK;
	}

	/**
	 * @brief 用户验证
	 * 
	 * @param user_info 用户信息
	 * @return enum UserInfo_Result 
	 */
	enum ContentResultE verify(UserSessionMsg &user_info)
	{
		UserSession *_session;

		pthread_mutex_lock(&mutex_);
		if (search_user_session(user_info.token(), &_session))
		{
			// 刷新会话时间
			_session->now_ = _session->alive_time_;
			pthread_mutex_unlock(&mutex_);
			return ContentResultE::R_CODE_OK;
		}

		pthread_mutex_unlock(&mutex_);

		return ContentResultE::R_CODE_USER_TOKEN_TIMEOUT;
	}

	/**
	 * @brief 用户注册 FIXME:数据库操作未作异常处理
	 * 
	 * @param user_info 用户信息
	 * @return enum UserInfo_Result 
	 */
	enum ContentResultE user_register(UserSessionMsg &user_info)
	{
		Statement query(*user_db, "SELECT * FROM " USER_TABLE " where name = ?");

		query.bind(1, user_info.user().user_name());

		query.executeStep();

		if (query.hasRow())
		{
			return ContentResultE::R_CODE_USER_EXIST;
		}

		Statement insert(*user_db, "INSERT INTO " USER_TABLE " VALUES (NULL, ?, ?, ?, ?)");

		insert.bind(1, user_info.user().user_name());
		insert.bind(2, user_info.user().user_pass());
		insert.bind(3, user_info.user().phone());
		insert.bind(4, user_info.user().email());
		insert.exec();

		if (SQLITE_DONE != user_db->getErrorCode())
		{
			return ContentResultE::R_CODE_ERROR;
		}

		UserSession session;
		sole::uuid uid = sole::uuid4();

		session.now_ = user_info.alive_time();
		session.alive_time_ = user_info.alive_time();
		session.token_ = uid.str();
		user_info.set_token(uid.str());

		return add_session(session) ? ContentResultE::R_CODE_OK : ContentResultE::R_CODE_ERROR;
	}
};

// template<typename T, typename... Ts>
// std::unique_ptr<T> make_unique(Ts&&... params)
// {
// 	return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
// }

/**
 * @brief 用户管理中间件处理
 * 
 * @tparam  
 * @param type 网络类型
 * @param req 请求数据
 * @param sdk_req sdk请求内容
 * @param sdk_res sdk应答内容
 * @return true 成功
 * @return false 失败
 */
bool _user_manange_midware_do(Sdk &sdk_req, Sdk &sdk_res)
{
	UserSessionMsg user_info = sdk_req.mutable_body()->user_session();
	enum ContentResultE user_ret = ContentResultE::R_CODE_OK;

	log_i("%s %s\n", user_info.user().user_name().c_str(), user_info.user().user_pass().c_str());

	switch (user_info.user_type())
	{
		case UserSessionMsg_UserMethod::UserSessionMsg_UserMethod_U_LOGIN:
			user_ret = user_manager->login(user_info);
			break;
		case UserSessionMsg_UserMethod::UserSessionMsg_UserMethod_U_LOGOUT:
			user_ret = user_manager->logout(user_info);
			break;
		case UserSessionMsg_UserMethod::UserSessionMsg_UserMethod_U_VERI:
			user_ret = user_manager->verify(user_info);
			break;
		case UserSessionMsg_UserMethod::UserSessionMsg_UserMethod_U_REGISTER:
			user_ret = user_manager->user_register(user_info);
			break;
		default:
			user_ret = ContentResultE::R_CODE_ERROR;
			break;
	}

	// sdk_res.mutable_body()->mutable_user_session()->set_result(user_ret);

	sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_status_code(user_ret);

	switch (user_ret)
	{
		case ContentResultE::R_CODE_OK:
			sdk_res.mutable_body()->mutable_user_session()->set_token(user_info.token());
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_status_code(ResponseResult::OK);
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_code("ok");
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_code("ok");
			return true;

		case ContentResultE::R_CODE_USER_EXIST:
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_status_code(ResponseResult::ERROR);
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_code("error");
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_code("user exist");
			return false;

		case ContentResultE::R_CODE_USER_NOT_EXIST:
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_status_code(ResponseResult::ERROR);
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_code("error");
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_code("user not exist");
			return false;

		case ContentResultE::R_CODE_USER_TOKEN_TIMEOUT:
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_status_code(ResponseResult::ERROR);
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_code("error");
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_code("user token timeout");
			return false;

		case ContentResultE::R_CODE_USER_IN_BLACK_LIST:
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_status_code(ResponseResult::ERROR);
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_code("error");
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_code("user in balcklist");
			return false;

		case ContentResultE::R_CODE_USER_NO_PERMISSION:
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_status_code(ResponseResult::ERROR);
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_code("error");
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_code("user no permission");
			return false;

		case ContentResultE::R_CODE_USER_PASS_ERROR:
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_status_code(ResponseResult::ERROR);
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_code("error");
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_code("user password error");
			return false;

		default:
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_status_code(ResponseResult::ERROR);
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_code("error");
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_status_code(ContentResultE::R_CODE_ERROR);
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_code("unknown error");
			return false;
	}
}

/**
 * @brief 用户管理中间件
 * 
 * @param type 网络类型
 * @param req 请求数据
 * @param sdk_req sdk请求内容
 * @param sdk_res sdk应答内容
 * @return true 成功
 * @return false 失败
 */
bool user_manange_midware_do(struct sdk_net_interface &interface, Sdk &sdk_req, Sdk &sdk_res)
{
    if (!_module_state)
    {
        return true;
    }

	return _user_manange_midware_do(sdk_req, sdk_res);
}

/**
 * @brief 用户数据库初始化
 * 
 * @return true 
 * @return false 
 */
bool user_db_init(void)
{
	try {
        if (!user_db)
        {
		    user_db = new Database(OS_SYS_USER_DB_PATH, OPEN_READWRITE|OPEN_CREATE);
        }
	} catch (std::exception& e) {
		log_e("%s\n", e.what());

		return false;
	}

	if (!user_db->tableExists(USER_TABLE))
	{
        // TODO:增加权限
		user_db->exec("CREATE TABLE " USER_TABLE " (id INTEGER PRIMARY KEY, name CHAR(30), password CHAR(128), phone CHAR(24), email CHAR(128))");
		user_db->exec("INSERT INTO sdk_user VALUES (1, \"admin\", \"admin\", \"15558198512\", \"wotsen@outlook.com\")");

		#if 0 // TODO:完善表结构
		user_db->exec("CREATE TABLE " USER_TABLE " ("
								"id INTEGER PRIMARY KEY, "			///< 主键
								"name CHAR(30), "					///< 用户名
								"password CHAR(128), "				///< 密码
								"phone CHAR(24), "					///< 电话号码
								"email CHAR(128)), "				///< 邮件
								"permission INT, "					///< 权限
								"inuse INT8, "						///< 是否有效
								"black INT8");						///< 是否黑名单
		user_db->exec("INSERT INTO sdk_user VALUES (1, \"admin\", \"admin\", \"15558198512\", \"wotsen@outlook.com\", 0xffffffff, 1, 0)");
		#endif
	}

	return true;
}

/**
 * @brief 用户管理任务
 * 
 * @param name 
 * @return void* 
 */
static void *user_manage_task(void *name)
{
	pthread_t tid = pthread_self();

    _module_state = true;

	for (; _module_state; )
    {
        task_alive(tid);          // 自身任务心跳

        ostime_delay(OS_SEC(1)); // 1秒刷新一次

		user_manager->update_session();
    }

	return (void *)0;
}

// 模块清除接口
static void _module_clean(void)
{
    _module_state = false;
}

///< 反初始化用户管理
void user_manager_finit(void)
{
    log_i("module can not support finit\n");
    // _module_clean();
}

///< 用户管理模块状态
bool user_manager_state(void)
{
    return _module_state;
}

/**
 * @brief 初始化用户管理
 * 
 */
void user_manager_init(void)
{
	user_db_init();

    if (!user_manager)
    {
	    user_manager = std::unique_ptr<UserManager>(new UserManager(get_user_manage_max_users_capability()));
    }

	task_create(user_manage_task, STACKSIZE(8000), "user_manage", OS_MIN(5), E_TASK_IGNORE, _module_clean);
}
