/*
 * @Date: 2019-08-11 22:13:20
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2019-08-17 21:25:46
 */
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

bool sys_capability_init(void);
neb::CJsonObject *get_json_capability(void);
