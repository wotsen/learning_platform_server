/**
 * @file user_manage.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-09-22
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include <iostream>
#include <list>
#include <memory>
#include "sdk_net/sdk_request.h"

///< 用户管理中间件
bool user_manange_midware_do(wotsen::SdkRequest &sdk_request);

///< 初始化用户管理
void user_manager_init(void);

///< 反初始化用户管理
void user_manager_finit(void);

///< 用户管理模块状态
bool user_manager_state(void);
