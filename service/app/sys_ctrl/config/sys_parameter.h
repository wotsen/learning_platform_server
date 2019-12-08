/**
 * @file sys_parameter.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 配置参数
 * @version 0.1
 * @date 2019-08-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include <pthread.h>
#include "sys_load_json.h"

/**
 * @brief 系统配置参数
 * 
 */
class SysParameter : public AbsSysLoadJson
{
public:
#define SYS_RUNTIME_CONFIG_FILENAME "../data/sys_runtime_param.json"
#define SYS_DEFAULT_CONFIG_FILENAME "../data/sys_default_param.json"

	~SysParameter();

	///< 获取系统参数指针
	static SysParameter *get_sys_param(void);

	///< 打开运行时参数文件
	bool open_runtime_file(void);

	///< 创建运行时参数文件
	bool create_runtime_file(void);

	///< 打开默认的配置参数文件
	bool open_default_file(void);

	///< 保存配置参数
	void save_param(void);

private:
	SysParameter(){};

	static SysParameter *instance;
	pthread_mutex_t mutex;
};

///< 初始化系统配置参数
bool sys_parameter_init(void);

///< 获取配置参数json指针
nlohmann::json &get_json_param(void);

/******************************************参数配置接口*******************************************************/