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
#include <loguru.hpp>
#include <string.h>
#include <errno.h>
#include <sys/prctl.h>
#include <signal.h>
#include <sys/types.h>
#include <memory>
#include <algorithm>
#include "util_time/util_time.h"
#include "task_manage.h"

namespace wotsen
{

// 超时事件时间30s
const static uint32_t task_time_error_range = 30;
// 超时次数
const static uint8_t max_task_timeout_times = 3;

/**
 * @brief 当前任务状态
 * 
 */
enum task_state
{
    E_TASK_ALIVE,				///< 活跃
    E_TASK_TIMEOUT,				///< 超时
    E_TASK_STOP,				///< 停止
    E_TASK_DEAD,				///< 死亡
    E_TASK_OVERLOAD				///< 等待重载
};

/**
 * @brief 任务记录信息
 * 
 */
struct task_record
{
    pthread_t tid;                              ///< 线程号
    char thread_name[MAX_THREAD_NAME_LEN + 1];  ///< 线程名
    size_t stacksize;                           ///< 线程栈大小
    int priority;                               ///< 线程优先级

    uint32_t create_time;                       ///< 创建时间
    uint32_t alive_time;                        ///< 生存时间
    uint32_t last_update_time;                  ///< 上次更新时间
    uint8_t timeout_times;                      ///< 超时次数

    pthread_mutex_t mutex;                      ///< 线程锁
    pthread_cond_t cond;                        ///< 互斥条件
    enum task_state state;                      ///< 线程状态

    thread_func func;                           ///< 任务入口
    thread_clean clean;                         ///< 任务清理
    enum task_deadlock action;                  ///< 异常处理
};

class TasksManage;

/* 任务运行 */
void *task_run(TasksManage *tasks);

/**
 * @brief 任务管理
 * 
 */
class TasksManage
{
public:
    static uint32_t max_tasks_;                  			///< 最大任务数
	static abnormal_task_do except_fun_;						///< 异常任务外部处理接口

private:
    TasksManage() {};

    static TasksManage *task_pool_;                          ///< 任务管理句柄
    pthread_mutex_t mutex_;                                  ///< 任务锁

    std::vector<std::shared_ptr<struct task_record>> tasks_; ///< 任务池

public:
    ~TasksManage();
    // 获取任务管理句柄
    static TasksManage *get_task_pool(void) noexcept;
    // 创建任务
    bool task_create(const struct task_record &task) noexcept;
    // 任务激活(刷新生存时间)
    void task_alive(const pthread_t &tid) noexcept;
    // 任务暂停,TODO:未使用
    bool task_stop(const pthread_t &tid) noexcept;
    // 任务等待信号
    void task_wait(const pthread_t &tid) noexcept;
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
    // 标记异常任务
    bool tag_except_task(std::shared_ptr<struct task_record> &_task) noexcept;
    // 任务超时处理
    void task_timeout_handler(std::shared_ptr<struct task_record> &_task) noexcept;
    // 任务重新启动,FIXME:不支持，任务不一定能注销
    void task_reload(std::shared_ptr<struct task_record> &_task) noexcept;
    // 任务出栈
    void task_pop(const pthread_t &tid) noexcept;
    // 杀死任务(FIXME:只能尝试处理，失败也没有办法，这里的kill任务也只用于超时的任务，一般是死锁了)
    void task_kill(std::shared_ptr<struct task_record> &_task) noexcept;
    // 任务注销,FIXME:不支持，不一定能注销
    void task_destroy(std::shared_ptr<struct task_record> &_task) noexcept;

