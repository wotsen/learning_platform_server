/*
 * @Date: 2019-08-12 23:33:56
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-12 23:50:31
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

#ifndef __SYS_LOAD_JSON_H__
#define __SYS_LOAD_JSON_H__
#include "../../tools/tfile/tfile.h"
#include "../../module/cjson_object/CJsonObject.hpp"

class AbsSysLoadJson {
public:
    virtual ~AbsSysLoadJson() { };
    neb::CJsonObject *get_json(void) {
        return json;
    }

protected:
    tfile::Reader *file;
    neb::CJsonObject *json;
};

#endif // !1__SYS_LOAD_JSON_H__