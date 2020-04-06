/**
 * @file sdk_midware_manage.cpp
 * @author yuwangliang (astralrovers@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-02-16
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <loguru.hpp>
#include "sdk_midware.h"
#include "sdk_midware_manage.h"

namespace wotsen
{

// 获取中间件列表
sdk_midware_list_t &SdkMidWareManager::midwares(void)
{
    return midwares_;
}

// 注册中间件
void SdkMidWareManager::register_midware(const sdk_midware_ptr_t &mid)
{
	// 不能重复注册
	if (is_midware_exist(mid->name())) { return; }

	midwares_.push_back(mid);
}

// 中间件是否存在
bool SdkMidWareManager::is_midware_exist(const std::string &name)
{
	for (auto &item : midwares_) {
		if (item->is_match(name)) { return true; }
	}

	return false;
}

std::shared_ptr<SdkMidWareManager> SdkMidWareManager::sdk_midware_manager(void)
{
	static std::shared_ptr<SdkMidWareManager> manager(new SdkMidWareManager);

	return manager;
}

bool sdk_midware_function(SdkRequest &sdk_request)
{
	auto midwares = SdkMidWareManager::sdk_midware_manager()->midwares();

    for (auto item : midwares) {
		if (!item->is_enable()) { continue; }

		if (!(*item)(sdk_request)) {
			LOG_F(ERROR, "proc midware %s failed\n", item->name().c_str());

			sdk_set_result(ResponseResult::ERROR, "error module : " + item->name(), sdk_request.response);

			// 一个中间件处理失败不再向后传递
			return false;
		}

		LOG_F(INFO, "proc midware %s success\n", item->name().c_str());
	}

	return true;
}

// 中间件注册
void register_sdk_midware(const std::shared_ptr<ISdkMidWare> &mid)
{
	SdkMidWareManager::sdk_midware_manager()->register_midware(mid);
}

} // !namespace wotsen
