/**
 * @file appmodule_table.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-04-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __wotsenLearnPlatSrv_APPMODULE_TABLE_H__
#define __wotsenLearnPlatSrv_APPMODULE_TABLE_H__

#include <vector>
#include "appmodule.h"

namespace wotsen
{

std::vector<appmodule_t>& get_sys_modules(void);
std::vector<appmodule_t>& get_ext_modules(void);
std::vector<appmodule_t>& get_sys_apps(void);
std::vector<appmodule_t>& get_ext_apps(void);

#ifdef DEBUG
// 打印模块信息
void _print_app_module_state();
#endif

}

#endif // !__wotsenLearnPlatSrv_APPMODULE_TABLE_H__