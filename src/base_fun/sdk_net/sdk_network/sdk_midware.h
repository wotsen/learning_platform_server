/**
 * @file sdk_midware.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef __SDK_MIDWARE_H__
#define __SDK_MIDWARE_H__

#include "sdk_interface.h"

using namespace insider::sdk;

// sdk中间件处理
using sdk_midware_fn = bool (*)(struct sdk_net_interface &sdk_interface, const Sdk &sdk_req, Sdk &sdk_res);

class ISdkMidWare
{
private:
	std::string name_;			///< 中间件名称
	bool enable_;				///< 使能
	sdk_midware_fn fn_;			///< 处理接口(该接口优先级更高)

public:
	ISdkMidWare(const std::string &name, const bool &enable, sdk_midware_fn fn = nullptr)
		: name_(name), enable_(enable), fn_(fn) {}
	virtual ~ISdkMidWare() {}

	// 中间件是否使能
	virtual bool is_enable(void) const;
	// 中间件是否匹配
	virtual bool is_match(const std::string &name) const;
	// 中间件名称
	virtual const std::string &name(void) const;

	// 仿函数，子类可重写该方法用于处理自身逻辑，
	// 如果不重写该方法则不会发生任何事情
	virtual bool operator () 
		(struct sdk_net_interface &sdk_interface, const Sdk &sdk_req, Sdk &sdk_res) const;
};

// 中间件注册
void register_sdk_midware(const std::shared_ptr<ISdkMidWare> &mid);

/****************************************************************************************************
 * 两种方式注册中间件：
 * 1.扩展ISdkMidWare子类，重载()方法
 * 2.使用类似函数接口注册
 * 无论哪种方式都会生成ISdkMidWare对象中间件
****************************************************************************************************/

// 使用中间件子类创建中间件对象
#define SDK_CREATE_MIDWARE(midware, name, enable) \
	register_sdk_midware(std::make_shared<mideware>(std::string(name), enable))

// 导入中间件接口用于创建中间件默认对象
#define SDK_IMPORT_MIDWARE(midware, name, enable) \
	extern bool midware(struct sdk_net_interface &sdk_interface, const Sdk &sdk_req, Sdk &sdk_res);\
	register_sdk_midware(std::make_shared<ISdkMidWare>(std::string(name), enable, midware))

// 中间件处理
bool sdk_midware_do(struct sdk_net_interface &sdk_interface, const Sdk &sdk_req, Sdk &sdk_res);

#endif // !__SDK_MIDWARE_H__