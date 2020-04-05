/**
 * @file appmodule_manager.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-04-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __wotsenLearnPlatSrv_APPMODULE_MANAGER_H__
#define __wotsenLearnPlatSrv_APPMODULE_MANAGER_H__

#include "appmodule.h"

namespace wotsen
{

class PlatSrvAppModuleManager
{
private:
	PlatSrvAppModuleManager(const std::initializer_list<appmodule_t> &sys_modules_li,
							const std::initializer_list<appmodule_t> &ext_modules_li,
							const std::initializer_list<appmodule_t> &sys_apps_li,
							const std::initializer_list<appmodule_t> &ext_apps_li);

	PlatSrvAppModuleManager(const std::vector<appmodule_t> &sys_modules_li,
							const std::vector<appmodule_t> &ext_modules_li,
							const std::vector<appmodule_t> &sys_apps_li,
							const std::vector<appmodule_t> &ext_apps_li);

	PlatSrvAppModuleManager(const std::vector<appmodule_t> &sys_modules_li);

public:
	~PlatSrvAppModuleManager();

public:
	static void init(void);

public:
	friend std::shared_ptr<IAppModules> appmodule(const AppModuleType &type);

	static std::shared_ptr<PlatSrvAppModuleManager> appmodule_manager(
		const std::initializer_list<appmodule_t> &sys_modules_li = {},
		const std::initializer_list<appmodule_t> &ext_modules_li = {},
		const std::initializer_list<appmodule_t> &sys_apps_li = {},
		const std::initializer_list<appmodule_t> &ext_apps_li = {});

	static std::shared_ptr<PlatSrvAppModuleManager> appmodule_manager(
		const std::vector<appmodule_t> &sys_modules_li = {},
		const std::vector<appmodule_t> &ext_modules_li = {},
		const std::vector<appmodule_t> &sys_apps_li = {},
		const std::vector<appmodule_t> &ext_apps_li = {});

private:
	std::shared_ptr<IAppModules> sys_modules_;
	std::shared_ptr<IAppModules> ext_modules_;
	std::shared_ptr<IAppModules> sys_apps_;
	std::shared_ptr<IAppModules> ext_apps_;
};

} // namespace wotsen

#endif // !__wotsenLearnPlatSrv_APPMODULE_MANAGER_H__