    /* 清理死亡任务：包含手动注销、自动注销，异常退出,NOTE:仅仅尝试注销任务 */
    void task_clean(void) noexcept;
    /* 重载超时任务 */
    void task_overload(void) noexcept;
};

/* 任务管理主线程 */
static void *task_manage(void *name) noexcept;

// 任务管理实例
TasksManage *TasksManage::task_pool_ = nullptr;

uint32_t TasksManage::max_tasks_ = 128;
abnormal_task_do TasksManage::except_fun_ = NULL;

/**
 * @brief 获取任务管理句柄
 * 
 * @return TasksManage* 
 */
TasksManage *TasksManage::get_task_pool(void) noexcept
{
    if (nullptr != task_pool_)
    {
        return task_pool_;
    }

    task_pool_ = new TasksManage();
    pthread_mutex_init(&task_pool_->mutex_, NULL);

    return task_pool_;
}

/**
 * @brief Destroy the Tasks Manage:: Tasks Manage object
 * 
 */
TasksManage::~TasksManage()
{
    for (auto item : task_pool_->tasks_)
    {
        // 释放所有线程任务
        if (pthread_cancel(item->tid) < 0)
        {
            LOG_F(ERROR, "%s, can not kill thread=[%d]\n", strerror(errno), item->tid);
        }

        pthread_mutex_unlock(&item->mutex);
        pthread_mutex_destroy(&item->mutex);
        pthread_cond_destroy(&item->cond);
    }

    // 清除任务池
    task_pool_->tasks_.clear();

    pthread_mutex_unlock(&task_pool_->mutex_);
    pthread_mutex_destroy(&task_pool_->mutex_);
}

/**
 * @brief 创建任务
 * 
 * @param task 任务信息
 * @return true 成功
 * @return false 失败
 */
bool TasksManage::task_create(const struct task_record &task) noexcept
{
    std::shared_ptr<struct task_record> _task(new struct task_record);

    if (NULL == task.func || NULL == task.thread_name)
    {
        LOG_F(ERROR, "can not create thread, parameter error!\n");
        return false;
    }

    if (task.stacksize < PTHREAD_STACK_MIN)
    {
        LOG_F(ERROR, "stacksize too little : %zu, need over %zu\n", task.stacksize, PTHREAD_STACK_MIN);
        return false;
    }

    pthread_mutex_lock(&task_pool_->mutex_);

    // 任务队列已满
    if (task_pool_->tasks_.size() > TasksManage::max_tasks_)
    {
        LOG_F(ERROR, "task_pool full!\n");
        goto err_return;
    }

    memcpy(&*_task, &task, sizeof(struct task_record));

    // 线程锁
    pthread_mutex_init(&_task->mutex, NULL);
    pthread_cond_init(&_task->cond, NULL);

    if (!create_thread(&_task->tid, task.stacksize, task.priority, (thread_func)task_run, this))
    {
        LOG_F(ERROR, "%s\n", strerror(errno));
        goto err_return;
    }

    _task->create_time = getostime();
    _task->last_update_time = _task->create_time;
    _task->timeout_times = 0;
    _task->state = E_TASK_ALIVE;

    LOG_F(INFO, "create task tid=[%zu], name=[%s]\n", _task->tid, _task->thread_name);

    // 任务入队
    task_pool_->tasks_.push_back(std::move(_task));
    pthread_mutex_unlock(&task_pool_->mutex_);

    return true;

err_return:
    if (_task)
    {
        pthread_mutex_destroy(&_task->mutex);
        pthread_cond_destroy(&_task->cond);
    }
    pthread_mutex_unlock(&task_pool_->mutex_);
    return false;
}

/**
 * @brief 任务是否存活
 * 
 * @param tid 
 * @return true 
 * @return false 
 */
bool TasksManage::task_exist_if(const pthread_t &tid) noexcept
{
    return if_thread_exsit(tid);
}

/**
 * @brief 获取目标任务
 * 
 * @param tid 线程id
 * @return auto 目标任务
 */
auto TasksManage::get_item_task(const pthread_t &tid) noexcept
{
    pthread_mutex_lock(&task_pool_->mutex_);

    for (auto item : task_pool_->tasks_)
    {
        if (tid == item->tid)
        {
            pthread_mutex_unlock(&task_pool_->mutex_);
            return item;
        }
    }

    pthread_mutex_unlock(&task_pool_->mutex_);

    return static_cast<std::shared_ptr<struct task_record>>(nullptr);
}

/**
 * @brief 任务暂停
 * 
 * @param tid 任务id
 * @return true 
 * @return false 
 */
bool TasksManage::task_stop(const pthread_t &tid) noexcept
{
    auto _task = get_item_task(tid);

    if (nullptr == _task)
    {
        return false;
    }

    pthread_mutex_lock(&_task->mutex);
    _task->state = E_TASK_STOP;
    pthread_mutex_unlock(&_task->mutex);

    return true;
}

/**
 * @brief 继续任务
 * 
 * @param tid 任务id
 * @return true 
 * @return false 
 */
bool TasksManage::task_continue(const pthread_t &tid) noexcept
{
    auto _task = get_item_task(tid);

    if (nullptr == _task)
    {
        return false;
    }

    pthread_mutex_lock(&_task->mutex);
    _task->state = E_TASK_ALIVE;
    pthread_cond_signal(&_task->cond);
    pthread_mutex_unlock(&_task->mutex);

    return true;
}

/**
 * @brief 等待任务
 * 
 * @param tid 
 */
void TasksManage::task_wait(const pthread_t &tid) noexcept
{
    auto _task = get_item_task(tid);

    if (nullptr == _task)
    {
        return;
    }

    pthread_mutex_lock(&_task->mutex);

    // 任务暂停就等待信号量
    while (E_TASK_STOP == _task->state)
    {
        pthread_cond_wait(&_task->cond, &_task->mutex);
    }

    pthread_mutex_unlock(&_task->mutex);
}

/**
 * @brief 更新任务记录时间点
 * 
 * @param tid 
 */
void TasksManage::task_alive(const pthread_t &tid) noexcept
{
    auto _task = get_item_task(tid);

    if (nullptr == _task)
    {
        return;
    }

    pthread_mutex_lock(&_task->mutex);
    _task->last_update_time = getostime();
    pthread_mutex_unlock(&_task->mutex);

    return;
}

/**
 * @brief 任务出栈
 * 
 * @param tid 
 */
void TasksManage::task_pop(const pthread_t &tid) noexcept
{
    pthread_mutex_lock(&task_pool_->mutex_);
    task_pool_->tasks_.erase(std::remove_if(task_pool_->tasks_.begin(), task_pool_->tasks_.end(), [tid](auto &_task) -> bool {
                               return _task->tid == tid;
                           }),
                           task_pool_->tasks_.end());
    pthread_mutex_unlock(&task_pool_->mutex_);
}

/**
 * @brief 任务清理
 * 
 */
void TasksManage::task_clean(void) noexcept
{
    pthread_mutex_lock(&task_pool_->mutex_);
    task_pool_->tasks_.erase(std::remove_if(task_pool_->tasks_.begin(),
                                          task_pool_->tasks_.end(),
                                          [](auto &_task) -> bool {
                                              if (E_TASK_DEAD == _task->state)
                                              {
                                                  // 死亡任务先尝试关闭
                                                  if (!release_thread(_task->tid))
                                                  {
                                                      LOG_F(ERROR, "%s, can not kill thread=[%d]\n", strerror(errno), _task->tid);
                                                  }
                                                  pthread_mutex_unlock(&_task->mutex);
                                                  pthread_mutex_destroy(&_task->mutex);
                                                  pthread_cond_destroy(&_task->cond);
                                                  return true;
                                              }
                                              return false;
                                          }),
                           task_pool_->tasks_.end());
    pthread_mutex_unlock(&task_pool_->mutex_);
}

/**
 * @brief 任务重载
 * 
 */
void TasksManage::task_overload(void) noexcept
{
    pthread_mutex_lock(&task_pool_->mutex_);

    std::vector<std::shared_ptr<struct task_record>> old_tasks;

    task_pool_->tasks_.erase(std::remove_if(task_pool_->tasks_.begin(),
                                          task_pool_->tasks_.end(),
                                          [&old_tasks](auto _task) -> bool {
                                              if (E_TASK_OVERLOAD == _task->state)
                                              {
                                                  old_tasks.push_back(_task);
                                                  return true;
                                              }
                                              return false;
                                          }),
                           task_pool_->tasks_.end());

    pthread_mutex_unlock(&task_pool_->mutex_);

    for (auto item : old_tasks)
    {
        pthread_mutex_unlock(&item->mutex);
        pthread_mutex_destroy(&item->mutex);
        pthread_cond_destroy(&item->cond);
        task_pool_->task_create(*item);
    }

    old_tasks.clear();
}

/**
 * @brief 任务关闭
 * 
 * @param _task 
 */
void TasksManage::task_kill(std::shared_ptr<struct task_record> &_task) noexcept
{
    if (!release_thread(_task->tid))
    {
        LOG_F(ERROR, "%s, can not kill thread=[%d]\n", strerror(errno), _task->tid);
    }
    if (NULL != _task->clean)
    {
        _task->clean();
    }
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
    task_pool_->task_kill(_task);
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
    task_pool_->task_kill(_task);
    _task->state = E_TASK_OVERLOAD;
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
    case E_TASK_REBOOT_SYSTEM:
        // TODO: 如何自启-->守护进程
        break;

    case E_TASK_IGNORE:
        break;

    case E_TASK_DEFAULT:
    default:
        // 其他的清楚掉，不再管理
        pthread_mutex_lock(&_task->mutex);
        // 先执行清理接口
        _task->clean ? _task->clean() : (void)0;
        // 标记超时
        _task->state = E_TASK_TIMEOUT;
        pthread_mutex_unlock(&_task->mutex);

        break;
    }
}

