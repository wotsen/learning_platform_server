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
#include "../uv_event/uv_event.h"

// 数据接收，通用网络协议使用统一数据接口，类似中间件
// 协议解析
// 内容分发
// 回复回调

#define DEFAULT_BACKLOG 128 // 连接队列

// sdk网络服务器
class UvSdkNetServer {
public:
	template <class T>
	struct UvSdkNetSrvType {
		T handle;
		struct sockaddr_in addr;
	};
	typedef struct
	{
		uv_write_t req;
		uv_buf_t buf;
	} write_req_t;
	UvSdkNetServer(uv_loop_t *loop) : loop(loop) {}

	// 创建tcp服务器
	bool create_tcp_server(const std::string &ipv4, const int &port);
	bool create_tcp_server(const char *ipv4, const int &port);

	// void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
	// void echo_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);
	// void on_new_connection(uv_stream_t *server, int status);

	~UvSdkNetServer();

private:
	uv_loop_t *loop;

	std::list<std::shared_ptr<UvSdkNetSrvType<uv_tcp_t>>> tcp_servers;
	std::list<std::shared_ptr<UvSdkNetSrvType<uv_udp_t>>> udp_servers;
	std::list<uv_timer_t *> timers;
};

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
void echo_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);
void on_new_connection(uv_stream_t *server, int status);

void sdk_uv_net_init(void);
