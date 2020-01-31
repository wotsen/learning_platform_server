/**
 * @file sys_parameter.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 配置参数
 * @version 0.1
 * @date 2019-08-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include <string>
#include <nlohmann/json.hpp>

///< 初始化系统配置参数
bool sys_parameter_init(void);

///< 获取配置参数json指针
nlohmann::json &get_json_param(void);

/******************************************参数配置接口*******************************************************/