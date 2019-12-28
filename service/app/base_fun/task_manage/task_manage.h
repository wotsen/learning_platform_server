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

/**
 * @brief 任务崩溃处理
 * 
 */
enum task_deadlock
{
    E_TASK_REBOOT_SYSTEM,		///< 系统重启
    E_TASK_IGNORE				///< 忽略
};

struct except_task_info {
    pthread_t tid;
    std::string thread_name;
    std::string reason;
};

// 线程清理接口
using thread_clean = void (*)(void);

// 异常任务外部处理回调接口
using abnormal_task_do = void (*)(const struct except_task_info &);

// 创建任务外部接口
bool task_create(thread_func func, const size_t stacksize, const char *thread_name,
                 const uint32_t alive_time, const enum task_deadlock action,
                 thread_clean clean = NULL, const int priority = SYS_THREAD_PRI_LV) noexcept;

// 刷新任务自身时间
void task_alive(const pthread_t tid) noexcept;

// 任务管理初始化
void task_manage_init(const uint32_t max_tasks = 128, abnormal_task_do except_fun = NULL) noexcept;

} // namespace wotsen
