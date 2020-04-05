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

SdkRouterNode::SdkRouterNode(const std::initializer_list<std::shared_ptr<SdkRouterNode>> &li) : url_(""),
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
							 const std::shared_ptr<SdkRouterNode> &father) : url_(url),
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
							 const std::vector<std::shared_ptr<SdkRouterNode>> &next_li) : url_(url),
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
							 const std::initializer_list<std::shared_ptr<SdkRouterNode>> &next_li) : url_(url),
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
							 const std::shared_ptr<SdkRouterNode> &father,
							 const std::vector<std::shared_ptr<SdkRouterNode>> &next_li) : url_(url),
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
							 const std::shared_ptr<SdkRouterNode> &father,
							 const std::initializer_list<std::shared_ptr<SdkRouterNode>> &next_li) : url_(url),
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

bool SdkRouterNode::append(const std::shared_ptr<SdkRouterNode> &node)
{
	next_.push_back(node);
	return true;
}

bool SdkRouterNode::append(const std::vector<std::shared_ptr<SdkRouterNode>> &next_li)
{
	for (auto item : next_li)
	{
		next_.push_back(item);
	}
	return true;
}

bool SdkRouterNode::mount(const std::shared_ptr<SdkRouterNode> &father)
{
	if (father)
	{
		father_ = father;
	}
	return true;
}

std::vector<std::shared_ptr<SdkRouterNode>>::iterator SdkRouterNode::begin()
{
	return next_.begin();
}

std::vector<std::shared_ptr<SdkRouterNode>>::iterator SdkRouterNode::end()
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

std::shared_ptr<SdkRouterNode> create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method,
	const std::shared_ptr<SdkRouterNode> &father,
	const std::vector<std::shared_ptr<SdkRouterNode>> &next_li)
{
	return std::make_shared<SdkRouterNode>(url, fun, method, father, next_li);
}

std::shared_ptr<SdkRouterNode> create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method,
	const std::shared_ptr<SdkRouterNode> &father,
	const std::initializer_list<std::shared_ptr<SdkRouterNode>> &next_li)
{
	return std::make_shared<SdkRouterNode>(url, fun, method, father, next_li);
}

std::shared_ptr<SdkRouterNode> create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method,
	const std::shared_ptr<SdkRouterNode> &father)
{
	return std::make_shared<SdkRouterNode>(url, fun, method, father, std::vector<std::shared_ptr<SdkRouterNode>>());
}

std::shared_ptr<SdkRouterNode> create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method,
	const std::vector<std::shared_ptr<SdkRouterNode>> &next_li)
{
	return std::make_shared<SdkRouterNode>(url, fun, method, nullptr, next_li);
}

std::shared_ptr<SdkRouterNode> create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method,
	const std::initializer_list<std::shared_ptr<SdkRouterNode>> &next_li)
{
	return std::make_shared<SdkRouterNode>(url, fun, method, nullptr, next_li);
}

std::shared_ptr<SdkRouterNode> create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method)
{
	return std::make_shared<SdkRouterNode>(url, fun, method, nullptr, std::vector<std::shared_ptr<SdkRouterNode>>());
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

static SdkRouterNode root_router = {
	create_router("", DO_NOTHING, GET,
					{
						create_router("/", DO_NOTHING, GET),
					}),
};

bool register_router(const std::shared_ptr<SdkRouterNode> &router)
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

static std::shared_ptr<SdkRouterNode> index_node = create_router("/index", DO_NOTHING, GET);
static SdkRouterNode index_node2 = {"/index2", DO_NOTHING, GET};

SDK_REGISTER_ROUTER(index_node);
SDK_REGISTER_ROUTER(index_node2);

} // namespace wotsen
