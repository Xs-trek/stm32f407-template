# F407_RT 索引（聚焦 bsp）

本索引为 F407_RT 工程的简明索引，重点说明 `bsp`（寄存器驱动）及顶层目录用途，便于快速定位与维护。风格：统一、简短、以实用为主。

## 目录说明

- `app`：用户应用代码。包含应用入口、任务实现、业务逻辑与测试示例。应依赖 `board` 和 `bsp` 提供的硬件抽象层，不直接操作寄存器。
- `board`：板级硬件配置与封装。负责映射板上外设、实现板级初始化（引脚、外设时钟、外设选择），并提供板级 API（例如 `board_init()`）和引脚定义。
- `bsp`：STM32F407 寄存器驱动（核心说明）
  - 定义：Board Support Package，聚焦 MCU 寄存器级驱动与底层外设控制。
  - 位置：常见于 `Projects/bsp/inc/`、`Projects/bsp/src/` 或仓库根 `bsp/` 目录。
  - 命名与组织：驱动以 `drv_<peripheral>.h/.c` 命名；`drv_config.h` 放置平台开关与引脚映射；中断/ DMA 配置与底层初始化函数集中管理。
  - 职责：提供初始化/反初始化、基本读写与配置接口（如 `drv_usart_init()`、`drv_spi_transfer()`）；管理外设时钟、GPIO、NVIC、DMA 等；为 `board` 与 `app` 提供稳定可重用的硬件接口。
  - 约束：不得包含应用逻辑或 RTOS 任务；避免导入大型闭源库；对外只暴露稳定的硬件 API。
  - 扩展建议：每个驱动实现 `init`/`deinit`，提供错误码与状态读取接口；头文件顶部写明依赖与使用示例。
- `CMSIS`：器件头文件与核心支持（如 `core_cm4.h`、`stm32f407xx.h`、`system_stm32f4xx.c`），供 BSP 与移植层使用。
- `config`：驱动与模块的配置宏（如 `f407_rt_config.h`、`f407_rt_modules.h`），用于集中管理编译开关、参数与引脚映射。
- `kernel`：rtthread nano 内核移植与封装代码，包括移植头文件与适配接口。
- `startup`：启动汇编与向量表（`startup_stm32f407xx.s`），负责复位入口、堆栈与中断向量的初始设置。

## 快速索引（常用路径）

- `Projects/app/src/` — 应用层
- `Projects/board/` — 板级封装
- `Projects/bsp/inc/`, `Projects/bsp/src/` — BSP 驱动
- `CMSIS/` — 器件与核心支持
- `config/` — 驱动与平台配置
- `kernel/` — 内核移植与源码
- `startup/startup_stm32f407xx.s` — 启动代码

## 维护建议

- BSP 保持纯净、开源友好：仅包含源代码与许可兼容的第三方代码；闭源二进制请单独管理并列入 `.gitignore`。
- 为每个驱动补充简短的使用示例与依赖说明，便于索引检索与快速上手。
- 将构建产物与个人 IDE 设置列入 `.gitignore`（仓库已更新）。

如需把本索引扩展为自动化导航或更详细的 API 目录，我可以继续实现。
