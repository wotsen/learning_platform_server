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

// 路由节点
class SdkRouterNode
{
public:
	SdkRouterNode(const SdkRouterNode &node);

	SdkRouterNode(const std::initializer_list<sdk_router_node_ptr_t> &li);

	SdkRouterNode(const std::string &url,
				  sdk_response_fun_t fun,
				  const uint32_t &method);

	SdkRouterNode(const std::string &url,
				  sdk_response_fun_t fun,
				  const uint32_t &method,
				  const sdk_router_node_ptr_t &father);

	SdkRouterNode(const std::string &url,
				  sdk_response_fun_t fun,
				  const uint32_t &method,
				  const std::vector<sdk_router_node_ptr_t> &next_li);

	SdkRouterNode(const std::string &url,
				  sdk_response_fun_t fun,
				  const uint32_t &method,
				  const std::initializer_list<sdk_router_node_ptr_t> &next_li);

	SdkRouterNode(const std::string &url,
				  sdk_response_fun_t fun,
				  const uint32_t &method,
				  const sdk_router_node_ptr_t &father,
				  const std::vector<sdk_router_node_ptr_t> &next_li);

	SdkRouterNode(const std::string &url,
				  sdk_response_fun_t fun,
				  const uint32_t &method,
				  const sdk_router_node_ptr_t &father,
				  const std::initializer_list<sdk_router_node_ptr_t> &next_li);

public:
	// 追加子节点
	bool append(const SdkRouterNode &node);
	bool append(const sdk_router_node_ptr_t &node);
	bool append(const std::vector<sdk_router_node_ptr_t> &next_li);
	// 挂载到父节点
	bool mount(const sdk_router_node_ptr_t &father);

	// 迭代
	std::vector<sdk_router_node_ptr_t>::iterator begin();
	std::vector<sdk_router_node_ptr_t>::iterator end();

	// 是否为空
	bool empty(void) { return next_.empty(); }

	// 显示路由信息
	void show(const std::string& prev);

	// 路由查找
	friend sdk_response_fun_t sdk_router_lookup(const std::string &url, const OperationType &method);

private:
	// 检查方法是否合法
	bool check_method(const OperationType &method);
	// 路由查找
	sdk_response_fun_t router_lookup(const std::string &url, const OperationType &method);

private:
	std::string url_;							///< url
	sdk_response_fun_t fun_;					///< 处理接口
	uint32_t method_;							///< 支持方法
	sdk_router_node_ptr_t father_;				///< 父节点
	std::vector<sdk_router_node_ptr_t> next_;	///< 子节点列表
};

/**
 * @brief Create a router object
 * 
 * @param url 
 * @param fun 处理接口
 * @param method 方法
 * @param father 父节点
 * @param next_li 子节点列表
 * @return sdk_router_node_ptr_t 
 */
sdk_router_node_ptr_t create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method,
	const sdk_router_node_ptr_t &father,
	const std::vector<sdk_router_node_ptr_t> &next_li);

sdk_router_node_ptr_t create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method,
	const sdk_router_node_ptr_t &father,
	const std::initializer_list<sdk_router_node_ptr_t> &next_li);

sdk_router_node_ptr_t create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method,
	const sdk_router_node_ptr_t &father);

sdk_router_node_ptr_t create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method,
	const std::vector<sdk_router_node_ptr_t> &next_li);

sdk_router_node_ptr_t create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method,
	const std::initializer_list<sdk_router_node_ptr_t> &next_li);

sdk_router_node_ptr_t create_router(
	const std::string &url,
	sdk_response_fun_t fun,
	const uint32_t &method);

// 路由注册
bool register_router(const SdkRouterNode &router);
bool register_router(const sdk_router_node_ptr_t &router);

// sdk是否支持该方法
bool is_support_sdk_method(const OperationType &method);
// url是否合法
bool is_sdk_url_valid(const std::string &url);
// 路由查找
sdk_response_fun_t sdk_router_lookup(const std::string &url, const OperationType &method);
// 打印路由表
void show_sdk_router_map(void);

// 路由注册，可用于全局注册
#define SDK_REGISTER_ROUTER(router) \
	static bool ___wotsen_learn_plat_srv_appmodule_router__##router = register_router(router)

} // namespace wotsen

#endif // !__wotsenLearnPlatSrv_SDK_ROUTER_H__
