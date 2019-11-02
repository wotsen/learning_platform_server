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
#include "../../tools/timer/timer.h"
#include "../task_manage/task_manage.h"
#include "sdk_protocol_do.h"
#include "sdk_package_distribution.h"

using namespace wotsen;

static void *task_sdk_stream_do(void *name) noexcept;

static const int MAX_STREAM_LIST_LEN = 1024;

static std::list<sdk_package<uv_stream_t>*> stream_list;

template <> bool push_sdk_package<uv_stream_t>(sdk_package<uv_stream_t> *package)
{
	if (!package)
	{
		log_w("package is null\n");
		return false;
	}

	if (stream_list.size() >= MAX_STREAM_LIST_LEN)
	{
		log_w("stream list full!\n");
		return false;
	}

	log_d("recv : %s\n", ((uv_buf_t *)(package->handle->data))->base);
	stream_list.push_back(package);

	return true;
}

static void *task_sdk_stream_do(void *name) noexcept
{
    // pthread_t tid = pthread_self();

    log_i("sdk stream 数据处理初始化完成...\n");

	char *res_buf = new char[MAX_SDK_MSG_LEN];

	sdk_package<uv_stream_t> *package = nullptr;

	struct SdkResponsePack res;

	res.alloc_size = MAX_SDK_MSG_LEN;
	res.res.len = 0;
	res.res.base = res_buf;

	for (;;)
	{
		if (!stream_list.empty())
		{
			res.res.len = 0;  // 清空长度
			memset(res_buf, 0, MAX_SDK_MSG_LEN);  // 清空数据

			package = stream_list.front();

			// if (sdk_protocol_do(SDK_TCP_DATA_TYPE, package, &res))
			{
				sdk_protocol_do(SDK_TCP_DATA_TYPE, package, &res);
				package->write(package->handle, &res.res);
			}

			stream_list.pop_back();
			delete package;
			package = nullptr;
		}
    }
}

void task_sdk_package_distribution_init(void)
{
    task_create(task_sdk_stream_do, STACKSIZE(50*1024), "task_sdk_stream_do", 0, OS_MIN(5), E_TASK_IGNORE);
}