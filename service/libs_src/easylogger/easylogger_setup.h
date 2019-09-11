/*
 * @Date: 2019-08-10 23:13:29
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-25 09:55:02
 */
/**
 * @file easylogger_setup.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-08-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef __EASYLOGGER_SETUP_H__
#define __EASYLOGGER_SETUP_H__
#include <stdbool.h>
#include "inc/elog.h"

#ifdef __cplusplus
extern "C" {
#endif

struct elog_custom_config {
    char *log_path;
};

bool easylogger_setup(struct elog_custom_config *elog_config);

#ifdef __cplusplus
}
#endif
#endif // !1__EASYLOGGER_SETUP_H__