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
#include <iterator>
#include "../sdk_network/sdk_interface.h"

using namespace insider::sdk;

// 节点处理接口
using sdk_app_node_fn = bool (*)(struct sdk_net_interface &sdk_interface, const Sdk &sdk_req, Sdk &sdk_res);

#define SDK_GET OperationType::GET				///< get方法
#define SDK_PUT OperationType::PUT				///< put方法
#define SDK_POST OperationType::POST			///< post方法
#define SDK_DELETE OperationType::DELETE		///< delete方法

// sdk tree节点
class SdkTreeNode;
// sdk tree 节点列表
class SdkTreeTable;

class SdkTreeTable
{
public:
	typedef std::vector<const SdkTreeNode *> sdk_tree_table_t;
private:
	// sdk实际树表格
	mutable sdk_tree_table_t table_;

public:
	// 可以不初始化，表为空
	SdkTreeTable();
	// 拷贝构造
	SdkTreeTable(const SdkTreeTable &table);
	// 支持初始化列表 {} 格式
	SdkTreeTable(const std::initializer_list<SdkTreeNode *> &li);

	// 适配for i : v 迭代
	sdk_tree_table_t::iterator begin() const;
	sdk_tree_table_t::iterator end() const;
	// 适配vector push_back
	void push_back(const SdkTreeNode *node);
	// 适配empty
	bool empty(void) const;
	// 查找url对应的处理接口
	const sdk_app_node_fn find_url_mapped_function(const OperationType &method, const std::string &url) const;
};

// sdk节点
class SdkTreeNode
{
public:
	int method_;						///< 节点方法
	std::string url_;					///< 节点url
	sdk_app_node_fn fn_;				///< 节点处理接口
	SdkTreeTable next_;					///< 下级节点,是一个表

	// 完整参数
	SdkTreeNode(const int &method, const std::string &url, const sdk_app_node_fn fn, const SdkTreeTable &table);
	// 下级表为空，填入nullptr
	SdkTreeNode(const int &method, const std::string &url, const sdk_app_node_fn fn, void *ptr);
	// 下级表为空，不填参数
	SdkTreeNode(const int &method, const std::string &url, const sdk_app_node_fn fn);
};

// sdk url
#define sdk_url(method, url, fn, next) new SdkTreeNode(method, url, fn, next)

// FIXME:现在是直接把指针放到表里面，如果是局部变量或者被堆变量被释放?

// 添加sdk tree(默认到根)
const SdkTreeNode *add_sdk_tree_node(const SdkTreeNode *node);
// 添加到指定的表当中
const SdkTreeNode *add_sdk_tree_node(const SdkTreeNode *node, SdkTreeTable &table);
// 作为指定节点的子节点
const SdkTreeNode *append_sdk_tree_node(const SdkTreeNode *node, SdkTreeNode &father);

// 模块添加sdk tree，一个模块(文件)只能添加一个
#define MODULE_ADD_SDK_TREE(method, url, fn, next) \
	static std::vector<const SdkTreeNode *> ___sdk_tree___{add_sdk_tree_node(sdk_url(method, url, fn, next))}

