/**
 * @file sdk_user_db_sqlite3.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-05-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <cstring>
#include <exception>
#include <loguru.hpp>
// #include "os_param.h"
#include "sdk_user_db_sqlite3.h"

namespace wotsen
{

using namespace SQLite;

#define SDK_USER_TABLE_DESC \
	"("\
	"id INTEGER PRIMARY KEY AUTOINCREMENT, " \
	"name CHAR(30) NOT NULL, " \
	"nik CHAR(30), " \
	"sex INT NOT NULL, " \
	"password CAHR(24) NOT NULL, " \
	"headpic BLOB, " \
	"phone CHAR(24), " \
	"email CHAR(128) NOT NULL, " \
	"permission INT NOT NULL, " \
	"valid BOOLEAN default 1, " \
	"black BOOLEAN default 0, " \
	"create_time DATETIME NOT NULL, " \
	"res BLOB" \
	")"
	
SdkUserDbSqlite3::SdkUserDbSqlite3(const std::string &db_path, const std::string &table_name) :
					db_soft_("sqlite3"), db_path_(db_path), table_name_(table_name)
{}

SdkUserDbSqlite3::~SdkUserDbSqlite3()
{}

SdkUserDBErr SdkUserDbSqlite3::init()
{
	try {
        if (!db_handle_)
        {
		    db_handle_ = std::make_shared<Database>(db_path_, OPEN_READWRITE|OPEN_CREATE);
        }
	} catch (std::exception& e) {
		LOG_F(ERROR, "%s\n", e.what());

		return e_sdk_user_db_init_failed;
	}

	if (!db_handle_->tableExists(table_name_))
	{
		std::string sql = "CREATE TABLE ";
		sql += table_name_;
		sql += " ";
		sql += SDK_USER_TABLE_DESC;

		LOG_F(INFO, "sql : %s\n", sql.c_str());

		db_handle_->exec(sql);

		// XXX:admin测试
		// TODO:计算密钥摘要
		std::string admin = "INSERT INTO ";
		admin += table_name_;
		admin += " (name, nik, sex, password, phone, email, permission, create_time)"
					" VALUES (\"admin\", \"wotsen\", 3, \"admin\", \"15558198512\", "
					"\"astralrovers@outlook.com\", 3, datetime('now'))";
		
		LOG_F(INFO, "sql : %s\n", admin.c_str());
		
		db_handle_->exec(admin);
	}

	return e_sdk_user_db_ok;
}

void SdkUserDbSqlite3::fill_query_result(Statement &query, SdkDbUser *user)
{
	user->id = query.getColumn(0).getUInt();
	user->name = query.getColumn(1).getString();
	user->nik = query.getColumn(2).getString();
	user->sex = query.getColumn(3).getInt();
	user->pass = query.getColumn(4).getString();

	if (query.getColumn(5).getBytes())
	{
		std::shared_ptr<char> head_pic(new char[query.getColumn(5).getBytes()]);
		user->head_pic.data = head_pic;
		user->head_pic.len = query.getColumn(5).getBytes();

		memcpy(head_pic.get(), query.getColumn(5).getBlob(), user->head_pic.len);
	}
	else
	{
		user->head_pic.len = 0;
	}

	user->phone = query.getColumn(6).getString();
	user->email = query.getColumn(7).getString();
	user->permission = query.getColumn(8).getInt();
	user->valid = query.getColumn(9).getInt();
	user->black = query.getColumn(10).getInt();
	user->create_time = query.getColumn(11).getString();

	if (query.getColumn(12).getBytes())
	{
		std::shared_ptr<char> res(new char[query.getColumn(12).getBytes()]);
		user->res.data = res;
		user->res.len = query.getColumn(12).getBytes();

		memcpy(res.get(), query.getColumn(12).getBlob(), user->res.len);
	}
	else
	{
		user->res.len = 0;
	}
}

SdkUserDBErr SdkUserDbSqlite3::query_by_name(const std::string &name, const std::string &name_type, SdkDbUser *user)
{
	std::string sql("SELECT * FROM ");
	sql += table_name_;
	sql += " WHERE ";
	sql += name_type;
	sql += " = ?";

	Statement query(*db_handle_, sql);
	query.bind(1, name);
	query.executeStep();

	if (!query.hasRow())
	{
		return e_sdk_user_not_exist;
	}

	fill_query_result(query, user);

	return e_sdk_user_db_ok;
}

SdkUserDBErr SdkUserDbSqlite3::query(const std::string &name, SdkDbUser *user)
{
	return query_by_name(name, "name", user);
}

SdkUserDBErr SdkUserDbSqlite3::query_by_nikname(const std::string &name, SdkDbUser *user)
{
	return query_by_name(name, "nik", user);
}

bool SdkUserDbSqlite3::user_exist(const SdkDbUser *user)
{
	std::string sql("SELECT id FROM ");

	sql += table_name_;
	sql += " WHERE name=? or nik=?";

	LOG_F(INFO, "sql : %s\n", sql.c_str());

	Statement query(*db_handle_, sql);
	query.bind(1, user->name);
	query.bind(2, user->nik);

	query.executeStep();

	// 先检查是否存在
	if (query.hasRow())
	{
		LOG_F(WARNING, "find name(%s), or nik name(%s).\n", user->name.c_str(), user->nik.c_str());
		return true;
	}
	return false;
}

SdkUserDBErr SdkUserDbSqlite3::add_user(const SdkDbUser *user)
{
	// 先检查是否存在
	if (user_exist(user))
	{
		return e_sdk_user_exist;
	}

	bool has_head_pic = (user->head_pic.len > 0 && user->head_pic.data);
	bool has_res = (user->res.len > 0 && user->res.data);

	std::string sql = "INSERT INTO ";
	sql += table_name_;

	// 要加的字段
	sql += " (name, nik, sex, password, ";
	sql = has_head_pic ? (sql + "headpic, ") : sql;
	sql += "phone, email, permission, create_time";
	sql = has_res ? (sql + ", res") : sql;
	sql += ")";

	// 值占位符
	sql += " VALUES (?, ?, ?, ?, ";
	sql = has_head_pic ? (sql + "?, ") : sql;
	sql += "?, ?, ?, datetime('now')";
	sql = has_res ? (sql + ", ?") : sql;
	sql += ")";

	LOG_F(INFO, "sql : %s\n", sql.c_str());

	Statement insert(*db_handle_, sql);

	int i = 1;

	// 添加值
	insert.bind(i++, user->name);
	insert.bind(i++, user->nik);
	insert.bind(i++, user->sex);
	insert.bind(i++, user->pass);
	if (has_head_pic)
	{
		insert.bind(i++, user->head_pic.data.get(), user->head_pic.len);
	}
	insert.bind(i++, user->phone);
	insert.bind(i++, user->email);
	insert.bind(i++, user->permission);
	if (has_res)
	{
		insert.bind(i++, user->res.data.get(), user->res.len);
	}

	// 插入
	insert.exec();

	return e_sdk_user_db_ok;
}

SdkUserDBErr SdkUserDbSqlite3::del_user(const SdkDbUser *user)
{
	// 先检查是否存在
	if (!user_exist(user))
	{
		return e_sdk_user_not_exist;
	}

	std::string sql("DELETE FROM ");

	sql += table_name_;
	sql += " WHERE name = ? or nik = ?";

	Statement del(*db_handle_, sql);

	del.bind(1, user->name);
	del.bind(2, user->nik);

	del.exec();

	return e_sdk_user_db_ok;
}

void SdkUserDbSqlite3::free_export_user(SdkDbUser **user)
{
	if (user && *user)
	{
		delete [] *user;
		*user = nullptr;
	}
}

SdkUserDBErr SdkUserDbSqlite3::export_user(SdkDbUser **user, uint32_t &num)
{
	std::string sql("select count(id) from ");

	sql += table_name_;

	Statement cnt(*db_handle_, sql);

	// 先查数量
	if (cnt.executeStep())
	{
		LOG_F(INFO, "total(%d)\n", cnt.getColumn(0).getInt());
	}

	num = cnt.getColumn(0).getInt();
	if (num <= 0)
	{
		return e_sdk_user_empty;
	}

	*user = new SdkDbUser[num];

	sql = "select * from ";
	sql += table_name_;

	Statement query(*db_handle_, sql);

	uint32_t i = 0;
	while (query.executeStep() && i < num)
	{
		fill_query_result(query, &(*user)[i]);
		i++;
	}

	// 正确值
	num = i;

	return e_sdk_user_db_ok;
}

std::string SdkUserDbSqlite3::db_soft(void)
{
	return db_soft_;
}

std::string SdkUserDbSqlite3::db_name(void)
{
	return db_path_;
}

std::string SdkUserDbSqlite3::table_name(void)
{
	return table_name_;
}

} // namespace wotsen
