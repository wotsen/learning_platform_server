#include <iostream>
#include <exception>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include "os_param.h"
#include "app.h"

// 移到main函数第一行
void alone_run(void)
{
	// 保证只有一个程序运行
    int fd = open(PID_FILE, O_RDWR | O_CREAT, 0666);
    char buf[16] = {'\0'};

    if (fd < 0)
    {
        std::cout << strerror(errno) << std::endl;
        exit(-1);
    }

	auto lock_file = [&](void) -> int
	{
		struct flock fl;

		fl.l_type = F_WRLCK; // 锁定文件，锁定失败则是已经运行了一个
		fl.l_start = 0;
		fl.l_whence = SEEK_SET;
		fl.l_len = 0;

		return fcntl(fd, F_SETLK, &fl);
	};

    if (lock_file() < 0)
    {
        if (errno == EACCES || errno == EAGAIN)
        {
            close(fd);
            std::cout << "proccessing is running!" << std::endl;
            exit(-1);
        }
		std::cout << "can't lock " << PID_FILE << ", reason :" << strerror(errno) << std::endl;
    }

    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);
}

int main(int argc, char **argv)
{
    alone_run();

    wotsen::PlatSrvApp::plat_srv_app()->run(argc, argv);

    while (1);

    return 0;
}
