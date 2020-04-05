/**
 * @file coredump.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-04
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef __wotsenLearnPlatSrv_COREDUMP_H__
#define __wotsenLearnPlatSrv_COREDUMP_H__

namespace wotsen
{

bool setup_coredump(const char *path_dir, size_t core_size);

} // namespace wotsen

#endif // !__wotsenLearnPlatSrv_COREDUMP_H__