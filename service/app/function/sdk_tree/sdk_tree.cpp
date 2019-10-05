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

typedef bool (*sdk_app_node_fn)(void);

struct sdk_app_node
{
	std::string method;
	std::string url;
	sdk_app_node_fn fn;
	struct sdk_app_node *next;
};

/*
vector SDK_TREE;

xxx(method, url, ...) {
	va_list arg
	arg.size
	arg chech type

	for i in arg :
		if ()
}

#define SDK_APP_DO(method, url, args...) 

SDK_APP_DO("GET", "/", fun) {
	return ...;
}

{
	{ "GET", "url", func, next }
};

*/