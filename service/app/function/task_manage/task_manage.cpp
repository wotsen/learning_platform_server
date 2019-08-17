/*
 * @Date: 2019-08-16 23:00:34
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-17 15:35:16
 */
/**
 * @file task_manage.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-08-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#define LOG_TAG "TASK_MANAGE"

#include <string.h>
#include <errno.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include "task_manage.h"
#include "../../module/easylogger/inc/elog.h"
#include "../../tools/timer/timer.h"

namespace aipTaskMana {

TasksManage *TasksManage::task_pool = nullptr;

TasksManage *TasksManage::get_task_pool(void)
{
    if (nullptr != task_pool)
    {
        return task_pool;
    }

    task_pool = new TasksManage();
    pthread_mutex_init(&task_pool->mutex, NULL);

    return task_pool;
}

TasksManage::~TasksManage()
{
    if (nullptr != task_pool)
    {
        delete task_pool;
        task_pool = nullptr;
        pthread_mutex_destroy(&task_pool->mutex);
    }
}

/**
 * @brief 创建任务
 * 
 * @param task 
 * @return true 
 * @return false 
 */
bool TasksManage::task_create(const struct task_record *task)
{
    if (NULL == task || NULL == task->func || NULL == task->thread_name)
    {
        log_e("can not create thread, parameter error!\n");
        return false;
    }

    if (task->stacksize < PTHREAD_STACK_MIN)
    { // 存在最小栈大小
        log_e("stacksize too little : %zu, need over %zu\n", task->stacksize, PTHREAD_STACK_MIN);
        return false;
    }

    pthread_mutex_lock(&task_pool->mutex);
    if (task_pool->tasks.size() > task_pool->max_tasks)
    {
        log_e("task_pool full!\n");
        goto err_return;
    }

    struct task_record _task;
    memset(&_task, 0, sizeof(struct task_record));

    memcpy(&_task, task, sizeof(struct task_record));

    memset(&_task.attr, 0, sizeof(pthread_attr_t));

    pthread_attr_init(&_task.attr);
    if (pthread_attr_setdetachstate(&_task.attr, PTHREAD_CREATE_DETACHED) < 0)
    { // 线程分离，自行回收资源，不用join，且无法切回PTHREAD_CREATE_JOINABLE状态
        log_e("%s\n", strerror(errno));
        pthread_attr_destroy(&_task.attr);
        goto err_return;
    }
    if (pthread_attr_setstacksize(&_task.attr, _task.stacksize) < 0)
    {
        log_e("%s\n", strerror(errno));
        pthread_attr_destroy(&_task.attr);
        goto err_return;
    }

    _task.create_time = getostime();
    _task.last_update_time = _task.create_time;
    _task.timeout_times = 0;

    if (pthread_create(&_task.tid, &_task.attr, _task.func, (void *)_task.thread_name) < 0)
    {
        log_e("%s\n", strerror(errno));
        pthread_attr_destroy(&_task.attr);
        goto err_return;
    }

    task_pool->tasks.push_back(_task);

    pthread_mutex_unlock(&task_pool->mutex);

    log_i("create task tid=[%zu], id=[%d], name=[%s]\n", _task.tid, _task.task_id, _task.thread_name);

    return true;

err_return:
    pthread_mutex_unlock(&task_pool->mutex);
    return false;
}

/**
 * @brief 更新任务记录时间点
 * 
 * @param tid 
 */
