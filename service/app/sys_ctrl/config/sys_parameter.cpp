/**
 * @file sys_parameter.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-10-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#define LOG_TAG "SYS_PARAMETER"

#include <iostream>
#include <iomanip>
#include "../../tools/tools_func/calculate_crc16.h"
#include <easylogger/inc/elog.h>
#include "sys_parameter.h"

using json = nlohmann::json;

SysParameter *SysParameter::instance = nullptr;

/**
 * @brief 获取系统参数指针
 * 
 * @return SysParameter* 
 */
SysParameter *SysParameter::get_sys_param(void)
{
    // 单例模式
    if (nullptr != instance)
    {
        return instance;
    }

    instance = new SysParameter();

    // 打开运行时文件
    if (!instance->open_runtime_file())
    {
        // 打开失败就创建
        if (!instance->create_runtime_file())
        {
            delete instance;
            throw("can not open json : " SYS_RUNTIME_CONFIG_FILENAME);
        }
    }

    // 解析
    try
    {
        instance->json_file >> instance->j;
    }
    catch (nlohmann::detail::parse_error &err)
    {
        log_e("%s\n", err.what());
        delete instance;
        throw("can not parse json : " SYS_RUNTIME_CONFIG_FILENAME);
    }
    if (instance->j.empty())
    {
        delete instance;
        throw("can not parse json : " SYS_RUNTIME_CONFIG_FILENAME);
    }

    return instance;
}

/**
 * @brief Destroy the Sys Parameter:: Sys Parameter object
 * 
 */
SysParameter::~SysParameter()
{
    if (this->json_file)
    {
        instance->json_file.close();
    }
}

/**
 * @brief 打开运行时参数文件
 * 
 * @return true 成功
 * @return false 失败
 */
bool SysParameter::open_runtime_file(void)
{
    this->json_file.open(SYS_RUNTIME_CONFIG_FILENAME, std::ios::in | std::ios::out);

    if (!this->json_file)
    {
        log_e("can not open " SYS_RUNTIME_CONFIG_FILENAME "\n");
        return false;
    }

    return true;
}

/**
 * @brief 创建运行时参数文件
 * 
 * @return true 成功
 * @return false 失败
 */
bool SysParameter::create_runtime_file(void)
{
    std::fstream in;

    // 创建运行时配置参数
    this->json_file.open(SYS_RUNTIME_CONFIG_FILENAME, std::ios::out);

    if (!this->json_file)
    {
        log_e("can not create " SYS_RUNTIME_CONFIG_FILENAME "\n");
        return false;
    }

    // 关闭
    this->json_file.close();

    // 以可读可写方式打开
    if (!this->open_runtime_file())
    {
        return false;
    }

    // 打开默认配置文件
    in.open(SYS_DEFAULT_CONFIG_FILENAME, std::ios::in);

    if (!in)
    {
        log_e("can not open " SYS_DEFAULT_CONFIG_FILENAME "\n");
        this->json_file.close();
        return false;
    }

    // 拷贝默认配置文件到运行时配置文件
    in.seekg(0, std::ios::beg);
    this->json_file.seekp(0, std::ios::beg);
    this->json_file << in.rdbuf();
    this->json_file.seekg(0, std::ios::beg);

    in.close();

    return true;
}

/**
 * @brief 打开默认的配置参数文件
 * 
 * @return true 
 * @return false 
 */
bool SysParameter::open_default_file(void)
{
    std::fstream in(SYS_DEFAULT_CONFIG_FILENAME, std::ios::in);

    if (!in)
    {
        log_e("can not open " SYS_DEFAULT_CONFIG_FILENAME "\n");
        return false;
    }

    return true;
}

/**
 * @brief 保存配置参数
 * 
 */
void SysParameter::save_param(void)
{
    this->json_file.seekp(0, std::ios::beg);
    this->json_file << std::setw(4) << this->j;
}

/**
 * @brief Get the json param object
 * 
 * @return json& 
 */
json &get_json_param(void)
{
    return SysParameter::get_sys_param()->get_json();
}

/**
 * @brief 初始化配置参数
 * 
 * @return true 
 * @return false 
 */
bool sys_parameter_init(void)
{
    json j;

    try
    {
        j = get_json_param();
    }
    catch (std::string &err)
    {
        log_e("%s\n", err.c_str());
        exit(0);
        return false;
    }
    if (j.empty())
    {
        exit(0);
        return false;
    }
    // log_d("sys_param json : %s\n", j.dump(4).c_str());
    log_i("parser sys param json success!\n");

    return true;
}