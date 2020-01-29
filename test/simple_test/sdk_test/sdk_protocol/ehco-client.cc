#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>
#include "calculate_crc16.h"
#include "test_sdk_pack.h"

using namespace std;
using namespace insider::sdk;

#define DEFAULT_PORT 7000
#define DEFAULT_BACKLOG 128

uv_loop_t *loop;
struct sockaddr_in addr;

typedef struct
{
	uv_write_t req;
	uv_buf_t buf;
} write_req_t;

// サーバからのレスポンスを表示
void echo_read(uv_stream_t *server, ssize_t nread, const uv_buf_t *buf)
{
	if (nread == -1)
	{
		fprintf(stderr, "error echo_read");
		return;
	}

	printf("read addr=[%p]\n", buf->base);
	// 結果を buf から取得して表示
	printf("result: %s\n", buf->base + sizeof(size_t));
	free(buf->base);
}

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
	buf->base = (char *)malloc(suggested_size);
	printf("alloc addr=[%p]\n", buf->base);
	buf->len = suggested_size;
}

// サーバへデータ送信後, サーバからのレスポンスを読み込む
void on_write_end(uv_write_t *req, int status)
{
	if (status == -1)
	{
		fprintf(stderr, "error on_write_end");
		return;
	}

	// 書き込みが終わったら、すぐに読み込みを開始
	// uv_read_start(req->handle, alloc_buffer, echo_read);
}

void on_connect(uv_connect_t *req, int status)
{
	if (status == -1)
	{
		fprintf(stderr, "error on_write_end");
		return;
	}

	std::cout << "connect" << std::endl;

	Sdk msg;
	std::string pack;

	test_sdk_pack(msg);

	msg.SerializeToString(&pack);

	uv_buf_t buf = uv_buf_init((char *)pack.data(), pack.size());

	std::cout << "data len = " << pack.size() << '\n';

	// ハンドルを取得
	uv_stream_t *tcp = req->handle;

	// 書き込み用構造体
	uv_write_t write_req;

	int buf_count = 1;

	// uv_read_start(req->handle, alloc_buffer, echo_read);
	// 書き込み
	// int uv_write(uv_write_t* req, uv_stream_t* handle, const uv_buf_t bufs[], unsigned int nbufs, uv_write_cb cb)
	// while(1) {
		uv_write(&write_req, tcp, &buf, buf_count, on_write_end);
		// sleep(1);
	// }
}

static void idle_cb(uv_idle_t* handle) {
}

int main(void)
{
	loop = uv_default_loop();

	// Network I/O の構造体
	uv_tcp_t client;
	uv_idle_t idle_handle;

	uv_idle_init(loop, &idle_handle);
	uv_idle_start(&idle_handle, idle_cb);

	// loop への登録
	uv_tcp_init(loop, &client);

	// アドレスの取得
	struct sockaddr_in req_addr;
	uv_ip4_addr("0.0.0.0", 9001, &req_addr);

	// TCP コネクション用の構造体
	uv_connect_t connect_req;
	
	struct sockaddr_in addr;
	uv_ip4_addr("127.0.0.1", 9002, &addr);

	uv_tcp_bind(&client, (const struct sockaddr *)&addr, 0);

	// 接続
	uv_tcp_connect(&connect_req, &client, (const struct sockaddr*)&req_addr, on_connect);
	uv_read_start((uv_stream_t *)&client, alloc_buffer, echo_read);

	// ループを開始
    return uv_run(loop, UV_RUN_DEFAULT);
}
