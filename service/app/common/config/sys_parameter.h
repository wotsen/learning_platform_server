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

class SysParameter : public AbsSysLoadJson
{
public:
#define SYS_RUNTIME_CONFIG_FILENAME "../data/sys_runtime_param.json"
#define SYS_DEFAULT_CONFIG_FILENAME "../data/sys_default_param.json"

	~SysParameter();
	static SysParameter *get_sys_param(void);

	bool open_runtime_file(void);
	bool create_runtime_file(void);
	bool open_default_file(void);

	void save_param(void);

private:
	SysParameter(){};

	static SysParameter *instance;
	pthread_mutex_t mutex;
};

bool sys_parameter_init(void);
nlohmann::json &get_json_param(void);

/******************************************参数配置接口*******************************************************/