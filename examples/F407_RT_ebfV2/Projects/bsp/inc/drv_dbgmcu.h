#ifndef __DRV_DBGMCU_H__
#define __DRV_DBGMCU_H__

#include "drv_config.h"

#ifdef DRV_USING_DBGMCU

#include "stm32f4xx.h"

/*
 * 说明：
 * 本头文件提供 STM32F407 DBGMCU 驱动的公共接口。
 *
 * 当前版本定义：
 * 1. 当前版本仅覆盖 DBGMCU 驱动本身闭合的基础功能。
 * 2. 当前版本覆盖：
 *    - DBGMCU 设备 ID / 版本 ID 读取
 *    - APB1 定时器冻结（TIM2~TIM5, IWDG, WWDG）
 *
 * 限制：
 * 1. 当前版本不含 APB2 冻结控制。
 * 2. 当前版本不含跟踪引脚配置。
 */

/* ==================== 设备标识 ==================== */

/* 说明：获取设备 ID（DBGMCU_IDCODE 的 DEV_ID 字段）。 */
uint16_t dbgmcu_get_dev_id(void);

/* 说明：获取版本 ID（DBGMCU_IDCODE 的 REV_ID 字段）。 */
uint16_t dbgmcu_get_rev_id(void);

/* ==================== APB1 冻结 ==================== */

/* 说明：内核停止时冻结 TIM2 计数器。 */
void dbgmcu_freeze_tim2(void);

/* 说明：内核停止时冻结 TIM3 计数器。 */
void dbgmcu_freeze_tim3(void);

/* 说明：内核停止时冻结 TIM4 计数器。 */
void dbgmcu_freeze_tim4(void);

/* 说明：内核停止时冻结 TIM5 计数器。 */
void dbgmcu_freeze_tim5(void);

/* 说明：内核停止时冻结 IWDG 计数器。 */
void dbgmcu_freeze_iwdg(void);

/* 说明：内核停止时冻结 WWDG 计数器。 */
void dbgmcu_freeze_wwdg(void);

#endif /* DRV_USING_DBGMCU */
#endif /* __DRV_DBGMCU_H__ */
