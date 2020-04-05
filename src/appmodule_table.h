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

// 获取系统模块表
std::vector<appmodule_t>& get_sys_modules(void);
// 获取扩展模块表，TODO:如何不重新编译进行加载，类似动态语言的模块导入
std::vector<appmodule_t>& get_ext_modules(void);
// 系统应用，TODO:支持容器
std::vector<appmodule_t>& get_sys_apps(void);
// 扩展应用
std::vector<appmodule_t>& get_ext_apps(void);

#ifdef DEBUG
// 打印模块信息
void _print_app_module_state();
#endif

} // !namespace wotsen

#endif // !__wotsenLearnPlatSrv_APPMODULE_TABLE_H__