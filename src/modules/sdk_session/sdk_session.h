/**
 * @file sdk_session.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-05-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __wotsenLearnPlatSrv_SDK_SESSION_H__
#define __wotsenLearnPlatSrv_SDK_SESSION_H__

#include <mutex>
#include <cinttypes>
#include <vector>
#include <memory>
#include <ctime>
#include <string>

namespace wotsen
{

struct SdkSessionInfo
{
	std::string token_;		///< token
	time_t alive_time_;		///< 生存时间
	time_t now_;			///< 当前时间
};

struct SdkUserSessionInfo : public SdkSessionInfo
{
	std::string user_name_;	///< 用户名
};

class SdkSession
{
public:
	SdkSession(const uint32_t max_session=128);
	~SdkSession();

public:
	bool verify(const std::string &token);
	bool add_session(const SdkUserSessionInfo &session);
	bool del_session(const std::string &token);
	void update(void);

private:
	std::mutex mtx_;
	std::vector<std::shared_ptr<SdkUserSessionInfo>> sessions_;
};

} // namespace wotsen

#endif // !1__wo __wotsenLearnPlatSrv_SDK_SESSION_H__