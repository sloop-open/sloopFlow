/**
 ******************************************************************************
 * @file    sloop
 * @author  sloop
 * @date    2024-12-16
 * @brief   提供 sloop 系统API,比如超时/周期任务的创建、并行任务创建、互斥任务切换等
 *
 * ==此文件用户不应变更==
 *****************************************************************************/

#include "sloop.h"

#define sl_check_task_not_null()      \
    if (task == NULL)                 \
    {                                 \
        sl_error("The task is null"); \
        return;                       \
    }

void print_null(const char *sFormat, ...) {}

/* 并行任务运行 */
void parallel_task_run(void);

/* ============================================================== */

/* 系统心跳 */
void system_heartbeat(void);
/* 计数循环次数 */
void loop_counter(void);
/* 计算 cpu 负载 */
void calcul_cpu_load(void);
/* 负载警告 */
void load_warning(void);

static volatile uint32_t tick;

static int loop;

static int load;

static int loop_us;

/* ============================================================== */

/* sloop 系统初始化 */
void sloop_init(void)
{
    sl_prt_brYellow("==================================");
    sl_prt_brYellow("========= sloop  (^-^) ==========");
    sl_prt_brYellow("==================================");

    /* 启用单次任务 */
    sl_task_start(once_task_run);

    /* 启动 loop 计数 */
    sl_task_start(loop_counter);

    /* 启动 cpu 负载计算 */
    sl_cycle_start(100, calcul_cpu_load);

    /* 启用系统心跳 */
    sl_cycle_start(1000, system_heartbeat);

    sl_printf("system heartbeat start");
}

/* sloop 系统运行 */
void sloop(void)
{
    /* 并行任务运行 */
    parallel_task_run();
}

/* ============================================================== */

/* mcu tick 中断 */
void sl_tick_irq(void)
{
    tick++;
}

/* ============================================================== */

/* 系统心跳 */
void system_heartbeat(void)
{
    static int count;

    SEGGER_RTT_SetTerminal(1);

    sl_prt_var(count);

    SEGGER_RTT_SetTerminal(0);

    count++;
}

/* ============================================================== */

/* CPU 负载基数，即 CPU = 100% 时，一个 loop 的时间 单位0.1us，约定当loop 达到100us时，CPU 占用为100% */
#define LOOP_BASE 1000

/* 计数循环次数 */
void loop_counter(void)
{
    loop++;
}

/* 计算 cpu 负载 */
void calcul_cpu_load(void)
{
    if (loop == 0)
        return;

    static char warning;

    loop_us = 1000000 / loop;

    load = loop_us * 1000 / LOOP_BASE;

    loop = 0;

    /* 负载超过 80% ,日志警告 */
    if (warning == 0)
    {
        if (load > 800)
        {
            sl_cycle_start(1000, load_warning);

            warning = 1;
        }

        return;
    }

    /* 解除警告 */
    if (load < 600)
    {
        sl_cycle_stop(load_warning);

        warning = 0;
    }
}

/* 负载警告 */
void load_warning(void)
{
    sl_error("cpu load over 80%%, reach %2d.%d%%, average loop time: %d.%d us", load / 10, load % 10, loop_us / 10, loop_us % 10);
}

/* ============================================================== */

/* 获取时间戳 */
uint32_t sl_get_tick(void)
{
    return tick;
}

/* 阻塞式延时 */
void sl_delay(int ms)
{
    uint32_t tick_start = tick;

    /* 传入1ms，实际延时至少1ms */
    ms == 1 ? ms++ : ms;

    while (1)
    {
        if ((uint32_t)(tick - tick_start) >= ms)
        {
            return;
        }
    }
}

/* ============================================================== */

/* 并行任务注册表 */
static pfunc task_reg[SL_PARALLEL_LIMIT];

/* 并行任务运行 */
void parallel_task_run(void)
{
    for (int i = 0; i < SL_PARALLEL_LIMIT; i++)
    {
        if (task_reg[i] == NULL)
            continue;

        task_reg[i]();
    }
}

/* 并行任务 */
void sl_task_start(pfunc task)
{
    sl_check_task_not_null();

    for (int i = 0; i < SL_PARALLEL_LIMIT; i++)
    {
        if (task_reg[i] == task)
        {
            return;
        }
    }

    for (int i = 0; i < SL_PARALLEL_LIMIT; i++)
    {
        if (task_reg[i] == NULL)
        {
            task_reg[i] = task;

            return;
        }
    }

    sl_error("parallel task overflow, limit %2d", SL_PARALLEL_LIMIT);
}

void sl_task_stop(pfunc task)
{
    sl_check_task_not_null();

    for (int i = 0; i < SL_PARALLEL_LIMIT; i++)
    {
        if (task_reg[i] == task)
        {
            task_reg[i] = NULL;

            return;
        }
    }
}

/************************** END OF FILE **************************/
