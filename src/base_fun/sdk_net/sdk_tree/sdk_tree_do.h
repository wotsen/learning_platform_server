/**
 * @file sdk_tree_do.h
 * @author yuwangliang (astralrovers@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-02-15
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once

#include "../sdk_network/sdk_interface.h"

// sdk树路径统计
void sdk_tree_map(void);

// sdk 路径处理
bool sdk_tree_do(struct sdk_net_interface &sdk_interface, const insider::sdk::Sdk &sdk_req, insider::sdk::Sdk &sdk_res);
