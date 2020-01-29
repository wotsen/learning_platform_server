/**
 * @file user_db.cpp
 * @author yuwangliang (astralrovers@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-01-28
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#define LOG_TAG "USER_DB"

#include <easylogger/easylogger_setup.h>
#include <SQLiteCpp/Backup.h>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <SQLiteCpp/Exception.h>
#include <SQLiteCpp/sqlite3.h> // for SQLITE_ERROR, SQLITE_RANGE and SQLITE_DONE
#include "os_param.h"
#include "user_manage_private.h"

using namespace SQLite;

#define USER_TABLE SDK_USER_TABLE

// 用户数据库
static Database *user_db = nullptr;

// 用户是否存在
bool user_exist(const UserSessionMsg &user_info)
{
    Statement query_user(*user_db, "SELECT * FROM " USER_TABLE " where name = ?");

	query_user.bind(1, user_info.user().user_name());
	query_user.executeStep();

    return query_user.hasRow();
}

// 用户名及密码验证
bool user_verify(const UserSessionMsg &user_info)
{
    // TODO : 这里的密码加密后存储
	Statement query_user_pass(*user_db, "SELECT * FROM " USER_TABLE " where name = ? and password = ?");
	query_user_pass.bind(1, user_info.user().user_name());
	query_user_pass.bind(2, user_info.user().user_pass());

	query_user_pass.executeStep();

    return query_user_pass.hasRow();
}

// 添加用户
bool add_user(const UserSessionMsg &user_info)
{
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

    return SQLITE_DONE != user_db->getErrorCode();
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
