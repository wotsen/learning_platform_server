/*
 * @Date: 2019-08-14 23:12:24
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-15 22:48:55
 */
/**
 * @file sys_parameter.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 系统参数
 * @version 0.1
 * @date 2019-08-14
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#define LOG_TAG "SYS_PARAMETER"

#include <iostream>
#include "../../tools/tools_func/calculate_crc16.h"
#include "../../module/easylogger/inc/elog.h"
#include "sys_parameter.h"

SysParameter *SysParameter::instance = nullptr;

SysParameter *SysParameter::get_sys_para(void)
{
    if (nullptr != instance) {
        return instance;
    }

    instance = new SysParameter();
    instance->para = new struct sys_parameter;

    memset(instance->para, 0, sizeof(struct sys_parameter));

    instance->fp = fopen(SYS_PARAMETER_FILENAME, "rb+");

    if (NULL == instance->fp) {
        instance->fp = fopen(SYS_PARAMETER_FILENAME, "wb+");
        if (NULL == instance->fp) {
            instance->para = NULL;
            delete instance;
            log_e("load sys parameter error : %s\n", strerror(errno));
            exit(-1);
        }
        if (!instance->reset_default_parameter()) {
            fclose(instance->fp);
            delete instance->para;
            delete instance;
            log_e("load sys parameter error : %s\n", strerror(errno));
            exit(-1);
        }
        return instance;
    }

    fseek(instance->fp, 0, SEEK_SET);

    fread(instance->para, sizeof(struct sys_parameter), 1, instance->fp);

    // TODO : 校验

    return instance;
}

SysParameter::~SysParameter(void)
{
    if (nullptr != instance) {
        if (NULL != instance->fp)
        {
            fclose(instance->fp);
            instance->fp = NULL;
        }

        if (NULL != instance->para) {
            delete instance->para;
            instance->para = NULL;
        }
    }
}

void SysParameter::cal_crc16(void)
{
    if (NULL == para) return;
    para->para_crc = calculate_crc16(0, (uint8_t *)para, sizeof(struct sys_parameter) - sizeof(para->para_crc));
}

bool SysParameter::reset_default_parameter(void)
{
    if (!is_ok()) { return false; }

    para->para_magic = SYS_PARAMETER_MAGIC;
    // TODO : other para
    cal_crc16();

    return save_para(0, sizeof(struct sys_parameter));
}

bool SysParameter::save_para(size_t start, size_t len)
{
    if (!is_ok()) { return false; }

    fseek(fp, start, SEEK_SET);
    return len != fwrite((uint8_t *)para + start, len, 1, fp);
}

struct sys_parameter * get_para_ptr(void)
{
    return SysParameter::get_sys_para()->get_para_ptr();
}

bool sys_para_init(void)
{
    struct sys_parameter *para = get_para_ptr();
    
    if (NULL != para) {
        log_i("sys parameter load success!");
        return true;
    }

    log_e("sys parameter load faild!");
    return false;
}