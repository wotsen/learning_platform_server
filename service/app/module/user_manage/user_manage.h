/**
 * @file user_manage.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-09-22
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include <iostream>
#include <list>
#include <memory>

struct UserInfo {
	std::string name;
	std::string pass;
	std::string token;
};

class UserManage {
	private:
	UserManage() {}
	public:
	~UserManage();

	static UserManage *get_users(void);

	std::list<std::shared_ptr<UserInfo>> &get_user_list(void)
	{
		return user_list;
	}

	private:
	static UserManage *users;
	std::list<std::shared_ptr<UserInfo>> user_list;
};