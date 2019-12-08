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
#ifndef __SESSION_H__
#define __SESSION_H__

#include <inttypes.h>

/**
 * @brief 会话
 * 
 */
class Session {
public:
	uint32_t sid;
	Session() : sid(0) {}
};

#endif // !__SESSION_H__