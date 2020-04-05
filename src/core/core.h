/**
 * @file core.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-04-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __wotsenLearnPlatSrv_CORE_H__
#define __wotsenLearnPlatSrv_CORE_H__

#include <memory>

namespace wotsen
{

class PlatSrvCore
{
private:
	PlatSrvCore();
public:
	~PlatSrvCore();

public:
	static std::shared_ptr<PlatSrvCore>& core(void);

private:
	void init(void);

private:
	// 事件
	// sdk网络
	// 模块管理
	// app管理
	// 第三方app管理
};

} // namespace wotsen


#endif // !__wotsenLearnPlatSrv_CORE_H__
