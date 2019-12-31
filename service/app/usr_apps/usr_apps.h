/**
 * @file usr_apps.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 管理所有的应用功能模块都在这里进行管理，包括自己实现的与第三方的
 * @version 0.1
 * @date 2019-08-17
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include <inttypes.h>
#include <vector>
#include <string>

/**
 * @brief 模块状态
 * 
 */
enum app_module_state {
    E_APP_MODULE_IDLE,              ///< 模块空闲
    E_APP_MODULE_INIT,              ///< 模块初始化中
    E_APP_MODULE_INSTALLED,         ///< 模块已经安装
    E_APP_MODULE_BAD,               ///< 模块损坏
};

/**
 * @brief 模块运行状态
 * 
 */
enum app_module_run_state {
    E_APP_MODULE_RUN_ST_OK,         ///< 运行状态正常
    E_APP_MODULE_RUN_ST_ERR,        ///< 运行状态异常
    E_APP_MODULE_RUN_ST_UNKNOWN,    ///< 运行状态未知
};

/**
 * @brief 模块配置权限
 * 
 */
enum app_module_cfg_permission {
    E_APP_MODULE_CFG_PERMISSION_ENABLE,         ///< 可配置
    E_APP_MODULE_CFG_PERMISSION_DISENABLE,      ///< 不可配置
};

/**
 * @brief 模块基本信息
 * 
 */
struct app_module_base_info {
    std::string name;               ///< 模块名称
    bool enable;                    ///< 使能
    enum app_module_state state;    ///< 模块状态
    enum app_module_cfg_permission permission;  ///< 模块配置权限
};

/**
 * @brief 模块输出信息
 * 
 */
struct app_module_cout_info {
    uint32_t id;
    enum app_module_run_state run_state;    ///< 模块运行状态
    struct app_module_base_info base_info;  ///< 模块基本配置信息
};

// 获取模块信息
std::vector<struct app_module_cout_info> &&app_modules_current_status(void) noexcept;

// 初始化单个模块
void init_single_app_module(const uint32_t id, const std::string &name) noexcept;

// 反初始化单个模块
void finit_single_app_module(const uint32_t id, const std::string &name) noexcept;

// 用户应用初始化
void usr_apps_init(void);
