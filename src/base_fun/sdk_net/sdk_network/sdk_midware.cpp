/**
 * @file sdk_midware.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#define LOG_TAG "SDK_MIDWARE"

#include <vector>
#include <easylogger/easylogger_setup.h>
#include "sdk_midware.h"

// 中间件管理
class SdkMidWareManager {
public:
	SdkMidWareManager () {};

	~SdkMidWareManager() {
		midwares_.clear();
	}

	// 注册中间件
	void register_midware(const std::shared_ptr<ISdkMidWare> &mid);

	// 处理中间件
	bool proc_midwares(struct sdk_net_interface &sdk_interface, const Sdk &sdk_req, Sdk &sdk_res) const;

	// 中间件是否存在
	bool is_midware_exist(const std::string &name);

private:
	// 中间件列表
	std::vector<std::shared_ptr<ISdkMidWare>> midwares_;
};

// 中间件管理器
static SdkMidWareManager midware_mannager;

// 中间件是否使能
bool ISdkMidWare::is_enable(void) const
{
	return enable_;
}

// 中间件是否匹配
bool ISdkMidWare::is_match(const std::string &name) const
{
	return name == name_;
}

// 中间件名称
const std::string &ISdkMidWare::name(void) const
{
	return name_;
}

// 仿函数，用于中间件实际处理
bool ISdkMidWare::operator () 
		(struct sdk_net_interface &sdk_interface, const insider::sdk::Sdk &sdk_req, insider::sdk::Sdk &sdk_res) const
{
	// 如果使用函数注册，则执行函数接口
	if (fn_) { return fn_(sdk_interface, sdk_req, sdk_res); }
	return true;
}

// 注册中间件
void SdkMidWareManager::register_midware(const std::shared_ptr<ISdkMidWare> &mid)
{
	// 不能重复注册
	if (is_midware_exist(mid->name())) { return; }

	midwares_.push_back(mid);
}

// 处理中间件
bool SdkMidWareManager::proc_midwares(struct sdk_net_interface &sdk_interface, const Sdk &sdk_req, Sdk &sdk_res) const
{
	for (auto &item : midwares_) {
		if (!item->is_enable()) { continue; }

		if (!(*item)(sdk_interface, sdk_req, sdk_res)) {
			log_d("proc midware %s failed\n", item->name().c_str());

			sdk_set_result(ResponseResult::ERROR, "error module : " + item->name(), sdk_res);

			// 一个中间件处理失败不再向后传递
			return false;
		}
		
		log_d("proc midware %s success\n", item->name().c_str());
	}

	return true;
}

// 中间件是否存在
bool SdkMidWareManager::is_midware_exist(const std::string &name)
{
	for (auto &item : midwares_) {
		if (item->is_match(name)) { return true; }
	}

	return false;
}

// 中间件注册
void register_sdk_midware(const std::shared_ptr<ISdkMidWare> &mid)
{
	midware_mannager.register_midware(mid);
}

/**
 * @brief 处理中间件
 * 
 * @param type sdk网络数据类型
 * @param req sdk数据包原始数据请求
 * @param req_msg sdk请求协议数据
 * @param res_msg sdk应答协议数据
 * @return true 成功
 * @return false 失败
 */
bool sdk_midware_do(struct sdk_net_interface &sdk_interface, const Sdk &sdk_req, Sdk &sdk_res)
{
	return midware_mannager.proc_midwares(sdk_interface, sdk_req, sdk_res);
}