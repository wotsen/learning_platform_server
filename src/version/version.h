/**
 * @file version.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-09-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef __wotsenLearnPlatSrv_VERSION_H__
#define __wotsenLearnPlatSrv_VERSION_H__

#include <string>

namespace wotsen
{

// 获取版本号
const std::string& get_service_version(void);

// 获取编译时间
const std::string& get_service_compile_time(void);

}

#endif // !__wotsenLearnPlatSrv_VERSION_H__
