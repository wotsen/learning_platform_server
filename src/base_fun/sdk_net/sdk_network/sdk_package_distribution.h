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
#include <uv.h>
#include "../sdk_protocol/in_sdk.pb.h"

// 最大一包数据64k
#define MAX_SDK_MSG_LEN (64 * 1024)

struct sdk_data_buf
{
	size_t len;	///< 实际接受长度
    void *data;
};

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

/**
 * @brief sdk 网络接口
 * 
 */
struct sdk_net_interface {
	insider::sdk::IpVersion ip_version;				///< ip版本
	insider::sdk::TransProto trans_protocol;		///< 传输层
	std::string interface;							///< 本地网口
	std::string gateway;							///< 本地网关
	std::string src_ip;								///< 源地址
	uint32_t src_port;								///< 源端口
	std::string des_ip;								///< 目的地址
	uint32_t des_port;								///< 目的端口
};

// 消息入队
template <class T>
bool push_sdk_package(sdk_package<T> *package);

// 初始化sdk消息分发
void task_sdk_package_distribution_init(void);

// tcp消息入队
template <> bool push_sdk_package<uv_stream_t>(sdk_package<uv_stream_t> *package);
