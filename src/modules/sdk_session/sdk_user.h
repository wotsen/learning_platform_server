/**
 * @file sdk_user.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-05-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef __wotsenLearnPlatSrv_SDK_USER_H__
#define __wotsenLearnPlatSrv_SDK_USER_H__

#include <vector>
#include <memory>
#include "base/sdk_protocol/in_sdk_body_user.pb.h"
#include "base/sdk_protocol/in_sdk_footer.pb.h"

namespace wotsen
{

using SdkUserInfo = insider::sdk::UserAllInfo;
using SdkUserErr = insider::sdk::ContentResultE;
using SdkUserPermission = insider::sdk::UserPermissson;

class ISdkUserDb;

class SdkUser
{
public:
	SdkUser(ISdkUserDb *db);
	~SdkUser();

public:
	static std::shared_ptr<SdkUser> &sdk_user(ISdkUserDb *db);

public:
/******************************************************************************************************************
 * !单个用户操作
 ******************************************************************************************************************/
	SdkUserErr sdk_user_verify(const std::string &username, const std::string &password);
	bool sdk_user_exist(const std::string &username);
	SdkUserErr sdk_user_change_password(const std::string &username, const std::string &password);
	SdkUserErr sdk_user_add(const SdkUserInfo &user);
	SdkUserErr sdk_user_del(const SdkUserInfo &user);
	SdkUserPermission sdk_user_permission(const std::string &username);

	bool sdk_user_info(const std::string &username, SdkUserInfo &user);

/******************************************************************************************************************
 * !批量用户操作
 ******************************************************************************************************************/
	SdkUserErr sdk_user_list(std::vector<std::shared_ptr<SdkUserInfo>> &list);

private:
	ISdkUserDb *user_db_;
};

} // namespace wotsen

#endif // !1__wotsenLearnPlatSrv_SDK_USER_H__