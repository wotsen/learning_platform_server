/**
 * @file user_proc_do.cpp
 * @author yuwangliang (astralrovers@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-01-28
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#define LOG_TAG "USER_PROC_DO"

#include <vector>
#include <fmt/format.h>
#include <easylogger/easylogger_setup.h>
#include "user_manage_private.h"
#include "user_manage.h"

// 处理是否成功
static bool is_proc_ok(enum ContentResultE &user_ret);

// 用户登录
static enum ContentResultE login_proc(const UserSessionMsg &user_info, UserSessionMsg *res);
// 用户登出
static enum ContentResultE logout_proc(const UserSessionMsg &user_info, UserSessionMsg *res);
// 用户验证
static enum ContentResultE verify_proc(const UserSessionMsg &user_info, UserSessionMsg *res);
// 用户注册
static enum ContentResultE user_register_proc(const UserSessionMsg &user_info, UserSessionMsg *res);

/**************************************************************************************/
// 查找请求类型处理
static UserRequestProc *find_user_proc(const enum UserSessionMsg_UserMethod &type);
// 填写返回短语
static bool fill_proc_result_code(const enum ContentResultE &user_ret, Sdk &sdk_res);
// 实际用户管理中间件处理
static bool _user_manange_midware_do(const Sdk &sdk_req, Sdk &sdk_res);

// 用户请求方法列表
static std::vector<UserRequestProc> user_request_procs = {
	{UserSessionMsg_UserMethod_U_LOGIN, login_proc},
	{UserSessionMsg_UserMethod_U_LOGOUT, logout_proc},
	{UserSessionMsg_UserMethod_U_VERI, verify_proc},
	{UserSessionMsg_UserMethod_U_REGISTER, user_register_proc},
};

/*************************************************************************************/

// 处理是否成功
static bool is_proc_ok(enum ContentResultE &user_ret)
{
	return ContentResultE::R_CODE_OK == user_ret;
}

// 用户登录
static enum ContentResultE login_proc(const UserSessionMsg &user_info, UserSessionMsg *res)
{
	std::string token;
	enum ContentResultE user_ret = get_user_manager()->login(user_info, token);

	if (is_proc_ok(user_ret))
	{
		res->set_token(token);
	}

	return user_ret;
}

// 用户登出
static enum ContentResultE logout_proc(const UserSessionMsg &user_info, UserSessionMsg *res)
{
	return get_user_manager()->logout(user_info);
}

// 用户验证
static enum ContentResultE verify_proc(const UserSessionMsg &user_info, UserSessionMsg *res)
{
	return get_user_manager()->verify(user_info);
}

// 用户注册
static enum ContentResultE user_register_proc(const UserSessionMsg &user_info, UserSessionMsg *res)
{
	std::string token;
	enum ContentResultE user_ret = get_user_manager()->user_register(user_info, token);

	if (is_proc_ok(user_ret))
	{
		res->set_token(token);
	}

	return user_ret;
}

// 查找用户处理方法
static UserRequestProc *find_user_proc(const enum UserSessionMsg_UserMethod &type)
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
static bool fill_proc_result_code(const enum ContentResultE &user_ret, Sdk &sdk_res)
{
    switch (user_ret)
	{
		case ContentResultE::R_CODE_OK:
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_code("ok");
			return true;

		case ContentResultE::R_CODE_USER_EXIST:
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_code("user exist");
			return false;

		case ContentResultE::R_CODE_USER_NOT_EXIST:
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_code("user not exist");
			return false;

		case ContentResultE::R_CODE_USER_TOKEN_TIMEOUT:
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_code("user token timeout");
			return false;

		case ContentResultE::R_CODE_USER_IN_BLACK_LIST:
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_code("user in balcklist");
			return false;

		case ContentResultE::R_CODE_USER_NO_PERMISSION:
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_code("user no permission");
			return false;

		case ContentResultE::R_CODE_USER_PASS_ERROR:
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_code("user password error");
			return false;

		case ContentResultE::R_CODE_USER_ALIVE_TIME_TOOLONG:
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_code(
						fmt::format("alive tiem too long, max is {}", max_alive_time));
			return false;

		default:
			log_d("unknown error\n");
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_status_code(ContentResultE::R_CODE_ERROR);
			sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_code("unknown error");
			return false;
	}

	return false;
}

// 实际用户管理中间件处理
static bool _user_manange_midware_do(const Sdk &sdk_req, Sdk &sdk_res)
{
	const UserSessionMsg &user_info = sdk_req.body().user_session();

	log_d("%s %s\n", user_info.user().user_name().c_str(), user_info.user().user_pass().c_str());

	// 查找处理方法
	UserRequestProc *item = find_user_proc(user_info.user_type());

	// 用户管理处理
	enum ContentResultE user_ret = !item ?
						R_CODE_ERROR :
						item->proc(user_info, sdk_res.mutable_body()->mutable_user_session());

	// 填写返回码，可能会被后续操作覆盖
	sdk_res.mutable_footer()->mutable_result()->mutable_content_result()->set_status_code(user_ret);

    // 填写返回短语
    return fill_proc_result_code(user_ret, sdk_res);
}

/**
 * @brief 用户管理中间件
 * 
 * @param interface 网络接口
 * @param sdk_req sdk请求内容
 * @param sdk_res sdk应答内容
 * @return true 成功
 * @return false 失败
 */
bool user_manange_midware_do(struct sdk_net_interface &interface, const Sdk &sdk_req, Sdk &sdk_res)
{
    if (!user_manager_state()) { return true; }

	return _user_manange_midware_do(sdk_req, sdk_res);
}