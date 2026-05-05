/**
 ******************************************************************************
 * @file    task_flow
 * @author  sloop
 * @date    2026-3-22
 * @brief   工作流编程示例
 *****************************************************************************/

#include "common.h"

/* 下单事件 */
FLOW_EVENT_DEFINE(evt_order);
/* 外卖送达事件 */
FLOW_EVENT_DEFINE(evt_arrive);
/* 用餐完成事件 */
FLOW_EVENT_DEFINE(evt_eat);

void flow_user(void);
void flow_delivery(void);
void flow_eat(void);
void flow_watch(void);

void task_flow(void)
{
    sl_task_start(flow_user);
    sl_task_start(flow_delivery);

    sl_task_stop(task_flow);
}

void flow_user(void)
{
    FLOW_BEGIN;

    /* 下单 */
    sl_printf("user: Place an order");
    FLOW_SEND_EVENT(evt_order);

    sl_task_start(flow_eat);
    sl_task_start(flow_watch);

    /* 等待用餐完成 */
    FLOW_WAIT_EVENT(evt_eat);

    /* 结束示例 */
    sl_task_stop(flow_watch);
    sl_task_stop(flow_delivery);
    sl_task_stop(flow_user);

    FLOW_END;
}

void flow_watch(void)
{
    FLOW_BEGIN;

    sl_printf("user: Watching drama...");
    FLOW_WAIT(1000);

    FLOW_END;
}

void flow_delivery(void)
{
    FLOW_BEGIN;

    FLOW_WAIT_EVENT(evt_order);
    sl_printf("delivery: Prepare meal");

    FLOW_WAIT(3000);
    FLOW_SEND_EVENT(evt_arrive);
    sl_printf("delivery: Delivered");

    FLOW_END;
}

void flow_eat(void)
{
    FLOW_BEGIN;

    /* 等待外卖送达 */
    FLOW_WAIT_EVENT(evt_arrive);
    sl_printf("eat: Takeaway arrived, start eating");

    FLOW_WAIT(2000);
    sl_printf("eat: Finished eating");

    FLOW_SEND_EVENT(evt_eat);

    sl_task_stop(flow_eat);

    FLOW_END;
}

/************************** END OF FILE **************************/