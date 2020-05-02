/**
 * @file sdk_user_db_sqlite3.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-05-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef __wotsenLearnPlatSrv_SDK_USER_DB_SQLITE3_H__
#define __wotsenLearnPlatSrv_SDK_USER_DB_SQLITE3_H__

#include <mutex>
#include <SQLiteCpp/Backup.h>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <SQLiteCpp/Exception.h>
#include <SQLiteCpp/sqlite3.h> // for SQLITE_ERROR, SQLITE_RANGE and SQLITE_DONE
#include "sdk_user_db.h"

namespace wotsen
{

class SdkUserDbSqlite3 : public ISdkUserDb
{
public:
	SdkUserDbSqlite3(const std::string &db_path, const std::string &table_name);
	virtual ~SdkUserDbSqlite3();

public:
	virtual SdkUserDBErr init() override;
	virtual SdkUserDBErr query(const std::string &name, SdkDbUser *user) override;
	virtual SdkUserDBErr query_by_nikname(const std::string &name, SdkDbUser *user) override;
	virtual SdkUserDBErr add_user(const SdkDbUser *user) override;
	virtual SdkUserDBErr del_user(const SdkDbUser *user) override;
	virtual SdkUserDBErr export_user(SdkDbUser **user, uint32_t &num) override;
	virtual void free_export_user(SdkDbUser **user) override;

public:
	virtual std::string db_soft(void) override;
	virtual std::string db_name(void) override;
	virtual std::string table_name(void) override;

private:
	SdkUserDBErr query_by_name(const std::string &name, const std::string &name_type, SdkDbUser *user);
	bool user_exist(const SdkDbUser *user);
	void fill_query_result(SQLite::Statement &query, SdkDbUser *user);

private:
	std::string db_soft_;
	std::string db_path_;
	std::string table_name_;
	std::shared_ptr<SQLite::Database> db_handle_;
	std::mutex mtx_;
};
	
} // namespace wotsen


#endif // !1__wotsenLearnPlatSrv_SDK_USER_DB_SQLITE3_H__