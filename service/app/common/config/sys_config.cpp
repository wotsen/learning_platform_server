/*
 * @Date: 2019-08-11 13:00:24
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2019-08-17 21:25:54
 */
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
    instance->file = nullptr;

    try
    {
        instance->file = new tfile::Reader(SYS_CONFIG_FILENAME);
    }
    catch (const std::runtime_error &err)
    {
        if (nullptr != instance)
        {
            delete instance;
            instance = nullptr;
        }
        throw("can not open " SYS_CONFIG_FILENAME);
    }

    if (nullptr == instance->file->get())
    {
        if (nullptr != instance)
        {
            delete instance;
            instance = nullptr;
        }
        throw("can not open " SYS_CONFIG_FILENAME);
    }

    std::string conf_str(2048, '\0');
    instance->file->read(conf_str);

    // 读完了就删掉
    instance->file->close();

    if (nullptr != instance->file)
    {
        delete instance->file;
        instance->file = nullptr;
    }

    // 解析json
    instance->j= json::parse(conf_str);

    if (instance->j.empty())
    {
        log_e("can not parse json : " SYS_CONFIG_FILENAME "\n");
    }

    return instance;
}

SysConfig::~SysConfig()
{
    if (nullptr != this->file)
    {
        instance->file->close();
        delete this->file;
        instance->file = nullptr;
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
    json j = get_json_config();
    if (j.empty())
    {
        exit(0);
        return false;
    }
    log_d("sys_config json : %s\n", j.dump(4).c_str());
    log_i("parser sys config json success!\n");

    return true;
}