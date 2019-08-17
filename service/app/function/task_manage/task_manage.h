/*
 * @Date: 2019-08-15 23:28:17
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-17 14:51:26
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

#ifndef __TASK_MANAGE_H__
#define __TASK_MANAGE_H__
#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <inttypes.h>
#include <time.h>
#include <vector>

namespace aipTaskMana {

typedef void *(*thread_func)(void *);
typedef void (*thread_clean)(void);

enum {
    TASK_MAIN_ID = 0,   // 主线程号
    TASK_MANAGE_ID = 1, // 任务管理线程号
    // 前面的都算做是模块类的任务，之后的算作临时的
    TASK_NORMAL_ID      // 普通短期线程
};

enum task_deadlock {
    E_TASK_RELOAD,
    E_TASK_KILL,
    E_TASK_REBOOT_SYSTEM,
    E_TASK_IGNORE
};

struct task_record {
    uint16_t task_id; // 任务id
    pthread_t tid; // 线程号
#define MAX_THREAD_NAME_LEN 15 // 线程名最长长度
#define STACKSIZE(k) ((k) * 1024 * 1024) // 栈长度，传入数字以k为单位
#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN 16384
#endif
    char thread_name[MAX_THREAD_NAME_LEN + 1];
    pthread_attr_t attr; // 线程属性
    size_t stacksize;
    thread_func func;
    thread_clean clean;

#define TASK_TIME_ERROR_RANGE 10 // 误差时间10s
    uint32_t create_time; // 创建时间
    uint32_t alive_time; // 生存时间
    uint32_t last_update_time; // 上次更新时间
#define MAX_TASK_TIMEOUT_TIMES 3
    uint8_t timeout_times; // 超时次数

    enum task_deadlock action;
};

class TasksManage {
public:
    ~TasksManage();
    static TasksManage *get_task_pool(void);
    bool task_create(const struct task_record *task);
    void task_alive(const pthread_t tid);
    bool task_destroy(const pthread_t tid);
    void task_update(void);

private:
    TasksManage() { };
    bool task_timeout_handler(const int task_index);
    bool task_reload(const int task_index);

    static TasksManage *task_pool;
    pthread_mutex_t mutex;

    const uint32_t max_tasks = 128;
    // pthread_t tasks_pid[max_tasks];
    std::vector<struct task_record> tasks;
};

bool task_create(thread_func func, const size_t stacksize, const char* thread_name,
                const uint16_t task_id, const uint32_t alive_time, enum task_deadlock action,
                thread_clean clean = NULL);
void task_alive(const pthread_t tid);

void task_init(void);
}

#endif // !1__TASK_MANAGE_H__