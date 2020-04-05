/**
 * @file handy_loop.h
 * @author yuwangliang (astralrovers@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-01-30
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef __wotsenLearnPlatSrv_HANDY_LOOP_H__
#define __wotsenLearnPlatSrv_HANDY_LOOP_H__

#include <handy/handy.h>

using namespace handy;

namespace wotsen
{

EventBase &handy_base(void);

void task_handy_loop_init(void);

} // !namespace wotsen

#endif // !__wotsenLearnPlatSrv_HANDY_LOOP_H__
