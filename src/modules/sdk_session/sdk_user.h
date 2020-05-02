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
#include "sdk_protocol/in_sdk_body_user.pb.h"
#include "sdk_protocol/in_sdk_footer.pb.h"

namespace wotsen
{

using SdkUser = insider::sdk::UserSessionMsg;
using SdkUserErr = insider::sdk::ContentResultE;

class ISdkUserDb;

class SdkUser
{
public:
	SdkUser();
	~SdkUser();

public:
	static std::shared_ptr<SdkUser> &sdk_user();

public:
/******************************************************************************************************************
 * !单个用户操作
 ******************************************************************************************************************/
	SdkUserErr sdk_user_verify(const std::string &username, const std::string &password);
	SdkUserErr sdk_user_change_password(const std::string &username, const std::string &password);
	SdkUserErr sdk_user_add(const SdkUser &user);
	SdkUserErr sdk_user_del(const SdkUser &user);

/******************************************************************************************************************
 * !批量用户操作
 ******************************************************************************************************************/
	SdkUserErr sdk_user_list(std::vector<SdkUser> &list);

private:
	std::shared_ptr<ISdkUserDb> user_db_;
};

} // namespace wotsen

#endif // !1__wotsenLearnPlatSrv_SDK_USER_H__