/**
 * @file uv_event.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-09-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include <easylogger/inc/elog.h>
#include "util_time/util_time.h"
#include "../../task_manage/task_manage.h"
#include "uv_event.h"

using namespace wotsen;

// libuv实例
UvEvent *UvEvent::uv_event = nullptr;

/**
 * @brief uv初始化
 * 
 */
void UvEvent::uv_event_init(void)
{
	loop = uv_default_loop();
}

/**
 * @brief 获取uv实例
 * 
 * @return UvEvent* 
 */
UvEvent *UvEvent::get_uv_event(void)
{
	if (uv_event)
	{
		return uv_event;
	}

	uv_event = new UvEvent();

	uv_event->uv_event_init();

	return uv_event;
}

/**
 * @brief 获取uv loop
 * 
 * @return uv_loop_t* 
 */
uv_loop_t *UvEvent::get_uv_loop(void)
{
	return loop;
}

/**
 * @brief Destroy the Uv Event:: Uv Event object
 * 
 */
UvEvent::~UvEvent()
{
	if (loop)
	{
		uv_loop_close(loop);
		free(loop);
		loop = nullptr;
	}
}

/**
 * @brief 开启libuv
 * 
 */
void UvEvent::uv_event_run(void)
{
	log_d("run loop\n");
	uv_run(loop, UV_RUN_DEFAULT);
	log_d("run error\n");
}

/**
 * @brief 定时刷新任务心跳
 * 
 * @param handle 
 */
static void alive_uv_event(uv_timer_t *handle)
{
	task_alive(*(pthread_t *)(handle->data));
}

/**
 * @brief libuv任务
 * 
 * @param name 任务名称
 * @return void* 
 */
static void *task_uv_event(void *name)
{
	pthread_t tid = pthread_self();
	UvEvent *event = UvEvent::get_uv_event();

	// uv定时器，用于维护libuv事件队列
	uv_timer_t timer_req;

	uv_timer_init(event->get_uv_loop(), &timer_req);

	timer_req.data = (void *)&tid;
	uv_timer_start(&timer_req, alive_uv_event, 0, 2000);

	log_i("uv-event任务初始化完成...\n");

	event->uv_event_run();
	log_i("................\n");

	for (;;)
	{
		task_alive(tid); // 自身任务心跳

		ostime_delay(OS_SEC(3)); // 3秒刷新一次
	}
	return (void *)0;
}

/**
 * @brief 初始化libuv任务
 * 
 */
void task_uv_event_init(void)
{
	task_create(task_uv_event, STACKSIZE(10 * 1024), "task_uv_event", OS_MIN(30), E_TASK_REBOOT_SYSTEM);
}