void TasksManage::task_alive(const pthread_t tid)
{
    pthread_mutex_lock(&mutex);
    if (task_pool->tasks.empty())
    {
        pthread_mutex_unlock(&mutex);
        return;
    }

    for (uint32_t i = 0; i < task_pool->tasks.size(); i++)
    {
        if (tid == task_pool->tasks[i].tid)
        {
            task_pool->tasks[i].last_update_time = getostime();
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
}

/**
 * @brief 任务销毁
 * 
 * @param tid 
 * @return true 
 * @return false 
 */
bool TasksManage::task_destroy(const pthread_t tid)
{
    if (task_pool->tasks.empty())
    {
        return false;
    }

    int ret = 0;
    for (uint32_t i = 0; i < task_pool->tasks.size(); i++)
    {
        if (tid == task_pool->tasks[i].tid)
        {
            ret = pthread_cancel(tid);
            if (ret < 0)
            {
                log_e("%s, can not kill thread=[%d]\n", strerror(errno), tid);
                return false;
            }
            pthread_attr_destroy(&task_pool->tasks[i].attr);
            task_pool->tasks.erase(task_pool->tasks.begin() + i);
            break;
        }
    }
    return true;
}

/**
 * @brief 任务重载
 * 
 * @param task_index 
 * @return true 
 * @return false 
 */
bool TasksManage::task_reload(const int task_index)
{
    struct task_record task;

    memset(&task, 0, sizeof(struct task_record));
    memcpy(&task, &task_pool->tasks[task_index], sizeof(struct task_record));

    if (!task_pool->task_destroy(task_pool->tasks[task_index].tid)) {
        return false;
    }

    if (NULL != task_pool->tasks[task_index].clean) {
        task_pool->tasks[task_index].clean();
    }

    return task_pool->task_create(&task);
}

/**
 * @brief 任务超时处理函数
 * 
 * @param task_index 任务对应的任务池中的id
 * @return true 
 * @return false 
 */
bool TasksManage::task_timeout_handler(const int task_index)
{
    switch (task_pool->tasks[task_index].action) {
        case E_TASK_KILL:
            return task_pool->task_destroy(task_pool->tasks[task_index].tid);
            break;
        case E_TASK_REBOOT_SYSTEM:
            // TODO: 如何自启-->守护进程
            break;
        case E_TASK_IGNORE:
            break;
        case E_TASK_RELOAD:
            return task_pool->task_reload(task_index);
            break;
        default:
            break;
    }
    return true;
}

/**
 * @brief 任务更新
 * 
 */
void TasksManage::task_update(void)
{
    pthread_mutex_lock(&task_pool->mutex);
    if (task_pool->tasks.empty())
    {
        pthread_mutex_unlock(&task_pool->mutex);
        return;
    }

    static uint32_t last_time = getostime();
    uint32_t now = getostime();

    // 时间向前跳变和时间向后跳变超过一分钟，重置任务时间
    if (now < last_time || (now - last_time) > OS_MIN(1)) {
        for (uint32_t i = 0; i < task_pool->tasks.size(); i++) {
            task_pool->tasks[i].last_update_time = now;
        }
        last_time = now;
        pthread_mutex_unlock(&task_pool->mutex);
        return;
    }

    for (uint32_t i = 0; i < task_pool->tasks.size(); i++)
    {
        now = getostime();
        // 检查任务超时
        if ((uint32_t)abs(now - task_pool->tasks[i].last_update_time) > (task_pool->tasks[i].alive_time + TASK_TIME_ERROR_RANGE)) {
            log_e("task tid=[%d], taskid=[%d], taskname=[%s]\n", task_pool->tasks[i].tid, task_pool->tasks[i].task_id, task_pool->tasks[i].thread_name);
            if (task_pool->tasks[i].timeout_times++ > MAX_TASK_TIMEOUT_TIMES) {
                if (!task_timeout_handler(i)) {
                    // TODO:
                }
            }
        } else {
            // 检查到一次未超时就重置超时次数
            task_pool->tasks[i].timeout_times = 0;
        }
    }
    pthread_mutex_unlock(&mutex);
}

/**
 * @brief 创建任务
 * 
 * @param func 任务主函数
 * @param stacksize 栈大小
 * @param thread_name 线程名
 * @param task_id 任务id
 * @param alive_time 超时时间
 * @param action 超时处理
 * @param clean 任务结束处理函数
 * @return true 创建成功
 * @return false 创建失败
 */
bool task_create(thread_func func, const size_t stacksize, const char *thread_name,
                const uint16_t task_id, const uint32_t alive_time, enum task_deadlock action, thread_clean clean)
{
    TasksManage *task_pool = TasksManage::get_task_pool();
    struct task_record task;

    memset(&task, 0, sizeof(struct task_record));

    task.func = func;
    task.stacksize = stacksize;
    snprintf(task.thread_name, MAX_THREAD_NAME_LEN + 1, "%s", thread_name);
    task.task_id = task_id;
    task.alive_time = alive_time;
    task.action = action;
    task.clean = clean;

    return task_pool->task_create(&task);
}

/**
 * @brief 更新任务存活窗台
 * 
 * @param tid 线程id
 */
void task_alive(const pthread_t tid)
{
    TasksManage *task_pool = TasksManage::get_task_pool();
    task_pool->task_alive(tid);
}

/**
 * @brief 任务管理
 * 
 * @param name 线程名
 * @return void* 
 */
static void *task_manage(void *name)
{
    char pname[MAX_THREAD_NAME_LEN+1] = {'\0'};
    // pid_t tid = gettid(); // 这玩意儿不能用?
    pthread_t tid = pthread_self();
    TasksManage *task_pool = TasksManage::get_task_pool();

    if (NULL !=  name) {
        sprintf(pname, "%s", (char *)name);
        prctl(PR_SET_NAME, pname);
    } else {
        sprintf(pname, "p%zu", tid);
        prctl(PR_SET_NAME, pname);
    }

    log_i("tid=[%zu], name=[%s] run\n", tid, pname);
    for ( ; ; ) {
        task_alive(tid);
        task_pool->task_update();

        ostime_delay(OS_MS(3));
    }
}

/**
 * @brief 创建任务管理进程
 * 
 */
void task_init(void)
{
    task_create(task_manage, STACKSIZE(8), "task_manage", TASK_MANAGE_ID, OS_MIN(5), E_TASK_REBOOT_SYSTEM);
}
}