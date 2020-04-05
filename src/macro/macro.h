/**
 * @file macro.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-04-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __wotsenLearnPlatSrv_MACRO_H__
#define __wotsenLearnPlatSrv_MACRO_H__

/* 结构体对齐 */
#define PACKED		__attribute__ ((packed))

/* 标识符转字符串 */
#define TO_STR(expr) #expr

/* 整形变量转字符串 */
#define VAL_TO_STR(num) TO_STR(num)

#endif // !__wotsenLearnPlatSrv_MACRO_H__
