/*
 * @Date: 2019-08-15 22:25:47
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-25 14:00:39
 */
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

#define LOG_TAG "SYS_CAPABILITY"

#include "sys_capability.h"
#include <easylogger/inc/elog.h>

using json = nlohmann::json;

SysCapability *SysCapability::instance = nullptr;

SysCapability *SysCapability::get_sys_capability(void)
{
    if (nullptr != instance)
    {
        return instance;
    }

    instance = new SysCapability();
    instance->file = nullptr;

    try
    {
        instance->file = new tfile::Reader(SYS_CAPAABILITY_FILENAME);
    }
    catch (const std::runtime_error &err)
    {
        if (nullptr != instance)
        {
            delete instance;
            instance = nullptr;
        }
        throw("can not open " SYS_CAPAABILITY_FILENAME);
    }

    if (nullptr == instance->file->get())
    {
        if (nullptr != instance)
        {
            delete instance;
            instance = nullptr;
        }
        throw("can not open " SYS_CAPAABILITY_FILENAME);
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
    instance->j = json::parse(conf_str);

    if (instance->j.empty())
    {
        log_e("can not parse json : " SYS_CAPAABILITY_FILENAME "\n");
    }

    return instance;
}

SysCapability::~SysCapability()
{
    instance->file->close();

    if (nullptr != this->file)
    {
        delete this->file;
        instance->file = nullptr;
    }
}

json &get_json_capability(void)
{
    return SysCapability::get_sys_capability()->get_json();
}

bool sys_capability_init(void)
{
    json j = get_json_capability();
    if (j.empty())
    {
        log_e("can not load capability json!");
        exit(0);
        return false;
    }
    log_d("sys_capability json : %s\n", j.dump(4).c_str());
    log_i("parser sys capability json success!\n");

    return true;
}