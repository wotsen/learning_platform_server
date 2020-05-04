/**
 * @file appmodule_table.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-04-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#define LOG_TAG "USR_APPS"

#include <easylogger/inc/elog.h>
#include "appmodule_table.h"

/**************************************************************************************
 * !sys模块导入
**************************************************************************************/
#include "handy_loop/handy_loop.h"
#include "sdk_net/sdk_network.h"
#include "upgrade/upgrade.h"
// #include "user_manage/user_manage.h"

/**************************************************************************************
 * !扩展模块导入
**************************************************************************************/

/**************************************************************************************
 * !sys app导入
**************************************************************************************/

/**************************************************************************************
 * !扩展app导入
**************************************************************************************/

namespace wotsen
{

// 生成一个模块的基本信息
static appmodule_t create_appmoudle(const char *name,
                                            const uint32_t &identifier,
                                            const bool &enable,
                                            const AppModuleState &state,
                                            const AppModuleCfgPermission &permission,
                                            const app_module_fun &init,
                                            const app_module_fun &exit,
                                            const app_module_status_fun &status)
{
    appmodule_t _module(new AppModuleConfig);

    _module->base_info.set_name(name);
    _module->base_info.set_identifier(identifier);
    _module->base_info.set_enable(enable);
    _module->base_info.set_state(state);
    _module->base_info.set_permission(permission);

    _module->init = init;
    _module->exit = exit;
    _module->status = status;

    return _module;
}

#ifdef DEBUG
// 打印模块信息
void _print_app_module_state()
{
    AppModuleCoutInfoList moduel_list;

    auto module = appmodule(E_SYS_MODULE);

    module->appmodules_current_status(moduel_list);

    int size = moduel_list.app_modules_size();

    using namespace std;

    for (int i = 0; i < size; i++)
    {
        const AppModuleCoutInfo &module = moduel_list.app_modules(i);

        cout << "module id : " << module.id() << endl;
        cout << "run state : " << module.run_state() << endl;

        cout << "modules name : " << module.base_info().name() << endl;
        cout << "modules type : " << module.base_info().type() << endl;
        cout << "modules identifier : " << module.base_info().identifier() << endl;
        cout << "modules enable : " << module.base_info().enable() << endl;
        cout << "state : " << module.base_info().state() << endl;
        cout << "permission : " << module.base_info().permission() << endl;

        cout << "########################################" << endl;
    }
}
#endif

/****************************************************************************************************************
 * !系统模块表
****************************************************************************************************************/

static std::vector<appmodule_t> _sys_modules = {
    create_appmoudle("handy loop", 1, true, E_APP_MODULE_IDLE, E_APP_MODULE_CFG_PERMISSION_DISENABLE,
        task_handy_loop_init, nullptr, []() -> bool { return true; }),

    create_appmoudle("sdk net", 2, true, E_APP_MODULE_IDLE, E_APP_MODULE_CFG_PERMISSION_DISENABLE,
        sdk_net_init, nullptr, []() -> bool { return true; }),

    create_appmoudle("system upgrade", 3, true, E_APP_MODULE_IDLE, E_APP_MODULE_CFG_PERMISSION_DISENABLE,
        system_upgrade_task_init, nullptr, system_upgrade_task_state),

    // create_appmoudle("user manage", 4, true, E_APP_MODULE_IDLE, E_APP_MODULE_CFG_PERMISSION_DISENABLE,
    //     user_manager_init, nullptr, user_manager_state),
};

/****************************************************************************************************************
 * !扩展模块表
****************************************************************************************************************/

static std::vector<appmodule_t> _ext_modules = {};

/****************************************************************************************************************
 * !系统应用表
****************************************************************************************************************/

static std::vector<appmodule_t> _sys_apps = {};

/****************************************************************************************************************
 * !扩展应用表
****************************************************************************************************************/

static std::vector<appmodule_t> _ext_apps = {};

std::vector<appmodule_t>& get_sys_modules(void)
{
	return _sys_modules;
}

std::vector<appmodule_t>& get_ext_modules(void)
{
	return _ext_modules;
}

std::vector<appmodule_t>& get_sys_apps(void)
{
	return _sys_apps;
}

std::vector<appmodule_t>& get_ext_apps(void)
{
	return _ext_apps;
}

}
