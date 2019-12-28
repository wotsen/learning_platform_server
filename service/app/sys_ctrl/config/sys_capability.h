/**
 * @file sys_capability.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 具备的能力，具备默认能力，加载json
 * @version 0.1
 * @date 2019-08-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "sys_load_json.h"

class SysCapability : public AbsSysLoadJson
{
public:
#define SYS_CAPAABILITY_FILENAME "../etc/sys_capability.json"

    ~SysCapability();

    static SysCapability *get_sys_capability(void);

private:
    SysCapability(){};

    static SysCapability *instance;
};

/*****************************************************
 * 初始化接口接口
 *****************************************************/

///< 能力集初始化
bool sys_capability_init(void);

///< 获取能力集json指针
nlohmann::json &get_json_capability(void);

/*****************************************************
 * 能力获取接口
 *****************************************************/
uint32_t get_user_manage_max_users_capability(void);
// 获取ip版能力
void get_ip_version_capability(std::string &ip_version);
