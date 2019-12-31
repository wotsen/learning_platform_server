/**
 * @file upgrade.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-08-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#define LOG_TAG "UPGRADE"

#include <easylogger/inc/elog.h>
#include "util_time/util_time.h"
#include "../../base_fun/task_manage/task_manage.h"
#include "upgrade.h"

using namespace wotsen;

// 升级协议头
// 升级包分类
// 升级包下载
// 执行升级
// 升级结束

namespace {

static bool _module_state = false;

static void *system_upgrade(void *name);

/**
 * @brief 升级任务
 * 
 * @param name 
 * @return void* 
 */
static void *system_upgrade(void *name)
{
    pthread_t tid = pthread_self();
    _module_state = true;

    log_i("升级任务初始化完成...\n");

    for (; _module_state; )
    {
        task_alive(tid); // 自身任务心跳

        ostime_delay(OS_SEC(3)); // 3秒刷新一次
    }

    return (void*)0;
}

}

///< 系统升级模块状态
bool system_upgrade_task_state(void)
{
    return _module_state;
}

/**
 * @brief 初始化升级任务
 * 
 */
void system_upgrade_task_init(void)
{
    task_create(system_upgrade, STACKSIZE(100 * 1024), "system_upgrade", OS_MIN(30), E_TASK_REBOOT_SYSTEM);
}
