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
#include <cstdio>
#include <cstring>
#include <time.h>
#include <string>
#include <inttypes.h>
#include <loguru.hpp>
#include "tools/calculate_crc16.h"
#include "sdk_router_distribution.h"
#include "sdk_protocol_do.h"
#include "sdk_midware_manage.h"

using namespace insider::sdk;
namespace wotsen
{

// header数据处理
static bool sdk_header_check(SdkRequest &sdk_request);
// 业务内容内容处理
static bool sdk_body_do(SdkRequest &sdk_request);

// 中间件处理，中间件处理失败时直接响应
static bool sdk_midware_proc(SdkRequest &sdk_request);
// body处理
static bool sdk_body_proc(SdkRequest &sdk_request);

// 打包响应消息头
static void sdk_pack_res_msg_header(SdkRequest &sdk_request);

// 魔术数字校验
static bool sdk_header_check_magic(const Header &header);
// 版本检查
static bool sdk_header_check_version(const Header &header);
// 网络接口检查
static bool sdk_header_check_net(const Header &header, const struct SdkNetInterface &sdk_interface);
// 数据时间检查
static bool sdk_header_check_time(const Header &header);
// 数据方向检查
static bool sdk_header_check_data_dir(const Header &header);

/**
 * @brief 业务内容内容处理
 * 
 * @param sdk_interface 网络接口
 * @param sdk_req 请求
 * @param sdk_res 响应
 * @return true 成功
 * @return false 失败
 */
static bool sdk_body_do(SdkRequest &sdk_request)
{
	const Body &body = sdk_request.request.body();
	OperationType op_type = body.method();

	if (op_type > OperationType::DELETE)
	{
		LOG_F(ERROR, "sdk method not match\n");
		return false;
	}

	LOG_F(WARNING, "requset url : %s\n", body.url().c_str());

	// 根据处理结果组包数据
	return sdk_router_distribution(sdk_request);
}

// 魔术数字校验
static bool sdk_header_check_magic(const Header &header)
{
	// 魔术数字检查
	if (SdkMagic::SDK_MAGIC != header.msg_magic())
	{
		LOG_F(ERROR, "msg magic is %x\n", header.msg_magic());
		return false;
	}

	return true;
}

// 版本检查
static bool sdk_header_check_version(const Header &header)
{
	// 版本检查，不同版本不兼容，后续可去除
	if (SdkVersion::SDK_CUR_VERSION != header.version())
	{
		LOG_F(ERROR, "msg version is %x\n", header.version());
		return false;
	}

	return true;
}

// 网络接口检查
// FIXME:存在代理、网关等跨网段时是否可能回有问题
static bool sdk_header_check_net(const Header &header, const struct SdkNetInterface &sdk_interface)
{
	// 网络类型匹配
	if (sdk_interface.trans_protocol != header.trans_proto() ||
		sdk_interface.ip_version != header.host().ip_version())
	{
		LOG_F(ERROR, "trans proto not match\n");
		return false;
	}

	// 源地址检查
	if (sdk_interface.src_ip != header.host().ip()
		|| (int)sdk_interface.src_port != header.host().port())
	{
		LOG_F(ERROR, "client ip = %s, port = %d, sdk ip = %s, sdk port = %d\n",
				sdk_interface.src_ip.c_str(), sdk_interface.src_port,
				header.host().ip().c_str(), header.host().port());
		return false;
	}

	// 目的地址检查
	if (sdk_interface.des_ip != header.dest().ip()
		|| (int)sdk_interface.des_port != header.dest().port())
	{
		LOG_F(ERROR, "server ip = %s, port = %d, sdk ip = %s, sdk port = %d\n",
				sdk_interface.des_ip.c_str(), sdk_interface.des_port,
				header.dest().ip().c_str(), header.dest().port());
		return false;
	}

	return true;
}

// 数据时间检查
static bool sdk_header_check_time(const Header &header)
{
	// 时间错误，两边的时间不同步
	if (time(NULL) < header.time().in_time())
	{
		LOG_F(ERROR, "time error\n");
		return false;
	}
	return true;
}

// 数据方向检查
static bool sdk_header_check_data_dir(const Header &header)
{
	// 数据方向
	if (DataFlow::DATA_IN != header.data_dir())
	{
		LOG_F(ERROR, "data dir error\n");
		return false;
	}
	return true;
}

// 打包响应消息头
static void sdk_pack_res_msg_header(SdkRequest &sdk_request)
{
	Header *header = sdk_request.response.mutable_header();

	// 魔术数字
	header->set_msg_magic(SdkMagic::SDK_MAGIC);
	// 版本
	header->set_version(SdkVersion::SDK_CUR_VERSION);
	// 包序号，使用请求的
	header->set_pack_id(sdk_request.request.header().pack_id());
	// 时间
	header->mutable_time()->set_out_time(time(NULL));
	// 方向
	header->set_data_dir(DataFlow::DATA_OUT);

	// 响应时，地址反过来
    header->mutable_host()->set_ip_version(sdk_request.interface.ip_version);
	header->mutable_host()->set_port(sdk_request.interface.des_port);
	header->mutable_host()->set_ip(sdk_request.interface.des_ip);

    header->mutable_dest()->set_ip_version(sdk_request.interface.ip_version);
	header->mutable_dest()->set_port(sdk_request.interface.src_port);
	header->mutable_dest()->set_ip(sdk_request.interface.src_ip);

	// 传输协议
	header->set_trans_proto(sdk_request.interface.trans_protocol);

	return ;
}

// header数据处理
static bool sdk_header_check(SdkRequest &sdk_request)
{
	const Header &header = sdk_request.request.header();

	return sdk_header_check_magic(header)
		   && sdk_header_check_version(header)
		   && sdk_header_check_net(header, sdk_request.interface)
		   && sdk_header_check_time(header)
		   && sdk_header_check_data_dir(header);
}

// 中间件处理，中间件处理失败时直接响应
static bool sdk_midware_proc(SdkRequest &sdk_request)
{
	return sdk_midware_function(sdk_request);
}

// body内容处理
static bool sdk_body_proc(SdkRequest &sdk_request)
{
	return sdk_body_do(sdk_request);
}

/**
 * @brief sdk协议处理
 * 
 * @param sdk_interface 网络接口
 * @param sdk_req 请求
 * @param sdk_res 应答
 */
void sdk_protocol_do(SdkRequest &sdk_request)
{
	// 头部检查
	if (!sdk_header_check(sdk_request))
	{
		return ;
	}

	// 需要填写回去
	sdk_request.response.mutable_body()->set_url(sdk_request.request.body().url());
	sdk_request.response.mutable_body()->set_method(sdk_request.request.body().method());

	/* NOTE:如果是中间件出错，其中的sdk错误码由中间件填写 */
	if (!sdk_midware_proc(sdk_request))
	{
		// 中间件响应消息不走正常流程，单独存在错误码，会填写具体中间件
		sdk_pack_res_msg_header(sdk_request);
		return ;
	}

	// 业务部分错误码由业务本身填写，原因在于业务功能相对中间件更为复杂，不能够统一处理
	if (sdk_body_proc(sdk_request))
	{
		sdk_set_result(ResponseResult::OK, "ok", sdk_request.response);
	}

	// 响应消息
	sdk_pack_res_msg_header(sdk_request);

	return ;
}

} // !namespace wotsen
