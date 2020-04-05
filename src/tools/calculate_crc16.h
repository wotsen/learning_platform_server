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
#ifndef __wotsenLearnPlatSrv_CACULATE_CRC16_H___
#define __wotsenLearnPlatSrv_CACULATE_CRC16_H___

#include <inttypes.h>

namespace wotsen
{

/* 计算crc16 */
uint16_t calculate_crc16(uint16_t crc, uint8_t *ptr, uint32_t len);

} // namespace wotsen

#endif // !__wotsenLearnPlatSrv_CACULATE_CRC16_H___
