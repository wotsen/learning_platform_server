/*
 * @Date: 2019-08-14 23:12:24
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-25 14:01:23
 */
/**
 * @file sys_parameter.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 系统参数
 * @version 0.1
 * @date 2019-08-14
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#define LOG_TAG "SYS_PARAMETER"

#include <iostream>
#include "../../tools/tools_func/calculate_crc16.h"
#include <easylogger/inc/elog.h>
#include "sys_parameter.h"

using json = nlohmann::json;

SysParameter *SysParameter::instance = nullptr;

SysParameter *SysParameter::get_sys_param(void)
{
    if (nullptr != instance)
    {
        return instance;
    }

    instance = new SysParameter();
    instance->file = nullptr;

// TODO:读运行时配置
// TODO:运行时配置读取失败则加载备份参数(每隔10分钟备份)，加载失败则加载默认配置并重置所有参数配置
// TODO:所有参数有3个接口：get/set/reset
// TODO:参数配置文件大小4M限制
// TODO:写入加锁，做好有类似装饰器的东西
    try
    {
        instance->file = new tfile::Reader(SYS_RUNTIME_CONFIG_FILENAME);
    }
    catch (const std::runtime_error &err)
    {
        // if (nullptr != instance)
        // {
        //     delete instance;
        //     instance = nullptr;
        // }
		instance->file = nullptr;
        log_e("can not open " SYS_RUNTIME_CONFIG_FILENAME "\n");
    }

	if (!instance->file)
	{
		try
		{
			instance->file = new tfile::Reader(SYS_DEFAULT_CONFIG_FILENAME);
		}
		catch (const std::runtime_error &err)
		{
			if (nullptr != instance)
			{
			    delete instance;
			    instance = nullptr;
			}
			instance->file = nullptr;
			throw("can not open " SYS_DEFAULT_CONFIG_FILENAME);
		}
	}

    if (nullptr == instance->file->get())
    {
        if (nullptr != instance)
        {
            delete instance;
            instance = nullptr;
        }
        throw("can not open " SYS_RUNTIME_CONFIG_FILENAME);
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
        log_d("can not parse json : " SYS_RUNTIME_CONFIG_FILENAME "\n");
    }

    return instance;
}

SysParameter::~SysParameter()
{
    if (nullptr != this->file)
    {
        instance->file->close();
        delete this->file;
        instance->file = nullptr;
    }
}

json &get_json_param(void)
{
    return SysParameter::get_sys_param()->get_json();
}

bool sys_parameter_init(void)
{
    json j = get_json_param();
    if (j.empty())
    {
        exit(0);
        return false;
    }
    log_d("sys_config json : %s\n", j.dump(4).c_str());
    log_i("parser sys config json success!\n");

    return true;
}