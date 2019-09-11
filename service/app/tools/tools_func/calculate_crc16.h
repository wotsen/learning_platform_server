/*
 * @Date: 2019-08-14 23:51:51
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-14 23:53:01
 */
/**
 * @file calculate_crc16.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-08-14
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include <inttypes.h>

uint16_t calculate_crc16(uint16_t crc, uint8_t *ptr, uint32_t len);
