/**
 * @file handy_loop.cpp
 * @author yuwangliang (astralrovers@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-01-30
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "util_time/util_time.h"
#include "task_manage/task_manage.h"
#include "handy_loop.h"

namespace wotsen
{

// handy基本事件
static EventBase _handy_base;

EventBase &handy_base(void)
{
    return _handy_base;
}

// handy loop线程，程序loop()会进入事件循环
static void *task_handy_loop(void *name)
{
	pthread_t tid = pthread_self();

    // 1ms后运行，定时器，用于维护handy loop
    _handy_base.runAt(time(NULL) * 1000 + 1, [&]() {
            task_alive(tid); // 自身任务心跳
        }, 5000); // 5s一次

    _handy_base.loop();

	return (void *)0;
}

// 初始化handy loop
void task_handy_loop_init(void)
{
	task_create(task_handy_loop, STACKSIZE(10 * 1024), "task_handy_loop", OS_MIN(30), E_TASK_REBOOT_SYSTEM);

	// 等待500ms，让handy_loop初始化完成
    ostime_delay(OS_MS(500));
}

} // !namespace wotsen