/**
 * @brief 任务事件矫正
 * 
 */
void TasksManage::task_correction_time(void) noexcept
{
    static uint32_t last_time = getostime();
    uint32_t now = getostime();

    pthread_mutex_lock(&task_pool_->mutex_);

    // 时间向前跳变和时间向后跳变超过一分钟，重置任务时间
    if (now < last_time || (now - last_time) > OS_MIN(1))
    {
        for (auto item : task_pool_->tasks_)
        {
            pthread_mutex_lock(&item->mutex);
            item->last_update_time = now;
            pthread_mutex_unlock(&item->mutex);
        }
        last_time = now;
    }

    pthread_mutex_unlock(&task_pool_->mutex_);
}

/**
 * @brief 标记异常任务
 * 
 * @param _task 任务
 * @return true 异常任务标记
 * @return false 任务正常
 */
bool TasksManage::tag_except_task(std::shared_ptr<struct task_record> &_task) noexcept
{
    pthread_mutex_lock(&_task->mutex);

    if (E_TASK_TIMEOUT == _task->state                      // 任务已经超时，则标记死亡
        || !task_pool_->task_exist_if(_task->tid))          // 任务不存在
    {
        _task->state = E_TASK_DEAD;

        pthread_mutex_unlock(&_task->mutex);

        return true;
    }

    // 检查任务超时
    if (E_TASK_ALIVE != _task->state)
    {
        pthread_mutex_unlock(&_task->mutex);

        return true;
    }

    pthread_mutex_unlock(&_task->mutex);

    return false;
}

