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

#include <nlohmann/json.hpp>
#include "sys_load_json.h"

struct sys_capability
{
    /* data */
};

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
 * 能力获取接口
 *****************************************************/
uint32_t get_user_manage_max_users(void);

/*****************************************************
 * 初始化接口接口
 *****************************************************/
bool sys_capability_init(void);
nlohmann::json &get_json_capability(void);
