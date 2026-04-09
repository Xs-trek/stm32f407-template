#ifndef __DRV_PWR_H__
#define __DRV_PWR_H__

#include "drv_config.h"

#ifdef DRV_USING_PWR

#include "stm32f4xx.h"

/*
 * 说明：
 * 本头文件提供 STM32F407 PWR 驱动的公共接口。
 *
 * 当前版本定义：
 * 1. 当前版本覆盖：
 *    - PWR 时钟使能
 *    - 备份域访问使能
 *    - PVD 电平设置 / 使能 / 禁用 / 状态读取
 *    - 低功耗模式进入（Sleep / Stop / Standby）
 *    - 唤醒标志 / 待机标志清除
 *
 * 限制：
 * 1. 当前版本不提供电压调节器模式配置接口。
 * 2. 当前版本不提供过驱使能接口。
 * 3. 低功耗模式进入后的唤醒源配置不属于本驱动范围。
 */

/* ==================== 时钟 / 备份域 ==================== */

/* 说明：使能 PWR 外设时钟。 */
void pwr_clk_enable(void);

/* 说明：使能备份域访问（DBP 位）。 */
void pwr_enable_backup_access(void);

/* ==================== PVD ==================== */

/* 说明：设置 PVD 检测电平（0~7）。 */
void pwr_set_pvd_level(uint8_t level);

/* 说明：使能 PVD。 */
void pwr_enable_pvd(void);

/* 说明：禁用 PVD。 */
void pwr_disable_pvd(void);

/* 说明：获取 PVD 输出状态（1 = VDD 低于阈值，0 = 高于阈值）。 */
uint8_t pwr_get_pvd_output(void);

/* ==================== 低功耗模式 ==================== */

/*
 * 说明：进入 Sleep 模式（WFI）。
 *
 * 当前版本定义：
 * 清除 SLEEPDEEP 后执行 WFI。
 */
void pwr_enter_sleep(void);

/*
 * 说明：进入 Stop 模式（WFI）。
 *
 * 当前版本定义：
 * 唤醒后自动清除 SLEEPDEEP。
 */
void pwr_enter_stop(void);

/*
 * 说明：进入 Standby 模式（WFI）。
 *
 * 当前版本定义：
 * 进入后仅可通过 NRST / WKUP 引脚 / RTC 唤醒。
 */
void pwr_enter_standby(void);

/* ==================== 标志清除 ==================== */

/* 说明：清除唤醒标志（CWUF）。 */
void pwr_clear_wakeup_flag(void);

/* 说明：清除待机标志（CSBF）。 */
void pwr_clear_standby_flag(void);

#endif /* DRV_USING_PWR */
#endif /* __DRV_PWR_H__ */
