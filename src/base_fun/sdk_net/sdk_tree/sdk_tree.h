/**
 * @file sdk_tree.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-09-28
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include <vector>
#include "../sdk_network/sdk_protocol_do.h"

using namespace insider::sdk;

// 节点处理接口
using sdk_app_node_fn = bool (*)(struct sdk_net_interface &interface, Sdk &sdk_req, Sdk &sdk_res);

#define SDK_GET OperationType::GET				///< get方法
#define SDK_PUT OperationType::PUT				///< put方法
#define SDK_POST OperationType::POST			///< post方法
#define SDK_DELETE OperationType::DELETE		///< delete方法

// sdk树节点
struct sdk_tree_node
{
	int method;							///< 节点方法
	std::string url;					///< 节点url
	sdk_app_node_fn fn;					///< 节点处理接口
	std::vector<const struct sdk_tree_node *> next;	///< 下级节点

	sdk_tree_node(int method, std::string url, sdk_app_node_fn fn, std::vector<const struct sdk_tree_node *> &next) :
		method(method), url(url), fn(fn), next(next) {}

	sdk_tree_node(int method, std::string url, sdk_app_node_fn fn, void *ptr) :
		method(method), url(url), fn(fn) {}

	sdk_tree_node(int method, std::string url, sdk_app_node_fn fn) :
		method(method), url(url), fn(fn) {}
};

// sdk url
#define sdk_url(method, url, fn, next) new struct sdk_tree_node(method, url, fn, next)

// 添加sdk tree
const struct sdk_tree_node *add_sdk_tree(const struct sdk_tree_node *sdk_tree);

// 模块添加sdk tree，一个模块只能添加一个
#define MODULE_ADD_SDK_TREE(method, url, fn, next) \
	static std::vector<const struct sdk_tree_node *> __sdk_tree___{add_sdk_tree(sdk_url(method, url, fn, next))}

// sdk 路径处理
bool sdk_tree_do(struct sdk_net_interface &interface, Sdk &sdk_req, Sdk &sdk_res);