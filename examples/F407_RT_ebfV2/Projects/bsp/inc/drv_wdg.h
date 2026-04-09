#ifndef __DRV_WDG_H__
#define __DRV_WDG_H__

/*
 * drv_wdg.h — 独立看门狗 (IWDG) 驱动接口
 *
 * 覆盖范围：
 *   - IWDG 预分频与重装载值配置
 *   - IWDG 启动及喂狗操作
 *
 * 限制：
 *   - 仅支持 STM32F407 内置 IWDG 外设
 *   - IWDG 一旦启动无法软件关闭，仅可通过复位停止
 *   - 不包含窗口看门狗 (WWDG)，WWDG 请使用 drv_wwdg
 */

#include "drv_config.h"

#ifdef DRV_USING_IWDG

#include "stm32f4xx.h"

/* ==================== 类型定义 ==================== */

typedef struct
{
    uint8_t  prescaler;   /* 预分频系数：0..6 对应 /4 .. /256 */
    uint16_t reload;      /* 重装载值：0..0xFFF */
} iwdg_config_t;

/* ==================== 公共接口 ==================== */

/* 说明：根据配置结构体初始化并启动 IWDG */
void iwdg_init(const iwdg_config_t *cfg);

/* 说明：喂狗，重装载 IWDG 计数器 */
void iwdg_reload(void);

#endif /* DRV_USING_IWDG */
#endif /* __DRV_WDG_H__ */
