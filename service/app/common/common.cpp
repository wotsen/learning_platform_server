/*
 * @Date: 2019-08-10 20:53:17
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-17 11:40:54
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

#define LOG_TAG     "COMMON"

#include "common.h"
#include "config/sys_config.h"
#include "config/sys_capability.h"
#include "config/sys_parameter.h"
#include "../module/module.h"

/**
 * @brief 系统初始化
 * 
 */
void sys_init(void)
{
    sys_config_init(); // 初始化配置

    easylogger_setup(); // 初始化日志
    log_i("日志模块初始化完成...");

    sys_capability_init(); // 初始化能力

    sys_para_init(); // 加载参数

    // network_setup(); // 初始化网络
}