/**
 * @brief 任务超时处理
 * 
 */
void TasksManage::task_check_timeout(void) noexcept
{
    pthread_mutex_lock(&task_pool_->mutex_);

    for (auto item : task_pool_->tasks_)
    {
        uint32_t now = getostime();

        // 存在异常任务标记则跳过
        if (tag_except_task(item))
        {
            continue;
        }

        pthread_mutex_lock(&item->mutex);

        // 任务超时
        if ((uint32_t)abs(now - item->last_update_time) > (item->alive_time + task_time_error_range))
        {
            LOG_F(ERROR, "task tid=[%d], taskname=[%s]\n", item->tid, item->thread_name);

            if (item->timeout_times++ > max_task_timeout_times)
            {
				LOG_F(ERROR, "task tid=[%d], taskname=[%s] : timeout with [%x]\n",
						item->tid, item->thread_name, item->action);

                // 超时处理
                task_timeout_handler(item);

                struct except_task_info err_info = {
                    item->tid,
                    item->thread_name,
                    "task timeout",
                };

                // 错误信息给到外部
				TasksManage::except_fun_ ? TasksManage::except_fun_(err_info) : (void)0;
            }
        }
        else
        {
            // 检查到一次未超时就重置超时次数
            item->timeout_times = 0;
        }
        pthread_mutex_unlock(&item->mutex);
    }

    pthread_mutex_unlock(&mutex_);
}

