/**
 * @file com_def.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-08-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

/* 结构体对齐 */
#define PACKED		__attribute__ ((packed))

/* 标识符转字符串 */
#define TO_STR(expr) #expr

/* 整形变量转字符串 */
#define VAL_TO_STR(num) TO_STR(num)
