/*
 * @Date: 2019-08-10 20:53:17
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2019-09-13 15:53:44
 */
/**
 * @file common.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-08-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#define LOG_TAG "COMMON"

#include <iostream>
#include "common.h"
#include "config/sys_config.h"
#include "config/sys_capability.h"
#include "config/sys_parameter.h"
#include <easylogger/easylogger_setup.h>
#include "../module/module.h"

static void _elog_init(void);

static void _elog_init(void)
{
    struct elog_custom_config elog_config = {
        .log_path = (char *)SYS_ELOG_PATH,
#ifdef NDEBUG
        .log_lv = ELOG_LVL_INFO
#else
        .log_lv = ELOG_LVL_VERBOSE
#endif
    };
    if (easylogger_setup(&elog_config)) {
        log_i("日志模块初始化完成...");
    }
    else
    {
        printf("日志模块初始化失败!");
        exit(0);
    } // 初始化日志
}

/**
 * @brief 系统初始化
 * 
 */
void sys_init(void)
{
    sys_config_init(); // 初始化配置

    _elog_init();

    sys_capability_init(); // 初始化能力

    sys_para_init(); // 加载参数

    // network_setup(); // 初始化网络
}
