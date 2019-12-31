/**
 * @file alarm.h
 * @author 余王亮 (astralrovers@outlook.com)
 * @brief 告警采用各模块主动出发告警的形式，由告警模块统一产生告警上报，各模块需要注册告警，
 * 			注册信息、数据内容格式，告警路径，等等。
 * 			告警模块采用扩展模式，能上报到的平台可扩展(邮件，web，snmp等)
 * @version 0.1
 * @date 2019-09-13
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

///< 告警任务初始化
void task_alarm_init(void);
