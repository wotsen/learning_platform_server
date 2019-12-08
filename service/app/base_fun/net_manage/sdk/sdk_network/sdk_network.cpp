/**
 * @file network.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-09-14
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#define LOG_TAG "NETWORK"

#include <iostream>
#include <easylogger/easylogger_setup.h>
#include "../../../../sys_ctrl/config/sys_config.h"
#include "../../../../tools/timer/timer.h"
#include "sdk_package_distribution.h"
#include "sdk_network.h"

#define DEFAULT_BACKLOG 128 // 连接队列

/**
 * @brief libuv应答
 * 
 */
typedef struct
{
	uv_write_t req;		///< 响应
	uv_buf_t buf;		///< 数据
} write_req_t;

// uv服务器
static UvSdkNetServer *uv_sdk_net_server = nullptr;

// 内存分配
static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);

// 读取tcp消息
static void read_stream_msg(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);

// 发送tcp消息回调接口
static void on_stream_write(uv_write_t *req, int status);

// tcp连接回调
static void on_tcp_connection(uv_stream_t *server, int status);

// tcp客户端断开连接
static void on_tcp_shutdown(uv_shutdown_t *req, int status);

// 关闭服务
static void on_close(uv_handle_t *peer);

// 发送tcp消息
static bool uv_stream_write(uv_stream_t *handle, const uv_buf_t *buf);

/**
 * @brief 创建tcp服务器
 * 
 * @param ipv4 ipv4地址
 * @param port 端口号
 * @return true 成功
 * @return false 失败
 */
bool UvSdkNetServer::create_tcp_server(const char *ipv4, const int &port)
{
	std::shared_ptr<UvSdkNetSrvType<uv_tcp_t>> server(new UvSdkNetSrvType<uv_tcp_t>);

	uv_tcp_init(loop, &server->handle);
	uv_ip4_addr(ipv4, port, &server->addr);

	uv_tcp_bind(&server->handle, (const struct sockaddr *)&server->addr, 0);

	int r = uv_listen((uv_stream_t *)&server->handle, DEFAULT_BACKLOG, on_tcp_connection);
	if (r)
	{
		log_e("Listen error %s\n", uv_strerror(r));
		return false;
	}

	tcp_servers.push_back(std::move(server));

	return true;
}

/**
 * @brief 创建tcp服务器
 * 
 * @param ipv4 ipv4地址
 * @param port 端口号
 * @return true 成功
 * @return false 失败
 */
bool UvSdkNetServer::create_tcp_server(const std::string &ipv4, const int &port)
{
	return create_tcp_server(ipv4.c_str(), port);
}

/**
 * @brief Destroy the Uv Sdk Net Server:: Uv Sdk Net Server object
 * 
 */
UvSdkNetServer::~UvSdkNetServer()
{
	for (auto &tcp_s : tcp_servers)
	{
		uv_close((uv_handle_t *)&tcp_s->handle, NULL);
	}
	tcp_servers.clear();
}

/**
 * @brief 发送tcp消息回调接口
 * 
 * @param req uv句柄
 * @param status 发送状态码
 */
static void on_stream_write(uv_write_t *req, int status)
{
	write_req_t *wr = (write_req_t *)req;
	uv_buf_t *recv = (uv_buf_t *)(wr->req.handle->data);

	if (status)
	{
		fprintf(stderr, "Write error %s\n", uv_strerror(status));
	}

	// 释放分配的内存
	delete recv->base;		///< 接受消息区
	delete recv;			///< uv_buf
	delete wr;				///< 句柄
}

/**
 * @brief 发送tcp消息
 * 
 * @param handle 回调接口间数据句柄
 * @param buf 数据部分
 * @return true 成功
 * @return false 失败
 */
static bool uv_stream_write(uv_stream_t *handle, const uv_buf_t *buf)
{
	write_req_t *wr = nullptr;

	wr = new write_req_t;
	wr->buf = uv_buf_init(buf->base, buf->len);

	if (0 != uv_write((uv_write_t *)wr, handle, &wr->buf, 1, on_stream_write))
	{
		log_e("uv_write failed\n");
		wr->req.handle = handle;
		// 发送失败也需要释放内存
		on_stream_write((uv_write_t *)wr, 0);
		return false;
	}

	return true;
}

/**
 * @brief 内存分配
 * 
 * @param handle 
 * @param suggested_size 预分配数据长度
 * @param buf 消息缓冲区
 */
static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
	// FIXME：每次都分配内存是否造成内存碎片，使用内存池?
	buf->base = new char[suggested_size];
	buf->len = suggested_size;
}

/**
 * @brief 读取tcp消息
 * 
 * @param client 持续化句柄
 * @param nread 读取到的长度
 * @param buf 缓冲区
 */
static void read_stream_msg(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
{
	uv_shutdown_t *sreq = nullptr;
	sdk_package<uv_stream_t> *package = nullptr;
	uv_buf_t *data = nullptr;

	if (nread > 0)
	{
		package = new sdk_package<uv_stream_t>;
		data = new uv_buf_t(std::move(uv_buf_init(buf->base, buf->len)));

		package->handle = client;
		package->recv_len = nread;
		package->handle->data = (void *)data;
		package->write = uv_stream_write;
		// FIXME:入队失败如何处理
		push_sdk_package(package);
	}

	if (0 == nread)
	{
		delete buf->base;
	}

	if (nread < 0)
	{
		log_d("close connect\n");
		if (nread != UV_EOF)
		{
			log_e("Read error %s\n", uv_err_name(nread));
		}
		delete buf->base;
		sreq = new uv_shutdown_t;
		// 关闭客户连接
		uv_shutdown(sreq, client, on_tcp_shutdown);
	}
}

/**
 * @brief 关闭套接字
 * 
 * @param req 
 * @param status 状态码
 */
static void on_tcp_shutdown(uv_shutdown_t *req, int status)
{
	uv_close((uv_handle_t *)req->handle, on_close);
	delete req;
}

/**
 * @brief 关闭连接回调接口，释放内存
 * 
 * @param peer 
 */
static void on_close(uv_handle_t *peer)
{
	delete peer;
}

/**
 * @brief 客户连接处理
 * 
 * @param server 服务器句柄
 * @param status 状态码
 */
static void on_tcp_connection(uv_stream_t *server, int status)
{
	if (status < 0)
	{
		log_e("New connection error %s\n", uv_strerror(status));
		return;
	}

	uv_tcp_t *client = new uv_tcp_t;

	uv_tcp_init(server->loop, client);

	// 监听客户连接
	if (0 == uv_accept(server, (uv_stream_t *)client))
	{
		uv_read_start((uv_stream_t *)client, alloc_buffer, read_stream_msg);
	}
	else
	{
		uv_close((uv_handle_t *)client, NULL);
	}
}

/**
 * @brief 启动sdk libuv服务器
 * 
 */
void sdk_uv_net_init(void)
{
	std::string ip_version;
	std::string ipv4;
	int port;

	uv_sdk_net_server = new UvSdkNetServer(UvEvent::get_uv_event()->get_uv_loop());

	get_sdk_tcp_host(ip_version, ipv4, port);

	uv_sdk_net_server->create_tcp_server(ipv4, port);

	log_i("sdk-uv网络任务初始化完成...\n");
}
