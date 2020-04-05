/**
 * @file app.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-04-03
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef ___wotsenLearnPlatSrv_APP_H__
#define ___wotsenLearnPlatSrv_APP_H__

#include <memory>

namespace wotsen
{

class PlatSrvApp
{
private:
	PlatSrvApp();

public:
	~PlatSrvApp();

public:
	static std::shared_ptr<PlatSrvApp> &plat_srv_app(void);

public:
	void run(void);
	void stop(void);

private:
	void init(void);
	void alone_run(void);
	void exception_setup(void);
};

} // namespace wotsen

#endif // !___wotsenLearnPlatSrv_APP_H__
