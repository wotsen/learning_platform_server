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
#include <easylogger/easylogger_setup.h>
#include "../base_fun/task_manage/task_manage.h"
#include "../base_fun/net_manage/net_manage.h"
#include "config/os_param.h"
#include "config/sys_config.h"
#include "config/sys_capability.h"
#include "config/sys_parameter.h"
#include "sys_ctrl.h"

static void _elog_init(void);

/**
 * @brief 初始化elog日志
 */
static void _elog_init(void)
{
    struct elog_custom_config elog_config = {
        .log_path = (char *)SYS_ELOG_PATH,
#ifdef NDEBUG
        .log_lv = ELOG_LVL_INFO
#else
        // debug版本打印所有日志
        .log_lv = ELOG_LVL_VERBOSE
#endif
    };
    if (easylogger_setup(&elog_config))
    {
        log_i("日志模块初始化完成...\n");
    }
    else
    {
        printf("日志模块初始化失败!\n");
        exit(0);
    }
}

void except_task_alarm(const struct except_task_info &except_task)
{
    // TODO:记录到异常任务日志，告警上报
}

/**
 * @brief 系统初始化
 * 
 */
void sys_init(void)
{
    _elog_init();

    sys_config_init();

    sys_capability_init();

    sys_parameter_init();

    wotsen::task_manage_init(OS_SYS_TASK_NUM, reinterpret_cast<wotsen::abnormal_task_do>(except_task_alarm));

    net_init();
}
