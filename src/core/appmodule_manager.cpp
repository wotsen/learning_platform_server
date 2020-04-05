/**
 * @file appmodule_manager.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-04-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <iostream>
#include "appmodule_manager.h"

namespace wotsen
{
PlatSrvAppModuleManager::PlatSrvAppModuleManager(
	const std::initializer_list<appmodule_t> &sys_modules_li,
	const std::initializer_list<appmodule_t> &ext_modules_li,
	const std::initializer_list<appmodule_t> &sys_apps_li,
	const std::initializer_list<appmodule_t> &ext_apps_li) : sys_modules_(new SysModules(sys_modules_li)),
															 ext_modules_(new SysApps(ext_modules_li)),
															 sys_apps_(new ExtModules(sys_apps_li)),
															 ext_apps_(new ExtApps(ext_apps_li))
{
}

PlatSrvAppModuleManager::PlatSrvAppModuleManager(
	const std::vector<appmodule_t> &sys_modules_li,
	const std::vector<appmodule_t> &ext_modules_li,
	const std::vector<appmodule_t> &sys_apps_li,
	const std::vector<appmodule_t> &ext_apps_li) : sys_modules_(new SysModules(sys_modules_li)),
												   ext_modules_(new SysApps(ext_modules_li)),
												   sys_apps_(new ExtModules(sys_apps_li)),
												   ext_apps_(new ExtApps(ext_apps_li))
{
}

PlatSrvAppModuleManager::~PlatSrvAppModuleManager()
{
}

std::shared_ptr<PlatSrvAppModuleManager> PlatSrvAppModuleManager::appmodule_manager(
	const std::initializer_list<appmodule_t> &sys_modules_li,
	const std::initializer_list<appmodule_t> &ext_modules_li,
	const std::initializer_list<appmodule_t> &sys_apps_li,
	const std::initializer_list<appmodule_t> &ext_apps_li)
{
	return appmodule_manager(std::vector<appmodule_t>(sys_modules_li),
							 std::vector<appmodule_t>(ext_modules_li),
							 std::vector<appmodule_t>(sys_apps_li),
							 std::vector<appmodule_t>(ext_apps_li));
}

std::shared_ptr<PlatSrvAppModuleManager> PlatSrvAppModuleManager::appmodule_manager(
	const std::vector<appmodule_t> &sys_modules_li,
	const std::vector<appmodule_t> &ext_modules_li,
	const std::vector<appmodule_t> &sys_apps_li,
	const std::vector<appmodule_t> &ext_apps_li)
{
	static std::shared_ptr<PlatSrvAppModuleManager> manager(new PlatSrvAppModuleManager(sys_modules_li,
																						ext_modules_li,
																						sys_apps_li,
																						ext_apps_li));

	return manager;
}

void PlatSrvAppModuleManager::init(void)
{
	auto sys_module = appmodule(E_SYS_MODULE);
	!sys_module ? (void)0 : sys_module->init_appmodules();

	auto ext_module = appmodule(E_THIRD_MODULE);
	ext_module ? ext_module->init_appmodules() : (void)0;

	auto sys_app = appmodule(E_SYS_APP);
	sys_app ? sys_app->init_appmodules() : (void)0;

	auto ext_app = appmodule(E_THIRD_APP);
	ext_app ? ext_app->init_appmodules() : (void)0;
}

std::shared_ptr<IAppModules> appmodule(const AppModuleType &type)
{
	static std::shared_ptr<IAppModules> _;

	switch (type)
	{
	case E_SYS_MODULE:
		return PlatSrvAppModuleManager::appmodule_manager(std::vector<appmodule_t>())->sys_modules_;
		break;

	case E_THIRD_MODULE:
		return PlatSrvAppModuleManager::appmodule_manager(std::vector<appmodule_t>())->ext_modules_;
		break;

	case E_SYS_APP:
		return PlatSrvAppModuleManager::appmodule_manager(std::vector<appmodule_t>())->sys_apps_;
		break;

	case E_THIRD_APP:
		return PlatSrvAppModuleManager::appmodule_manager(std::vector<appmodule_t>())->ext_apps_;
		break;

	default:
		return _;
	}
}

} // namespace wotsen
