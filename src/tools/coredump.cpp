/**
 * @file coredump.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-04
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "coredump.h"

namespace wotsen
{

static const char * const core_format = "core-%e-%p-%t";

/**
 * @brief Set the up coredump object
 * 
 * @param path_dir : coredump生成路径
 * @param core_size : 文件大小
 * @return true : 设置成功
 * @return false : 设置失败
 */
bool setup_coredump(const char *path_dir, size_t core_size)
{
	struct rlimit rlmt;
	char core_path[1024];

	if (NULL == path_dir) return false;

	if (getrlimit(RLIMIT_CORE, &rlmt) < 0)
	{
        return false; 
    }

	rlmt.rlim_cur = (rlim_t)core_size;
    rlmt.rlim_max  = (rlim_t)core_size;

	if (setrlimit(RLIMIT_CORE, &rlmt) < 0)
	{
        return false; 
    }

	if (path_dir[strlen(path_dir) - 1] != '/')
	{
		sprintf(core_path, "echo %s/%s > /proc/sys/kernel/core_pattern", path_dir, core_format);
	}
	else
	{
		sprintf(core_path, "echo %s%s > /proc/sys/kernel/core_pattern", path_dir, core_format);
	}

	sprintf(core_path, "echo %s/%s > /proc/sys/kernel/core_pattern", path_dir, core_format);
	system(core_path);

	system("echo 1 > /proc/sys/kernel/core_uses_pid");

	return true;
}

} // namespace wotsen
