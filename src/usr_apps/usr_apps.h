/**
 * @file usr_apps.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 管理所有的应用功能模块都在这里进行管理，包括自己实现的与第三方的
 * @version 0.1
 * @date 2019-08-17
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include <inttypes.h>
#include <vector>
#include <string>
#include "sdk_net/sdk_protocol/in_sdk_body_appmodule.pb.h"

// 获取模块信息
void app_modules_current_status(insider::sdk::AppModuleCoutInfoList &moduel_list) noexcept;

// 初始化单个模块
void init_single_app_module(const uint32_t id, const std::string &name) noexcept;

// 反初始化单个模块
void finit_single_app_module(const uint32_t id, const std::string &name) noexcept;

// 用户应用初始化
void usr_apps_init(void);
