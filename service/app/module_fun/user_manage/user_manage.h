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
#include "../../base_fun/net_manage/sdk/sdk_network/sdk_midware.h"

///< 用户管理中间件
bool user_manange_midware_do(const enum sdk_net_data_type type, void *req, insider::sdk::Sdk &sdk_req, insider::sdk::Sdk &sdk_res);

///< 初始化用户管理
void user_manager_init(void);