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

/**
 * @brief 实际sdk传输数据协议
 * 
 */
struct SdkMsgProtocol
{
	size_t data_len;
	uint16_t crc16;
	char data[0];
};

// body内容处理
static bool sdk_body_do(struct sdk_net_interface &interface, Sdk &req, Sdk &res);
// header数据处理
static bool check_sdk_header(struct sdk_net_interface &interface, Sdk &req_sdk, struct sdk_data_buf &res);
// sdk消息处理
static bool sdk_msg_do(struct sdk_net_interface &interface, struct sdk_data_buf &req, struct sdk_data_buf &res);
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
static bool sdk_body_do(struct sdk_net_interface &interface, Sdk &req, Sdk &res)
{
	Body *body = req.mutable_body();

	if (!body)
	{
		log_e("body is NULL\n");
		return false;
	}

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
bool check_sdk_header(struct sdk_net_interface &interface, Sdk &req_sdk, struct sdk_data_buf &res)
{
	Header *header = req_sdk.mutable_header();

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
	if (interface.trans_protocol != header->trans_proto() || 
		interface.ip_version != header->mutable_host()->ip_version())
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

	if (interface.src_ip != header->mutable_host()->ip()
		|| (int)interface.src_port != header->mutable_host()->port())
	{
		log_i("client ip = %s, port = %d, sdk ip = %s, sdk port = %d\n",
				interface.src_ip.c_str(), interface.src_port,
				header->mutable_host()->ip().c_str(), header->mutable_host()->port());
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

static bool sdk_req_msg_parser(struct sdk_data_buf &req, Sdk &req_msg)
{
	SdkMsgProtocol *msg = NULL;

	// 转换为sdk协议消息
	msg = (SdkMsgProtocol *)req.data;

	// 校验消息有效性
	if (!check_sdk_protocol_head(msg, req.len))
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

static bool sdk_pack_res_msg(struct sdk_net_interface interface, struct sdk_data_buf &res, Sdk &res_msg)
{
	SdkMsgProtocol *msg = NULL;

	// 响应sdk
	msg = (SdkMsgProtocol *)res.data;

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

	if (res.len < msg->data_len + sizeof(SdkMsgProtocol))
	{
		log_e("response packet len [%d] over max [%d]\n", msg->data_len + sizeof(SdkMsgProtocol), res.len);
		return false;
	}

	// 实际发送数据长度需要加上头部校验部分
	res.len = msg->data_len + sizeof(SdkMsgProtocol);

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
static bool sdk_msg_do(struct sdk_net_interface &interface, struct sdk_data_buf &req, struct sdk_data_buf &res)
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
	if (!check_sdk_header(interface, req_msg, req))
	{
		log_e("sdk header verify error\n");
		goto sdk_err;
	}

	// 中间件
	if (!sdk_midware_do(interface, req_msg, res_msg))
	{
		log_e("sdk midware proc failed\n");
		goto sdk_err;
	}

	// sdk内容部分
	if (!sdk_body_do(interface, req_msg, res_msg))
	{
		log_e("sdk body verify error\n");
		goto sdk_err;
	}

	// 组包
	if (!sdk_pack_res_msg(interface, res, res_msg))
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
bool sdk_protocol_do(struct sdk_net_interface &interface, struct sdk_data_buf &req, struct sdk_data_buf &res)
{
	if (!res.data || res.len > MAX_SDK_MSG_LEN)
	{
		return false;
	}

	return sdk_msg_do(interface, req, res);
}
