/**
 * @file soft_setup.cpp
 * @author yuwangliang (astralrovers@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-01-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "tools_func/coredump.h"
#include "os_param.h"
#include "soft_setup.h"

static int lock_file(int fd);
static bool alone_run(void);

/**
 * @brief 锁定文件
 *
 * @param fd : 文件描述符
 *
 * @return : 非0失败
 */
static int lock_file(int fd)
{
    struct flock fl;

    fl.l_type = F_WRLCK; // 锁定文件，锁定失败则是已经运行了一个
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;

    return fcntl(fd, F_SETLK, &fl);
}

/**
 * @brief 唯一程序运行处理
 *
 * @return true-无已经运行的程序; false-程序已经运行
 */
static bool alone_run(void)
{
    int fd = -1;
    char buf[16] = {'\0'};
    fd = open(PID_FILE, O_RDWR | O_CREAT, 0666);

    if (fd < 0)
    {
        std::cout << strerror(errno) << std::endl;
        return false;
    }

    if (lock_file(fd) < 0)
    {
        if (errno == EACCES || errno == EAGAIN)
        {
            close(fd);
            std::cout << "proccessing is running!" << std::endl;
            return false;
        }
        printf("can't lock %s: %s\n", PID_FILE, strerror(errno));
    }
    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);

    return true;
}

/**
 * @brief 软件配置
 * 
 */
void soft_setup(void)
{
    // 保证只有一个程序运行
    alone_run() ? (void)0 : exit(0);

#if defined(DEBUG)
    // 调试版本开启coredump
    setup_coredump(SYS_SOFT_CORDUMP_PATH, 1024*1024) ? (void)0 : exit(0);
#endif

    return ;
}