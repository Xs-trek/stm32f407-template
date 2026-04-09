#ifndef __DRV_RNG_H__
#define __DRV_RNG_H__

/*
 * drv_rng.h - STM32F407 RNG 驱动头文件
 *
 * 覆盖范围：
 *   - RNG 外设时钟使能
 *   - RNG 外设初始化与反初始化
 *   - 获取 32 位随机数（阻塞等待）
 *   - 查询数据就绪状态
 *   - 检测种子错误和时钟错误标志
 *
 * 限制：
 *   - rng_get() 为阻塞调用，若 RNG 未使能将永久等待
 *   - 不包含错误自动恢复机制，需调用方自行处理
 *   - 无中断模式支持，仅 CPU 轮询
 */

#include "drv_config.h"

#ifdef DRV_USING_RNG

#include "stm32f4xx.h"

/* ==================== API 声明 ==================== */

/* 说明：使能 RNG 外设的 AHB2 总线时钟 */
void     rng_clk_enable(void);

/* 说明：使能时钟并开启 RNG 外设 */
void     rng_init(void);

/* 说明：关闭 RNG 外设 */
void     rng_deinit(void);

/* 说明：阻塞等待并返回一个 32 位随机数 */
uint32_t rng_get(void);

/* 说明：查询 RNG 数据是否就绪，就绪返回 1，否则返回 0 */
uint8_t  rng_data_ready(void);

/* 说明：检查 RNG 是否存在种子错误或时钟错误，有错误返回 1 */
uint8_t  rng_check_error(void);

#endif /* DRV_USING_RNG */
#endif /* __DRV_RNG_H__ */
