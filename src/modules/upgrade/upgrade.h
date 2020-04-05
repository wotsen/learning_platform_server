/**
 * @file upgrade.h
 * @author 余王亮 (astralrovers@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-09-13
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#ifndef __wotsenLearnPlatSrv_UPGRADE_H__
#define __wotsenLearnPlatSrv_UPGRADE_H__

namespace wotsen
{

///< 初始化升级任务
void system_upgrade_task_init(void);

///< 系统升级模块状态
bool system_upgrade_task_state(void);

} // !namespace wotsen

#endif // !__wotsenLearnPlatSrv_UPGRADE_H__
