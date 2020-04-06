/**
 * @file appmodule.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-04-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <loguru.hpp>
#include "appmodule.h"

namespace wotsen
{

IAppModules::IAppModules(const std::initializer_list<appmodule_t> &li) : appmodule_list_(li)
{
}

IAppModules::IAppModules(const std::vector<appmodule_t> &li) : appmodule_list_(li)
{
}

IAppModules::~IAppModules()
{
}

void IAppModules::init_appmodules(void)
{
	std::unique_lock<std::mutex> lock(mtx_);

	for (auto &item : appmodule_list_)
	{
		if (item->base_info.enable() && E_APP_MODULE_IDLE == item->base_info.state() && item->init)
		{
			LOG_F(WARNING, "init app modules : %s\n", item->base_info.name().c_str());
			item->init();
			item->base_info.set_state(E_APP_MODULE_INSTALLED);
		}
	}
}

void IAppModules::finit_appmodules(void)
{
	std::unique_lock<std::mutex> lock(mtx_);

	for (auto &item : appmodule_list_)
	{
		if (item->base_info.enable()
			&& E_APP_MODULE_INSTALLED == item->base_info.state()
			&& item->exit
			&& E_APP_MODULE_CFG_PERMISSION_ENABLE == item->base_info.permission())
		{
			LOG_F(WARNING, "init app modules : %s\n", item->base_info.name().c_str());
			LOG_F(WARNING, "finit app modules : %s\n", item->base_info.name().c_str());
			item->exit();
			item->base_info.set_state(E_APP_MODULE_IDLE);
		}
	}
}

// 单个模块初始化
void IAppModules::init_single_appmodule(const uint32_t identifier, const std::string &name) noexcept
{
	std::unique_lock<std::mutex> lock(mtx_);

	for (auto &item : appmodule_list_)
	{
		if (name == item->base_info.name() && identifier == item->base_info.identifier() && item->base_info.enable() && E_APP_MODULE_IDLE == item->base_info.state() && item->init && E_APP_MODULE_CFG_PERMISSION_ENABLE == item->base_info.permission())
		{
			LOG_F(WARNING, "init app modules : %s\n", item->base_info.name().c_str());
			item->init();
			item->base_info.set_state(E_APP_MODULE_INSTALLED);
		}
	}
}

// 单个模块卸载
void IAppModules::finit_single_appmodule(const uint32_t identifier, const std::string &name) noexcept
{
	std::unique_lock<std::mutex> lock(mtx_);

	for (auto &item : appmodule_list_)
	{
		if (name == item->base_info.name() && identifier == item->base_info.identifier() && item->base_info.enable() && E_APP_MODULE_INSTALLED == item->base_info.state() && item->exit && E_APP_MODULE_CFG_PERMISSION_ENABLE == item->base_info.permission())
		{
			LOG_F(WARNING, "exit app modules : %s\n", item->base_info.name().c_str());
			item->exit();
			item->base_info.set_state(E_APP_MODULE_INSTALLED);
		}
	}
}

// 获取当前所有模块状态
void IAppModules::appmodules_current_status(AppModuleCoutInfoList &moduel_list) noexcept
{
	uint32_t id = 0;
	std::unique_lock<std::mutex> lock(mtx_);

	for (auto &item : appmodule_list_)
	{
		// 没有初始化接口的模块认为是无效的
		if (!item->init)
		{
			continue;
		}

		// 添加一个模块
		AppModuleCoutInfo *module = moduel_list.add_app_modules();
		AppModuleBaseInfo *info = module->mutable_base_info();

		id++;

		module->set_id(id);
		module->set_run_state(item->status
								  ? (item->status() ? E_APP_MODULE_RUN_ST_OK : E_APP_MODULE_RUN_ST_ERR)
								  : E_APP_MODULE_RUN_ST_UNKNOWN);

		info->set_name(item->base_info.name());
		info->set_type(item->base_info.type());
		info->set_identifier(item->base_info.identifier());
		info->set_enable(item->base_info.enable());
		info->set_state(item->base_info.state());
		info->set_permission(item->base_info.permission());
	}
}

uint16_t IAppModules::valid_appmodule_num(void)
{
	uint32_t valid_module = 0;

	for (auto &item : appmodule_list_)
	{
		if (item->init)
		{
			valid_module++;
		}
	}

	return valid_module;
}

SysModules::SysModules(const std::initializer_list<appmodule_t> &li) : IAppModules(li)
{
	for (auto &item : appmodule_list_)
	{
		item->base_info.set_type(E_SYS_MODULE);
	}
}

SysModules::SysModules(const std::vector<appmodule_t> &li) : IAppModules(li)
{
	for (auto &item : appmodule_list_)
	{
		item->base_info.set_type(E_SYS_MODULE);
	}
}

SysApps::SysApps(const std::initializer_list<appmodule_t> &li) : IAppModules(li)
{
	for (auto &item : appmodule_list_)
	{
		item->base_info.set_type(E_SYS_APP);
	}
}

SysApps::SysApps(const std::vector<appmodule_t> &li) : IAppModules(li)
{
	for (auto &item : appmodule_list_)
	{
		item->base_info.set_type(E_SYS_APP);
	}
}

ExtModules::ExtModules(const std::initializer_list<appmodule_t> &li) : IAppModules(li)
{
	for (auto &item : appmodule_list_)
	{
		item->base_info.set_type(E_THIRD_MODULE);
	}
}

ExtModules::ExtModules(const std::vector<appmodule_t> &li) : IAppModules(li)
{
	for (auto &item : appmodule_list_)
	{
		item->base_info.set_type(E_THIRD_MODULE);
	}
}

ExtApps::ExtApps(const std::initializer_list<appmodule_t> &li) : IAppModules(li)
{
	for (auto &item : appmodule_list_)
	{
		item->base_info.set_type(E_THIRD_APP);
	}
}

ExtApps::ExtApps(const std::vector<appmodule_t> &li) : IAppModules(li)
{
	for (auto &item : appmodule_list_)
	{
		item->base_info.set_type(E_THIRD_APP);
	}
}

} // namespace wotsen
