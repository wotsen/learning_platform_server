/**
 * @file uv_event.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-09-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include <iostream>
#include <uv.h>

class UvEvent {
private:
	UvEvent() : loop(nullptr) { }
public:
	// 初始化uv事件
	void uv_event_init(void);
	// 创建实例
	static UvEvent *get_uv_event(void);
	// 获取uv loop
	uv_loop_t *get_uv_loop(void);
	// 析构
	~UvEvent();
	// 开启uv
	void uv_event_run(void);
private:
	uv_loop_t *loop;
	uv_idle_t idler;

	static UvEvent *uv_event;
};

void wait_for_a_while(uv_idle_t *handle);

void task_uv_event_init(void);