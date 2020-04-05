/**
 * @file sdk_midware_manage.h
 * @author yuwangliang (astralrovers@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-02-16
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once

#include <list>
#include <string>
#include <memory>
#include "sdk_request.h"

using namespace insider::sdk;

namespace wotsen
{

class ISdkMidWare;

using sdk_midware_ptr_t = std::shared_ptr<ISdkMidWare>;
using sdk_midware_list_t = std::list<sdk_midware_ptr_t>;

// 中间件管理
class SdkMidWareManager
{
private:
	SdkMidWareManager () {}
public:
	~SdkMidWareManager() { midwares_.clear(); }

public:
	// 注册中间件
	void register_midware(const sdk_midware_ptr_t &mid);
	// 中间件是否存在
	bool is_midware_exist(const std::string &name);

    sdk_midware_list_t &midwares(void);

public:
	static std::shared_ptr<SdkMidWareManager> sdk_midware_manager(void);

private:
	// 中间件列表
	sdk_midware_list_t midwares_;
};

bool sdk_midware_function(SdkRequest &sdk_request);

} // !namespace wotsen
