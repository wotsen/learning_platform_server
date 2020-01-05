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
using sdk_midware = bool (*)(struct sdk_net_interface &interface, insider::sdk::Sdk &sdk_req, insider::sdk::Sdk &sdk_res);

void _register_sdk_midware(const char *name, sdk_midware midware_fn, bool enable);

// 中间件注册
#define resgister_sdk_midware(midware_fn, enable) _register_sdk_midware(#midware_fn, midware_fn, enable)

#define SDK_IMPORT_MIDWARE(midware, enable) extern bool midware(struct sdk_net_interface &interface, insider::sdk::Sdk &sdk_req, insider::sdk::Sdk &sdk_res);\
											resgister_sdk_midware(midware, enable)

// 中间件处理
bool sdk_midware_do(struct sdk_net_interface &interface, insider::sdk::Sdk &sdk_req, insider::sdk::Sdk &sdk_res);

#endif // !__SDK_MIDWARE_H__