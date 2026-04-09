#ifndef __DRV_EXTI_H__
#define __DRV_EXTI_H__

#include "drv_config.h"

#ifdef DRV_USING_EXTI

#include "stm32f4xx.h"

/*
 * 说明：
 * 本头文件提供 STM32F407 EXTI 驱动的公开接口。
 *
 * 当前版本语义：
 * 1. EXTI line 0~15 对应 GPIO 外部中断线。
 * 2. EXTI line 16~22 对应内部事件线。
 * 3. 当前版本覆盖：
 *    - SYSCFG 时钟使能
 *    - GPIO 到 EXTI line 的映射（仅 0~15）
 *    - line 级触发配置（0~22）
 *    - 中断屏蔽 / 事件屏蔽（0~22）
 *    - 挂起查询 / 清除（0~22）
 *    - 软件触发（0~22）
 *    - GPIO EXTI 的 IRQn 查询（仅 0~15）
 *    - GPIO EXTI 轻量初始化辅助函数
 *
 * 限制：
 * 1. GPIO 模式、上下拉配置不属于本驱动范围。
 * 2. NVIC 配置不属于本驱动范围。
 * 3. line 16~22 不支持 GPIO 映射。
 * 4. exti_get_irqn() 仅适用于 GPIO EXTI line 0~15。
 */

#define EXTI_LINE_MAX      22U
#define EXTI_GPIO_MAX      15U
#define EXTI_INVALID_IRQN  ((IRQn_Type)(-100))

/* 说明：EXTI 触发方式。 */
typedef enum
{
    EXTI_TRIGGER_RISING = 0,
    EXTI_TRIGGER_FALLING,
    EXTI_TRIGGER_BOTH
} exti_trigger_t;

/* ==================== SYSCFG 时钟 ==================== */

/* 说明：使能 SYSCFG 时钟，用于 GPIO 到 EXTI 的映射配置。 */
void exti_syscfg_clk_enable(void);

/* ==================== GPIO 到 EXTI 映射 ==================== */

/*
 * 说明：
 * 将 GPIO 引脚映射到 EXTI line。
 *
 * 前提：
 * 1. port 为有效 GPIO 端口实例
 * 2. pin 范围为 0~15
 *
 * 限制：
 * 1. 仅适用于 EXTI line 0~15
 * 2. STM32F407 上 PI12~PI15 不可作为 EXTI 输入源
 *
 * 当前版本语义：
 * 本函数内部会自动使能 SYSCFG 时钟。
 */
void exti_map_gpio(GPIO_TypeDef *port, uint8_t pin);

/* ==================== 触发与屏蔽控制 ==================== */

/*
 * 说明：
 * 配置指定 EXTI line 的触发方式。
 *
 * 前提：
 * 1. line 范围为 0~22
 * 2. trigger 为合法枚举值
 */
void exti_set_trigger(uint8_t line, exti_trigger_t trigger);

/* 说明：开放指定 line 的中断请求。 */
void exti_enable_interrupt(uint8_t line);

/* 说明：屏蔽指定 line 的中断请求。 */
void exti_disable_interrupt(uint8_t line);

/* 说明：开放指定 line 的事件请求。 */
void exti_enable_event(uint8_t line);

/* 说明：屏蔽指定 line 的事件请求。 */
void exti_disable_event(uint8_t line);

/* ==================== 挂起与软件触发 ==================== */

/* 说明：读取指定 line 的挂起状态。返回 1 表示挂起，0 表示未挂起。 */
uint8_t exti_get_pending(uint8_t line);

/*
 * 说明：
 * 清除指定 line 的挂起状态。
 *
 * 当前版本语义：
 * EXTI_PR 采用写 1 清零。
 */
void exti_clear_pending(uint8_t line);

/*
 * 说明：
 * 对指定 line 触发一次软件中断/事件请求。
 *
 * 当前版本语义：
 * 若 IMR / EMR 已开放，则会产生对应请求。
 */
void exti_software_trigger(uint8_t line);

/* ==================== IRQn 查询 ==================== */

/*
 * 说明：
 * 获取 GPIO EXTI line 0~15 对应的 IRQn。
 *
 * 前提：
 * pin 范围为 0~15。
 *
 * 限制：
 * 1. 仅适用于 GPIO EXTI line 0~15
 * 2. 不适用于内部 EXTI line 16~22
 *
 * 当前版本语义：
 * 非法 pin 返回 EXTI_INVALID_IRQN。
 */
IRQn_Type exti_get_irqn(uint8_t pin);

/* ==================== 轻量初始化辅助 ==================== */

/*
 * 说明：
 * 对 GPIO EXTI 执行轻量初始化：
 * - GPIO 映射到 EXTI
 * - 配置触发方式
 * - 开放中断屏蔽
 *
 * 前提：
 * 1. port 为有效 GPIO 端口实例
 * 2. pin 范围为 0~15
 * 3. trigger 为合法枚举值
 *
 * 限制：
 * 1. 仅适用于 GPIO EXTI line 0~15
 * 2. 不负责 GPIO 输入模式配置
 * 3. 不负责 NVIC 配置
 * 4. 不主动清除挂起位
 */
void exti_init_gpio_interrupt(GPIO_TypeDef *port, uint8_t pin, exti_trigger_t trigger);

#endif /* DRV_USING_EXTI */
#endif /* __DRV_EXTI_H__ */
