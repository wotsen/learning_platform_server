/**
 * @file sdk_tree.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-09-28
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include <iostream>
#include "sdk_tree.h"

// TODO:url和接口对应，可全匹配，可正则匹配，可分级，可导入，可使用中间件

// sdk tree根节点
static std::vector<const struct sdk_tree_node *> __sdk_tree_root;

/**
 * @brief 冲突检测
 * 
 * @return true 
 * @return false 
 */
bool sdk_tree_conflict(void)
{
	return false;
}

void sdk_tree_map(void)
{
	// TODO:sdk树路径统计
}

/**
 * @brief 添加sdk 树节点
 * 
 * @param sdk_tree 
 * @return const struct sdk_tree_node* 
 */
const struct sdk_tree_node *add_sdk_tree(const struct sdk_tree_node *sdk_tree)
{
	// FIXME:冲突检测
	__sdk_tree_root.push_back(sdk_tree);

	return sdk_tree;
}

// 基础树(默认路径)
static std::vector<const struct sdk_tree_node *> base_sdk_tree = {
	sdk_url(SDK_GET, "index", nullptr, nullptr),
};

MODULE_ADD_SDK_TREE(SDK_GET, "/", nullptr, base_sdk_tree);

// 递归查找路径
sdk_app_node_fn _search_url_function(const OperationType &method, const std::string &url, std::vector<const sdk_tree_node *> tree)
{
	if (tree.empty())
	{
		return nullptr;
	}

	sdk_app_node_fn fn = nullptr;

	for (auto item : __sdk_tree_root)
	{
		// 匹配到url,FIXME:使用正则匹配
		if (url == item->url && method == item->method)
		{
			return item->fn;
		}

		// url更长
		if (url.size() > item->url.size())
		{
			// 截取配置表中的url
			std::string pre_url = url.substr(0, item->url.size());
			// 截取url的下级路径
			std::string next_url = url.substr(item->url.size());

			// url前缀匹配
			if (pre_url == item->url && method == item->method)
			{
				// 查找url下级
				fn = _search_url_function(method, next_url, item->next);
				if (fn) { return fn; }
			}
		}

		// 没配到则查找下一条路径
	}

	return nullptr;
}

// 查找url对应的处理接口
sdk_app_node_fn find_url_function(const OperationType &method, const std::string &url)
{
	if (__sdk_tree_root.empty())
	{
		return nullptr;
	}

	return _search_url_function(method, url, __sdk_tree_root);
}

// sdk 路径解析
bool sdk_tree_do(struct sdk_net_interface &interface, Sdk &sdk_req, Sdk &sdk_res)
{
	Body *body = sdk_req.mutable_body();

	OperationType method = body->method();
	std::string url = body->url();

	sdk_app_node_fn fn = find_url_function(method, url);

	if (!fn)
	{
		return false;
	}

	return fn(interface, sdk_req, sdk_res);
}