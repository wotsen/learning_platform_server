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
#include "os_param.h"
#include "sys_load_json.h"
#include "sys_config.h"

using json = nlohmann::json;

class SysConfig : public AbsSysLoadJson
{
public:

    ~SysConfig();

    ///< 获取系统配置
    static SysConfig *get_sys_config(void);

private:
    SysConfig(){};

    static SysConfig *instance;
};

// 系统只读配置
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

    log_i("parser sys config json success!\n");

    return true;
}

// 获取sdk tcp端口
int get_sdk_tcp_port_config(void)
{
    json &j = get_json_config();

    return j["modules"]["base_fun"]["network"]["sdk"]["tcp"]["port"];
}

// 获取sdk udp端口
int get_sdk_udp_port_config(void)
{
    json &j = get_json_config();

    return j["modules"]["base_fun"]["network"]["sdk"]["udp"]["port"];
}

// 获取sdkip地址
std::string get_sdk_ip_config(void)
{
    json &j = get_json_config();

    return j["modules"]["base_fun"]["network"]["ip"];
}

// 获取ip版本
std::string get_ip_version_config(void)
{
    json &j = get_json_config();

    return j["modules"]["base_fun"]["network"]["ip-version"];
}

// 获取网络接口
std::string get_net_interface_config(void)
{
    json &j = get_json_config();

    return j["modules"]["base_fun"]["network"]["interface"];
}

// 获取网络接口
std::string get_net_gateway_config(void)
{
    json &j = get_json_config();

    return j["modules"]["base_fun"]["network"]["gateway"];
}