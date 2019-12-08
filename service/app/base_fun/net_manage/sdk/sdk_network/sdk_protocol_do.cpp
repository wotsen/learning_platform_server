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
#include <inttypes.h>
#include <easylogger/easylogger_setup.h>
#include "../../../../tools/tools_func/calculate_crc16.h"
#include "sdk_protocol_do.h"
#include "sdk_midware.h"

using namespace insider::sdk;

// body内容处理
static bool sdk_body_do(const enum sdk_net_data_type type, Sdk &req, Sdk &res);

// header数据处理
template <typename T>
bool check_sdk_header(const enum sdk_net_data_type type, Sdk &req, sdk_package<T> *uv_req);

// sdk消息处理
template <typename T>
static bool sdk_msg_do(const enum sdk_net_data_type type, sdk_package<T> *req, struct SdkResponsePack *res);

// sdk传输数据校验
static bool check_sdk_protocol_head(SdkMsgProtocol *msg, const size_t real_recv_len);

/**
 * @brief body内容处理
 * 
 * @param type 网络类型
 * @param req 请求
 * @param res 响应
 * @return true 成功
 * @return false 失败
 */
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

/**
 * @brief tcp header数据处理
 * 
 * @tparam  
 * @param type 网络类型
 * @param req 请求
 * @param uv_req 响应
 * @return true 
 * @return false 
 */
template <>
bool check_sdk_header(const enum sdk_net_data_type type, Sdk &req, sdk_package<uv_stream_t> *uv_req)
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

	if (SdkMagic::SDK_MAGIC != header->msg_magic())
	{
		log_e("msg magic is %x\n", header->msg_magic());
		return false;
	}

	if (SdkVersion::SDK_CUR_VERSION != header->version())
	{
		log_e("msg version is %x\n", header->version());
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

	log_i("client ip = %s, port = %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

	if (ip != header->mutable_host()->ipv4() || port != header->mutable_host()->port())
	{
		log_i("client ip = %s, port = %d, sdk ip = %s, sdk port = %d\n",
				inet_ntoa(addr.sin_addr), ntohs(addr.sin_port),
				header->mutable_host()->ipv4().c_str(), header->mutable_host()->port());
		return false;
	}

	return true;
}

/**
 * @brief sdk传输数据校验
 * 
 * @param msg 传输消息
 * @param real_recv_len 实际接受长度
 * @return true 合法
 * @return false 非法
 */
static bool check_sdk_protocol_head(SdkMsgProtocol *msg, const size_t real_recv_len)
{
	uint16_t crc = 0;

	/* 实际接收数量是否等于sdk协议部分加数据头部 */
	if (real_recv_len != (msg->data_len + sizeof(SdkMsgProtocol)))
	{
		log_i("recv msg len[%d] not match pack len[%d]\n", real_recv_len, msg->data_len);
		return false;
	}

	/* 校验crc */
	crc = calculate_crc16(0, (uint8_t *)msg->data, msg->data_len);
	if (crc != msg->crc16)
	{
		log_i("crc16 not match cal[%d] != [%d]\n", crc, msg->crc16);
		return false;
	}

	return true;
}

template <typename T>
static bool sdk_req_msg_parser(sdk_package<T> *req, Sdk &req_msg)
{
	SdkMsgProtocol *msg = NULL;
	uv_buf_t *req_buf = (uv_buf_t *)req->handle->data;

	// 转换为sdk协议消息
	msg = (SdkMsgProtocol *)req_buf->base;

	// 校验消息有效性
	if (!check_sdk_protocol_head(msg, req->recv_len))
	{
		log_i("sdk msg head invalid\n");
		return false;
	}

	// 解析协议部分
	if (!req_msg.ParseFromArray(msg->data, msg->data_len))
	{
		log_e("can not parse sdk protocol\n");
		return false;
	}

	return true;
}

static bool sdk_pack_res_msg(struct SdkResponsePack *res, Sdk &res_msg)
{
	SdkMsgProtocol *msg = NULL;

	// 响应sdk
	msg = (SdkMsgProtocol *)res->res.base;

	msg->data_len = res_msg.ByteSizeLong();
	msg->crc16 = 0;

	// TODO:添加响应信息

	// 响应包，转换为数组
	if (!res_msg.SerializeToArray(msg->data, res_msg.ByteSizeLong()))
	{
		log_e("serial response packet error\n");
		return false;
	}

	msg->crc16 = calculate_crc16(0, (uint8_t *)msg->data, msg->data_len);

	// 实际发送数据长度需要加上头部校验部分
	res->res.len = msg->data_len + sizeof(SdkMsgProtocol);

	return true;
}

/**
 * @brief sdk消息处理
 * 
 * @tparam T uv类型
 * @param type 网络类型
 * @param req 请求
 * @param res 响应
 * @return true 
 * @return false 
 */
template <typename T>
static bool sdk_msg_do(const enum sdk_net_data_type type, sdk_package<T> *req, struct SdkResponsePack *res)
{
	Sdk req_msg;
	Sdk res_msg;

	// 解析sdk原始数据
	if (!sdk_req_msg_parser(req, req_msg))
	{
		log_e("parser sdk msg failed\n");
		return false;
	}

	// sdk头检查
	if (!check_sdk_header(type, req_msg, req))
	{
		log_e("sdk header verify error\n");
		goto sdk_err;
	}

	// 中间件
	if (!sdk_midware_do(type, req, req_msg, res_msg))
	{
		log_e("sdk midware proc failed\n");
		goto sdk_err;
	}

	// sdk内容部分
	if (!sdk_body_do(type, req_msg, res_msg))
	{
		log_e("sdk body verify error\n");
		goto sdk_err;
	}

	// 组包
	if (!sdk_pack_res_msg(res, res_msg))
	{
		log_e("pack response msg failed\n");
		goto sdk_err;
	}

	google::protobuf::ShutdownProtobufLibrary();

	return true;

sdk_err:
	google::protobuf::ShutdownProtobufLibrary();

	return false;
}

/**
 * @brief tcp sdk协议处理
 * 
 * @tparam  uv类型
 * @param type 网络类型
 * @param req 请求
 * @param res 响应
 * @return true 
 * @return false 
 */
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

	return sdk_msg_do(SDK_TCP_DATA_TYPE, req, res);
}
