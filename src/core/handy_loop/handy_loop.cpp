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

#include <task/task.h>
#include "util_time/util_time.h"
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
static void task_handy_loop(void)
{
    // 1ms后运行，定时器，用于维护handy loop
    _handy_base.runAt(time(NULL) * 1000 + 1, [&]() {
            Task::is_task_alive(task_id());
        }, 5000); // 5s一次

    _handy_base.loop();
}

// 初始化handy loop
void task_handy_loop_init(void)
{
    TaskRegisterInfo reg_info;

	reg_info.task_attr.task_name = "task_handy_loop";
	reg_info.task_attr.stacksize = TASK_STACKSIZE(10);
	reg_info.task_attr.priority = e_sys_task_pri_lv;
	reg_info.alive_time = OS_MIN(30);
	reg_info.e_action = e_task_reboot_system;

	auto ret = Task::register_task(reg_info, task_handy_loop);

    // 启动
	Task::task_run(ret.tid);

	// 等待500ms，让handy_loop初始化完成
    ostime_delay(OS_MS(500));
}

} // !namespace wotsen
