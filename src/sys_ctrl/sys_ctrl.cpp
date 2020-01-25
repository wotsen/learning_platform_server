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
#include "config/os_param.h"
#include "config/sys_config.h"
#include "config/sys_capability.h"
#include "config/sys_parameter.h"
#include "log_init.h"
#include "soft_setup.h"
#include "sys_ctrl.h"

/**
 * @brief 系统初始化
 * 
 */
void sys_init(void)
{
    // 软件配置
    soft_setup();

    // 日志初始化
    log_init();

    // 系统配置参数初始化
    sys_config_init();

    // 系统能力集初始化
    sys_capability_init();

    // 系统运行时参数初始化
    sys_parameter_init();

    // TODO:网络配置
}
