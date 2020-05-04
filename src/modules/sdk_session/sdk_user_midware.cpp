/**
 * @file sdk_user_midware.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-05-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <pthread.h>
#include <fmt/format.h>
#include "loguru.hpp"
#include "util_time/util_time.h"
#include "os_param.h"
#include "sdk_user_db_sqlite3.h"
#include "sdk_session.h"
#include "sdk_user.h"
#include "sdk_user_midware.h"

namespace wotsen
{

using namespace insider::sdk;

struct SdkUserRequestProc
{
    // 请求类型
	enum UserSessionMsg_UserMethod type;
    // 处理接口
	enum ContentResultE (*proc)(const UserSessionMsg &user_info, UserSessionMsg *res);
};

// 用户登录
static enum ContentResultE _login_proc(const UserSessionMsg &user_info, UserSessionMsg *res);
// 用户登出
static enum ContentResultE _logout_proc(const UserSessionMsg &user_info, UserSessionMsg *res);
// 用户验证
static enum ContentResultE _verify_proc(const UserSessionMsg &user_info, UserSessionMsg *res);
// 用户注册
static enum ContentResultE _user_register_proc(const UserSessionMsg &user_info, UserSessionMsg *res);

/**************************************************************************************/
// 查找请求类型处理
static SdkUserRequestProc *_find_user_proc(const enum UserSessionMsg_UserMethod &type);
// 填写返回短语
static bool _fill_proc_result_code(const enum ContentResultE &user_ret, Sdk &sdk_res);

// 用户请求方法列表
static std::vector<SdkUserRequestProc> user_request_procs = {
	{UserSessionMsg_UserMethod_U_LOGIN, _login_proc},
	{UserSessionMsg_UserMethod_U_LOGOUT, _logout_proc},
	{UserSessionMsg_UserMethod_U_VERI, _verify_proc},
	{UserSessionMsg_UserMethod_U_REGISTER, _user_register_proc},
};

/*************************************************************************************/

// 最长存活时间，7天
const static time_t max_alive_time = OS_HOUR(24 * 7) / OS_SEC(1);

static pthread_once_t user_once_init = PTHREAD_ONCE_INIT;

static std::shared_ptr<SdkUserDbSqlite3> sqlite3_db;
static std::shared_ptr<SdkUser> sdk_user;
static std::shared_ptr<SdkSession> sdk_session;

static void sdk_user_init(void)
{
	LOG_F(INFO, "\n");
    sqlite3_db = std::make_shared<SdkUserDbSqlite3>(OS_SYS_USER_DB_PATH, SDK_USER_TABLE);
    sdk_user = std::make_shared<SdkUser>(sqlite3_db.get());
    sdk_session = std::make_shared<SdkSession>();
}

// 用户登录
static enum ContentResultE _login_proc(const UserSessionMsg &user_info, UserSessionMsg *res)
{
    // 先查数据库
    auto ret = sdk_user->sdk_user_verify(user_info.user().user_name(), user_info.user().user_pass());

    if (R_CODE_OK != ret)
    {
        return ret;
    }

    SdkUserSessionInfo session;

    session.alive_time_ = user_info.alive_time();
    session.user_name_ = user_info.user().user_name();

	if (max_alive_time < session.alive_time_)
	{
		LOG_F(WARNING, "not support %zu alive time\n", session.alive_time_);
		return R_CODE_USER_ALIVE_TIME_TOOLONG;
	}

    // 数据库有了再创建会话
    if (!sdk_session->add_session(session))
    {
        return R_CODE_ERROR;
    }

    res->set_token(session.token_);

    return R_CODE_OK;
}

// 用户登出
static enum ContentResultE _logout_proc(const UserSessionMsg &user_info, UserSessionMsg *res)
{
    // 删除会话
    if (!sdk_session->del_session(user_info.token()))
    {
        return R_CODE_ERROR;
    }

    return R_CODE_OK;
}

