/**
 * @file usr_apps.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-08-17
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#define LOG_TAG "USR_APPS"

#include <easylogger/inc/elog.h>
#include "util_time/util_time.h"
#include "../module_fun/upgrade/upgrade.h"
#include "../module_fun/user_manage/user_manage.h"
#include "usr_apps.h"

void usr_apps_init(void)
{
    task_upgrade_init(); // 升级任务

    user_manager_init(); // 任务管理

    // 告警
    // 升级，文件传输校验以及如何安装，设计扩展
    // 图像上传下载、图像信息、搜索
    // ai模型加载及运行，级联python
}
