#ifndef __DRV_DCMI_H__
#define __DRV_DCMI_H__

#include "drv_config.h"

#ifdef DRV_USING_DCMI

#include "stm32f4xx.h"

/*
 * 说明：
 * 本头文件提供 STM32F407 DCMI 驱动的公共接口。
 *
 * 当前版本定义：
 * 1. 当前版本仅覆盖 DCMI 驱动本身闭合的基础功能。
 * 2. 当前版本覆盖：
 *    - DCMI 外设时钟使能
 *    - DCMI 初始化（捕获速率、极性、数据模式等）
 *    - DCMI 启动 / 停止捕获
 *    - DCMI 使能 / 禁用
 *
 * 限制：
 * 1. 不含 DMA 配置。
 * 2. 不含裁剪窗口配置。
 * 3. 不含中断管理。
 */

/* ==================== 类型定义 ==================== */

/* 说明：DCMI 配置结构体。 */
typedef struct
{
    uint8_t capture_rate;           /* 捕获速率：0=全部, 1=隔帧, 2=每四帧 */
    uint8_t hsync_polarity;         /* 行同步极性：0=低有效, 1=高有效 */
    uint8_t vsync_polarity;         /* 场同步极性：0=低有效, 1=高有效 */
    uint8_t pixel_clock_polarity;   /* 像素时钟极性：0=下降沿, 1=上升沿 */
    uint8_t capture_mode_continuous; /* 捕获模式：0=快照, 1=连续 */
    uint8_t extended_data_mode;     /* 扩展数据模式：0=8位, 1=10位, 2=12位, 3=14位 */
} dcmi_config_t;

/* ==================== 时钟使能 ==================== */

/* 说明：使能 DCMI 外设时钟。 */
void dcmi_clk_enable(void);

/* ==================== 初始化 ==================== */

/* 说明：初始化 DCMI。 */
void dcmi_init(const dcmi_config_t *cfg);

/* ==================== 控制接口 ==================== */

/* 说明：启动捕获（使能 + 捕获位）。 */
void dcmi_start(void);

/* 说明：停止捕获（禁用 + 清除捕获位）。 */
void dcmi_stop(void);

/* 说明：使能 DCMI（仅 CR_ENABLE，不设置捕获位）。 */
void dcmi_enable(void);

/* 说明：禁用 DCMI（仅 CR_ENABLE，不设置捕获位）。 */
void dcmi_disable(void);

#endif /* DRV_USING_DCMI */
#endif /* __DRV_DCMI_H__ */
