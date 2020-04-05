#include <iostream>
#include <exception>
#include <signal.h>
#include "app.h"

// 信号屏蔽集
static sigset_t signal_mask;

// 屏蔽所有信号
static void mask_all_signal(void)
{
    sigemptyset(&signal_mask);
    // 加入所有信号
    sigfillset(&signal_mask);

    // 所有线程都屏蔽该信号集
    if (pthread_sigmask(SIG_BLOCK, &signal_mask, nullptr) != 0)
    {
        std::cout << "error." << std::endl;
        exit(-1);
    }
}

// 由主线程统一处理所有信号
static void wait_signal(void)
{
    while (1)
    {
        int sig = 0;

        if (sigwait(&signal_mask, &sig))
        {
            continue;
        }

        // 特殊处理
        if (SIGINT == sig || SIGTERM == sig || SIGKILL == sig)
        {
            wotsen::PlatSrvApp::plat_srv_app()->stop();
            std::cout << "exit." << std::endl;
            exit(0);
        }
    }
}

int main(int argc, char **argv)
{
    // mask_all_signal();

    std::cout << "start app.................." << std::endl;

    wotsen::PlatSrvApp::plat_srv_app()->run();

    // wait_signal();
    while (1);

    return 0;
}
