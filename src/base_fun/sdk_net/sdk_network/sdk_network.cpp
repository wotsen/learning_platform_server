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
#include "util_time/util_time.h"
#include "os_param.h"
#include "sys_config.h"
#include "sys_capability.h"
#include "sdk_interface.h"
#include "sdk_protocol_do.h"
#include "sdk_network.h"

using namespace insider::sdk;

// sdk服务器
static SdkServer _sdk_server(OS_SYS_SDK_TCP_CONNECT_NUM);

// 使用handy初始化sdk网络
static void sdk_handy_net_init(void);
// 解码
static bool sdk_msg_decode(Buffer &input, Sdk &req);
// 编码
static bool sdk_msg_encode(const Sdk &res, Buffer &output);
// 获取地址
template <int trans_proto = TCP>
static bool get_host_addr(const Ip4Addr &src, const Ip4Addr &dest, struct sdk_net_interface &sdk_interface);

// 解码
static bool sdk_msg_decode(Buffer &input, Sdk &req)
{
	bool ret = true;
	if (!req.ParseFromString(std::string(input.data())))
	{
		log_e("parser sdk msg failed\n");
		ret = false;
	}

	input.consume(input.size());
	return ret;
}

// 编码
static bool sdk_msg_encode(const Sdk &res, Buffer &output)
{
	std::string out;

	if (!res.SerializeToString(&out) || out.empty())
	{
		log_e("serial sdk msg error\n");
		return false;
	}

	// size_t offset = output.size();

	output.append(out.data(), out.size());
	
	// *(char *) (output.begin() + offset) = output.size() - offset;

	return true;
}

// 获取地址
template <int trans_proto = TCP>
static bool get_host_addr(const Ip4Addr &src, const Ip4Addr &dest, struct sdk_net_interface &sdk_interface)
{
	std::string ip_version;

	get_ip_version(ip_version);

	// 本地信息从配置获取
	get_net_interface_config(sdk_interface.interface);
	get_net_gateway_config(sdk_interface.gateway);

	sdk_interface.trans_protocol = static_cast<TransProto>(trans_proto);

	if (ip_version == "ipv4") {
		sdk_interface.ip_version = IpVersion::IPV4;
	} else {
		sdk_interface.ip_version = IpVersion::IPV6;
	}

	// 源地址
	sdk_interface.src_ip = src.ip();
	sdk_interface.src_port = src.port();

	// 目的地址
	sdk_interface.des_ip = dest.ip();
	sdk_interface.des_port = dest.port();

	return true;
}

SdkServer::SdkServer(const uint32_t _max_links)
	: max_links(_max_links), tcp_links(0), tcp_srv_(nullptr), udp_srv_(nullptr)
{}

// 连接已经达到最大
bool SdkServer::is_tcp_links_max(void)
{
	return tcp_links >= max_links;
}

// 增加tcp连接
bool SdkServer::tcp_links_inc(void)
{
	if (is_tcp_links_max()) { return false; }

	tcp_links++;
	return true;
}

// 减少tcp连接
void SdkServer::tcp_links_dec(void)
{
	if (tcp_links) { tcp_links--; }
}

// tcp连接处理
bool SdkServer::tcp_connection_made(void)
{
	if (!tcp_srv_) { return false; }

	tcp_srv_->onConnCreate([&] {
        TcpConnPtr con(new TcpConn);
        con->onState([&](const TcpConnPtr &con) {
            if (con->getState() == TcpConn::Connected) {
				log_d("[%s] connetd\n", con->peer_.toString().c_str());
				if (!tcp_links_inc()) {
					log_i("tcp_links_inc faild\n");
					con->close();
				}
            } else if (con->getState() == TcpConn::Closed) {
                tcp_links_dec(); // FIXME:手动close是否会执行此处
				log_d("close links\n");
            } else {
				/* code */
			}
        });

		con->onRead([&](TcpConnPtr con) {
			Buffer &buf = con->getInput();

			if (buf.empty()) { return ; }

			Sdk req;
			Sdk res;
			struct sdk_net_interface sdk_interface;

			// 解码
			if (!sdk_msg_decode(buf, req)) {
				con->close();
				return ;
			}

			// 消息处理
			get_host_addr<TCP>(con->peer_, con->local_, sdk_interface);
			sdk_protocol_do(sdk_interface, req, res);

			// 响应
			Buffer &send = con->getOutput();
			if (sdk_msg_encode(res, send)) { con->sendOutput(); }
    	});

        return con;
    });

	return true;
}

// udp数据接收处理
bool SdkServer::udp_data_received(void)
{
	if (!udp_srv_) { return false; }

	udp_srv_->onMsg([](const UdpServerPtr &p, Buffer buf, Ip4Addr peer) {
		if (buf.empty()) { return ; }

		Sdk req;
		Sdk res;
		struct sdk_net_interface sdk_interface;

		// 解码
		if (!sdk_msg_decode(buf, req)) {
			return ;
		}

		// 消息处理
		get_host_addr<UDP>(peer, p->getAddr(), sdk_interface);
		sdk_protocol_do(sdk_interface, req, res);

		// 响应
		Buffer send;
		if (sdk_msg_encode(res, send)) {
			p->sendTo(send, peer);
		}
    });

	return true;
}

// 创建tcp服务
bool SdkServer::create_tcp_srv(void)
{
	std::string ip_version;
	std::string ip_version_cap;
	std::string ip;
	int port;

	get_sdk_tcp_host_config(ip_version, ip, port);
	get_ip_version_capability(ip_version_cap);

	// 校验配置与能力
	if (ip_version_cap != ip_version)
	{
		log_e("not support ip version : %s\n", ip_version.c_str());
		return false;
	}

	tcp_srv_ = TcpServer::startServer(&handy_base(), ip, port, true);

	return tcp_connection_made();
}

// 创建udp服务
bool SdkServer::create_udp_srv(void)
{
	std::string ip_version;
	std::string ip_version_cap;
	std::string ip;
	int port;

	get_sdk_udp_host_config(ip_version, ip, port);
	get_ip_version_capability(ip_version_cap);

	// 校验配置与能力
	if (ip_version_cap != ip_version)
	{
		log_e("not support ip version : %s\n", ip_version.c_str());
		return false;
	}

	udp_srv_ = UdpServer::startServer(&handy_base(), ip, port, true);

	return udp_data_received();
}

// 初始化handy sdk网络实例
static void sdk_handy_net_init(void)
{
	_sdk_server.create_tcp_srv();
}

void sdk_net_init(void)
{
	sdk_handy_net_init();
}