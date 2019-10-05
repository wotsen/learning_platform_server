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
#include "../../tools/timer/timer.h"
#include "usr_apps.h"
#include "../../function/task_manage/task_manage.h"
#include "../../function/upgrade/upgrade.h"
#include "../../function/uv_event/uv_event.h"
#include "../../function/sdk_network/sdk_network.h"
#include "../../function/sdk_network/sdk_package_distribution.h"

void usr_apps_init(void)
{
    wotsen::task_manage_init(); // 初始化任务队列

    task_uv_event_init();
    ostime_delay(OS_MS(500)); // 等待500ms，让event初始化完成

    sdk_uv_net_init();

    task_sdk_package_distribution_init();

    task_upgrade_init(); // 升级任务

    // 通用网络协议接口，使用回调分发到各组业务的队列，使用非阻塞消息队列通知
    // 告警
    // 升级，文件传输校验以及如何安装，设计扩展
    // 图像上传下载、图像信息、搜索
    // ai模型加载及运行，级联python
}
