/**
 * @file _sdk_tree.h
 * @author yuwangliang (astralrovers@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-02-15
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once

#include "sdk_tree.h"

SdkTreeTable &get_sdk_tree_root(void);

// 方法是否合法（包含关系）
bool method_valid(const OperationType &method, const int &methods);
// 请求方法是否支持
bool is_support_method(const OperationType &method);
// 校验url
bool is_url_valid(const std::string &url);