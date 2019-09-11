/*
 * @Date: 2019-08-11 12:45:30
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2019-08-17 21:26:01
 */
/**
 * @file sys_config.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 系统配置参数，基础的必须配置项，用于系统程序运行，与功能项无关
 * @version 0.1
 * @date 2019-08-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "sys_load_json.h"

#define SYS_ELOG_PATH "../log/elog_file.log"

class SysConfig : public AbsSysLoadJson
{
public:
#define SYS_CONFIG_FILENAME "../etc/sys_config.json"
#define SYS_CONFIG_FILE_MAX 2048

    ~SysConfig();
    static SysConfig *get_sys_config(void);

private:
    SysConfig(){};

    static SysConfig *instance;
};

bool sys_config_init(void);
neb::CJsonObject *get_json_config(void);
