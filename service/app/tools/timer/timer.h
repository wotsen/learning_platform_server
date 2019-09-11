/*
 * @Date: 2019-08-16 21:32:05
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-16 21:46:25
 */
#pragma once

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ZH_CN)
#define LANGUAGE_ZH
#endif

#define OS_TICKS_PER_SEC	200ul			/* 5毫秒为一个tick */
#define TICK_NANOSEC	((1000 / OS_TICKS_PER_SEC) * 1000 * 1000)				/* 一个tick 对应的纳秒数 */
#define SYS_TIME_STR_LEN	255													/* 系统时间长度 */

struct seria_time
{
	char     tm_zone[100]; // 时区
	uint16_t cur_days;	// 当前天数
	uint16_t year;		// 年
	uint8_t  month;		// 月
	uint8_t  day;		// 日
	char     week[16];	// 星期
	uint8_t  hour;		// 时
	uint8_t  min;		// 分
	uint8_t  sec;		// 秒
	uint32_t usec;		// 毫秒
};

#define OS_MS(ms)		(uint32_t)(((ms) * OS_TICKS_PER_SEC + 999) / 1000L) 	/* ms非零时保证至少一个tick */
#define OS_SEC(sec)		(uint32_t)((sec) * OS_TICKS_PER_SEC) 					/* 一秒钟 */
#define OS_MIN(min)		(uint32_t)((min) * 60L * OS_TICKS_PER_SEC) 				/* 一分钟 */
#define OS_HOUR(hr)		(uint32_t)((hr) * 60L * 60L * OS_TICKS_PER_SEC) 		/* 一小时 */

uint32_t getostime(void); 						/* 获取系统时间,返回秒数 */
void ostime_delay(uint32_t tick); 				/* 指定系统延时 */
void print_ostime(void); 						/* 打印时间 */
char *getostimestr(char *str); 					/* 获取时间字符串 */
struct tm *getostime_format(struct tm *tm); 	/* 获取格式化时间 */
struct seria_time *get_serail_time(struct seria_time *time); // 相对1900年修正过的时间

void print_monthly(uint8_t month); 				/* 打印一个月的月历 */
void print_calendar(uint16_t year); 			/* 打印一年的年历 */

#ifdef __cplusplus
}
#endif
