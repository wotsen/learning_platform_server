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
#include <easylogger/easylogger_setup.h>
#include "task_manage/task_manage.h"
#include "sys_capability.h"
#include "user_manage_private.h"
#include "user_manage.h"

namespace wotsen
{

// 模块状态
static bool _module_state = false;
// 用户管理
static std::unique_ptr<UserManager> user_manager;

// 模块清理
static void _module_clean(void);
// 用户管理任务
static void *user_manage_task(void *name);

// 模块清除接口
static void _module_clean(void)
{
    _module_state = false;
}

///< 用户管理模块状态
bool user_manager_state(void)
{
    return _module_state;
}

// 构造函数
UserManager::UserManager(const uint32_t max_session) : SessionManager<UserSession>(max_session) {
	pthread_mutex_init(&mutex_, NULL);
}

// 析构函数
UserManager::~UserManager() {
	pthread_mutex_destroy(&mutex_);
}

// TODO:修改密码
bool UserManager::change_password(const UserSessionMsg &user_info)
{
	return true;
}

// TODO:查询用户信息
std::vector<UserAllInfo> UserManager::query_user_list(void)
{
	std::vector<UserAllInfo> user_list;
	// FIXME:无法一次查完?
	return user_list;
}

// TODO:用户权限校验中间件
bool UserManager::user_permisson_verify(const std::string &username, const UserPermissson &permission)
{
	return true;
}

/**
 * @brief 更新会话
 * 
 */
void UserManager::update_session(void)
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
							if (0 == _session->now_) {
								log_d("user %s token over time\n", _session->name_.c_str());
								return true;
							}

							return false;
					}),
					sessions_.end());
	
	pthread_mutex_unlock(&mutex_);
}

/**
 * @brief 使用token搜索会话
 * 
 * @param token 
 * @param session 输出，搜索到的用户
 * @return true 搜索到
 * @return false 未搜索到
 */
bool UserManager::search_user_session_by_token(const std::string &token, UserSession **session) const
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
 * @brief 使用用户名搜索会话
 * 
 * @param name 用户名
 * @param session 输出，搜索到的用户
 * @return true 搜索到
 * @return false 未搜索到
 */
bool UserManager::search_user_session_by_name(const std::string &name, UserSession **session) const
{
	for (auto &item : sessions_)
	{
		if (name == item->name_)
		{
			*session = &(*item);
			return true;
		}
	}
	return false;
}

/**
 * @brief 增加会话
 * 
 * @param user_info 用户信息
 * @param token 
 * @return true 
 * @return false 
 */
bool UserManager::add_session(const UserSessionMsg &user_info, const std::string &token)
{
	std::shared_ptr<UserSession> new_session(new UserSession);

	pthread_mutex_lock(&mutex_);

	// 用户管理满
	if (sessions_.size() >= max_session_)
	{
		log_d("user session overs max\n");
		pthread_mutex_unlock(&mutex_);
		return false;
	}

	new_session->name_ = user_info.user().user_name();
	new_session->now_ = user_info.alive_time();
	new_session->alive_time_ = user_info.alive_time();
	new_session->token_ = token;

	log_d("user %s token %s\n", new_session->name_.c_str(), token.c_str());

	sessions_.push_back(new_session);
	pthread_mutex_unlock(&mutex_);

	return true;
}

/**
 * @brief 创建用户会话
 * 
 * @param user_info 
 * @param token 输出
 * @return true 
 * @return false 
 */
bool UserManager::create_user_session(const UserSessionMsg &user_info, std::string &token)
{
	UserSession *_session;

	// 用户已经存在只是掉线了，token也丢失
	if (search_user_session_by_name(user_info.user().user_name(), &_session))
	{
		pthread_mutex_lock(&mutex_);
		_session->alive_time_ = user_info.alive_time();
		_session->now_ = user_info.alive_time();
		pthread_mutex_unlock(&mutex_);

		token = _session->token_;

		log_d("user %s login ready\n", user_info.user().user_name().c_str());
		log_d("user %s token %s\n", _session->name_.c_str(), token.c_str());

		return true;
	}

	token = sole::uuid4().str();

	return add_session(user_info, token);
}

/**
 * @brief 删除用户会话信息
 * 
 * @param token 会话token
 */
void UserManager::delete_session(const std::string &token)
{
	pthread_mutex_lock(&mutex_);
	sessions_.erase(std::remove_if(sessions_.begin(), sessions_.end(), [&token](auto &_session) -> bool {
							log_d("delete user %s\n", _session->name_.c_str());
							return _session->token_ == token;
					}),
					sessions_.end());
	pthread_mutex_unlock(&mutex_);
}

/**
 * @brief 用户登陆
 * 
 * @param user_info 用户信息,FIXME:数据库操作未作异常处理，FIXME:用户掉线后再次发出登录操作
 * @param token 
 * @return enum UserInfo_Result 
 */
enum ContentResultE UserManager::login(const UserSessionMsg &user_info, std::string &token)
{
	if (max_alive_time < user_info.alive_time())
	{
		log_d("not support %zu alive time\n", user_info.alive_time());
		return ContentResultE::R_CODE_USER_ALIVE_TIME_TOOLONG;
	}

	if (!user_exist(user_info))
	{
		log_i("user : %s not exsit\n", user_info.user().user_name().c_str());
		return ContentResultE::R_CODE_USER_NOT_EXIST;
	}

	if (!user_verify(user_info))
	{
		log_i("user : %s password error\n", user_info.user().user_name().c_str());
		return ContentResultE::R_CODE_USER_PASS_ERROR;
	}

	return create_user_session(user_info, token) ? ContentResultE::R_CODE_OK : ContentResultE::R_CODE_ERROR;
}

/**
 * @brief 用户登出
 * 
 * @param user_info 用户信息
 * @return enum UserInfo_Result 
 */
enum ContentResultE UserManager::logout(const UserSessionMsg &user_info)
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
enum ContentResultE UserManager::verify(const UserSessionMsg &user_info)
{
	UserSession *_session;

	pthread_mutex_lock(&mutex_);
	if (search_user_session_by_token(user_info.token(), &_session))
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
 * @param token 
 * @return enum ContentResultE 
 */
enum ContentResultE UserManager::user_register(const UserSessionMsg &user_info, std::string &token)
{
	if (max_alive_time < user_info.alive_time())
	{
		log_d("not support %zu alive time\n", user_info.alive_time());
		return ContentResultE::R_CODE_USER_ALIVE_TIME_TOOLONG;
	}

	if (user_exist(user_info))
	{
		log_i("user : %s exsit\n", user_info.user().user_name().c_str());
		return ContentResultE::R_CODE_USER_EXIST;
	}

	if (!add_user(user_info))
	{
		log_i("add user : %s failed\n", user_info.user().user_name().c_str());
		return ContentResultE::R_CODE_ERROR;
	}

	return create_user_session(user_info, token) ? ContentResultE::R_CODE_OK : ContentResultE::R_CODE_ERROR;
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

///< 反初始化用户管理
void user_manager_finit(void)
{
    log_i("module can not support finit\n");
    // _module_clean();
}

// 获取用户管理句柄
const std::unique_ptr<UserManager> &get_user_manager(void)
{
	return user_manager;
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

} // !namespace wotsen
