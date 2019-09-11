/*
 * @Date: 2019-08-16 23:00:34
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-25 18:32:50
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
#include <signal.h>
#include <sys/types.h>
#include <memory>
#include <algorithm>
#include "task_manage.h"
#include <easylogger/inc/elog.h>
#include "../../tools/timer/timer.h"

namespace aipTaskMana
{

TasksManage *TasksManage::task_pool = nullptr;

static void *task_manage(void *name) noexcept;

TasksManage *TasksManage::get_task_pool(void) noexcept
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
    for (auto item : task_pool->tasks) {
        pthread_mutex_lock(&item->mutex);
        if (pthread_cancel(item->tid) < 0)
        {
            log_e("%s, can not kill thread=[%d]\n", strerror(errno), item->tid);
        }
        pthread_mutex_unlock(&item->mutex);

        pthread_mutex_destroy(&item->mutex);

        pthread_attr_destroy(&item->attr);
    }

    task_pool->tasks.clear();

    pthread_mutex_unlock(&task_pool->mutex);
    pthread_mutex_destroy(&task_pool->mutex);
}

bool TasksManage::task_create(const struct task_record *task) noexcept
{
    std::shared_ptr<struct task_record> _task = nullptr;

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

    _task = std::make_shared<struct task_record>(*task);

    memset(&_task->attr, 0, sizeof(pthread_attr_t));
    pthread_mutex_init(&_task->mutex, NULL);
    pthread_cond_init(&_task->cond, NULL);

    pthread_attr_init(&_task->attr);
    if (pthread_attr_setdetachstate(&_task->attr, PTHREAD_CREATE_DETACHED) < 0)
    { // 线程分离，自行回收资源，不用join，且无法切回PTHREAD_CREATE_JOINABLE状态
        log_e("%s\n", strerror(errno));
        goto err_return;
    }
    if (pthread_attr_setstacksize(&_task->attr, _task->stacksize) < 0)
    {
        log_e("%s\n", strerror(errno));
        goto err_return;
    }

    _task->create_time = getostime();
    _task->last_update_time = _task->create_time;
    _task->timeout_times = 0;
    _task->state = E_TASK_ALIVE;

    if (pthread_create(&_task->tid, &_task->attr, _task->func, (void *)_task->thread_name) < 0)
    {
        log_e("%s\n", strerror(errno));
        goto err_return;
    }

    log_i("create task tid=[%zu], id=[%d], name=[%s]\n", _task->tid, _task->task_id, _task->thread_name);

    task_pool->tasks.push_back(std::move(_task));
    pthread_mutex_unlock(&task_pool->mutex);

    return true;

err_return:
    if (_task) {
        pthread_attr_destroy(&_task->attr);
        pthread_mutex_destroy(&_task->mutex);
    }
    pthread_mutex_unlock(&task_pool->mutex);
    return false;
}

bool TasksManage::task_exist_if(const pthread_t tid) noexcept
{
    int pthread_kill_err;
    pthread_kill_err = pthread_kill(tid, 0);

    if (ESRCH == pthread_kill_err) {
        log_i("task=[%ld] is not exist!/n", tid);
        return false;
    } else if (EINVAL == pthread_kill_err) {
        log_i("task=[%ld], send invalid signal to it!/n", tid);
        return false;
    } else {
        log_i("task=[%d] exist!/n", tid);
        return true;
    }
}

auto TasksManage::get_item_task(const pthread_t tid) noexcept
{
    pthread_mutex_lock(&task_pool->mutex);

    for (auto item : task_pool->tasks) {
        if (tid == item->tid) {
            pthread_mutex_unlock(&mutex);
            return item;
        }
    }

    pthread_mutex_unlock(&task_pool->mutex);
    return static_cast<std::shared_ptr<struct task_record>>(nullptr);
}

bool TasksManage::task_stop(const pthread_t tid) noexcept
{
    auto _task = get_item_task(tid);

    if (nullptr == _task) {
        return false;
    }

    pthread_mutex_lock(&_task->mutex);
    _task->state = E_TASK_STOP;
    pthread_mutex_unlock(&_task->mutex);
    return true;
}

bool TasksManage::task_continue(const pthread_t tid) noexcept
{
    auto _task = get_item_task(tid);

    if (nullptr == _task) {
        return false;
    }

    pthread_mutex_lock(&_task->mutex);
    _task->state = E_TASK_ALIVE;
    pthread_mutex_unlock(&_task->mutex);
    return true;
}

void TasksManage::task_wait(const pthread_t tid) noexcept
{
    auto _task = get_item_task(tid);

    if (nullptr == _task) {
        return ;
    }

    pthread_mutex_lock(&_task->mutex);

    while (E_TASK_ALIVE == _task->state) {
        pthread_cond_wait(&_task->cond, &_task->mutex);
    }

    pthread_mutex_unlock(&_task->mutex);
}

/**
 * @brief 更新任务记录时间点
 * 
 * @param tid 
 */
void TasksManage::task_alive(const pthread_t tid) noexcept
{
    auto _task = get_item_task(tid);

    if (nullptr == _task) {
        return ;
    }

    pthread_mutex_lock(&_task->mutex);
    _task->last_update_time = getostime();
    pthread_mutex_unlock(&_task->mutex);

    return ;
}

void TasksManage::task_pop(const pthread_t tid) noexcept
{
    pthread_mutex_lock(&task_pool->mutex);
    task_pool->tasks.erase(std::remove_if(task_pool->tasks.begin(), task_pool->tasks.end(), [tid](auto _task) -> bool {
        return _task->tid == tid;
    }), task_pool->tasks.end());
    pthread_mutex_unlock(&task_pool->mutex);
}

