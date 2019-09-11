/*
 * @Date: 2019-08-12 23:33:56
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-25 14:09:48
 */
/**
 * @file sys_load_json.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-08-12
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include <tfile/tfile.h>
#include <CJsonObject/CJsonObject.hpp>

class AbsSysLoadJson
{
public:
    virtual ~AbsSysLoadJson(){};
    neb::CJsonObject *get_json(void)
    {
        return json;
    }

protected:
    tfile::Reader *file;
    neb::CJsonObject *json;
};
