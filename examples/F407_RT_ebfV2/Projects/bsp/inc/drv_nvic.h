#ifndef __DRV_NVIC_H__
#define __DRV_NVIC_H__

#include "drv_config.h"

#ifdef DRV_USING_NVIC

#include "stm32f4xx.h"

/*
 * 说明：
 * 本头文件提供 STM32F407 NVIC 驱动的公共接口。
 *
 * 当前版本定义：
 * 1. 当前版本是对 CMSIS NVIC 函数的薄封装层。
 * 2. 当前版本覆盖：
 *    - 优先级分组设置
 *    - 中断优先级设置（抢占 + 子优先级编码）
 *    - 中断使能 / 禁用
 *    - 挂起状态设置 / 清除
 *    - 活动状态查询
 *
 * 限制：
 * 1. 当前版本不提供 NVIC 系统复位接口（由 drv_scb 负责）。
 * 2. 当前版本不提供中断向量表重定位接口。
 */

/* ==================== 优先级分组 ==================== */

/* 说明：设置 NVIC 优先级分组（0~7）。 */
void nvic_set_priority_grouping(uint32_t group);

/* ==================== 优先级设置 ==================== */

/*
 * 说明：
 * 设置指定中断的优先级。
 *
 * 当前版本定义：
 * 内部根据当前分组自动编码 preempt_prio 和 sub_prio。
 */
void nvic_set_priority(IRQn_Type irqn, uint32_t preempt_prio, uint32_t sub_prio);

/* ==================== 使能 / 禁用 ==================== */

/* 说明：使能指定外部中断。 */
void nvic_enable_irq(IRQn_Type irqn);

/* 说明：禁用指定外部中断。 */
void nvic_disable_irq(IRQn_Type irqn);

/* ==================== 挂起状态 ==================== */

/* 说明：清除指定中断的挂起状态。 */
void nvic_clear_pending(IRQn_Type irqn);

/* 说明：设置指定中断为挂起状态。 */
void nvic_set_pending(IRQn_Type irqn);

/* ==================== 活动状态 ==================== */

/* 说明：查询指定中断是否正在活动（返回非零表示活动）。 */
uint32_t nvic_get_active(IRQn_Type irqn);

#endif /* DRV_USING_NVIC */
#endif /* __DRV_NVIC_H__ */
