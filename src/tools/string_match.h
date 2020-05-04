/**
 * @file string_match.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-05-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef __wotsenLearnPlatSrv_STRING_MATCH_H__
#define __wotsenLearnPlatSrv_STRING_MATCH_H__

#include <string>

namespace wotsen
{

// 邮箱校验
bool email_match(const std::string &email);
// 手机号码校验
bool zh_cellphone_number_match(const std::string &number);
// 身份证校验
bool id_card_match(const std::string &number);
// ipv4 校验
bool ipv4_match(const std::string &ip);
// ipv6 校验
bool ipv6_match(const std::string &ip);
	
} // namespace wotsen


#endif // !__wotsenLearnPlatSrv_STRING_MATCH_H__