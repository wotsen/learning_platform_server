/**
 * @file sys_capability.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 具备的能力，具备默认能力，加载json
 * @version 0.1
 * @date 2019-08-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include <string>
#include <nlohmann/json.hpp>

/*****************************************************
 * 初始化接口接口
 *****************************************************/

///< 能力集初始化
bool sys_capability_init(void);

///< 获取能力集json指针
nlohmann::json &get_json_capability(void);

/*****************************************************
 * 能力获取接口
 *****************************************************/

// 获取用户管理最大用户数量能力
uint32_t get_user_manage_max_users_capability(void);

// 获取ip版本能力
std::string get_ip_version_capability(void);

// 获取最大任务个数能力
uint32_t get_max_task_num_capability(void);

// 获取tcp最大连接数能力
uint32_t get_max_tcp_connect_capability(void);
