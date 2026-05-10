/**
 ******************************************************************************
 * @file    task_flow
 * @author  sloop
 * @date    2026-3-22
 * @brief   工作流编程示例
 *****************************************************************************/

#include "common.h"

/* 下单事件 */
sl_event_define(evt_order);
/* 外卖送达事件 */
sl_event_define(evt_arrive);
/* 用餐完成事件 */
sl_event_define(evt_eat);

void flow_user(void);
void flow_delivery(void);
void flow_eat(void);
void flow_watch(void);

void task_flow(void)
{
    sl_task_start(flow_user);
    sl_task_start(flow_delivery);

    sl_goto(task_idle);
}

void flow_user(void)
{
    SL_BEGIN;

    /* 下单 */
    sl_printf("user: Place an order");
    sl_send_event(evt_order);

    sl_task_start(flow_eat);
    sl_task_start(flow_watch);

    /* 等待用餐完成 */
    sl_wait_event(evt_eat);

    /* 结束示例 */
    sl_task_stop(flow_watch);
    sl_task_stop(flow_delivery);
    sl_task_stop(flow_user);

    SL_END;
}

void flow_watch(void)
{
    SL_BEGIN;

    sl_printf("user: Watching drama...");
    sl_wait(1000);

    SL_END;
}

void flow_delivery(void)
{
    SL_BEGIN;

    sl_wait_event(evt_order);
    sl_printf("delivery: Prepare meal");

    sl_wait(3000);
    sl_send_event(evt_arrive);
    sl_printf("delivery: Delivered");

    SL_END;
}

void flow_eat(void)
{
    SL_BEGIN;

    /* 等待外卖送达 */
    sl_wait_event(evt_arrive);
    sl_printf("eat: Takeaway arrived, start eating");

    sl_wait(2000);
    sl_printf("eat: Finished eating");

    sl_send_event(evt_eat);

    sl_task_stop(flow_eat);

    SL_END;
}

/************************** END OF FILE **************************/