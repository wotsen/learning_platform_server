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
#include <string>
#include <inttypes.h>
#include <easylogger/easylogger_setup.h>
#include "tools_func/calculate_crc16.h"
#include "sdk_protocol_do.h"
#include "sdk_midware.h"

using namespace insider::sdk;

// body内容处理
static bool sdk_body_do(struct sdk_net_interface &interface, const Sdk &sdk_req, Sdk &sdk_res);
// header数据处理
static bool check_sdk_header(struct sdk_net_interface &interface, const Sdk &sdk_req);
// sdk消息处理
static bool sdk_msg_do(struct sdk_net_interface &interface, const std::string &req, std::string &res);

/**
 * @brief body内容处理
 * 
 * @param interface 网络接口
 * @param sdk_req 请求
 * @param sdk_res 响应
 * @return true 成功
 * @return false 失败
 */
static bool sdk_body_do(struct sdk_net_interface &interface, const Sdk &sdk_req, Sdk &sdk_res)
{
	const Body &body = sdk_req.body();

	OperationType op_type = body.method();

	if (op_type > OperationType::DELETE)
	{
		log_e("sdk method not match\n");
		return false;
	}

	// TODO:处理数据内容
	log_i("requset url : %s\n", body.url().c_str());

	// 根据处理结果组包数据

	return true;
}

/**
 * @brief tcp header数据处理
 * 
 * @tparam  
 * @param interface 网络接口
 * @param req_sdk 请求
 * @return true 
 * @return false 
 */
bool check_sdk_header(struct sdk_net_interface &interface, const Sdk &req_sdk)
{
	const Header &header = req_sdk.header();

	if (SdkMagic::SDK_MAGIC != header.msg_magic())
	{
		log_e("msg magic is %x\n", header.msg_magic());
		return false;
	}

	if (SdkVersion::SDK_CUR_VERSION != header.version())
	{
		log_e("msg version is %x\n", header.version());
		return false;
	}

	// 网络类型匹配
	if (interface.trans_protocol != header.trans_proto() || 
		interface.ip_version != header.host().ip_version())
	{
		log_e("trans proto not match\n");
		return false;
	}

	// 时间错误，两边的时间不同步
	if (time(NULL) < header.time().in_time())
	{
		log_e("time error\n");
		return false;
	}

	// 数据方向
	if (DataFlow::DATA_IN != header.data_dir())
	{
		log_e("data dir error\n");
		return false;
	}

	if (interface.src_ip != header.host().ip()
		|| (int)interface.src_port != header.host().port())
	{
		log_i("client ip = %s, port = %d, sdk ip = %s, sdk port = %d\n",
				interface.src_ip.c_str(), interface.src_port,
				header.host().ip().c_str(), header.host().port());
		return false;
	}

	return true;
}

/**
 * @brief 打包sdk数据
 * 
 * @param interface 
 * @param sdk_res 
 * @param res 
 * @return true 
 * @return false 
 */
static bool sdk_pack_res_msg(struct sdk_net_interface interface, const Sdk &sdk_res, std::string &res)
{
	// TODO:添加响应信息

	// 响应包
	if (!sdk_res.SerializeToString(&res))
	{
		log_e("serial response packet error\n");
		return false;
	}

	return true;
}

/**
 * @brief sdk消息处理
 * 
 * @tparam T uv类型
 * @param interface 网络接口
 * @param req 请求
 * @param res 响应
 * @return true 
 * @return false 
 */
static bool sdk_msg_do(struct sdk_net_interface &interface, const std::string &req, std::string &res)
{
	Sdk sdk_req;
	Sdk sdk_res;

	// 解析sdk原始数据
	if (!sdk_req.ParseFromString(req))
	{
		log_e("parser sdk msg failed\n");
		return false;
	}

	// sdk头检查
	if (!check_sdk_header(interface, sdk_req))
	{
		log_e("sdk header verify error\n");
		goto sdk_err;
	}

	// 中间件
	if (!sdk_midware_do(interface, sdk_req, sdk_res))
	{
		log_e("sdk midware proc failed\n");
		goto sdk_err;
	}

	// sdk内容部分
	if (!sdk_body_do(interface, sdk_req, sdk_res))
	{
		log_e("sdk body verify error\n");
		goto sdk_err;
	}

	// 组包
	if (!sdk_pack_res_msg(interface, sdk_res, res))
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
 * @param interface 网络接口
 * @param req 请求
 * @param res 响应
 * @return true 
 * @return false 
 */
bool sdk_protocol_do(struct sdk_net_interface &interface, const std::string &req, std::string &res)
{
	if (req.empty())
	{
		log_e("requset msg is empty\n");
		return false;
	}

	return sdk_msg_do(interface, req, res);
}
