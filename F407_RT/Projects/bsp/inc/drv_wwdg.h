#ifndef __DRV_WWDG_H__
#define __DRV_WWDG_H__

#include "drv_config.h"

#ifdef DRV_USING_WWDG

#include "stm32f4xx.h"

/*
 * 说明：
 * 本头文件提供 STM32F407 窗口看门狗（WWDG）驱动的公开接口。
 *
 * 当前版本语义：
 * 1. 当前版本覆盖：
 *    - WWDG 时钟使能
 *    - 预分频 / 窗口值 / 计数器 / EWI 配置与启动
 *    - 喂狗（计数器刷新）
 *    - EWI 标志清除
 *
 * 限制：
 * 1. 仅支持 STM32F407 内置 WWDG 外设。
 * 2. NVIC 配置不属于本驱动范围。
 * 3. EWI 中断服务函数需用户自行实现。
 */

/* ==================== 类型定义 ==================== */

/*
 * 说明：WWDG 初始化配置。
 *
 * 前提：
 * 1. prescaler 取值范围 0~3，对应分频系数 /1 /2 /4 /8。
 * 2. window 取值范围 0x40~0x7F。
 * 3. counter 取值范围 0x40~0x7F，且初值应满足 counter >= window。
 * 4. ewi_enable 非零表示使能早期唤醒中断。
 */
typedef struct
{
    uint8_t prescaler;
    uint8_t window;
    uint8_t counter;
    uint8_t ewi_enable;
} wwdg_config_t;

/* ==================== 时钟控制 ==================== */

/* 说明：使能 WWDG APB1 总线时钟。 */
void wwdg_clk_enable(void);

/* ==================== 初始化 ==================== */

/*
 * 说明：
 * 根据配置结构体初始化并启动 WWDG。
 *
 * 前提：
 * cfg 非空，且各字段满足 wwdg_config_t 的取值约束。
 *
 * 当前版本语义：
 * WWDG 一旦启动，无法通过软件关闭，只能通过复位解除。
 */
void wwdg_init(const wwdg_config_t *cfg);

/* ==================== 运行时控制 ==================== */

/*
 * 说明：
 * 刷新 WWDG 计数器（喂狗）。
 *
 * 前提：
 * 1. counter 必须满足：window >= counter > 0x3F。
 * 2. counter <= 0x3F 时，T6 = 0，WWDG 立即触发复位。
 * 3. counter > window 时，视为过早喂狗，WWDG 立即触发复位。
 *
 * 当前版本语义：
 * 本函数不检查 counter 是否在合法范围内，由调用者保证。
 */
void wwdg_refresh(uint8_t counter);

/* ==================== 标志控制 ==================== */

/* 说明：清除早期唤醒中断（EWI）标志位。 */
void wwdg_clear_ewi_flag(void);

#endif /* DRV_USING_WWDG */
#endif /* __DRV_WWDG_H__ */
