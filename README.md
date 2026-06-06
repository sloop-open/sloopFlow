# sloopFlow - 嵌入式宏协程工作流框架 (V2.0)

基于 sloop 实时操作系统内核的嵌入式工作流编程框架，专为 STM32G030 系列微控制器设计。核心创新在于 **flow 宏协程机制**，让开发者用同步代码写法实现异步执行逻辑。

## 项目简介

sloopFlow 的核心是 **flow 宏协程机制**——一种轻量级的状态机实现技术，通过宏展开将顺序代码转换为可中断/恢复的状态机。这意味着你可以用熟悉的同步编程思维编写复杂的异步逻辑，无需回调函数、无需状态机手动设计。

### 核心特性

- **宏协程 Flow**: 用同步写法实现异步执行，消除回调地狱
- **事件驱动**: 灵活的事件发送与等待机制，实现任务间通信
- **轻量级内核**: 极低的资源占用（RAM < 2KB），适合资源受限的嵌入式系统
- **并发任务**: 支持多个 flow 协程并行执行
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

## flow 宏协程机制

### 什么是 flow 宏协程

flow 是一种**宏级别的协程实现**，通过 `SL_BEGIN` 和 `SL_END` 宏包裹代码块，编译器会自动将其转换为状态机。当执行到 `sl_wait_event()` 或 `sl_wait()` 等阻塞操作时，flow 会自动保存当前状态并让出 CPU；当条件满足时，从断点处恢复执行。

**核心优势**：
- **同步写法**: 线性代码结构，易于理解和维护
- **异步执行**: 阻塞调用自动让出 CPU，不阻塞其他任务
- **零额外开销**: 纯宏实现，无需堆内存分配

### API 参考

#### 内核 API

```c
/* 系统初始化 */
void sloop_init(void);

/* 系统运行（主循环）*/
void sloop_run(void);

/* 获取系统时间戳 */
uint32_t sl_get_tick(void);

/* 启动并发 flow 协程 */
void sl_task_start(pfunc task);

/* 停止 flow 协程 */
void sl_task_stop(pfunc task);
```

#### flow 协程 API

```c
/* 定义事件（全局）*/
sl_event_define(evt_name);

/* 发送事件给其他 flow */
sl_send_event(evt);

/* 等待事件（让出 CPU，事件到达后恢复）*/
sl_wait_event(evt);

/* 延时等待（让出 CPU，定时后恢复）*/
sl_wait(ms);

/* flow 协程边界宏 */
SL_BEGIN;  // 标记协程开始
SL_END;    // 标记协程结束
```

## flow 协程编程示例

### 示例1：同步写法实现异步延时

```c
void flow_blink_led(void) {
    SL_BEGIN;
    
    while(1) {
        LED_ON();
        sl_wait(500);  // 等待 500ms，期间让出 CPU
        LED_OFF();
        sl_wait(500);  // 等待 500ms，期间让出 CPU
    }
    
    SL_END;
}
```

**关键特性**：`sl_wait(500)` 不是阻塞式延时，而是保存状态并让出 CPU，500ms 后自动恢复执行。其他任务可在此期间运行。

### 示例2：事件驱动的外卖点餐流程

```c
/* 定义事件 */
sl_event_define(evt_order);    // 下单事件
sl_event_define(evt_arrive);   // 餐品送达事件
sl_event_define(evt_eat_done); // 用餐完成事件

// 用户流程
void flow_user(void) {
    SL_BEGIN;
    
    sl_printf("用户: 下单");
    sl_send_event(evt_order);   // 发送下单事件
    
    sl_task_start(flow_eat);    // 启动用餐任务
    sl_task_start(flow_watch);  // 启动追剧任务（并行）
    
    sl_wait_event(evt_eat_done); // 等待用餐完成（让出 CPU）
    sl_printf("用户: 用餐结束");
    
    SL_END;
}

// 骑手配送流程
void flow_rider(void) {
    SL_BEGIN;
    
    sl_wait_event(evt_order);   // 等待下单
    sl_printf("骑手: 接单");
    sl_wait(3000);              // 配送中（3秒）
    sl_printf("骑手: 送达");
    sl_send_event(evt_arrive);  // 发送送达事件
    
    SL_END;
}

// 用餐流程
void flow_eat(void) {
    SL_BEGIN;
    
    sl_wait_event(evt_arrive);  // 等待送达
    sl_printf("用餐: 开始吃饭");
    sl_wait(5000);              // 吃饭中（5秒）
    sl_printf("用餐: 吃完了");
    sl_send_event(evt_eat_done); // 发送用餐完成事件
    
    SL_END;
}
```

### 对比传统方式

| 方式 | 代码结构 | 可读性 | 维护性 | 资源开销 |
|------|----------|--------|--------|----------|
| **传统状态机** | 分散的 case 分支 | 差 | 差 | 低 |
| **回调函数** | 嵌套回调地狱 | 极差 | 极差 | 中 |
| **flow 宏协程** | 线性同步代码 | 优秀 | 优秀 | 低 |

## 执行原理

flow 宏协程的实现基于 **switch-case 状态机模式**：

```c
// 编译器展开后的伪代码示意
void flow_blink_led(void) {
    static int _state = 0;  // 状态变量
    switch(_state) {
        case 0:
            LED_ON();
            _state = 1;      // 保存状态
            return;          // 让出 CPU
        case 1:
            if(timer_expired(500)) {
                LED_OFF();
                _state = 2;
                return;
            }
            break;
        // ... 后续状态
    }
}
```

每次调用 flow 函数时，根据 `_state` 变量跳转到上次暂停的位置继续执行。

## 版本历史

| 版本 | 日期 | 说明 |
|------|------|------|
| V2.0 | 2026-03-22 | **核心更新**: 引入 flow 宏协程机制，支持同步写法异步执行 |
| V1.0 | 2024-12-16 | 初始版本，基础内核实现 |

## 许可证

本项目采用 MIT 许可证，详见 LICENSE 文件。

## 联系方式

如有问题或建议，请通过以下方式联系：

- 作者: sloop
- 邮箱: sloop@example.com

---

**sloopFlow** - 让嵌入式开发更高效