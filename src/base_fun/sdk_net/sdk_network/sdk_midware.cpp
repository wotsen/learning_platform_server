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

class SdkMidWareManager {
public:
	struct midware {
		std::string name;			///< 中间件名称
		sdk_midware _midware;		///< 中间件
		bool enable;				///< 使能
	};

	SdkMidWareManager () {};

	/**
	 * @brief 注册中间件
	 * 
	 * @param name 名称
	 * @param midware_fn 中间件接口
	 * @param enable 使能
	 */
	void register_midware(const char *name, sdk_midware midware_fn, bool enable)
	{
		std::string midware_name(name);

		if (is_midware_exist(midware_name))
		{
			log_i("midware %s exist\n", name);
			return ;
		}

		std::shared_ptr<struct midware> _midware(new struct midware);

		_midware->name = midware_name;
		_midware->_midware = midware_fn;
		_midware->enable = enable;

		midwares.push_back(std::move(_midware));
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
	bool proc_midwares(struct sdk_net_interface &interface, const insider::sdk::Sdk &sdk_req, insider::sdk::Sdk &sdk_res) const
	{
		for (auto &item : midwares)
		{
			if (item->_midware && item->enable)
			{
				if (!item->_midware(interface, sdk_req, sdk_res))
				{
					log_d("proc midware %s failed\n", item->name.c_str());
					
					sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_status_code(ResponseResult::ERROR);
					sdk_res.mutable_footer()->mutable_result()->mutable_sdk_result()->set_code("error module : " + item->name);

					// 一个中间件处理失败不再向后传递
					return false;
				}
				log_d("proc midware %s success\n", item->name.c_str());
			}
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
	bool is_midware_exist(const std::string &name)
	{
		for (auto &item : midwares)
		{
			if (name == item->name)
			{
				return true;
			}
		}
		return false;
	}
	
	~SdkMidWareManager() {
		midwares.clear();
	}

private:
	std::vector<std::shared_ptr<struct midware>> midwares;
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
bool sdk_midware_do(struct sdk_net_interface &interface, const insider::sdk::Sdk &sdk_req, insider::sdk::Sdk &sdk_res)
{
	return midware_mannager.proc_midwares(interface, sdk_req, sdk_res);
}