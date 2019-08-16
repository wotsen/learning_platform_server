/*
 * @Date: 2019-08-15 23:28:17
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-15 23:31:28
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

void task_create(void);
void task_alive(void);

void rebuild_task(void);

void kill_task(void);
void task_update(void);

void task_init(void);

#endif // !1__TASK_MANAGE_H__