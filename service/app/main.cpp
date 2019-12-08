/**
 * @file main.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-08-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#define LOG_TAG "MAIN"

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "tools/tools_func/coredump.h"
#include "sys_ctrl/config/version.h"
#include "sys_ctrl/sys_ctrl.h"
#include "usr_apps/usr_apps.h"

#define PID_FILE "/var/run/ai_plat_service.pid"

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

int main(int argc, char **argv)
{
    // 保证只有一个程序运行
    if (!alone_run())
    {
        return 0;
    }

#if !defined(NDEBUG)
    // 调试版本开启coredump
    if (!setup_coredump("/home/wotsen/work", 1024*1024))
    {
        return 0;
    }
#endif

    std::cout << "start app..................version: " << get_service_version() << std::endl;
    sys_init();

    usr_apps_init();
    // tail_work(); // 收尾工作
    while (1)
    {
        sleep(100);
    }

    return 0;
}
