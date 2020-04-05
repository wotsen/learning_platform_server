/**
 * @file sys_config.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 加载配置文件
 * @version 0.1
 * @date 2019-08-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "sys_config.h"
#include "config.h"

using json = nlohmann::json;
using namespace wotsen;

// 获取sdk tcp端口
int get_sdk_tcp_port_config(void)
{
    json &j = Configures::sys_config();

    return j["modules"]["base_fun"]["network"]["sdk"]["tcp"]["port"];
}

// 获取sdk udp端口
int get_sdk_udp_port_config(void)
{
    json &j = Configures::sys_config();

    return j["modules"]["base_fun"]["network"]["sdk"]["udp"]["port"];
}

// 获取sdkip地址
std::string get_sdk_ip_config(void)
{
    json &j = Configures::sys_config();

    return j["modules"]["base_fun"]["network"]["ip"];
}

// 获取ip版本
std::string get_ip_version_config(void)
{
    json &j = Configures::sys_config();

    return j["modules"]["base_fun"]["network"]["ip-version"];
}

// 获取网络接口
std::string get_net_interface_config(void)
{
    json &j = Configures::sys_config();

    return j["modules"]["base_fun"]["network"]["interface"];
}

// 获取网络接口
std::string get_net_gateway_config(void)
{
    json &j = Configures::sys_config();

    return j["modules"]["base_fun"]["network"]["gateway"];
}