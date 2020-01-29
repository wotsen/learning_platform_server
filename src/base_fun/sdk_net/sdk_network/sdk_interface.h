/**
 * @file sdk_interface.h
 * @author yuwangliang (astralrovers@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-01-29
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once

#include <string>
#include <inttypes.h>
#include "../sdk_protocol/in_sdk.pb.h"

/**
 * @brief sdk 网络接口
 * 
 */
struct sdk_net_interface {
	insider::sdk::IpVersion ip_version;				///< ip版本
	insider::sdk::TransProto trans_protocol;		///< 传输层
	std::string interface;							///< 本地网口
	std::string gateway;							///< 本地网关
	std::string src_ip;								///< 源地址
	uint32_t src_port;								///< 源端口
	std::string des_ip;								///< 目的地址
	uint32_t des_port;								///< 目的端口
};

// 设置sdk响应码
void sdk_set_result(const insider::sdk::ResponseResult &status_code, const std::string &code, insider::sdk::Sdk &sdk_res);
void sdk_set_result(const insider::sdk::ResponseResult &status_code, const char *code, insider::sdk::Sdk &sdk_res);

// 设置业务内容响应码
void sdk_content_set_result(const insider::sdk::ContentResultE &status_code, const std::string &code, insider::sdk::Sdk &sdk_res);
void sdk_content_set_result(const insider::sdk::ContentResultE &status_code, const char *code, insider::sdk::Sdk &sdk_res);
void sdk_content_set_result(const insider::sdk::ContentResultE &status_code, insider::sdk::Sdk &sdk_res);
void sdk_content_set_result(const std::string &code, insider::sdk::Sdk &sdk_res);
void sdk_content_set_result(const char *code, insider::sdk::Sdk &sdk_res);
