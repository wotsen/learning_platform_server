/**
 * @file session.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef __wotsenLearnPlatSrv_SESSION_H__
#define __wotsenLearnPlatSrv_SESSION_H__

#include <inttypes.h>

namespace wotsen
{

/**
 * @brief 会话
 * 
 */
class Session {
public:
	uint32_t sid;
	Session() : sid(0) {}
};

} // !namespace wotsen

#endif // !__wotsenLearnPlatSrv_SESSION_H__