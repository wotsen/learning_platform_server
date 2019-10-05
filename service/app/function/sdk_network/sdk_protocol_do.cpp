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
#include <time.h>
#include <easylogger/easylogger_setup.h>
#include "sdk_protocol/in_sdk.pb.h"
#include "sdk_protocol_do.h"

using namespace insider::sdk;

static bool sdk_body_do(const enum sdk_net_data_type type, Sdk &req, Sdk &res);
template <typename T>
bool sdk_header_do(const enum sdk_net_data_type type, Sdk &req, sdk_package<T> *uv_req);
template <typename T>
static bool sdk_msg_do(const enum sdk_net_data_type type, sdk_package<T> *req, struct SdkResponsePack *res);

static bool sdk_body_do(const enum sdk_net_data_type type, Sdk &req, Sdk &res)
{
	Body *body = req.mutable_body();
	// UserInfo *user_info = nullptr;

	if (!body)
	{
		log_e("body is NULL\n");
		return false;
	}

	// user_info = body->mutable_user();
	// TODO:用户验证
	// TODO:记录访问者，ip，端口，时间(),json格式

	OperationType op_type = body->method();

	if (op_type > OperationType::DELETE)
	{
		log_e("sdk method not match\n");
		return false;
	}

	// TODO:处理数据内容
	log_i("requset url : %s\n", body->url().c_str());

	// 根据处理结果组包数据

	return true;
}

template <>
bool sdk_header_do(const enum sdk_net_data_type type, Sdk &req, sdk_package<uv_stream_t> *uv_req)
{
	struct sockaddr_in addr;
	Header *header = nullptr;
	int namelen = sizeof(struct sockaddr);
	uv_stream_t *client = NULL;

	if (NULL == uv_req)
	{
		return false;
	}

	client = (uv_stream_t *)uv_req->handle;

	header = req.mutable_header();

	if (!header)
	{
		log_e("header is NULL\n");
		return false;
	}

	if (SDK_NET_PROTOCOL_MAGIC != header->msg_magic())
	{
		log_e("msg magic is %x\n", header->msg_magic());
		return false;
	}

	// 长度匹配
	if (req.ByteSizeLong() != (size_t)header->pack_len())
	{
		log_e("sdk len is not match byte len = [%zu], pack_len = [%ld]\n", req.ByteSizeLong(), header->pack_len());
		return false;
	}

	// 网络类型匹配
	if (SDK_TCP_DATA_TYPE == type && TransProto::TCP != header->trans_proto())
	{
		log_e("trans proto not match\n");
		return false;
	}

	// 时间错误，两边的时间不同步
	if (time(NULL) < header->mutable_time()->in_time())
	{
		log_e("time error\n");
		return false;
	}

	// 数据方向
	if (DataFlow::DATA_IN != header->data_dir())
	{
		log_e("data dir error\n");
		return false;
	}

	// TODO:验证端口地址
	uv_tcp_getsockname((uv_tcp_t *)client, (struct sockaddr *)&addr, &namelen);

	std::string ip(inet_ntoa(addr.sin_addr));
	int port = ntohs(addr.sin_port);

	log_i("clinet ip = %s, port = %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

	if (ip != header->mutable_host()->ipv4() || port != header->mutable_host()->port())
	{
		return false;
	}

	return true;
}

template <typename T>
static bool sdk_msg_do(const enum sdk_net_data_type type, sdk_package<T> *req, struct SdkResponsePack *res)
{
	Sdk req_msg;
	Sdk res_msg;
	
	uv_buf_t *req_buf = (uv_buf_t *)req->handle->data;

	if (!req_msg.ParseFromArray(req_buf->base, req_buf->len))
	{
		log_e("can not parse sdk protocol\n");
		return false;
	}

	if (!sdk_header_do(type, req_msg, req))
	{
		log_e("sdk header verify error\n");
		goto sdk_err;
	}

	if (!sdk_body_do(type, req_msg, res_msg))
	{
		log_e("sdk body verify error\n");
		goto sdk_err;
	}

	// 响应包
	if (!res_msg.SerializeToArray(res->res.base, res_msg.ByteSizeLong()))
	{
		log_e("serial response packet error\n");
		goto sdk_err;
	}
	res->res.len = res_msg.ByteSizeLong();

	google::protobuf::ShutdownProtobufLibrary();

	return true;

sdk_err:
	google::protobuf::ShutdownProtobufLibrary();

	return false;
}

template <>
bool sdk_protocol_do(const enum sdk_net_data_type type, sdk_package<uv_stream_s> *req, struct SdkResponsePack *res)
{
	if (!res || !res->res.base || !req)
	{
		return false;
	}

	if (res->alloc_size > MAX_SDK_MSG_LEN)
	{
		return false;
	}

	// sprintf(res->res.base, "hello client");
	// res->res.len = strlen("hello client") + 1;

	return sdk_msg_do(SDK_TCP_DATA_TYPE, req, res);
}
