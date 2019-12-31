/**
 * @file sys_config.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 系统配置参数，基础的必须配置项，用于系统程序运行，与功能项无关
 * @version 0.1
 * @date 2019-08-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "sys_load_json.h"

class SysConfig : public AbsSysLoadJson
{
public:

    ~SysConfig();

    ///< 获取系统配置
    static SysConfig *get_sys_config(void);

private:
    SysConfig(){};

    static SysConfig *instance;
};

///< 系统配置初始化
bool sys_config_init(void);

///< 配置参数json指针
nlohmann::json &get_json_config(void);

/************************************************************************************************/
/* 获取sdk tcp网络配置 */
void get_sdk_tcp_host_config(std::string &ip_version, std::string &ip, int &port);

// 获取网络接口
void get_net_interface_config(std::string &interface);

// 获取网络接口
void get_net_gateway_config(std::string &gateway);
