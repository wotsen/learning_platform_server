/**
 * @file sdk_session.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-05-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <sole/sole.hpp>
#include <loguru.hpp>
#include "handy_loop/handy_loop.h"
#include "sdk_session.h"

namespace wotsen
{

SdkSession::SdkSession(const uint32_t &max_session)
{
    handy_base().runAt(time(nullptr) * 1000 + 500, [&](){
        this->update();
    }, 1000); // 1s 更新一次
}

SdkSession::~SdkSession()
{}

bool SdkSession::verify(const std::string &token)
{
    return true;
}

bool SdkSession::add_session(SdkUserSessionInfo &session)
{
    std::unique_lock<std::mutex> lock(mtx_);

	for (auto &item : sessions_)
	{
		if (session.user_name_ == item->user_name_)
		{
            LOG_F(INFO, "user(%s) session exist.\n", session.user_name_.c_str());
            item->alive_time_ = session.alive_time_;
            item->now_ = session.alive_time_;
            session.token_ = item->token_;
			return true;
		}
	}

    std::shared_ptr<SdkUserSessionInfo> new_session(new SdkUserSessionInfo);

    new_session->alive_time_ = session.alive_time_;
    new_session->now_ = session.alive_time_;
    new_session->user_name_ = session.user_name_;
    new_session->token_ = sole::uuid4().str();

    session.token_ = new_session->token_;

    sessions_.push_back(new_session);

    return true;
}

bool SdkSession::del_session(const std::string &token)
{
    std::unique_lock<std::mutex> lock(mtx_);
	sessions_.erase(std::remove_if(sessions_.begin(), sessions_.end(), [&token](auto &_session) -> bool {
							LOG_F(INFO, "delete user %s\n", _session->user_name_.c_str());
							return _session->token_ == token;
					}),
					sessions_.end());
    return true;
}

void SdkSession::update(void)
{
    std::unique_lock<std::mutex> lock(mtx_);
	for (auto &item : sessions_)
	{
		if (0 == item->now_)
		{
			continue;
		}

		// 刷新登陆时间
		item->now_--;
	}

	// 移除过期用户
	sessions_.erase(std::remove_if(sessions_.begin(), sessions_.end(), [](auto &_session) -> bool {
							if (0 == _session->now_) {
								LOG_F(INFO, "user %s token over time\n", _session->user_name_.c_str());
								return true;
							}

							return false;
					}),
					sessions_.end());
}

}