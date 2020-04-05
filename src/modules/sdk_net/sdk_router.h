/**
 * @file sdk_router.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-04-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __wotsenLearnPlatSrv_SDK_ROUTER_H__
#define __wotsenLearnPlatSrv_SDK_ROUTER_H__

#include <vector>
#include <iterator>
#include "sdk_request.h"

namespace wotsen
{

class SdkRouterNode;

using sdk_response_fun_t = bool (*)(SdkRequest &);
using sdk_router_node_ptr_t = std::shared_ptr<SdkRouterNode>;
using sdk_father_router_node = std::shared_ptr<SdkRouterNode>;
using sdk_children_router_node = std::shared_ptr<SdkRouterNode>;

class SdkRouterNode
{
public:
	SdkRouterNode(const SdkRouterNode &node);
	SdkRouterNode(const std::initializer_list<std::shared_ptr<SdkRouterNode>> &li);
	SdkRouterNode(const std::string &url,
				  sdk_response_fun_t fun,
				  const uint32_t &method);
	SdkRouterNode(const std::string &url,
				  sdk_response_fun_t fun,
				  const uint32_t &method,
				  const std::shared_ptr<SdkRouterNode> &father);
	SdkRouterNode(const std::string &url,
				  sdk_response_fun_t fun,
				  const uint32_t &method,
				  const std::vector<std::shared_ptr<SdkRouterNode>> &next_li);
	SdkRouterNode(const std::string &url,
				  sdk_response_fun_t fun,
				  const uint32_t &method,
				  const std::initializer_list<std::shared_ptr<SdkRouterNode>> &next_li);
	SdkRouterNode(const std::string &url,
				  sdk_response_fun_t fun,
				  const uint32_t &method,
				  const std::shared_ptr<SdkRouterNode> &father,
				  const std::vector<std::shared_ptr<SdkRouterNode>> &next_li);
	SdkRouterNode(const std::string &url,
				  sdk_response_fun_t fun,
				  const uint32_t &method,
				  const std::shared_ptr<SdkRouterNode> &father,
				  const std::initializer_list<std::shared_ptr<SdkRouterNode>> &next_li);

public:
	bool append(const SdkRouterNode &node);
	bool append(const std::shared_ptr<SdkRouterNode> &node);
	bool append(const std::vector<std::shared_ptr<SdkRouterNode>> &next_li);
	bool mount(const std::shared_ptr<SdkRouterNode> &father);

	std::vector<std::shared_ptr<SdkRouterNode>>::iterator begin();
	std::vector<std::shared_ptr<SdkRouterNode>>::iterator end();
	bool empty(void) { return next_.empty(); }
	void show(const std::string& prev);

	friend sdk_response_fun_t sdk_router_lookup(const std::string &url, const OperationType &method);

private:
	bool check_method(const OperationType &method);
	sdk_response_fun_t router_lookup(const std::string &url, const OperationType &method);

private:
	std::string url_;
	sdk_response_fun_t fun_;
	uint32_t method_;
	std::shared_ptr<SdkRouterNode> father_;
	std::vector<std::shared_ptr<SdkRouterNode>> next_;
};

/**
 * @brief Create a router object
 * 
 * @param url 
 * @param fun 处理接口
 * @param method 方法
 * @param father 父节点
 * @param next_li 子节点列表
 * @return std::shared_ptr<SdkRouterNode> 
 */
std::shared_ptr<SdkRouterNode> create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method,
	const std::shared_ptr<SdkRouterNode> &father,
	const std::vector<std::shared_ptr<SdkRouterNode>> &next_li);

std::shared_ptr<SdkRouterNode> create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method,
	const std::shared_ptr<SdkRouterNode> &father,
	const std::initializer_list<std::shared_ptr<SdkRouterNode>> &next_li);

std::shared_ptr<SdkRouterNode> create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method,
	const std::shared_ptr<SdkRouterNode> &father);

std::shared_ptr<SdkRouterNode> create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method,
	const std::vector<std::shared_ptr<SdkRouterNode>> &next_li);

std::shared_ptr<SdkRouterNode> create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method,
	const std::initializer_list<std::shared_ptr<SdkRouterNode>> &next_li);

std::shared_ptr<SdkRouterNode> create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method);

// 路由注册
bool register_router(const SdkRouterNode &router);
bool register_router(const std::shared_ptr<SdkRouterNode> &router);

bool is_support_sdk_method(const OperationType &method);
bool is_sdk_url_valid(const std::string &url);
sdk_response_fun_t sdk_router_lookup(const std::string &url, const OperationType &method);
void show_sdk_router_map(void);

// 路由注册，可用于全局注册
#define SDK_REGISTER_ROUTER(router) \
	static bool ___wotsen_learn_plat_srv_appmodule_router__##router = register_router(router)

} // namespace wotsen

#endif // !__wotsenLearnPlatSrv_SDK_ROUTER_H__
