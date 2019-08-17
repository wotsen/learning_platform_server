/*
 * @Date: 2019-08-17 11:45:23
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-17 11:50:09
 */
/**
 * @file usr_apps.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-08-17
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#define LOG_TAG "USR_APPS"

#include "usr_apps.h"
#include "../../module/easylogger/inc/elog.h"
#include "../../function/task_manage/task_manage.h"

using namespace aipTaskMana;

void usr_apps_init(void)
{
    task_init(); // 初始化任务队列

}
