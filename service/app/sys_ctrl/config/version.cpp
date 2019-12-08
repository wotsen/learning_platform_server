/**
 * @file version.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-09-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "version.h"

#define VERSION "v1.0.0"

/**
 * @brief Get the service version object
 * 
 * @return const std::string& 版本号
 */
const std::string & get_service_version(void)
{
    static std::string version(VERSION " " __DATE__);

    return version;
}

/**
 * @brief Get the service compile time object
 * 
 * @return const std::string& 编译时间
 */
const std::string & get_service_compile_time(void)
{
    static std::string compile_time(__DATE__ "" __TIME__);

    return compile_time;
}