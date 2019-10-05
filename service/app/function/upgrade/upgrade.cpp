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
#include "../task_manage/task_manage.h"
#include "../../tools/timer/timer.h"
#include "upgrade.h"

using namespace wotsen;

// 升级协议头
// 升级包分类
// 升级包下载
// 执行升级
// 升级结束

static void *task_upgrade(void *name);

static void *task_upgrade(void *name)
{
    pthread_t tid = pthread_self();

    log_i("升级任务初始化完成...\n");

    for (;;)
    {
        task_alive(tid); // 自身任务心跳

        ostime_delay(OS_SEC(3)); // 3秒刷新一次
    }
}

void task_upgrade_init(void)
{
    task_create(task_upgrade, STACKSIZE(100 * 1024), "task_upgrade", TASK_UPGRADE_ID, OS_MIN(30), E_TASK_REBOOT_SYSTEM);
}