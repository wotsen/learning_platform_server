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

#include <uv.h>

template <class T>
struct sdk_package
{
	T *handle;
	uv_buf_t buf;
	bool (*write)(T *handle, const uv_buf_t *bufs);
};

template <class T>
bool push_sdk_package(sdk_package<T> *package);

void task_sdk_package_distribution_init(void);

template <> bool push_sdk_package<uv_stream_t>(sdk_package<uv_stream_t> *package);