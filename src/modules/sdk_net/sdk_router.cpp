/**
 * @file sdk_router.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-04-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#define LOG_TAG "SDK_ROUTER_DISTRIBUTION"

#include <easylogger/easylogger_setup.h>
#include <iostream>
#include <regex>
#include "sdk_router.h"

namespace wotsen
{

SdkRouterNode::SdkRouterNode(const SdkRouterNode &node) : url_(node.url_),
														  fun_(node.fun_),
														  method_(node.method_),
														  father_(node.father_),
														  next_(node.next_)
{
}

SdkRouterNode::SdkRouterNode(const std::initializer_list<sdk_router_node_ptr_t> &li) : url_(""),
																								fun_(nullptr),
																								method_(GET),
																								next_(li)
{
}

SdkRouterNode::SdkRouterNode(const std::string &url,
							 sdk_response_fun_t fun,
							 const uint32_t &method) : url_(url),
													   fun_(fun),
													   method_(method)
{
}

SdkRouterNode::SdkRouterNode(const std::string &url,
							 sdk_response_fun_t fun,
							 const uint32_t &method,
							 const sdk_router_node_ptr_t &father) : url_(url),
																			 fun_(fun),
																			 method_(method)
{
	if (father)
	{
		father_ = father;
	}
}

SdkRouterNode::SdkRouterNode(const std::string &url,
							 sdk_response_fun_t fun,
							 const uint32_t &method,
							 const std::vector<sdk_router_node_ptr_t> &next_li) : url_(url),
																						   fun_(fun),
																						   method_(method)
{
	for (auto item : next_li)
	{
		next_.push_back(item);
	}
}

SdkRouterNode::SdkRouterNode(const std::string &url,
							 sdk_response_fun_t fun,
							 const uint32_t &method,
							 const std::initializer_list<sdk_router_node_ptr_t> &next_li) : url_(url),
																									 fun_(fun),
																									 method_(method)
{
	for (auto item : next_li)
	{
		next_.push_back(item);
	}
}

SdkRouterNode::SdkRouterNode(const std::string &url,
							 sdk_response_fun_t fun,
							 const uint32_t &method,
							 const sdk_router_node_ptr_t &father,
							 const std::vector<sdk_router_node_ptr_t> &next_li) : url_(url),
																						   fun_(fun),
																						   method_(method)
{
	if (father)
	{
		father_ = father;
	}

	for (auto item : next_li)
	{
		next_.push_back(item);
	}
}

SdkRouterNode::SdkRouterNode(const std::string &url,
							 sdk_response_fun_t fun,
							 const uint32_t &method,
							 const sdk_router_node_ptr_t &father,
							 const std::initializer_list<sdk_router_node_ptr_t> &next_li) : url_(url),
																									 fun_(fun),
																									 method_(method)
{
	if (father)
	{
		father_ = father;
	}

	for (auto item : next_li)
	{
		next_.push_back(item);
	}
}

bool SdkRouterNode::append(const SdkRouterNode &node)
{
	next_.push_back(std::make_shared<SdkRouterNode>(node));
	return true;
}

bool SdkRouterNode::append(const sdk_router_node_ptr_t &node)
{
	next_.push_back(node);
	return true;
}

bool SdkRouterNode::append(const std::vector<sdk_router_node_ptr_t> &next_li)
{
	for (auto item : next_li)
	{
		next_.push_back(item);
	}
	return true;
}

bool SdkRouterNode::mount(const sdk_router_node_ptr_t &father)
{
	if (father)
	{
		father_ = father;
	}
	return true;
}

std::vector<sdk_router_node_ptr_t>::iterator SdkRouterNode::begin()
{
	return next_.begin();
}

std::vector<sdk_router_node_ptr_t>::iterator SdkRouterNode::end()
{
	return next_.end();
}

bool SdkRouterNode::check_method(const OperationType &method)
{
	return method & method_;
}

sdk_response_fun_t SdkRouterNode::router_lookup(const std::string &url, const OperationType &method)
{
	sdk_response_fun_t fn = nullptr;

	std::smatch sm;
	// 路径当中忘了加^起始则添加，用于正则表达式
	std::regex pat(url_[0] == '^' ? url_ : ("^" + url_));

	// 匹配当前节点路径前缀
	if (!std::regex_search(url, sm, pat))
	{
		return nullptr;
	}

	// 后缀为空则完全匹配
    if (sm.suffix().str().empty()) {
		// 检查当前节点是否支持该方法
		if (!check_method(method)) {
			return nullptr;
		}
		return fun_;
	}

	for (auto item : next_)
	{
		// 使用后缀进行下级匹配
		fn = item->router_lookup(sm.suffix().str(), method);
		if (fn) { return fn; }
	}

	return nullptr;
}

void SdkRouterNode::show(const std::string& prev)
{
	std::cout << prev << url_ << std::endl;

	for (auto item : next_)
	{
		item->show(prev + url_);
	}
}

sdk_router_node_ptr_t create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method,
	const sdk_router_node_ptr_t &father,
	const std::vector<sdk_router_node_ptr_t> &next_li)
{
	return std::make_shared<SdkRouterNode>(url, fun, method, father, next_li);
}

sdk_router_node_ptr_t create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method,
	const sdk_router_node_ptr_t &father,
	const std::initializer_list<sdk_router_node_ptr_t> &next_li)
{
	return std::make_shared<SdkRouterNode>(url, fun, method, father, next_li);
}

sdk_router_node_ptr_t create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method,
	const sdk_router_node_ptr_t &father)
{
	return std::make_shared<SdkRouterNode>(url, fun, method, father, std::vector<sdk_router_node_ptr_t>());
}

sdk_router_node_ptr_t create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method,
	const std::vector<sdk_router_node_ptr_t> &next_li)
{
	return std::make_shared<SdkRouterNode>(url, fun, method, nullptr, next_li);
}

sdk_router_node_ptr_t create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method,
	const std::initializer_list<sdk_router_node_ptr_t> &next_li)
{
	return std::make_shared<SdkRouterNode>(url, fun, method, nullptr, next_li);
}

sdk_router_node_ptr_t create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method)
{
	return std::make_shared<SdkRouterNode>(url, fun, method, nullptr, std::vector<sdk_router_node_ptr_t>());
}

// 请求方法是否支持
bool is_support_sdk_method(const OperationType &method)
{
    switch (method)
    {
        case GET:
        case PUT:
        case POST:
        case DELETE:
            return true;
        default:
            return false;
    }
}

bool is_sdk_url_valid(const std::string &url)
{
	// 非法字符(除/a到zA到Z_-0到9之外的)
    std::regex pat("[^/a-zA-Z_\\-0-9]");

    return !std::regex_search(url, pat);
}

#define DO_NOTHING [](SdkRequest &) -> bool { std::cout << "root do nothing." << std::endl; return true; }

/********************************************************************************************************
 * !路由根节点
********************************************************************************************************/
static SdkRouterNode root_router = {
	create_router("", DO_NOTHING, GET,
					{
						create_router("/", DO_NOTHING, GET),
					}),
};

bool register_router(const sdk_router_node_ptr_t &router)
{
	return root_router.append(router);
}

bool register_router(const SdkRouterNode &router)
{
	return root_router.append(router);
}

sdk_response_fun_t sdk_router_lookup(const std::string &url, const OperationType &method)
{
	return root_router.router_lookup(url, method);
}

void show_sdk_router_map(void)
{
	root_router.show("");
}

/********************************************************************************************************
 * !测试节点
********************************************************************************************************/
static sdk_router_node_ptr_t index_node = create_router("/index", DO_NOTHING, GET);
static SdkRouterNode index_node2 = {"/index2", DO_NOTHING, GET};

SDK_REGISTER_ROUTER(index_node);
SDK_REGISTER_ROUTER(index_node2);

} // namespace wotsen
