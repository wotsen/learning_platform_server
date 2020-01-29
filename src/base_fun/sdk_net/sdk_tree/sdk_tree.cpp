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
#define LOG_TAG "SDK_TREE"

#include <iostream>
#include <regex>
#include <easylogger/easylogger_setup.h>
#include "sdk_tree.h"

// 方法是否合法（包含关系）
static inline bool method_valid(const OperationType &method, const int &methods);
// 请求方法是否支持
static inline bool is_support_method(const OperationType &method);
// 查找url对应的处理接口
static sdk_app_node_fn find_url_function(const OperationType &method, const std::string &url);
// 递归查找路径
static sdk_app_node_fn _search_url_function(const OperationType &method, const std::string &url, std::vector<const sdk_tree_node *> tree);
static void _sdk_tree_map(std::vector<const sdk_tree_node *> tree);

// sdk tree根节点
static std::vector<const struct sdk_tree_node *> __sdk_tree_root;

/********************************************节点树方法top*****************************************************/
static bool _index_fn(struct sdk_net_interface &sdk_interface, const Sdk &sdk_req, Sdk &sdk_res);
/********************************************节点树方法bot*****************************************************/

// 基础树(默认路径)
static std::vector<const struct sdk_tree_node *> base_sdk_tree = {
	sdk_url(SDK_GET, "index", _index_fn, nullptr),
};

// 添加根节点
MODULE_ADD_SDK_TREE(SDK_GET|SDK_PUT|SDK_POST|SDK_DELETE, "/", nullptr, base_sdk_tree);

/********************************************节点树方法top*****************************************************/
static bool _index_fn(struct sdk_net_interface &sdk_interface, const Sdk &sdk_req, Sdk &sdk_res)
{
    log_i("do nothing, i am '/index' node\n");
    return true;
}
/*********************************************节点树方法bot****************************************************/

// 方法是否合法（包含关系）
static inline bool method_valid(const OperationType &method, const int &methods)
{
    return method & methods;
}

// 请求方法是否支持
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

// 递归查找路径，查用正则表达式匹配
static sdk_app_node_fn _search_url_function(const OperationType &method, const std::string &url, std::vector<const sdk_tree_node *> tree)
{
	if (tree.empty()) { return nullptr; }

	sdk_app_node_fn fn = nullptr;

	for (auto item : tree)
	{
        if (item->url.empty()) { continue; }

        std::smatch sm;
        // 路径当中忘了加^起始则添加，用于正则表达式
        std::regex pat(item->url[0] == '^' ? item->url : ("^" + item->url));

        // 路径未匹配或者method未匹配则跳过
        if (!std::regex_search(url, sm, pat) || !method_valid(method, item->method))
        {
            continue;
        }

        // 后缀为空则完全匹配
        if (sm.suffix().str().empty()) { return item->fn; }

        // 有后缀则查找url下级
        fn = _search_url_function(method, sm.suffix().str(), item->next);
        if (fn) { return fn; }
	}

	return nullptr;
}

// 查找url对应的处理接口
static sdk_app_node_fn find_url_function(const OperationType &method, const std::string &url)
{
	if (__sdk_tree_root.empty()) { return nullptr; }

	return _search_url_function(method, url, __sdk_tree_root);
}

static void _sdk_tree_map(std::vector<const sdk_tree_node *> tree)
{
    if (tree.empty()) { return; }

    for (auto item : tree)
    {
        if (item->url.empty()) { continue; }

        std::cout << "method" << item->method << " | url : " << item->url << std::endl;

        _sdk_tree_map(item->next);
    }
}

// sdk树路径统计
void sdk_tree_map(void)
{
    _sdk_tree_map(__sdk_tree_root);
}

/**
 * @brief 添加sdk 树节点
 * 
 * @param sdk_tree 
 * @return const struct sdk_tree_node* 
 */
const struct sdk_tree_node *add_sdk_tree(const struct sdk_tree_node *sdk_tree)
{
	// 冲突检测不做，以先查找到的为准(先添加)
	__sdk_tree_root.push_back(sdk_tree);

	return sdk_tree;
}

// sdk 路径解析
bool sdk_tree_do(struct sdk_net_interface &sdk_interface, const Sdk &sdk_req, Sdk &sdk_res)
{
	const Body &body = sdk_req.body();

    if (!is_support_method(body.method()))
    {
        log_i("not support method : %d\n", body.method());
        sdk_set_result(ResponseResult::ERROR, "not support method", sdk_res);

        return false;
    }

	std::string url = body.url();
    // 非法字符(除/a到zA到Z_-0到9之外的)
    std::regex pat("[^/a-zA-Z_\\-0-9]");

    // 校验url合法性
    if (std::regex_search(url, pat))
    {
        log_i("url not valid : %s\n", url.c_str());
        sdk_set_result(ResponseResult::ERROR, "invalid url", sdk_res);

        return false;
    }

    // 查找路径对应的处理接口
	sdk_app_node_fn fn = find_url_function(body.method(), url);

    if (!fn)
    {
        log_i("url not find : %s\n", url.c_str());
        sdk_set_result(ResponseResult::ERROR, "url not find or not support", sdk_res);

        return false;
    }

	return fn(sdk_interface, sdk_req, sdk_res);
}
