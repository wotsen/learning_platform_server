/**
 * @file sdk_package_distribution.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-09-21
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#define LOG_TAG "SDK_PACKAGE_DISTRIBUTION"

#include <iostream>
#include <list>
#include <cstring>
#include <easylogger/easylogger_setup.h>
#include "util_time/util_time.h"
#include "sys_config.h"
#include "task_manage/task_manage.h"
#include "sdk_protocol_do.h"
#include "sdk_package_distribution.h"

using namespace wotsen;

// tcp消息处理接口
static void *task_sdk_stream_do(void *name) noexcept;
// TCP消息队列最大消息数量
static const int MAX_STREAM_LIST_LEN = 1024;
// tcp消息队列
static std::list<sdk_package<uv_stream_t>*> stream_list;

// 获取uv tcp网络接口
static void _get_uv_tcp_interface(uv_stream_t *handle, struct sdk_net_interface &interface);
// 处理sdk tcp消息
static void sdk_stream_do(sdk_package<uv_stream_t> *package);

// tcp消息处理任务
static void *task_sdk_stream_do(void *name) noexcept;

/**
 * @brief tcp消息入队
 * 
 * @tparam  libuv数据类型
 * @param package sdk数据包
 * @return true 入队成功
 * @return false 入队失败
 */
template <> bool push_sdk_package<uv_stream_t>(sdk_package<uv_stream_t> *package)
{
	if (!package)
	{
		log_w("package is null\n");
		return false;
	}

	// 队列已满
	if (stream_list.size() >= MAX_STREAM_LIST_LEN)
	{
		// FIXME:入队失败如何处理，入队失败直接进行处理,积累过多消息会导致资源不足
		log_w("stream list full!\n");
		return false;
	}

	stream_list.push_back(package);

	return true;
}

// 获取tcp地址信息
static void _get_uv_tcp_interface(uv_stream_t *handle, struct sdk_net_interface &interface)
{
	std::string ip_version;
	std::string ip_version_cap;

	struct sockaddr_in addr;
	int namelen = sizeof(struct sockaddr);

	if (!handle)
	{
		log_i("handle is null\n");
		return ;
	}

    // 本地信息从配置获取
	get_sdk_tcp_host_config(ip_version, interface.des_ip, (int &)interface.des_port);
	get_net_interface_config(interface.interface);
	get_net_gateway_config(interface.gateway);

	interface.trans_protocol = insider::sdk::TransProto::TCP;

	if (ip_version == "ipv4")
	{
		interface.ip_version = insider::sdk::IpVersion::IPV4;
	}
	else
	{
		interface.ip_version = insider::sdk::IpVersion::IPV6;
	}

	uv_tcp_getsockname((uv_tcp_t *)handle, (struct sockaddr *)&addr, &namelen);

	// 源地址
	interface.src_ip = inet_ntoa(addr.sin_addr);
	interface.src_port = ntohs(addr.sin_port);

	log_i("client ip = %s, port = %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

	return;
}

// 处理sdk tcp消息
static void sdk_stream_do(sdk_package<uv_stream_t> *package)
{
	std::string req_data(((uv_buf_t *)package->handle->data)->base);
	std::string res_data;
	struct sdk_net_interface interface;

	// 获取uv 网络接口信息
	_get_uv_tcp_interface(package->handle, interface);

	// 协议处理
	sdk_protocol_do(interface, req_data, res_data);

	// 发送响应消息
	// FIXME:需要确认libuv发送数据是否同步发送，如是异步发送,res_data只是栈空间变量，会被销毁
	package->write(package->handle, res_data);

	return;
}

/**
 * @brief tcp消息处理任务
 * 
 * @param name 任务名称
 * @return void* 
 */
static void *task_sdk_stream_do(void *name) noexcept
{
    log_i("sdk stream 数据处理初始化完成...\n");
    pthread_t tid = pthread_self();

	for (;;)
	{
		task_alive(tid);

		if (stream_list.empty()) { continue; }

		// 获取队列首部消息
		sdk_package<uv_stream_t> *package = stream_list.front();

		// 处理sdk tcp消息
		sdk_stream_do(package);

		// 消息出队
		stream_list.pop_back();
		delete package;
		package = nullptr;
    }
}

/**
 * @brief 初始化sdk消息分发
 * 
 */
void task_sdk_package_distribution_init(void)
{
    task_create(task_sdk_stream_do, STACKSIZE(50*1024), "task_sdk_stream_do", OS_MIN(5), E_TASK_IGNORE);
}
