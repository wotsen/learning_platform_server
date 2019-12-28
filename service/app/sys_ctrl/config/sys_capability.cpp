/**
 * @file sys_capability.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-08-15
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#define LOG_TAG "SYS_CAPABILITY"

#include "sys_capability.h"
#include <easylogger/inc/elog.h>

using json = nlohmann::json;

SysCapability *SysCapability::instance = nullptr;

/**
 * @brief 获取系统能力集
 * 
 * @return SysCapability* 
 */
SysCapability *SysCapability::get_sys_capability(void)
{
    if (nullptr != instance)
    {
        return instance;
    }

    instance = new SysCapability();

    instance->json_file.open(SYS_CAPAABILITY_FILENAME, std::ios::in | std::ios::out);

    if (!instance->json_file)
    {
        delete instance;
        throw("can not open " SYS_CAPAABILITY_FILENAME);
    }

    try
    {
        instance->json_file >> instance->j;
    }
    catch (nlohmann::detail::parse_error &err)
    {
        log_e("%s\n", err.what());
        delete instance;
        throw("can not parse json : " SYS_CAPAABILITY_FILENAME);
    }
    if (instance->j.empty())
    {
        delete instance;
        throw("can not parse json : " SYS_CAPAABILITY_FILENAME);
    }

    return instance;
}

/**
 * @brief Destroy the Sys Capability:: Sys Capability object
 * 
 */
SysCapability::~SysCapability()
{
    if (this->json_file)
    {
        instance->json_file.close();
    }
}

/**
 * @brief Get the json capability object
 * 
 * @return json& 
 */
json &get_json_capability(void)
{
    return SysCapability::get_sys_capability()->get_json();
}

/**
 * @brief 能力集初始化
 * 
 * @return true 成功
 * @return false 失败
 */
bool sys_capability_init(void)
{
    json j;

    try
    {
        j = get_json_capability();
    }
    catch (std::string &err)
    {
        log_e("%s\n", err.c_str());
        exit(0);
        return false;
    }

    if (j.empty())
    {
        log_e("can not load capability json!\n");
        exit(0);
        return false;
    }
    log_d("sys_capability json : %s\n", j.dump(4).c_str());
    log_i("parser sys capability json success!\n");

    return true;
}

/**
 * @brief Get the user manage max users object
 * 
 * @return uint32_t 
 */
uint32_t get_user_manage_max_users_capability(void)
{
    json j = get_json_capability();

    return j["service"]["modules"]["module_fun"]["user_manage"]["max_users"];
}

void get_ip_version_capability(std::string &ip_version)
{
    json j = get_json_capability();

    ip_version = j["service"]["modules"]["base_fun"]["network"]["sdk"]["support-version"][0];
}