/**
 * @file sys_config.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 系统配置参数，基础的必须配置项，用于系统程序运行，与功能项无关
 * @version 0.1
 * @date 2019-08-11
 * 
 * @note 这些配置都是无法通过运行时修改的，只读
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include <string>
#include <cinttypes>

/*****************************************************获取配置接口*******************************************/
/* 获取sdk tcp网络配置 */

// 获取sdk tcp端口
int get_sdk_tcp_port_config(void);

// 获取sdk udp端口
int get_sdk_udp_port_config(void);

// 获取sdkip地址
std::string get_sdk_ip_config(void);

// 获取ip版本
std::string get_ip_version_config(void);

// 获取网络接口
std::string get_net_interface_config(void);

// 获取网络接口
std::string get_net_gateway_config(void);
