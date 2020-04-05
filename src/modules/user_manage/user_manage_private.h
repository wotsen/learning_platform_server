/**
 * @file user_manage_private.h
 * @author yuwangliang (astralrovers@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-01-28
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once

#include <string>
#include <easylogger/easylogger_setup.h>
#include "pthread.h"
#include <sole/sole.hpp>
#include "util_time/util_time.h"
#include "session/session_manage.h"
#include "sdk_net/sdk_request.h"

using namespace insider::sdk;

// 最长存活时间，7天
const static time_t max_alive_time = OS_HOUR(24 * 7) / OS_SEC(1);

struct UserRequestProc
{
    // 请求类型
	enum UserSessionMsg_UserMethod type;
    // 处理接口
	enum ContentResultE (*proc)(const UserSessionMsg &user_info, UserSessionMsg *res);
};

/**
 * @brief 用户会话
 * 
 */
class UserSession : public Session {
public:
	std::string token_;             ///< token
	std::string name_;              ///< 用户名
	time_t alive_time_;             ///< 生存时间
	time_t now_;                    ///< 当前时间

	UserSession() {}
};

/**
 * @brief 用户管理，WARNING:这里的会话只用于用户登录，无保活功能,FIXME:这会造成如果多个用户掉线时造成资源浪费
 * 
 */
class UserManager : public SessionManager<UserSession> {
private:
    // 会话操作互斥锁
	pthread_mutex_t mutex_;

public:
	UserManager(const uint32_t max_session);

	~UserManager();

	// TODO:修改密码
	bool change_password(const UserSessionMsg &user_info);

	// TODO:查询用户信息
	std::vector<UserAllInfo> query_user_list(void);

	// TODO:用户权限校验中间件
	bool user_permisson_verify(const std::string &username, const UserPermissson &permission);

    // 更新会话
	void update_session(void);

    // 使用token搜索会话
	bool search_user_session_by_token(const std::string &token, UserSession **session) const;

    // 使用用户名搜索会话
	bool search_user_session_by_name(const std::string &name, UserSession **session) const;

    // 增加用户会话
	bool add_session(const UserSessionMsg &user_info, const std::string &token);

    // 创建用户会话
    bool create_user_session(const UserSessionMsg &user_info, std::string &token);

    // 删除用户会话信息
	void delete_session(const std::string &token);

    // 用户登陆
	enum ContentResultE login(const UserSessionMsg &user_info, std::string &token);

    // 用户登出
	enum ContentResultE logout(const UserSessionMsg &user_info);

    // 用户验证
	enum ContentResultE verify(const UserSessionMsg &user_info);

    // 用户注册
	enum ContentResultE user_register(const UserSessionMsg &user_info, std::string &token);
};

// 获取用户管理句柄
const std::unique_ptr<UserManager> &get_user_manager(void);

// 初始化用户数据库
bool user_db_init(void);

/***********************************数据库操作封装****************************************/

// 用户是否存在
bool user_exist(const UserSessionMsg &user_info);

// 用户名及密码验证
bool user_verify(const UserSessionMsg &user_info);

// 添加用户
bool add_user(const UserSessionMsg &user_info);