void TasksManage::task_clean(void) noexcept
{
    pthread_mutex_lock(&task_pool->mutex);
    task_pool->tasks.erase(std::remove_if(task_pool->tasks.begin(), task_pool->tasks.end(), [](auto _task) -> bool {
        if (E_TASK_DEAD == _task->state) {
            pthread_attr_destroy(&_task->attr);
            pthread_mutex_unlock(&_task->mutex);
            pthread_mutex_destroy(&_task->mutex);
            return true;
        }
        return false;
    }), task_pool->tasks.end());
    pthread_mutex_unlock(&task_pool->mutex);
}

/**
 * @brief 任务销毁
 * 
 * @param tid 
 * @return true 
 * @return false 
 */
void TasksManage::task_destroy(std::shared_ptr<struct task_record> &_task) noexcept
{
    if (pthread_cancel(_task->tid) < 0)
    {
        log_e("%s, can not kill thread=[%d]\n", strerror(errno), _task->tid);
    }

    _task->state = E_TASK_DEAD;
}

/**
 * @brief 任务重载
 * 
 * @param task_index 
 * @return true 
 * @return false 
 */
void TasksManage::task_reload(std::shared_ptr<struct task_record> &_task) noexcept
{
    struct task_record *task = new struct task_record(*_task);

    task_pool->task_destroy(_task);

    if (NULL != _task->clean)
    {
        _task->clean();
    }

    if (!task_pool->task_create(task)) {
        log_e("reload tid=[%ld]\n", task->tid);
    }

    delete task;
}

/**
 * @brief 任务超时处理函数
 * 
 * @param task_index 任务对应的任务池中的id
 * @return true 
 * @return false 
 */
void TasksManage::task_timeout_handler(std::shared_ptr<struct task_record> &_task) noexcept
{
    switch (_task->action)
    {
    case E_TASK_KILL:
        task_pool->task_destroy(_task);
        break;
    case E_TASK_REBOOT_SYSTEM:
        // TODO: 如何自启-->守护进程
        break;
    case E_TASK_IGNORE:
        break;
    case E_TASK_RELOAD:
        task_pool->task_reload(_task);
        break;
    default:
        break;
    }
}

/**
 * @brief 任务更新
 * 
 */
void TasksManage::task_update(void) noexcept
{
    pthread_mutex_lock(&task_pool->mutex);

    static uint32_t last_time = getostime();
    uint32_t now = getostime();

    // 时间向前跳变和时间向后跳变超过一分钟，重置任务时间
    if (now < last_time || (now - last_time) > OS_MIN(1))
    {
        for (auto item : task_pool->tasks)
        {
            item->last_update_time = now;
        }
        last_time = now;
        pthread_mutex_unlock(&task_pool->mutex);
        return;
    }

    for (auto item : task_pool->tasks)
    {
        now = getostime();
        // 检查任务超时
        pthread_mutex_lock(&item->mutex);
        if (E_TASK_ALIVE != item->state) {
            pthread_mutex_unlock(&item->mutex);
            continue;
        }

        // 非任务管理关闭的任务
        if (!task_pool->task_exist_if(item->tid)) {
            item->state = E_TASK_DEAD;
            pthread_mutex_unlock(&item->mutex);
            continue;
        }

        if ((uint32_t)abs(now - item->last_update_time) > (item->alive_time + TASK_TIME_ERROR_RANGE))
        {
            log_e("task tid=[%d], taskid=[%d], taskname=[%s]\n", item->tid, item->task_id, item->thread_name);
            if (item->timeout_times++ > MAX_TASK_TIMEOUT_TIMES)
            {
                // 超时处理
                task_timeout_handler(item);
            }
        }
        else
        {
            // 检查到一次未超时就重置超时次数
            item->timeout_times = 0;
        }
        pthread_mutex_unlock(&item->mutex);
    }

    // 清理死亡任务
    task_pool->task_clean();
    pthread_mutex_unlock(&mutex);
}

// 任务管理
static void *task_manage(void *name) noexcept
{
    TasksManage *task_pool = TasksManage::get_task_pool();
    pthread_t tid = pthread_self();

    task_set_name((const char *)name);

    log_i("tid=[%zu], name=[%s] run\n", tid, NULL == name ? __func__ : name);

    for (;;)
    {
        task_alive(tid); // 自身任务心跳
        task_pool->task_update(); // 更新所有任务状态

        ostime_delay(OS_MS(3));  // 3秒刷新一次
    }
}

void task_set_name(const char * name) noexcept
{
    char pname[MAX_THREAD_NAME_LEN + 1] = {'\0'};
    pthread_t tid = pthread_self();

    // pid_t tid = gettid(); // 这玩意儿不能用?

    if (NULL != name)
    {
        sprintf(pname, "%s", (char *)name);
        prctl(PR_SET_NAME, pname);
    }
    else
    {
        sprintf(pname, "p%zu", tid);
        prctl(PR_SET_NAME, pname);
    }
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
                 const uint16_t task_id, const uint32_t alive_time, enum task_deadlock action, thread_clean clean) noexcept
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

void task_alive(const pthread_t tid) noexcept
{
    TasksManage *task_pool = TasksManage::get_task_pool();

    task_pool->task_alive(tid);

    task_pool->task_wait(tid);
}

// 初始出化任务管理任务
void task_init(void) noexcept
{
    task_create(task_manage, STACKSIZE(800), "task_manage", TASK_MANAGE_ID, OS_MIN(5), E_TASK_REBOOT_SYSTEM);
}
} // namespace aipTaskMana