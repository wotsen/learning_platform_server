/*
 * @Date: 2019-08-11 21:42:59
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-15 00:09:41
 */
/**
 * @file sys_parameter.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 配置参数
 * @version 0.1
 * @date 2019-08-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef __SYS_PARAMETER_H__
#define __SYS_PARAMETER_H__
#include <stdio.h>
#include "inttypes.h"
#include "../../tools/tfile/tfile.h"
#include "../../tools/com_def.h"

struct sys_parameter {
#define SYS_PARAMETER_MAGIC 0x190ADB
    uint32_t para_magic;
    uint16_t para_crc;
}PACKED;

class SysParameter : tfile::ReaderWriter {
public:
#define SYS_PARAMETER_FILENAME "ai_runtime/data/para.bin"

    ~SysParameter();

    struct sys_parameter *get_para_ptr(void) {
        return para;
    }

    static SysParameter * get_sys_para(void);

    bool reset_default_parameter(void);

    void cal_crc16(void);

    bool save_para(size_t start, size_t len);

    bool is_ok(void) const {
        return (NULL != para && NULL != fp);
    }

private:
    SysParameter() { }

    static SysParameter *instance;
    struct sys_parameter *para;
    FILE *fp;
};

bool sys_para_init(void);
struct sys_parameter * get_para_ptr(void);

#endif // !1__SYS_PARAMETER_H__