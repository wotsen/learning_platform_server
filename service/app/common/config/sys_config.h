/*
 * @Date: 2019-08-11 12:45:30
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-12 23:58:29
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

#ifndef __SYS_CONFIG_H__
#define __SYS_CONFIG_H__
#include "sys_load_json.h"

class SysConfig : public AbsSysLoadJson {
public:
#define SYS_CONFIG_FILENAME "ai_runtime/etc/sys_config.json"
#define SYS_CONFIG_FILE_MAX 2048

    ~SysConfig();
    static SysConfig * get_sys_config(void);

private:
    SysConfig() { };

    static SysConfig *instance;
};

bool sys_config_init(void);
neb::CJsonObject *get_json_config(void);

#endif // !1__SYS_CONFIG_H__