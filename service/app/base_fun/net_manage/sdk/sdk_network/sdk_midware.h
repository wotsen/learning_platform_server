/**
 * @file sdk_midware.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef __SDK_MIDWARE_H__
#define __SDK_MIDWARE_H__

#include "sdk_protocol_do.h"

// sdk中间件处理
using sdk_midware = bool (*)(const enum sdk_net_data_type type, void *req, insider::sdk::Sdk &sdk_req, insider::sdk::Sdk &sdk_res);

void _register_sdk_midware(const char *name, sdk_midware midware_fn, bool enable);

#define resgister_sdk_midware(midware_fn, enable) _register_sdk_midware(#midware_fn, midware_fn, enable)

bool sdk_midware_do(const enum sdk_net_data_type type, void *req, insider::sdk::Sdk &sdk_req, insider::sdk::Sdk &sdk_res);

#endif // !__SDK_MIDWARE_H__