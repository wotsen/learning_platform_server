/**
 * @file core.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-04-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "core.h"

namespace wotsen
{

PlatSrvCore::PlatSrvCore()
{

}

PlatSrvCore::~PlatSrvCore()
{

}

std::shared_ptr<PlatSrvCore>& PlatSrvCore::core(void)
{
	static std::shared_ptr<PlatSrvCore> core(new PlatSrvCore);

	return core;
}

} // namespace wotsen
