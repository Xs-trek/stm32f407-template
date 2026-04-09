#ifndef __DRV_RCC_H__
#define __DRV_RCC_H__

#include "drv_config.h"

#ifdef DRV_USING_RCC

#include "stm32f4xx.h"

/*
 * 说明：
 * 本头文件提供 STM32F407 RCC 驱动的公共接口。
 *
 * 当前版本定义：
 * 1. 当前版本仅覆盖 RCC 驱动本身闭合的基础功能。
 * 2. 当前版本覆盖：
 *    - 常用外设时钟使能（GPIO / DMA / SPI / TIM / USART / I2C / ADC / CAN）
 *    - 系统时钟频率查询（SYSCLK / HCLK / PCLK1 / PCLK2）
 *    - 定时器时钟频率查询（APB1 TIM / APB2 TIM）
 *    - PLLI2S 输出时钟查询
 *    - 复位标志查询与清除
 *
 * 限制：
 * 1. 当前版本不提供 PLL 配置接口（PLL 配置属于 board 层职责）。
 * 2. 当前版本不提供外设复位接口。
 * 3. 当前版本不提供低功耗时钟门控接口。
 * 4. 外设时钟使能函数按需扩展，不追求覆盖全部外设。
 */

/* ==================== 外设时钟使能 ==================== */

/* 说明：使能指定 GPIO 端口时钟。 */
void rcc_enable_gpio_clk(GPIO_TypeDef *port);

/* 说明：使能指定 DMA 控制器时钟。 */
void rcc_enable_dma_clk(DMA_TypeDef *dma);

/* 说明：使能指定 SPI 外设时钟。 */
void rcc_enable_spi_clk(SPI_TypeDef *spi);

/* 说明：使能指定 TIM 外设时钟。 */
void rcc_enable_tim_clk(TIM_TypeDef *tim);

/* 说明：使能指定 USART/UART 外设时钟。 */
void rcc_enable_usart_clk(USART_TypeDef *usart);

/* 说明：使能指定 I2C 外设时钟。 */
void rcc_enable_i2c_clk(I2C_TypeDef *i2c);

/* 说明：使能指定 ADC 外设时钟。 */
void rcc_enable_adc_clk(ADC_TypeDef *adc);

/* 说明：使能指定 CAN 外设时钟。 */
void rcc_enable_can_clk(CAN_TypeDef *can);

/* ==================== 时钟频率查询 ==================== */

/* 说明：获取 SYSCLK 频率（Hz）。 */
uint32_t rcc_get_sysclk(void);

/* 说明：获取 HCLK（AHB）频率（Hz）。 */
uint32_t rcc_get_hclk(void);

/* 说明：获取 PCLK1（APB1）频率（Hz）。 */
uint32_t rcc_get_pclk1(void);

/* 说明：获取 PCLK2（APB2）频率（Hz）。 */
uint32_t rcc_get_pclk2(void);

/* 说明：获取 APB1 定时器时钟频率（Hz）。 */
uint32_t rcc_get_timclk1(void);

/* 说明：获取 APB2 定时器时钟频率（Hz）。 */
uint32_t rcc_get_timclk2(void);

/* 说明：获取 PLLI2S 输出时钟频率（Hz）。 */
uint32_t rcc_get_plli2s_clk(void);

/* ==================== 复位标志 ==================== */

/* 说明：获取 CSR 中的复位源标志。 */
uint32_t rcc_get_reset_flags(void);

/* 说明：清除所有复位标志（置位 RMVF）。 */
void rcc_clear_reset_flags(void);

#endif /* DRV_USING_RCC */
#endif /* __DRV_RCC_H__ */
