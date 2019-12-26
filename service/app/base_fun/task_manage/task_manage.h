/**
 * @file task_manage.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-08-15
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include <iostream>
#include <memory>
#include <stdio.h>
#include <pthread.h>
#include <inttypes.h>
#include <time.h>
#include <vector>
#include <util_pthread/util_pthread.h>

namespace wotsen
{

#if 0
/**
 * @brief 任务编号
 * 
 */
enum
{
    TASK_MAIN_ID = 0,     // 主线程号
    TASK_MANAGE_ID = 1,   // 任务管理线程号
    TASK_UPGRADE_ID = 2,  // 升级任务
    TASK_NETWORK_ID = 3,  // 网络任务
    TASK_ALARM_ID = 4,    // 告警任务
    TASK_AI_SCHEDULE = 5, // ai调度
    // 前面的都算做是模块类的任务，之后的算作临时的
    TASK_NORMAL_ID // 普通短期线程
};
#endif

/**
 * @brief 任务崩溃处理
 * 
 */
enum task_deadlock
{
    E_TASK_RELOAD,				///< 任务重新启动
    E_TASK_KILL,				///< 结束任务
    E_TASK_REBOOT_SYSTEM,		///< 系统重启
    E_TASK_IGNORE				///< 忽略
};

// 线程清理接口
using thread_clean = void (*)(void);

using abnormal_task_do = void (*)(const pthread_t tid, const char *thread_name, const char *reason);

// 创建任务外部接口
bool task_create(thread_func func, const size_t stacksize, const char *thread_name,
                 const uint32_t alive_time, const enum task_deadlock action,
                 thread_clean clean = NULL, const int priority = SYS_THREAD_PRI_LV) noexcept;

// 刷新任务自身时间
void task_alive(const pthread_t tid) noexcept;

// 任务管理初始化
void task_manage_init(const uint32_t max_tasks = 128, abnormal_task_do except_fun = NULL) noexcept;

} // namespace wotsen
