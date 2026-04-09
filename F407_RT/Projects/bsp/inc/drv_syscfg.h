#ifndef __DRV_SYSCFG_H__
#define __DRV_SYSCFG_H__

#include "drv_config.h"

#ifdef DRV_USING_SYSCFG

#include "stm32f4xx.h"

/*
 * 说明：
 * 本头文件提供 STM32F407 SYSCFG 驱动的公共接口。
 *
 * 当前版本定义：
 * 1. 当前版本覆盖：
 *    - SYSCFG 时钟使能
 *    - EXTI 端口映射
 *
 * 限制：
 * 1. 当前版本不提供内存重映射接口。
 * 2. 当前版本不提供补偿单元控制接口。
 */

/* ==================== 时钟控制 ==================== */

/* 说明：使能 SYSCFG 外设时钟。 */
void syscfg_clk_enable(void);

/* ==================== EXTI 端口映射 ==================== */

/*
 * 说明：
 * 将 EXTI line 映射到指定 GPIO 端口。
 *
 * 前提：
 * 1. pin 范围为 0~15
 * 2. port_code：0=PA, 1=PB, ..., 8=PI
 *
 * 当前版本定义：
 * 函数内部自动使能 SYSCFG 时钟。
 */
void syscfg_set_exti_port(uint8_t pin, uint8_t port_code);

#endif /* DRV_USING_SYSCFG */
#endif /* __DRV_SYSCFG_H__ */
