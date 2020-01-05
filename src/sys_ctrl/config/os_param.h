/**
 * @file os_param.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-01-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once

// 系统任务个数
#define OS_SYS_TASK_NUM 1024

// 能力集配置文件
#define OS_SYS_CAPAABILITY_FILENAME "../etc/sys_capability.json"

// 参数配置文件
#define SYS_RUNTIME_CONFIG_FILENAME "../data/sys_runtime_param.json"
#define SYS_DEFAULT_CONFIG_FILENAME "../data/sys_default_param.json"

// 系统配置文件
#define SYS_CONFIG_FILENAME "../etc/sys_config.json"
// #define SYS_CONFIG_FILE_MAX 2048

// 日志文件路径
#define SYS_ELOG_PATH "../log/elog_file.log"

// app模块最大个数
#define OS_SYS_MAX_APP_MODULES 1024

// 用户管理数据库路径
#define OS_SYS_USER_DB_PATH "../data/ai_platform_service_user.db3"

/*******************表名********************/
#define SDK_USER_TABLE "sdk_user"			///< 账户表
/******************************************/


