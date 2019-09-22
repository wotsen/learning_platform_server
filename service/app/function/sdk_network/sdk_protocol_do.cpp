/**
 * @file sdk_protocol_do.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-09-22
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#define LOG_TAG "SDK_PROTOCOL_DO"

#include <cstdio>
#include <cstring>
#include <easylogger/easylogger_setup.h>
#include "sdk_protocol/in_sdk.pb.h"
#include "sdk_protocol_do.h"

using namespace insider::sdk;

bool sdk_protocol_do(uv_buf_t *req, struct SdkResponsePack *res)
{
	if (!res || !res->res.base || !req)
	{
		return false;
	}

	if (res->alloc_size > MAX_SDK_MSG_LEN)
	{
		return false;
	}

	sprintf(res->res.base, "hello client");
	res->res.len = strlen("hello client") + 1;

	return true;
}