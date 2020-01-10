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
#include <regex>
#include "sdk_tree.h"

static inline bool method_valid(const OperationType &method, const int &methods)
{
    return method & methods;
}

static inline bool is_support_method(const OperationType &method)
{
    switch (method)
    {
        case SDK_GET:
        case SDK_PUT:
        case SDK_POST:
        case SDK_DELETE:
            return true;
        default:
            return false;
    }
}

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

MODULE_ADD_SDK_TREE(SDK_GET|SDK_PUT|SDK_POST|SDK_DELETE, "/", nullptr, base_sdk_tree);

// 递归查找路径
sdk_app_node_fn _search_url_function(const OperationType &method, const std::string &url, std::vector<const sdk_tree_node *> tree)
{
	if (tree.empty()) { return nullptr; }

	sdk_app_node_fn fn = nullptr;

	for (auto item : __sdk_tree_root)
	{
        if (item->url.empty())
        {
            continue;
        }

        std::smatch sm;
        // 路径当中忘了加^起始则添加
        std::regex pat(item->url[0] == '^' ? item->url : ("^" + item->url));

        // 路径未匹配或者method未匹配则跳过
        if (!std::regex_search(url, sm, pat) || !method_valid(method, item->method))
        {
            continue;
        }

        // 后缀为空则完全匹配
        if (sm.suffix().str().empty())
        {
			return item->fn;
        }

        // 有后缀则查找url下级
        fn = _search_url_function(method, sm.suffix().str(), item->next);
        if (fn) { return fn; }
	}

	return nullptr;
}

// 查找url对应的处理接口
sdk_app_node_fn find_url_function(const OperationType &method, const std::string &url)
{
	if (__sdk_tree_root.empty()) { return nullptr; }

	return _search_url_function(method, url, __sdk_tree_root);
}

// sdk 路径解析
bool sdk_tree_do(struct sdk_net_interface &interface, Sdk &sdk_req, Sdk &sdk_res)
{
	Body *body = sdk_req.mutable_body();

	OperationType method = body->method();
    if (!is_support_method(method))
    {
        return false;
    }

	std::string url = body->url();

    // 非法字符(除/a到zA到Z_-0到9之外的)
    std::regex pat("[^/a-zA-Z_\\-0-9]");

    if (std::regex_search(url, pat))
    {
        return false;
    }

	sdk_app_node_fn fn = find_url_function(method, url);

	return fn ? fn(interface, sdk_req, sdk_res) : false;
}