/**
 * @brief 任务更新
 * 
 */
void TasksManage::task_update(void) noexcept
{
    // 系统时间异常矫正
    task_pool_->task_correction_time();
    // 检查时间超时
    task_pool_->task_check_timeout();
    // 清理死亡任务
    task_pool_->task_clean();
    // 重载超时任务
    task_pool_->task_overload();
}

/**
 * @brief 任务管理
 * 
 * @param name 任务
 * @return void* 
 */
static void *task_manage(void *name) noexcept
{
    TasksManage *task_pool = TasksManage::get_task_pool();
    pthread_t tid = pthread_self();

    LOG_F(INFO, "任务管理初始化完成...\n");

    for (;;)
    {
        task_alive(tid);          // 自身任务心跳
        task_pool->task_update(); // 更新所有任务状态

        ostime_delay(OS_SEC(3)); // 3秒刷新一次
    }
}

/**
 * @brief 创建任务
 * 
 * @param func 任务主函数
 * @param stacksize 栈大小
 * @param thread_name 线程名
 * @param alive_time 超时时间
 * @param action 超时处理
 * @param clean 任务结束处理函数
 * @return true 创建成功
 * @return false 创建失败
 */
bool task_create(thread_func func, const size_t stacksize, const char *thread_name,
                 const uint32_t alive_time, const enum task_deadlock action,
                 thread_clean clean, const int priority) noexcept
{
    TasksManage *task_pool = TasksManage::get_task_pool();
    struct task_record task;

    memset(&task, 0, sizeof(struct task_record));

    task.func = func;
    task.stacksize = stacksize;
    task.priority = priority;
    snprintf(task.thread_name, MAX_THREAD_NAME_LEN + 1, "%s", thread_name);
    task.alive_time = alive_time;
    task.action = action;
    task.clean = clean;

    return task_pool->task_create(task);
}

void task_alive(const pthread_t &tid) noexcept
{
    TasksManage *task_pool = TasksManage::get_task_pool();

    task_pool->task_alive(tid);

    task_pool->task_wait(tid);
}

/**
 * @brief 任务运行
 *
 * @param tasks : 任务管理器
 *
 * @return : none
 */
void *task_run(TasksManage *tasks)
{
    pthread_t tid = pthread_self();

    // 等线程加入任务池
    pthread_mutex_lock(&tasks->mutex_);
    pthread_mutex_unlock(&tasks->mutex_);

    auto _task = tasks->get_item_task(tid);

    if (nullptr == _task)
    {
        LOG_F(ERROR, "not find task info = [%zu], can not run task!\n", tid);
        return (void *)0;
    }

    set_thread_name((const char *)_task->thread_name);

    _task->func((void *)_task->thread_name);

    return (void *)0;
}

// 初始出化任务管理任务
void task_manage_init(const uint32_t max_tasks, abnormal_task_do except_fun) noexcept
{
	TasksManage::max_tasks_ = max_tasks;
	TasksManage::except_fun_ = except_fun;
    task_create(task_manage, STACKSIZE(800), "task_manage", OS_MIN(5), E_TASK_REBOOT_SYSTEM);
}

} // namespace wotsen
