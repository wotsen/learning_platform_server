/**
 * @file sdk_package_distribution.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-09-21
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include <string>
#include <inttypes.h>
#include <uv.h>
#include "../sdk_protocol/in_sdk.pb.h"

/**
 * @brief sdk处理数据包
 * 
 * @tparam T 
 */
template <class T>
struct sdk_package
{
	T *handle;			///< uv handle
	bool (*write)(T *handle, const std::string &data);	///< 响应回调接口
};

// 消息入队
template <class T>
bool push_sdk_package(sdk_package<T> *package);

// 初始化sdk消息分发
void task_sdk_package_distribution_init(void);

// tcp消息入队
template <> bool push_sdk_package<uv_stream_t>(sdk_package<uv_stream_t> *package);
