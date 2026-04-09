#ifndef __DRV_I2S_H__
#define __DRV_I2S_H__

#include "drv_config.h"

#ifdef DRV_USING_I2S

#include "stm32f4xx.h"

/*
 * 说明：
 * 本头文件提供 STM32F407 I2S 驱动的公共接口。
 *
 * 当前版本定义：
 * 1. 当前版本覆盖 SPI2/SPI3 的 I2S 模式基础功能。
 * 2. 当前版本覆盖：
 *    - I2S 初始化（模式 / 标准 / 数据格式 / 时钟极性 / 采样率 / MCLK）
 *    - I2S 反初始化
 *    - 阻塞发送 / 接收
 *    - DMA 使能 / 禁用
 *
 * 限制：
 * 1. 当前版本不提供全双工 I2S 扩展接口。
 * 2. 当前版本不提供中断方式收发接口。
 * 3. 阻塞收发无超时保护。
 * 4. 采样率计算基于 PLLI2S 时钟，简化为 32 位通道帧。
 */

/* ==================== 类型定义 ==================== */

/* 说明：I2S 模式。 */
typedef enum
{
    I2S_MODE_SLAVE_TX  = 0,
    I2S_MODE_SLAVE_RX  = 1,
    I2S_MODE_MASTER_TX = 2,
    I2S_MODE_MASTER_RX = 3
} i2s_mode_t;

/* 说明：I2S 标准。 */
typedef enum
{
    I2S_STANDARD_PHILIPS = 0,
    I2S_STANDARD_MSB     = 1,
    I2S_STANDARD_LSB     = 2,
    I2S_STANDARD_PCM     = 3
} i2s_standard_t;

/* 说明：I2S 数据格式。 */
typedef enum
{
    I2S_DATA_16B     = 0,
    I2S_DATA_16B_EXT = 1,
    I2S_DATA_24B     = 3,
    I2S_DATA_32B     = 5
} i2s_datafmt_t;

/* 说明：I2S 初始化配置。 */
typedef struct
{
    SPI_TypeDef      *instance;
    i2s_mode_t        mode;
    i2s_standard_t    standard;
    i2s_datafmt_t     data_format;
    uint8_t           cpol;
    uint32_t          audio_freq;
    uint8_t           mclk_output;
} i2s_config_t;

/* ==================== 初始化 / 反初始化 ==================== */

/* 说明：初始化 I2S。 */
void i2s_init(const i2s_config_t *cfg);

/* 说明：反初始化 I2S（禁用并复位 I2S 配置）。 */
void i2s_deinit(SPI_TypeDef *spi);

/* ==================== 数据收发 ==================== */

/* 说明：阻塞发送一个 16 位数据。 */
void i2s_send(SPI_TypeDef *spi, uint16_t data);

/* 说明：阻塞接收一个 16 位数据。 */
uint16_t i2s_recv(SPI_TypeDef *spi);

/* ==================== DMA ==================== */

/* 说明：使能 I2S DMA 发送。 */
void i2s_enable_dma_tx(SPI_TypeDef *spi);

/* 说明：使能 I2S DMA 接收。 */
void i2s_enable_dma_rx(SPI_TypeDef *spi);

/* 说明：禁用 I2S DMA 收发。 */
void i2s_disable_dma(SPI_TypeDef *spi);

#endif /* DRV_USING_I2S */
#endif /* __DRV_I2S_H__ */
