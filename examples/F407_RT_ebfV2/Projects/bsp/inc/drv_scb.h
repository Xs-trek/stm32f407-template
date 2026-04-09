#ifndef __DRV_SCB_H__
#define __DRV_SCB_H__

#include "drv_config.h"

#ifdef DRV_USING_SCB

#include "stm32f4xx.h"

/*
 * 说明：
 * 本头文件提供 STM32F407 SCB 驱动的公共接口。
 *
 * 当前版本定义：
 * 1. 当前版本是对 Cortex-M4 SCB 寄存器的薄封装层。
 * 2. 当前版本覆盖：
 *    - 系统复位
 *    - SLEEPDEEP / SLEEPONEXIT 控制
 *    - 向量表重定位
 *    - ICSR / AIRCR 状态读取
 *
 * 限制：
 * 1. 当前版本不提供 FPU 控制接口。
 * 2. 当前版本不提供 MPU 配置接口。
 * 3. 当前版本不提供 fault handler 配置接口。
 */

/* ==================== 系统复位 ==================== */

/* 说明：通过 AIRCR 执行系统复位（不会返回）。 */
void scb_system_reset(void);

/* ==================== 睡眠控制 ==================== */

/* 说明：使能或禁用 SLEEPDEEP 位（enable != 0 为使能）。 */
void scb_set_sleepdeep(uint8_t enable);

/* 说明：使能或禁用 SLEEPONEXIT 位（enable != 0 为使能）。 */
void scb_set_sleeponexit(uint8_t enable);

/* ==================== 向量表 ==================== */

/*
 * 说明：
 * 重定位向量表。
 *
 * 限制：
 * offset 必须按向量表大小对齐。
 */
void scb_set_vector_table(uint32_t base, uint32_t offset);

/* ==================== 状态读取 ==================== */

/* 说明：获取 ICSR 寄存器值。 */
uint32_t scb_get_icsr(void);

/* 说明：获取 AIRCR 寄存器值。 */
uint32_t scb_get_aircr(void);

#endif /* DRV_USING_SCB */
#endif /* __DRV_SCB_H__ */
