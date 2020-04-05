/**
 * @file sys_capability.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-08-15
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "sys_capability.h"
#include "config.h"

using json = nlohmann::json;

namespace wotsen
{

// 获取用户管理最大用户数量能力
uint32_t get_user_manage_max_users_capability(void)
{
    json &j = Configures::sys_capability();

    return j["service"]["modules"]["module_fun"]["user_manage"]["max_users"];
}

// 获取ip版本能力
std::string get_ip_version_capability(void)
{
    json &j = Configures::sys_capability();

    return j["service"]["modules"]["base_fun"]["network"]["sdk"]["support-version"][0];
}

// 获取最大任务个数能力
uint32_t get_max_task_num_capability(void)
{
    json &j = Configures::sys_capability();

    return j["service"]["system"]["sys_max_task_num"];
}

// 获取tcp最大连接数能力
uint32_t get_max_tcp_connect_capability(void)
{
    json &j = Configures::sys_capability();

    return j["service"]["modules"]["base_fun"]["network"]["sdk"]["max_tcp_connect"];
}

} // !namespace wotsen
