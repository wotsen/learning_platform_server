/**
 * @file sdk_tree_do.cpp
 * @author yuwangliang (astralrovers@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-02-15
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#define LOG_TAG "SDK_TREE_DO"

#include <easylogger/easylogger_setup.h>
#include "_sdk_tree.h"
#include "sdk_tree_do.h"

/********************************************节点树方法top*****************************************************/
static bool _index_fn(struct sdk_net_interface &sdk_interface, const Sdk &sdk_req, Sdk &sdk_res);
/********************************************节点树方法bot*****************************************************/

// 基础树(默认路径)
static SdkTreeTable base_sdk_tree = {
	new_sdk_node(SDK_GET, "index", _index_fn, nullptr),
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

// 遍历树
static void _sdk_tree_map(const SdkTreeTable &tree)
{
    if (tree.empty()) { return; }

    for (auto item : tree)
    {
        if (item->url_.empty()) { continue; }

        std::cout << "method" << item->method_ << " | url : " << item->url_ << std::endl;

        _sdk_tree_map(item->next_);
    }
}

// sdk树路径统计
void sdk_tree_map(void)
{
    _sdk_tree_map(get_sdk_tree_root());
}

// 查找url对应的处理接口
static sdk_app_node_fn find_url_function(const OperationType &method, const std::string &url)
{
    return get_sdk_tree_root().find_url_mapped_function(method, url);
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

    // 校验url合法性
    if (!is_url_valid(url))
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