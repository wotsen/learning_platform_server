/**
 * @file sdk_midwares_table.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-04-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "sdk_net/sdk_midware.h"
#include "sdk_midwares_table.h"

namespace wotsen
{

using namespace wotsen;

void register_sdk_midwares(void)
{
	// SDK_IMPORT_MIDWARE(user_manange_midware_do, "user_manange_midware", true);
	SDK_IMPORT_MIDWARE(sdk_user_midware, "sdk user midware", true);
	// TODO:记录访问者，ip，端口，时间(),json格式
}

}
