/**
 ******************************************************************************
 * @file    sloop
 * @author  sloop
 * @date    2024-12-16
 * @brief   提供 sloop 系统API,比如超时/周期任务的创建、并行任务创建、互斥任务切换等
 * ==此文件用户不应变更==
 *****************************************************************************/

#ifndef __sloop_H
#define __sloop_H

#include "sl_common.h"

/* sloop 系统初始化 */
void sloop_init(void);
/* sloop 系统运行 */
void sloop(void);

/* MCU tick 中断 */
void sl_tick_irq(void);

/* 获取时间戳 */
uint32_t sl_get_tick(void);

/* 并发任务启动 */
void sl_task_start(pfunc task);

/* 并发任务停止 */
void sl_task_stop(pfunc task);

/* 流转任务切换 */
void sl_goto(pfunc task);

#endif /* __sloop_H */

/************************** END OF FILE **************************/
