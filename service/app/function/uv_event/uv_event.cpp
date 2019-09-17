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
#include "../task_manage/task_manage.h"
#include "../../tools/timer/timer.h"
#include "uv_event.h"

using namespace wotsen;

UvEvent *UvEvent::uv_event = nullptr;

void wait_for_a_while(uv_idle_t* handle) {
	if (handle) {}
	log_i("idle............................\n");
}

void UvEvent::uv_event_init(void)
{
	loop = uv_default_loop();
}

UvEvent *UvEvent::get_uv_event(void)
{
	if (uv_event)
	{
		return uv_event;
	}

	uv_event = new UvEvent();

	uv_event->uv_event_init();
	// uv_idle_init(uv_event->loop, &uv_event->idler);
	// uv_idle_start(&uv_event->idler, wait_for_a_while);

	return uv_event;
}

uv_loop_t *UvEvent::get_uv_loop(void)
{
	return loop;
}

UvEvent::~UvEvent()
{
	if (loop)
	{
		// uv_idle_stop(&idler);
		uv_loop_close(loop);
		free(loop);
		loop = nullptr;
	}
}

void UvEvent::uv_event_run(void)
{
	log_d("run loop\n");
	uv_run(loop, UV_RUN_DEFAULT);
	log_d("run error\n");
}

static void alive_uv_event(uv_timer_t *handle) {
    task_alive(*(pthread_t *)(handle->data));          // 自身任务心跳
	log_d("......................\n");
}


static void *task_uv_event(void *name) {
    pthread_t tid = pthread_self();
	UvEvent *event = UvEvent::get_uv_event();

	uv_timer_t timer_req;

	uv_timer_init(event->get_uv_loop(), &timer_req);

	timer_req.data = (void *)&tid;
	uv_timer_start(&timer_req, alive_uv_event, 0, 2000);

    log_i("uv-event任务初始化完成...\n");

	event->uv_event_run();
	log_i("................\n");

	// FIXME:资源回收

    for (;;)
    {
        task_alive(tid);          // 自身任务心跳

        ostime_delay(OS_MS(3)); // 3秒刷新一次
    }
	return (void *)0;
}

void task_uv_event_init(void)
{
    task_create(task_uv_event, STACKSIZE(10*1024), "task_uv_event", 0, OS_MIN(30), E_TASK_REBOOT_SYSTEM);
}