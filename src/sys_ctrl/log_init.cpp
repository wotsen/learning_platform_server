/**
 * @file log_init.cpp
 * @author yuwangliang (astralrovers@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-01-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <iostream>
#include <easylogger/easylogger_setup.h>
#include "os_param.h"
#include "log_init.h"

static void elog_config(void);

/**
 * @brief 初始化elog日志
 */
static void elog_config(void)
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
        std::cout << "日志模块初始化失败!\n";
        exit(0);
    }
}

void log_init(void)
{
    // 日志采用elog
    elog_config();
}