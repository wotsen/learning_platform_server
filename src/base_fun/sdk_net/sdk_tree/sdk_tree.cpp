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
#include "_sdk_tree.h"

SdkTreeTable::SdkTreeTable() {}

// 拷贝构造
SdkTreeTable::SdkTreeTable(const SdkTreeTable &table) : table_(table.table_) {}

// 支持初始化列表 {} 格式
SdkTreeTable::SdkTreeTable(const std::initializer_list<SdkTreeNode *> &li)
{
    for (auto it : li)
    {
        table_.push_back(it);
    }
}

// 适配for i : v 迭代
typename SdkTreeTable::sdk_tree_table_t::iterator SdkTreeTable::begin() const
{
    return table_.begin();
}

typename SdkTreeTable::sdk_tree_table_t::iterator SdkTreeTable::end() const
{
    return table_.end();
}

// 适配empty
bool SdkTreeTable::empty(void) const
{
    return table_.empty();
}

// 适配vector push_back
void SdkTreeTable::push_back(const SdkTreeNode *node)
{
    table_.push_back(node);
}

// 查找url对应的处理接口
const sdk_app_node_fn SdkTreeTable::find_url_mapped_function(const OperationType &method, const std::string &url) const
{
    if (table_.empty()) { return nullptr; }

    sdk_app_node_fn fn = nullptr;

    for (auto item : table_)
    {
        if (item->url_.empty()) { continue; }

        std::smatch sm;
        // 路径当中忘了加^起始则添加，用于正则表达式
        std::regex pat(item->url_[0] == '^' ? item->url_ : ("^" + item->url_));

        // 路径未匹配或者method未匹配则跳过
        if (!std::regex_search(url, sm, pat) || !method_valid(method, item->method_))
        {
            continue;
        }

        // 后缀为空则完全匹配
        if (sm.suffix().str().empty()) { return item->fn_; }

        // 有后缀则查找url下级
        fn = item->next_.find_url_mapped_function(method, sm.suffix().str());
        if (fn) { return fn; }
    }

    return nullptr;
}

SdkTreeNode::SdkTreeNode(const int &method, const std::string &url, const sdk_app_node_fn fn, const SdkTreeTable &table) :
    method_(method), url_(url), fn_(fn), next_(table) {}

SdkTreeNode::SdkTreeNode(const int &method, const std::string &url, const sdk_app_node_fn fn, void *ptr) :
    method_(method), url_(url), fn_(fn) {}

SdkTreeNode::SdkTreeNode(const int &method, const std::string &url, const sdk_app_node_fn fn) :
    method_(method), url_(url), fn_(fn) {}

/**
 * @brief 添加sdk 树节点
 * 
 * @param node 
 * @return const SdkTreeNode* 
 */
const SdkTreeNode *add_sdk_tree_node(const SdkTreeNode *node)
{
    if (node)
    {
        // 冲突检测不做，以先查找到的为准(先添加)
	    get_sdk_tree_root().push_back(node);
    }
	
	return node;
}

// 添加到指定的表当中
const SdkTreeNode *add_sdk_tree_node(const SdkTreeNode *node, SdkTreeTable &table)
{
    if (node)
    {
        table.push_back(node);
    }

    return node;
}

// 作为指定节点的子节点
const SdkTreeNode *append_sdk_tree_node(const SdkTreeNode *node, SdkTreeNode &father)
{
    if (node)
    {
        father.next_.push_back(node);
    }

    return node;
}
