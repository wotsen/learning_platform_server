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

SysConfig *SysConfig::get_sys_config(void)
{
    if (nullptr != instance)
    {
        return instance;
    }

    instance = new SysConfig();

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

SysConfig::~SysConfig()
{
    if (this->json_file)
    {
        instance->json_file.close();
    }
}

json &get_json_config(void)
{
    return SysConfig::get_sys_config()->get_json();
}

void get_sdk_tcp_host(std::string &ip_version, std::string &ip, int &port)
{
    json j = get_json_config();

    ip_version = j["network"]["sdk"]["tcp"]["ip-version"];
    ip = j["network"]["sdk"]["tcp"]["ipv4"];
    port = j["network"]["sdk"]["tcp"]["port"];
}

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
    log_d("sys_config json : %s\n", j.dump(4).c_str());
    log_i("parser sys config json success!\n");

    return true;
}