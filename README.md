# sloopFlow - 嵌入式工作流编程框架 (V2.0)

基于 sloop 实时操作系统内核的嵌入式工作流编程框架，专为 STM32G030 系列微控制器设计。

## 项目简介

sloopFlow 是一个轻量级的嵌入式工作流编程框架，提供事件驱动的并发任务管理能力。该框架基于自主研发的 sloop 实时操作系统内核，支持任务的创建、停止、事件发送与等待等操作。

### 核心特性

- **工作流编程**: 基于事件驱动的状态机编程模型
- **并发任务**: 支持多个任务并行执行
- **事件机制**: 灵活的事件发送与等待机制
- **轻量级内核**: 极低的资源占用，适合资源受限的嵌入式系统
- **RTT 调试**: 集成 SEGGER RTT 实现高效调试

## 项目结构

```
project/
├── Core/                 # STM32 HAL 核心文件
│   ├── Inc/              # 头文件
│   └── Src/              # 源文件
├── Drivers/              # 硬件驱动
│   ├── CMSIS/            # ARM CMSIS 核心
│   └── STM32G0xx_HAL_Driver/  # STM32G0 HAL 驱动
├── cmake/                # CMake 配置
│   ├── stm32cubemx/      # STM32CubeMX 生成配置
│   ├── gcc-arm-none-eabi.cmake
│   └── starm-clang.cmake
└── user/                 # 用户应用代码
    ├── app/              # 应用层
    │   ├── config/       # 配置文件
    │   ├── tasks/        # 任务实现
    │   ├── _main.c       # 应用入口
    │   └── _main.h       # 应用头文件
    └── sloop/            # sloop 内核
        ├── kernel/       # 内核实现
        └── RTT/          # SEGGER RTT 调试
```

## 技术栈

| 组件 | 说明 |
|------|------|
| **MCU** | STM32G030xx (ARM Cortex-M0+) |
| **内核** | sloop Real-Time Operating System |
| **构建工具** | CMake 3.22+ |
| **编译器** | ARM GCC / ARM Clang |
| **调试工具** | SEGGER RTT |

## 快速开始

### 环境要求

- CMake 3.22 或更高版本
- ARM GCC 工具链 (arm-none-eabi-gcc)
- STM32CubeMX (可选，用于生成项目配置)
- J-Link 调试器 (推荐)

### 构建项目

```bash
cd project
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4
```

### 烧录与调试

使用 J-Link 调试器进行烧录：

```bash
# 使用 OpenOCD
openocd -f jlink.txt -c "program project.elf verify reset exit"

# 或使用 J-Link Commander
JLinkExe -device STM32G030F6Px -if SWD -speed 4000
loadbin project.bin, 0x08000000
r
g
```

## API 参考

### 内核 API

```c
/* 系统初始化 */
void sloop_init(void);

/* 系统运行 */
void sloop_run(void);

/* 获取系统时间戳 */
uint32_t sl_get_tick(void);

/* 启动并发任务 */
void sl_task_start(pfunc task);

/* 停止并发任务 */
void sl_task_stop(pfunc task);

/* 切换流转任务 */
void sl_goto(pfunc task);
```

### 工作流 API

```c
/* 定义事件 */
sl_event_define(evt_name);

/* 发送事件 */
sl_send_event(evt);

/* 等待事件 */
sl_wait_event(evt);

/* 延时等待 */
sl_wait(ms);

/* 工作流宏 */
SL_BEGIN;
SL_END;
```

## 工作流编程示例

项目包含一个完整的工作流示例，模拟外卖点餐流程：

```c
/* 定义事件 */
sl_event_define(evt_order);    // 下单事件
sl_event_define(evt_arrive);   // 送达事件
sl_event_define(evt_eat);      // 用餐完成事件

void flow_user(void) {
    SL_BEGIN;
    
    sl_printf("user: Place an order");
    sl_send_event(evt_order);   // 发送下单事件
    
    sl_task_start(flow_eat);    // 启动用餐任务
    sl_task_start(flow_watch);  // 启动追剧任务
    
    sl_wait_event(evt_eat);     // 等待用餐完成
    
    SL_END;
}
```

## 版本历史

| 版本 | 日期 | 说明 |
|------|------|------|
| V2.0 | 2026-03-22 | 新增工作流编程模型，优化事件驱动架构 |
| V1.0 | 2024-12-16 | 初始版本，基础内核实现 |

## 许可证

本项目采用 MIT 许可证，详见 LICENSE 文件。

## 联系方式

如有问题或建议，请通过以下方式联系：

- 作者: sloop
- 邮箱: sloop@example.com

---

**sloopFlow** - 让嵌入式开发更高效