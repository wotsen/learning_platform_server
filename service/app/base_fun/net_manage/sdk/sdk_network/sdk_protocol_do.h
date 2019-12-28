/**
 * @file sdk_protocol_do.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-09-22
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include <inttypes.h>
#include "sdk_package_distribution.h"

// sdk协议处理
bool sdk_protocol_do(struct sdk_net_interface &interface, struct sdk_data_buf &req, struct sdk_data_buf &res);
