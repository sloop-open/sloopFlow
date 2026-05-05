/**
 ******************************************************************************
 * @file    task_baseInit
 * @author  sloop
 * @date    2025-1-13
 * @brief   基础初始化
 *****************************************************************************/

#include "common.h"

/* 基础驱动初始化 */
void task_baseInit(void)
{
    sl_task_start(task_flow);

    sl_task_stop(task_baseInit);
}

/************************** END OF FILE **************************/
