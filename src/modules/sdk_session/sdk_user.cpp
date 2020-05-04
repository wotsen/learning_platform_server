/**
 * @file sdk_user.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-05-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <cassert>
#include <loguru.hpp>
#include "sdk_user_db.h"
#include "sdk_user.h"

namespace wotsen
{

using namespace insider::sdk;

SdkUser::SdkUser(ISdkUserDb *db) : user_db_(db)
{
	user_db_->init();
}

SdkUser::~SdkUser()
{
}

std::shared_ptr<SdkUser> &SdkUser::sdk_user(ISdkUserDb *db)
{
	static std::shared_ptr<SdkUser> user;

	if (user)
	{
		return user;
	}

	assert(db);

	user = std::make_shared<SdkUser>(db);

	return user;
}

/******************************************************************************************************************
 * !单个用户操作
 ******************************************************************************************************************/
SdkUserErr SdkUser::sdk_user_verify(const std::string &username, const std::string &password)
{
	std::shared_ptr<SdkDbUser> user(new SdkDbUser);

	if (e_sdk_user_db_ok != user_db_->query(username, user.get()))
	{
		return R_CODE_USER_NOT_EXIST;
	} else if (user->permission == U_PERMISSION_INVALID) {
		return R_CODE_USER_NO_PERMISSION;
	} else if (password != user->pass) {
		return R_CODE_USER_PASS_ERROR;
	} else if (!user->valid) {
		return R_CODE_USER_NOT_EXIST;
	} else if (user->black)
	{
		return R_CODE_USER_IN_BLACK_LIST;
	}

	return R_CODE_OK;
}

bool SdkUser::sdk_user_exist(const std::string &username)
{
	std::shared_ptr<SdkDbUser> user(new SdkDbUser);

	if (e_sdk_user_db_ok != user_db_->query(username, user.get()))
	{
		return false;
	}

	return true;
}

SdkUserPermission SdkUser::sdk_user_permission(const std::string &username)
{
	std::shared_ptr<SdkDbUser> _user(new SdkDbUser);

	if (e_sdk_user_db_ok == user_db_->query(username, _user.get()))
	{
		return U_PERMISSION_INVALID;
	}

	return static_cast<SdkUserPermission>(_user->permission);
}

SdkUserErr SdkUser::sdk_user_change_password(const std::string &username, const std::string &password)
{
	// TODO:
	return R_CODE_OK;
}

SdkUserErr SdkUser::sdk_user_add(const SdkUserInfo &user)
{
	std::shared_ptr<SdkDbUser> _user(new SdkDbUser);

	// TODO:校验用户名长度
	// TODO:校验密码长度，密码摘要
	// TODO:权限校验，管理员就一个
	// TODO:校验邮箱
	// TODO:校验电话号码

	if (e_sdk_user_db_ok == user_db_->query(user.user_base_info().user_name(), _user.get()))
	{
		return R_CODE_USER_EXIST;
	}

	std::shared_ptr<SdkDbUser> adduser(new SdkDbUser);

	adduser->name = user.user_base_info().user_name();
	// TODO:
	adduser->nik = user.user_base_info().user_name();
	adduser->pass = user.user_base_info().user_pass();
	// TODO:
	adduser->sex = 0;
	adduser->permission = user.user_base_info().permission();
	adduser->phone = user.user_base_info().phone();
	adduser->email = user.user_base_info().email();
	adduser->valid = user.valid();
	adduser->black = user.in_blacklist();

	if (e_sdk_user_db_ok != user_db_->add_user(adduser.get()))
	{
		return R_CODE_ERROR;
	}

	return R_CODE_OK;
}

SdkUserErr SdkUser::sdk_user_del(const SdkUserInfo &user)
{
	std::shared_ptr<SdkDbUser> _user(new SdkDbUser);

	if (e_sdk_user_db_ok != user_db_->query(user.user_base_info().user_name(), _user.get()))
	{
		return R_CODE_USER_NOT_EXIST;
	}

	std::shared_ptr<SdkDbUser> deluser(new SdkDbUser);

	deluser->name = user.user_base_info().user_name();
	// TODO:
	deluser->nik = user.user_base_info().user_name();
	deluser->pass = user.user_base_info().user_pass();
	// TODO:
	deluser->sex = 0;
	deluser->permission = user.user_base_info().permission();
	deluser->phone = user.user_base_info().phone();
	deluser->email = user.user_base_info().email();
	deluser->valid = user.valid();
	deluser->black = user.in_blacklist();

	if (e_sdk_user_db_ok != user_db_->add_user(deluser.get()))
	{
		return R_CODE_ERROR;
	}

	return R_CODE_OK;
}

bool SdkUser::sdk_user_info(const std::string &username, SdkUserInfo &user)
{
	std::shared_ptr<SdkDbUser> _user(new SdkDbUser);

	if (e_sdk_user_db_ok == user_db_->query(username, _user.get()))
	{
		return false;
	}

	user.mutable_user_base_info()->set_user_name(_user->name);
	user.mutable_user_base_info()->set_user_pass(_user->pass);
	user.mutable_user_base_info()->set_permission(static_cast<UserPermissson>(_user->permission));
	user.mutable_user_base_info()->set_phone(_user->phone);
	user.mutable_user_base_info()->set_email(_user->email);
	user.set_valid(_user->valid);
	user.set_in_blacklist(_user->black);

	return true;
}

/******************************************************************************************************************
 * !批量用户操作
 ******************************************************************************************************************/
SdkUserErr SdkUser::sdk_user_list(std::vector<std::shared_ptr<SdkUserInfo>> &list)
{
    SdkDbUser *ex = nullptr;
    uint32_t num = 0;

    if (e_sdk_user_db_ok == user_db_->export_user(&ex, num))
    {
        for (uint32_t i = 0; i < num; i++)
        {
			LOG_F(INFO, "id(%d), name(%s), nik(%s), sex(%d), pass(%s), head_pic(%s), email(%s), create_time(%s)\n",
				  ex[i].id, ex[i].name.c_str(), ex[i].nik.c_str(), ex[i].sex, ex[i].pass.c_str(), ex[i].head_pic.data.get(),
				  ex[i].email.c_str(), ex[i].create_time.c_str());

			std::shared_ptr<SdkUserInfo> user(new SdkUserInfo);

			user->mutable_user_base_info()->set_user_name(ex[i].name);
			user->mutable_user_base_info()->set_user_pass(ex[i].pass);
			user->mutable_user_base_info()->set_permission(static_cast<UserPermissson>(ex[i].permission));
			user->mutable_user_base_info()->set_phone(ex[i].phone);
			user->mutable_user_base_info()->set_email(ex[i].email);
			user->set_valid(ex[i].valid);
			user->set_in_blacklist(ex[i].black);

			list.push_back(user);
		}

        user_db_->free_export_user(&ex);
    }

	assert(!ex);

	return R_CODE_OK;
}

} // !namespace wotsen