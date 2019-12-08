/**
 * @file session_manage.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef __SESSION_MANAGE_H__
#define __SESSION_MANAGE_H__

#include <vector>
#include <inttypes.h>
#include "session.h"

/**
 * @brief 会话管理
 * 
 * @tparam T 会话类别
 */
template <typename T>
class SessionManager {
public:
	uint32_t max_session;
	std::vector<std::shared_ptr<T>> sessions;

	// virtual bool add_session(const T &session);
	// virtual bool delete_session(const T &session);

	SessionManager(const uint32_t max_session) : max_session(max_session) {}
	virtual ~SessionManager() {
		sessions.clear();
	}
};

#endif // !__SESSION_MANAGE_H__