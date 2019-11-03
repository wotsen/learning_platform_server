/**
 * @file sdk_network.h
 * @author 余王亮 (astralrovers@outlook.com)
 * @brief 系统通用网络接口，非三方协议
 * @version 0.1
 * @date 2019-09-13
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include <iostream>
#include <list>
#include <memory>
#include "../../uv_event/uv_event.h"

// 数据接收，通用网络协议使用统一数据接口，类似中间件
// 协议解析
// 内容分发
// 回复回调

/**
 * @brief sdk网络服务器
 * 
 */
class UvSdkNetServer
{
public:
	/**
	 * @brief sdk网络类型
	 * 
	 * @tparam T uv类型
	 */
	template <class T>
	struct UvSdkNetSrvType
	{
		T handle;
		struct sockaddr_in addr;
	};

	UvSdkNetServer(uv_loop_t *loop) : loop(loop) {}

	// 创建tcp服务器
	bool create_tcp_server(const std::string &ipv4, const int &port);
	bool create_tcp_server(const char *ipv4, const int &port);

	~UvSdkNetServer();

private:
	uv_loop_t *loop;

	std::list<std::shared_ptr<UvSdkNetSrvType<uv_tcp_t>>> tcp_servers;
	std::list<std::shared_ptr<UvSdkNetSrvType<uv_udp_t>>> udp_servers;
	std::list<uv_timer_t *> timers;
};

void sdk_uv_net_init(void);
