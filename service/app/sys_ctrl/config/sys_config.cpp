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

#define LOG_TAG "SYS_CONFIG"

#include <iostream>
#include <easylogger/inc/elog.h>
#include "sys_config.h"
#include "../../tools/debug_print/debug_print.h"

using json = nlohmann::json;

SysConfig *SysConfig::instance = nullptr;

/**
 * @brief 获取系统配置
 * 
 * @return SysConfig* : 系统配置指针
 */
SysConfig *SysConfig::get_sys_config(void)
{
    // 单例模式
    if (nullptr != instance)
    {
        return instance;
    }

    instance = new SysConfig();

    // 配置文件是只读的
    instance->json_file.open(SYS_CONFIG_FILENAME, std::ios::in | std::ios::out);

    if (!instance->json_file)
    {
        delete instance;
        throw("can not open " SYS_CONFIG_FILENAME);
    }

    try
    {
        instance->json_file >> instance->j;
    }
    catch (nlohmann::detail::parse_error &err)
    {
        delete instance;
        log_e("%s\n", err.what());
        throw("can not parse json : " SYS_CONFIG_FILENAME);
    }
    if (instance->j.empty())
    {
        delete instance;
        throw("can not parse json : " SYS_CONFIG_FILENAME);
    }

    return instance;
}

/**
 * @brief Destroy the Sys Config:: Sys Config object
 * 
 */
SysConfig::~SysConfig()
{
    if (this->json_file)
    {
        instance->json_file.close();
    }
}

/**
 * @brief Get the json config object
 * 
 * @return json& : 配置参数json指针
 */
json &get_json_config(void)
{
    return SysConfig::get_sys_config()->get_json();
}

/**
 * @brief : 系统配置初始化
 * 
 * @return true 
 * @return false 
 */
bool sys_config_init(void)
{
    json j;

    try
    {
        j = get_json_config();
    }
    catch (std::string &err)
    {
        log_e("%s\n", err.c_str());
        exit(0);
        return false;
    }

    if (j.empty())
    {
        log_e("can not load config json!\n");
        exit(0);
        return false;
    }
    // log_d("sys_config json : %s\n", j.dump(4).c_str());
    log_i("parser sys config json success!\n");

    return true;
}

/**
 * @brief Get the sdk tcp host object
 * 
 * @param ip_version : ip版本
 * @param ip : ip地址
 * @param port : 端口
 */
void get_sdk_tcp_host_config(std::string &ip_version, std::string &ip, int &port)
{
    json j = get_json_config();

    ip_version = j["modules"]["base_fun"]["network"]["ip-version"];
    ip = j["modules"]["base_fun"]["network"]["ip"];
    port = j["modules"]["base_fun"]["network"]["sdk"]["tcp"]["port"];
}

// 获取网络接口
void get_net_interface_config(std::string &interface)
{
    json j = get_json_config();

    interface = j["modules"]["base_fun"]["network"]["interface"];
}

// 获取网络接口
void get_net_gateway_config(std::string &gateway)
{
    json j = get_json_config();

    gateway = j["modules"]["base_fun"]["network"]["gateway"];
}