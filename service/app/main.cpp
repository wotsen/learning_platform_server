/*
 * @Date: 2019-08-10 20:46:55
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-17 11:48:59
 */
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
#include "common/common.h"
#include "common/usr_apps/usr_apps.h"

int main(int argc, char** argv)
{
    // TODO: 保证只有一个程序运行
    std::cout << "start app.................." << std::endl;
    sys_init();

    usr_apps_init(); // 应用初始化
    // tail_work(); // 收尾工作
    while (1) { sleep(100); }

    return 0;
}