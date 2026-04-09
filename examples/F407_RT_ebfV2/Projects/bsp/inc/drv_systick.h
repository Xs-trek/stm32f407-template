#ifndef __DRV_SYSTICK_H__
#define __DRV_SYSTICK_H__

#include "drv_config.h"

#ifdef DRV_USING_SYSTICK

#include "stm32f4xx.h"

/*
 * 说明：
 * 本头文件提供 STM32F407 SysTick 驱动的公共接口。
 *
 * 当前版本定义：
 * 1. 当前版本是对 Cortex-M4 SysTick 定时器的薄封装层。
 * 2. 当前版本覆盖：
 *    - SysTick 初始化（重装值 + 使能）
 *    - 时钟源选择
 *    - 计数器使能 / 禁用
 *    - 中断使能 / 禁用
 *    - 当前计数值读取
 *    - COUNTFLAG 查询
 *
 * 限制：
 * 1. 当前版本不提供延时函数（延时属于应用/board 层职责）。
 */

/* ==================== 初始化 ==================== */

/*
 * 说明：
 * 配置 SysTick 重装值并使能。
 *
 * 当前版本定义：
 * 返回 0 表示成功，非零表示 ticks 超出范围。
 */
uint32_t systick_init(uint32_t ticks);

/* ==================== 时钟源 ==================== */

/* 说明：设置时钟源（1 = 处理器时钟，0 = 外部参考时钟）。 */
void systick_set_clock_source(uint32_t source);

/* ==================== 使能 / 禁用 ==================== */

/* 说明：使能 SysTick 计数器。 */
void systick_enable(void);

/* 说明：禁用 SysTick 计数器。 */
void systick_disable(void);

/* 说明：使能 SysTick 中断。 */
void systick_enable_irq(void);

/* 说明：禁用 SysTick 中断。 */
void systick_disable_irq(void);

/* ==================== 状态 ==================== */

/* 说明：获取当前计数器值。 */
uint32_t systick_get_current(void);

/* 说明：检查 COUNTFLAG（返回非零表示自上次读取后计数到 0）。 */
uint32_t systick_get_countflag(void);

#endif /* DRV_USING_SYSTICK */
#endif /* __DRV_SYSTICK_H__ */
