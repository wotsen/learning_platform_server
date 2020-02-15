/**
 * @file _sdk_tree.cpp
 * @author yuwangliang (astralrovers@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-02-15
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <regex>
#include "_sdk_tree.h"

// sdk tree根节点
static SdkTreeTable __sdk_tree_root;

// 方法是否合法（包含关系）
bool method_valid(const OperationType &method, const int &methods)
{
    return method & methods;
}

// 请求方法是否支持
bool is_support_method(const OperationType &method)
{
    switch (method)
    {
        case OperationType::GET:
        case OperationType::PUT:
        case OperationType::POST:
        case OperationType::DELETE:
            return true;
        default:
            return false;
    }
}

// 校验url
bool is_url_valid(const std::string &url)
{
    // 非法字符(除/a到zA到Z_-0到9之外的)
    std::regex pat("[^/a-zA-Z_\\-0-9]");

    return !std::regex_search(url, pat);
}

// 获取根节点
SdkTreeTable &get_sdk_tree_root(void)
{
    return __sdk_tree_root;
}