// 会话验证
static enum ContentResultE _verify_proc(const UserSessionMsg &user_info, UserSessionMsg *res)
{
    // 会话验证
    if (!sdk_session->verify(user_info.token()))
    {
        return R_CODE_ERROR;
    }

    return R_CODE_OK;
}

// 用户注册
static enum ContentResultE _user_register_proc(const UserSessionMsg &user_info, UserSessionMsg *res)
{
	if (sdk_user->sdk_user_exist(user_info.user().user_name()))
	{
		return R_CODE_USER_EXIST;
	}

	SdkUserInfo sdk_user_info;

	auto base_info = sdk_user_info.mutable_user_base_info();

	base_info->set_user_name(user_info.user().user_name());
	base_info->set_user_pass(user_info.user().user_pass());
	base_info->set_permission(user_info.user().permission());
	base_info->set_email(user_info.user().email());
	base_info->set_phone(user_info.user().phone());

	sdk_user_info.set_in_blacklist(false);
	sdk_user_info.set_valid(true);

	return sdk_user->sdk_user_add(sdk_user_info);
}

// 查找用户处理方法
static SdkUserRequestProc *_find_user_proc(const enum UserSessionMsg_UserMethod &type)
{
	for (size_t i = 0; i < user_request_procs.size(); i++)
	{
		if (type == user_request_procs[i].type)
		{
			return &user_request_procs[i];
		}
	}

	return nullptr;
}

// 填写返回短语
static bool _fill_proc_result_code(const enum ContentResultE &user_ret, Sdk &sdk_res)
{
	// 填写返回码，可能会被后续操作覆盖
	sdk_content_set_result(user_ret, sdk_res);

    switch (user_ret)
	{
		case ContentResultE::R_CODE_OK:
			sdk_content_set_result("ok", sdk_res);
			return true;

		case ContentResultE::R_CODE_USER_EXIST:
			sdk_content_set_result("user exist", sdk_res);
			return false;

		case ContentResultE::R_CODE_USER_NOT_EXIST:
			sdk_content_set_result("user not exist", sdk_res);
			return false;

		case ContentResultE::R_CODE_USER_TOKEN_TIMEOUT:
			sdk_content_set_result("user token timeout", sdk_res);
			return false;

		case ContentResultE::R_CODE_USER_IN_BLACK_LIST:
			sdk_content_set_result("user in balcklist", sdk_res);
			return false;

		case ContentResultE::R_CODE_USER_NO_PERMISSION:
			sdk_content_set_result("user no permission", sdk_res);
			return false;

		case ContentResultE::R_CODE_USER_PASS_ERROR:
			sdk_content_set_result("user password error", sdk_res);
			return false;

		case ContentResultE::R_CODE_USER_ALIVE_TIME_TOOLONG:
			sdk_content_set_result(fmt::format("alive tiem too long, max is {}", max_alive_time), sdk_res);
			return false;

		default:
			LOG_F(ERROR, "unknown error\n");
			sdk_content_set_result(ContentResultE::R_CODE_ERROR, "unknown error", sdk_res);
			return false;
	}

	return false;
}

bool sdk_user_midware(wotsen::SdkRequest &sdk_request)
{
    // 初始化数据库
    pthread_once(&user_once_init, sdk_user_init);

	const UserSessionMsg &user_info = sdk_request.request.body().user_session();

	LOG_F(INFO, "%s %s\n", user_info.user().user_name().c_str(), user_info.user().user_pass().c_str());

	// 查找处理方法
	SdkUserRequestProc *item = _find_user_proc(user_info.user_type());

	// 用户管理处理
	enum ContentResultE user_ret = !item ?
						R_CODE_ERROR :
						item->proc(user_info, sdk_request.response.mutable_body()->mutable_user_session());

    // 填写返回短语
    return _fill_proc_result_code(user_ret, sdk_request.response);
}

} // namespace wotsen
