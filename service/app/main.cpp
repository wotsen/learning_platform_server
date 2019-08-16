/*
 * @Date: 2019-08-10 20:46:55
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-14 23:08:55
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
#include "common/common.h"

int main(int argc, char** argv)
{
    std::cout << "start app.................." << std::endl;
    sys_init();

    // task_init(); // 初始化任务队列
    // tail_work(); // 收尾工作
    // while (1) { sleep(100); }

    return 0;
}