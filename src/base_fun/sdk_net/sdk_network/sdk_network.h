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
#include "handy_loop/handy_loop.h"

using namespace handy;

class SdkServer {
public:
	SdkServer(const uint32_t _max_links = 128);

	// 创建tcp服务
	bool create_tcp_srv(void);
	// 创建udp服务
	bool create_udp_srv(void);
	// tcp连接处理
	bool tcp_connection_made(void);
	// udp数据接收处理
	bool udp_data_received(void);
	// tcp连接是否达到最大
	bool is_tcp_links_max(void);
	// 增加tcp连接
	bool tcp_links_inc(void);
	// 减少tcp连接
	void tcp_links_dec(void);

private:
	// 最大连接数量
	const uint32_t max_links;
	// tcp连接数量，用于限制连接数
	uint32_t tcp_links;

private:
	// tcp服务器
	TcpServerPtr tcp_srv_;
	// udp服务器
	UdpServerPtr udp_srv_;
};

void sdk_net_init(void);
