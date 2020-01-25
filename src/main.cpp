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
#include <iostream>
#include "util_time/util_time.h"
#include "sys_ctrl/config/version.h"
#include "sys_ctrl/sys_ctrl.h"
#include "usr_apps/usr_apps.h"

int main(int argc, char **argv)
{
    std::cout << "start app..................version: " << get_service_version() << std::endl;

    // 系统初始化
    sys_init();

    // 应用初始化
    usr_apps_init();

    // tail_work(); // 收尾工作

    while (1)
    {
        ostime_delay(OS_SEC(100));
    }

    return 0;
}
