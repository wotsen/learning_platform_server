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

namespace wotsen
{

using thread_func = void *(*)(void *);
using thread_clean = void (*)(void);

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
    E_TASK_DEAD,
    E_TASK_OVERLOAD
};

struct task_record
{
#define MAX_THREAD_NAME_LEN 15 // 线程名最长长度

#define STACKSIZE(k) ((k)*1024) // 栈长度，传入数字以k为单位
#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN 16384 // 最小栈
#endif

#define TASK_TIME_ERROR_RANGE 10 // 误差时间10s
#define MAX_TASK_TIMEOUT_TIMES 3 // 超时次数

    uint16_t task_id; // 任务id
    pthread_t tid;    // 线程号
    char thread_name[MAX_THREAD_NAME_LEN + 1];
    pthread_attr_t attr; // 线程属性
    size_t stacksize;
    thread_func func;
    thread_clean clean;

    uint32_t create_time;      // 创建时间
    uint32_t alive_time;       // 生存时间
    uint32_t last_update_time; // 上次更新时间
    uint8_t timeout_times;     // 超时次数

    pthread_mutex_t mutex; // 线程锁
    pthread_cond_t cond;   // 互斥条件
    enum task_state state; // 线程状态

    enum task_deadlock action; // 异常处理
};

class TasksManage
{
private:
    TasksManage(){};

    static TasksManage *task_pool; // 任务管理句柄
    pthread_mutex_t mutex;         // 任务锁

    static const uint32_t max_tasks = 128;                  // 最大任务数
    std::vector<std::shared_ptr<struct task_record>> tasks; // 任务池

public:
    ~TasksManage();
    // 获取任务管理句柄
    static TasksManage *get_task_pool(void) noexcept;
    // 创建任务
    bool task_create(const struct task_record *task) noexcept;
    // 任务激活(刷新生存时间)
    void task_alive(const pthread_t &tid) noexcept;
    // 任务等待信号
    void task_wait(const pthread_t &tid) noexcept;
    // 任务暂停
    bool task_stop(const pthread_t &tid) noexcept;
    // 任务继续
    bool task_continue(const pthread_t &tid) noexcept;
    /* 更新任务状态 */
    void task_update(void) noexcept;

    // 任务运行
    friend void *task_run(TasksManage *tasks);

private:
    // 获取目标任务
    auto get_item_task(const pthread_t &tid) noexcept;
    // 检测任务是否异常
    bool task_exist_if(const pthread_t &tid) noexcept;
    // 修正系统时间引起的时间跳变
    void task_correction_time(void) noexcept;
    // 检查任务超时
    void task_check_timeout(void) noexcept;
    // 任务超时处理
    void task_timeout_handler(std::shared_ptr<struct task_record> &_task) noexcept;
    // 任务重新启动
    void task_reload(std::shared_ptr<struct task_record> &_task) noexcept;
    // 任务出栈
    void task_pop(const pthread_t &tid) noexcept;
    // 杀死任务
    void task_kill(std::shared_ptr<struct task_record> &_task) noexcept;
    // 任务注销
    void task_destroy(std::shared_ptr<struct task_record> &_task) noexcept;

    /* 清理死亡任务：包含手动注销、自动注销，异常退出 */
    void task_clean(void) noexcept;
    /* 重载超时任务 */
    void task_overload(void) noexcept;
};
void *task_run(TasksManage *tasks);

// 创建任务外部接口
bool task_create(thread_func func, const size_t stacksize, const char *thread_name,
                 const uint16_t task_id, const uint32_t alive_time, enum task_deadlock action,
                 thread_clean clean = NULL) noexcept;
// 刷新任务自身时间
void task_alive(const pthread_t tid) noexcept;

// 设置任务名称
void task_set_name(const char *name) noexcept;

// 任务管理初始化
void task_manage_init(void) noexcept;
} // namespace wotsen
