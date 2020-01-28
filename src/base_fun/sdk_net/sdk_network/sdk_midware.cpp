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

using namespace insider::sdk;

// 中间件管理
class SdkMidWareManager {
public:
	struct midware {
		std::string name_;			///< 中间件名称
		sdk_midware midware_;		///< 中间件
		bool enable_;				///< 使能
	};

	SdkMidWareManager () {};

	~SdkMidWareManager() {
		midwares_.clear();
	}

	// 注册中间件
	void register_midware(const char *name, sdk_midware midware_fn, bool enable);

	// 处理中间件
	bool proc_midwares(struct sdk_net_interface &interface, const Sdk &sdk_req, Sdk &sdk_res) const;

	// 中间件是否存在
	bool is_midware_exist(const std::string &name);

private:
	// 中间件列表
	std::vector<std::shared_ptr<struct midware>> midwares_;
};

// 中间件管理器
static SdkMidWareManager midware_mannager;

/**
 * @brief 注册中间件
 * 
 * @param name 名称
 * @param midware_fn 中间件接口
 * @param enable 使能
 */
void SdkMidWareManager::register_midware(const char *name, sdk_midware midware_fn, bool enable)
{
	std::string midware_name(name);

	if (is_midware_exist(midware_name))
	{
		log_i("midware %s exist\n", name);
		return ;
	}

	std::shared_ptr<struct midware> _midware(new struct midware);

	_midware->name_ = midware_name;
	_midware->midware_ = midware_fn;
	_midware->enable_ = enable;

	midwares_.push_back(std::move(_midware));
}

/**
 * @brief 处理中间件
 * 
 * @param type sdk网络数据类型
 * @param req sdk数据包原始数据请求
 * @param sdk_req sdk请求协议数据
 * @param sdk_res sdk应答协议数据
 * @return true 成功
 * @return false 失败
 */
bool SdkMidWareManager::proc_midwares(struct sdk_net_interface &interface, const Sdk &sdk_req, Sdk &sdk_res) const
{
	for (auto &item : midwares_)
	{
		if (!item->midware_ || !item->enable_) { continue; }

		if (!item->midware_(interface, sdk_req, sdk_res))
		{
			log_d("proc midware %s failed\n", item->name_.c_str());

			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_status_code(ResponseResult::ERROR);
			sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_code("error module : " + item->name_);

			// 一个中间件处理失败不再向后传递
			return false;
		}
		
		log_d("proc midware %s success\n", item->name_.c_str());
	}

	return true;
}

/**
 * @brief 中间件是否存在
 * 
 * @param name 名称
 * @return true 存在
 * @return false 不存在
 */
bool SdkMidWareManager::is_midware_exist(const std::string &name)
{
	for (auto &item : midwares_)
	{
		if (name == item->name_)
		{
			return true;
		}
	}
	return false;
}

/**
 * @brief 注册中间件
 * 
 * @param name 名称
 * @param midware_fn 中间件接口
 * @param enable 使能
 */
void _register_sdk_midware(const char *name, sdk_midware midware_fn, bool enable)
{
	midware_mannager.register_midware(name, midware_fn, enable);
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
bool sdk_midware_do(struct sdk_net_interface &interface, const Sdk &sdk_req, Sdk &sdk_res)
{
	return midware_mannager.proc_midwares(interface, sdk_req, sdk_res);
}