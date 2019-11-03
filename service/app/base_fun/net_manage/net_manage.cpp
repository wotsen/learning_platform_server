/**
 * @file net_manage.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-03
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#define LOG_TAG "NET_MANAGE"

#include <easylogger/inc/elog.h>
#include "../../tools/timer/timer.h"
#include "uv_event/uv_event.h"
#include "sdk/sdk_network/sdk_network.h"
#include "sdk/sdk_network/sdk_package_distribution.h"
#include "net_manage.h"

void net_init(void)
{
	// 初始化uv任务
	task_uv_event_init();
	// 等待500ms，让event初始化完成
    ostime_delay(OS_MS(500));

	// 初始化sdk网络
	sdk_uv_net_init();

	// 初始化sdk消息分发任务
	task_sdk_package_distribution_init();
}