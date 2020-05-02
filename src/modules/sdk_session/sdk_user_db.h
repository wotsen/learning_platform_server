/**
 * @file sdk_user_db.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-05-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef __wotsenLearnPlatSrv_SDK_USER_DB_H__
#define __wotsenLearnPlatSrv_SDK_USER_DB_H__

#include <climits>
#include <cinttypes>
#include <string>
#include <memory>

namespace wotsen
{

/**
 * @brief 用户头像
 * 
 */
struct SdkUserHeadPic
{
	std::string name;	///< 图片名
	std::shared_ptr<char> data;			///< 图片数据
	uint32_t len;		///< 数据长度
};

struct SdkDbUser
{
	uint32_t id;		///< id
	std::string name;	///< 用户名
	std::string nik;	///< 昵称
	char sex;			///< 性别，0-保密，1-双性，2-女，3-男
	std::string pass;	///< 密码
	SdkUserHeadPic head_pic;	///< 头像
	std::string phone;	///< 电话号码
	std::string email;	///< 邮箱
	int permission;		///< 权限
	std::string create_time;	///< 创建时间
	bool valid;			///< 有效性
	bool black;			///< 黑名单

	// 国籍，身份证，居住地，籍贯, 真实姓名...

	struct {
		std::shared_ptr<char> data;
		uint32_t len;
	} res;
};

enum SdkUserDBErr : uint32_t
{
	e_sdk_user_db_ok = 0,
	e_sdk_user_db_init_failed = 1,
	e_sdk_user_not_exist = 2,
	e_sdk_user_exist = 3,
	e_sdk_user_empty = 4,

	e_sdk_user_db_unknown = UINT32_MAX,
};

class ISdkUserDb
{
public:
	ISdkUserDb();
	virtual ~ISdkUserDb();

public:
	virtual SdkUserDBErr init() = 0;
	virtual SdkUserDBErr query(const std::string &name, SdkDbUser *user) = 0;
	virtual SdkUserDBErr query_by_nikname(const std::string &name, SdkDbUser *user) = 0;
	virtual SdkUserDBErr add_user(const SdkDbUser *user) = 0;
	virtual SdkUserDBErr del_user(const SdkDbUser *user) = 0;
	virtual SdkUserDBErr export_user(SdkDbUser **user, uint32_t &num) = 0;
	virtual void free_export_user(SdkDbUser **user) = 0;

public:
	virtual std::string db_soft(void) = 0;
	virtual std::string db_name(void) = 0;
	virtual std::string table_name(void) = 0;

};

} // namespace wotsen


#endif // !1__wotsenLearnPlatSrv_SDK_USER_DB__H__