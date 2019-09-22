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
#include "../../common/config/sys_config.h"
#include "../../tools/timer/timer.h"
#include "sdk_package_distribution.h"
#include "sdk_network.h"

#define DEFAULT_BACKLOG 128 // 连接队列

typedef struct
{
	uv_write_t req;
	uv_buf_t buf;
} write_req_t;

static UvSdkNetServer *uv_sdk_net_server = nullptr;

static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
static void read_stream_msg(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);
static void on_stream_write(uv_write_t *req, int status);
static void on_tcp_connection(uv_stream_t *server, int status);
static void on_tcp_shutdown(uv_shutdown_t *req, int status);
static void on_close(uv_handle_t *peer);

static bool uv_stream_write(uv_stream_t *handle, const uv_buf_t *buf);

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

// 创建tcp服务器
bool UvSdkNetServer::create_tcp_server(const std::string &ipv4, const int &port)
{
	return create_tcp_server(ipv4.c_str(), port);
}

UvSdkNetServer::~UvSdkNetServer()
{
	for (auto &tcp_s : tcp_servers)
	{
		uv_close((uv_handle_t *)&tcp_s->handle, NULL);
	}
	tcp_servers.clear();
}

static void on_stream_write(uv_write_t *req, int status)
{
	write_req_t *wr = (write_req_t *)req;
	uv_buf_t *recv = (uv_buf_t *)(wr->req.handle->data);
	log_i("write recv addr = [%p]\n", recv);
	log_i("write base addr = [%p]\n", recv->base);

	if (status)
	{
		fprintf(stderr, "Write error %s\n", uv_strerror(status));
	}

	// delete wr->buf.base;
	delete recv->base;
	delete recv;
	delete wr;
}

static bool uv_stream_write(uv_stream_t *handle, const uv_buf_t *buf)
{
	write_req_t *wr = nullptr;

	wr = new write_req_t;
	wr->buf = uv_buf_init(buf->base, buf->len);

	log_d("send...........\n");

	if (!buf || !buf->base || 0 == buf->len || uv_write((uv_write_t *)wr, handle, &wr->buf, 1, on_stream_write))
	{
		log_e("uv_write failed\n");
		wr->req.handle = handle;
		on_stream_write((uv_write_t *)wr, 0);
		return false;
	}

	return true;
}

static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
	buf->base = new char[suggested_size];
	log_d("read base addr = [%p]\n", buf->base);
	buf->len = suggested_size;
}

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
		package->handle->data = (void *)data;
		log_d("read recv addr = [%p]\n", data);
		// package->buf = uv_buf_init(buf->base, buf->len);
		package->write = uv_stream_write;
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
		uv_shutdown(sreq, client, on_tcp_shutdown);
		// uv_close((uv_handle_t *)client, on_close);
	}
}

static void on_tcp_shutdown(uv_shutdown_t *req, int status)
{
	uv_close((uv_handle_t *)req->handle, on_close);
	delete req;
}

static void on_close(uv_handle_t *peer)
{
	delete peer;
}

static void on_tcp_connection(uv_stream_t *server, int status)
{
	if (status < 0)
	{
		log_e("New connection error %s\n", uv_strerror(status));
		return;
	}

	uv_tcp_t *client = new uv_tcp_t;

	uv_tcp_init(server->loop, client);

	if (0 == uv_accept(server, (uv_stream_t *)client))
	{
		uv_read_start((uv_stream_t *)client, alloc_buffer, read_stream_msg);
	}
	else
	{
		uv_close((uv_handle_t *)client, NULL);
	}
}

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
