/**
 * @file sdk_midware_register.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#define LOG_TAG "SDK_MIDWARE_REGISTER"

#include <easylogger/inc/elog.h>
#include "sdk/sdk_network/sdk_midware.h"
#include "sdk_midware_register.h"

// 各个中间件的头文件引用
#include "../../module_fun/user_manage/user_manage.h"

/**
 * @brief 所有中间件的注册集中在这里
 * 
 */
void register_sdk_midwares(void)
{
	resgister_sdk_midware(user_manange_midware_do, true);
}
