/**
 * @file sdk_router_distribution.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-04-05
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#define LOG_TAG "SDK_ROUTER_DISTRIBUTION"

#include <easylogger/easylogger_setup.h>
#include "sdk_router_distribution.h"

namespace wotsen
{

bool sdk_router_distribution(SdkRequest &sdk_request)
{
	const Body &body = sdk_request.request.body();

    if (!is_support_sdk_method(body.method()))
    {
        log_i("not support method : %d\n", body.method());
        sdk_set_result(ResponseResult::ERROR, "not support method", sdk_request.response);

        return false;
    }

	std::string url = body.url();

    // 校验url合法性
    if (!is_sdk_url_valid(url))
    {
        log_i("url not valid : %s\n", url.c_str());
        sdk_set_result(ResponseResult::ERROR, "invalid url", sdk_request.response);

        return false;
    }

    // 查找路径对应的处理接口
	sdk_response_fun_t fn = sdk_router_lookup(url, body.method());

    if (!fn)
    {
        log_i("url not find : %s\n", url.c_str());
        sdk_set_result(ResponseResult::ERROR, "url not find or not support", sdk_request.response);

        return false;
    }

	return fn(sdk_request);
}

} // !namespace wotsen
