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

#include <fstream>
#include <nlohmann/json.hpp>

class AbsSysLoadJson
{
public:
    virtual ~AbsSysLoadJson(){};
    nlohmann::json &get_json(void)
    {
        return j;
    }

protected:
    // tfile::Reader *file;
    std::fstream json_file;
    // neb::CJsonObject *json;
    nlohmann::json j;
};
