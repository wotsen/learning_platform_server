/*
 * @Date: 2019-08-11 13:00:24
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-17 11:34:52
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
#include "sys_config.h"
#include "../../tools/debug_print/debug_print.h"

SysConfig * SysConfig::instance = nullptr;

SysConfig * SysConfig::get_sys_config(void)
{
    if (nullptr != instance) {
        return instance;
    }

    instance = new SysConfig();
    instance->json = nullptr;
    instance->file = nullptr;

    try{
        instance->file = new tfile::Reader(SYS_CONFIG_FILENAME);
    } catch (const std::runtime_error &err) {
        throw ("can not open " SYS_CONFIG_FILENAME);
    }

    if (nullptr == instance->file->get()) {
        throw ("can not open " SYS_CONFIG_FILENAME);
    }

    std::string conf_str(2048, '\0');
    instance->file->read(conf_str);

    // 读完了就删掉
    instance->file->close();

    if (nullptr != instance->file) {
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

SysConfig::~SysConfig()
{
    if (nullptr != instance) {
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

        delete instance;
        instance = nullptr;
    }
}

neb::CJsonObject *get_json_config(void)
{
    neb::CJsonObject * conf = nullptr;
    try {
        conf = SysConfig::get_sys_config()->get_json();
    } catch (std::string str) {
        std::cout << str << std::endl;
        conf = nullptr;
    }

    return conf;
}

bool sys_config_init(void)
{
    neb::CJsonObject *json_conf = get_json_config();
    if (nullptr == json_conf) {
        exit(0);
        return false;
    }
    dbg_print("sys_config json : %s\n", get_json_config()->ToFormattedString().c_str());
    dbg_print("parser sys config json success!\n");

    return true;
}