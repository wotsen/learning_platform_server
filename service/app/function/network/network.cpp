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

#include <easylogger/easylogger_setup.h>
#include "../task_manage/task_manage.h"
#include "../../tools/timer/timer.h"
#include "../../common/sdk_protocol/in_sdk.pb.h"
#include "network.h"

using namespace insider::sdk;
using namespace wotsen;

// 创建tcp服务器
bool UvSdkNetServer::create_tcp_server(const std::string &ipv4, const int &port) {
	std::shared_ptr<UvSdkNetSrvType<uv_tcp_t>> server(new UvSdkNetSrvType<uv_tcp_t>);

	uv_tcp_init(loop, &server->handle);
	uv_ip4_addr(ipv4.c_str(), port, &server->addr);

	uv_tcp_bind(&server->handle, (const struct sockaddr*)&server->addr, 0);

	server->handle.data = (void *)loop;

	int r = uv_listen((uv_stream_t*)&server->handle, DEFAULT_BACKLOG, on_new_connection);
	if (r)
	{
		log_e("Listen error %s\n", uv_strerror(r));
		return false;
	}

	tcp_servers.push_back(std::move(server));

	return true;

}

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    buf->base = (char*) malloc(suggested_size);
    buf->len = suggested_size;
}

void echo_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
{
    if (nread > 0) {
		// 放入数据处理链表
		// FIXME:应该发送完后进行释放
		log_d("read str = [%s]\n", buf->base);
    	free(buf->base);
        // write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));
        // req->buf = uv_buf_init(buf->base, nread);
        // uv_write((uv_write_t*) req, client, &req->buf, 1, echo_write);
        // return;
    }
    if (nread < 0) {
        if (nread != UV_EOF) {
            log_e("Read error %s\n", uv_err_name(nread));
		}
        uv_close((uv_handle_t*) client, NULL);
    }

    // free(buf->base);
}

void on_new_connection(uv_stream_t *server, int status) {
	if (status < 0)
	{
		log_e("New connection error %s\n", uv_strerror(status));
		return ;
	}

	uv_tcp_t *client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
	uv_tcp_init((uv_loop_t *)server->loop, client);
	if (0 == uv_accept(server, (uv_stream_t *)client))
	{
		uv_read_start((uv_stream_t *)client, alloc_buffer, echo_read);
	}
	else
	{
		uv_close((uv_handle_t *)client, NULL);
	}
}

UvSdkNetServer::~UvSdkNetServer() {
	// for (; !tcp_servers.empty(); ) {
	// 	auto tcp_s = tcp_servers.pop_front();
	// 	uv_close((uv_handle_t *)&tcp_s->handle, NULL);
	// 	delete tcp_s;
	// }
	for (auto &tcp_s : tcp_servers) {
		uv_close((uv_handle_t *)&tcp_s->handle, NULL);
	}
	tcp_servers.clear();
}

static void *task_sdk_uv_net(void *name) {
    pthread_t tid = pthread_self();
	std::string ipv4("0.0.0.0");
	int port = 8001;

	UvEvent *event = UvEvent::get_uv_event();

	UvSdkNetServer uv_sdk_net_server(event->get_uv_loop());

	uv_sdk_net_server.create_tcp_server(ipv4, port);

    log_i("sdk-uv网络任务初始化完成...\n");

    for (;;)
    {
        task_alive(tid);          // 自身任务心跳

        ostime_delay(OS_MS(3)); // 3秒刷新一次
    }
}

void task_network_init(void)
{
	// task_sdk_uv_net(NULL);
    task_create(task_sdk_uv_net, STACKSIZE(100*1024), "task_sdk_uv_net", 0, OS_MIN(30), E_TASK_REBOOT_SYSTEM);
}