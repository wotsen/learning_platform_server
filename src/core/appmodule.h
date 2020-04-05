/**
 * @file appmodule.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-04-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __wotsenLearnPlatSrv_APP_MODULE_H__
#define __wotsenLearnPlatSrv_APP_MODULE_H__

#include <mutex>
#include <vector>
#include <memory>
#include <cinttypes>
#include "base/sdk_protocol/in_sdk_body_appmodule.pb.h"

namespace wotsen
{

using namespace insider::sdk;

using app_module_fun = void (*)(void);
using app_module_status_fun = bool (*)(void);

struct AppModuleConfig
{
    AppModuleConfig() {}

    AppModuleConfig(const AppModuleBaseInfo &base_info) :
										base_info(base_info),
										init(nullptr),
										exit(nullptr),
										status(nullptr)
    {
    }

    AppModuleConfig(const AppModuleBaseInfo &base_info,
					const app_module_fun &init,
					app_module_fun exit,
					app_module_status_fun status) :
								base_info(base_info),
								init(init),
								exit(exit),
								status(status)
    {
        // static_assert(init != nullptr, "app module init is null!");
    }

    AppModuleBaseInfo base_info;    ///< 模块基本配置信息
    app_module_fun init;            ///< 模块初始化接口
    app_module_fun exit;            ///< 模块卸载接口
    app_module_status_fun status;   ///< 模块状态接口
};

using appmodule_t = std::shared_ptr<AppModuleConfig>;

class IAppModules
{
public:
	IAppModules(const std::initializer_list<appmodule_t> &li);
	IAppModules(const std::vector<appmodule_t> &li);
	virtual ~IAppModules();

public:
	virtual void init_appmodules(void);
	virtual void finit_appmodules(void);

	// 单个模块初始化
    virtual void init_single_appmodule(const uint32_t identifier, const std::string &name) noexcept;
    // 单个模块卸载
    virtual void finit_single_appmodule(const uint32_t identifier, const std::string &name) noexcept;
	// 获取当前所有模块状态
    virtual void appmodules_current_status(AppModuleCoutInfoList &moduel_list) noexcept;

	virtual uint16_t valid_appmodule_num(void);

public:
	std::mutex mtx_;
	std::vector<appmodule_t> appmodule_list_;
};

class SysModules : public IAppModules
{
public:
	SysModules(const std::initializer_list<appmodule_t> &li);
	SysModules(const std::vector<appmodule_t> &li);
};

class SysApps : public IAppModules
{
public:
	SysApps(const std::initializer_list<appmodule_t> &li);
	SysApps(const std::vector<appmodule_t> &li);
};

class ExtModules : public IAppModules
{
public:
	ExtModules(const std::initializer_list<appmodule_t> &li);
	ExtModules(const std::vector<appmodule_t> &li);
};

class ExtApps : public IAppModules
{
public:
	ExtApps(const std::initializer_list<appmodule_t> &li);
	ExtApps(const std::vector<appmodule_t> &li);
};

std::shared_ptr<IAppModules> appmodule(const AppModuleType &type);

} // namespace wotsen

#endif // !__wotsenLearnPlatSrv_APP_MODULE_H__