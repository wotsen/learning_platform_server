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
#include <loguru.hpp>
#include <task/task.h>
#include "util_time/util_time.h"
#include "upgrade.h"

namespace wotsen
{

// 升级协议头
// 升级包分类
// 升级包下载
// 执行升级
// 升级结束

namespace {

static bool _module_state = false;

/**
 * @brief 升级任务
 * 
 * @param name 
 * @return void* 
 */
static void system_upgrade(void)
{
    _module_state = true;

    for (; Task::is_task_alive(task_id()) && _module_state; )
    {
        ostime_delay(OS_SEC(3)); // 3秒刷新一次
    }
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
    TaskRegisterInfo reg_info;

	reg_info.task_attr.task_name = "system_upgrade";
	reg_info.task_attr.stacksize = TASK_STACKSIZE(20 * 1024);
	reg_info.task_attr.priority = e_sys_task_pri_lv;
	reg_info.alive_time = OS_MIN(30);
	reg_info.e_action = e_task_reboot_system;

	auto ret = Task::register_task(reg_info, system_upgrade);

    // 添加退出回调
	Task::add_task_exit_action(ret.tid, [&]() { _module_state = false; });
	Task::add_task_timeout_action(ret.tid, [&]() { _module_state = false; });
	Task::add_task_except_action(ret.tid, [&]() { _module_state = false; });

    // 启动
	Task::task_run(ret.tid);
}

} // !namespace wotsen
