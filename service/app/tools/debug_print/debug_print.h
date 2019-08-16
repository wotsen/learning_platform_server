/*
 * @Date: 2019-08-11 17:30:33
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-11 21:29:52
 */
/**
 * @file debug_print.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 调试打印输出，开启NDEBUG才有用
 * @version 0.1
 * @date 2019-08-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef __DEBUG_PRINT_H__
#define __DEBUG_PRINT_H__

#ifndef LOG_TAG
#define LOG_TAG "DBG_PRINT"
#endif

#ifndef NDEBUG
#include <stdio.h>
#include <time.h>

#define DBG_GREEN "\033[32;40;22m\033m"

#define dbg_print(fmt, args...) { \
    time_t timep; \
    struct tm *p = NULL; \
    time(&timep); \
    p = localtime(&timep); \
    if (NULL == p) { \
        fprintf(stdout, "%sI/%s    [%s %s %d] " fmt, DBG_GREEN, LOG_TAG, __FILE__, __func__, __LINE__, ##args); \
    } else { \
        fprintf(stdout, "%sI/%s    [%02d-%02d %02d:%02d:%02d %s %s %d] " fmt, \
                DBG_GREEN, LOG_TAG, \
                p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, \
                __FILE__, __func__, __LINE__, ##args); \
    } \
    }

#else

#define dbg_print(fmt, args...) (void)0

#endif // NDEBUG

#endif // !1__DEBUG_PRINT_H__