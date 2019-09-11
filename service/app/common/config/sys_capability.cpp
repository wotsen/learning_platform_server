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

SysCapability *SysCapability::instance = nullptr;

SysCapability *SysCapability::get_sys_capability(void)
{
    if (nullptr != instance)
    {
        return instance;
    }

    instance = new SysCapability();
    instance->json = nullptr;
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
    instance->json = new neb::CJsonObject(conf_str);

    if (instance->json->IsEmpty())
    {
        throw(instance->json->GetErrMsg());
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

    if (nullptr != instance->json)
    {
        delete this->json;
        instance->json = nullptr;
    }
}

neb::CJsonObject *get_json_capability(void)
{
    neb::CJsonObject *conf = nullptr;
    try
    {
        conf = SysCapability::get_sys_capability()->get_json();
    }
    catch (std::string str)
    {
        log_e(str.c_str());
        conf = nullptr;
    }

    return conf;
}

bool sys_capability_init(void)
{
    neb::CJsonObject *json_conf = get_json_capability();
    if (nullptr == json_conf)
    {
        log_e("can not load capability json!");
        exit(0);
        return false;
    }
    log_d("sys_config json : %s\n", get_json_capability()->ToFormattedString().c_str());
    log_i("parser sys config json success!\n");

    return true;
}