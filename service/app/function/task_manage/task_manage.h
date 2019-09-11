/*
 * @Date: 2019-08-15 23:28:17
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-25 17:46:19
 */
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

namespace aipTaskMana
{

using thread_func = void *(*)(void *);
using thread_clean = void (*)(void);

enum
{
    TASK_MAIN_ID = 0,   // 主线程号
    TASK_MANAGE_ID = 1, // 任务管理线程号
    // 前面的都算做是模块类的任务，之后的算作临时的
    TASK_NORMAL_ID // 普通短期线程
};

enum task_deadlock
{
    E_TASK_RELOAD,
    E_TASK_KILL,
    E_TASK_REBOOT_SYSTEM,
    E_TASK_IGNORE
};

enum task_state
{
    E_TASK_ALIVE,
    E_TASK_STOP,
    E_TASK_DEAD
};

struct task_record
{
    uint16_t task_id;                  // 任务id
    pthread_t tid;                     // 线程号
#define MAX_THREAD_NAME_LEN 15         // 线程名最长长度
#define STACKSIZE(k) ((k)*1024) // 栈长度，传入数字以k为单位
#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN 16384
#endif
    char thread_name[MAX_THREAD_NAME_LEN + 1];
    pthread_attr_t attr; // 线程属性
    size_t stacksize;
    thread_func func;
    thread_clean clean;

#define TASK_TIME_ERROR_RANGE 10 // 误差时间10s
    uint32_t create_time;        // 创建时间
    uint32_t alive_time;         // 生存时间
    uint32_t last_update_time;   // 上次更新时间
#define MAX_TASK_TIMEOUT_TIMES 3
    uint8_t timeout_times; // 超时次数

    pthread_mutex_t mutex;  // 线程锁
    pthread_cond_t cond;  // 互斥条件
    enum task_state state;  // 线程状态

    enum task_deadlock action;  // 异常处理
};

class TasksManage
{
private:
    TasksManage(){};

    static TasksManage *task_pool;  // 任务管理句柄
    pthread_mutex_t mutex;  // 任务锁

    static const uint32_t max_tasks = 128;  // 最大任务数
    std::vector<std::shared_ptr<struct task_record>> tasks;  // 任务池

public:
    ~TasksManage();
    static TasksManage *get_task_pool(void) noexcept;
    bool task_create(const struct task_record *task) noexcept;
    void task_alive(const pthread_t tid) noexcept;
    bool task_stop(const pthread_t tid) noexcept;
    bool task_continue(const pthread_t tid) noexcept;

private:
    void task_timeout_handler(std::shared_ptr<struct task_record> &_task) noexcept;
    void task_reload(std::shared_ptr<struct task_record> &_task) noexcept;
    void task_pop(const pthread_t tid) noexcept;
    void task_destroy(std::shared_ptr<struct task_record> &_task) noexcept;
    void task_update(void) noexcept;
    /* 清理死亡任务：包含手动注销、自动注销，异常退出 */
    void task_clean(void) noexcept;
    void task_wait(const pthread_t tid) noexcept;
    auto get_item_task(const pthread_t tid) noexcept;
    bool task_exist_if(const pthread_t tid) noexcept;
};

bool task_create(thread_func func, const size_t stacksize, const char *thread_name,
                 const uint16_t task_id, const uint32_t alive_time, enum task_deadlock action,
                 thread_clean clean = NULL) noexcept;
void task_alive(const pthread_t tid) noexcept;

void task_set_name(const char * name) noexcept;

void task_init(void) noexcept;
} // namespace aipTaskMana
