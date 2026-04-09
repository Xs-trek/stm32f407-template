#ifndef __DRV_SPI_H__
#define __DRV_SPI_H__

#include "drv_config.h"

#ifdef DRV_USING_SPI

/*
 * ============================================================
 *  drv_spi.h — STM32F407 SPI 驱动头文件
 * ============================================================
 *  当前版本功能范围：
 *    - SPI1 / SPI2 / SPI3 主机模式（软件 NSS）
 *    - 8 位 / 16 位数据帧，可配置 CPOL、CPHA、LSB/MSB
 *    - 阻塞式字节、半字及缓冲区收发
 *    - 软件片选（GPIO）辅助接口
 *    - 中断使能 / 禁止（RXNE、TXE、ERR）
 *    - 可选 DMA 收发使能（需开启 DRV_USING_DMA）
 *    - CRC、TI 模式、双向模式等扩展配置
 *
 *  已知限制：
 *    - 不支持从机模式
 *    - 不支持硬件 NSS 管理
 *    - 阻塞式收发无超时保护
 *    - DMA 仅提供使能/禁止接口，未封装完整传输流程
 *    - 未支持 I2S 模式
 * ============================================================
 */

#include "stm32f4xx.h"

/* ==================== 类型定义 ==================== */

typedef struct
{
    SPI_TypeDef  *instance;     /* SPI 外设实例（SPI1/SPI2/SPI3） */
    GPIO_TypeDef *cs_port;      /* 片选 GPIO 端口 */
    uint8_t       cs_pin;       /* 片选引脚编号（0~15） */
    uint8_t       prescaler;    /* 分频系数 0~7 => fPCLK/2 .. fPCLK/256 */
    uint8_t       cpol;         /* 时钟极性 */
    uint8_t       cpha;         /* 时钟相位 */
    uint8_t       data_16bit;   /* 非零则使用 16 位数据帧 */
    uint8_t       lsb_first;    /* 非零则低位先发 */
} spi_dev_t;

/* ==================== 时钟接口 ==================== */

/* 说明：使能指定 SPI 外设的总线时钟 */
void spi_clk_enable(SPI_TypeDef *spi);

/* ==================== 初始化与使能 ==================== */

/* 说明：根据 spi_dev_t 配置初始化 SPI（主机模式、软件 NSS） */
void spi_init(spi_dev_t *dev);

/* 说明：复位 SPI 寄存器并关闭外设 */
void spi_deinit(spi_dev_t *dev);

/* 说明：使能 SPI 外设（置位 SPE） */
void spi_enable(spi_dev_t *dev);

/* 说明：禁止 SPI 外设（清除 SPE） */
void spi_disable(spi_dev_t *dev);

/* ==================== 片选控制 ==================== */

/* 说明：拉低片选引脚（选中从设备） */
static inline void spi_cs_low(spi_dev_t *dev)
{
    if (dev == NULL || dev->cs_port == NULL || dev->cs_pin > 15U)
    {
        return;
    }
    dev->cs_port->BSRR = (1U << (dev->cs_pin + 16U));
}

/* 说明：拉高片选引脚（释放从设备） */
static inline void spi_cs_high(spi_dev_t *dev)
{
    if (dev == NULL || dev->cs_port == NULL || dev->cs_pin > 15U)
    {
        return;
    }
    dev->cs_port->BSRR = (1U << dev->cs_pin);
}

/* ==================== 阻塞式收发 ==================== */

/* 说明：等待 SPI 总线空闲（BSY 标志清除） */
void     spi_wait_idle(spi_dev_t *dev);

/* 说明：阻塞式发送并接收一个字节 */
uint8_t  spi_txrx_byte(spi_dev_t *dev, uint8_t tx);

/* 说明：阻塞式发送并接收一个半字（16 位） */
uint16_t spi_txrx_halfword(spi_dev_t *dev, uint16_t tx);

/* 说明：阻塞式发送缓冲区（忽略接收数据） */
void     spi_tx_buffer(spi_dev_t *dev, const uint8_t *buf, uint32_t len);

/* 说明：阻塞式接收缓冲区（发送 0xFF 作为时钟驱动） */
void     spi_rx_buffer(spi_dev_t *dev, uint8_t *buf, uint32_t len);

/* 说明：阻塞式同时发送与接收缓冲区 */
void     spi_txrx_buffer(spi_dev_t *dev, const uint8_t *tx, uint8_t *rx, uint32_t len);

/* 说明：完整帧传输——自动拉低/拉高片选，先发后收 */
void     spi_transfer_frame(spi_dev_t *dev,
                             const uint8_t *tx_buf, uint32_t tx_len,
                             uint8_t *rx_buf, uint32_t rx_len);

/* ==================== 中断控制 ==================== */

/* 说明：使能 RXNE（接收缓冲区非空）中断 */
void spi_enable_rxne_irq(spi_dev_t *dev);

/* 说明：禁止 RXNE 中断 */
void spi_disable_rxne_irq(spi_dev_t *dev);

/* 说明：使能 TXE（发送缓冲区空）中断 */
void spi_enable_txe_irq(spi_dev_t *dev);

/* 说明：禁止 TXE 中断 */
void spi_disable_txe_irq(spi_dev_t *dev);

/* 说明：使能错误中断（OVR / MODF / CRCERR） */
void spi_enable_err_irq(spi_dev_t *dev);

/* 说明：禁止错误中断 */
void spi_disable_err_irq(spi_dev_t *dev);

/* ==================== 状态标志 ==================== */

/* 说明：判断发送缓冲区是否为空 */
static inline int spi_is_txe(SPI_TypeDef *spi)    { return (spi->SR & SPI_SR_TXE)  ? 1 : 0; }

/* 说明：判断接收缓冲区是否非空 */
static inline int spi_is_rxne(SPI_TypeDef *spi)    { return (spi->SR & SPI_SR_RXNE) ? 1 : 0; }

/* 说明：判断 SPI 是否正忙 */
static inline int spi_is_busy(SPI_TypeDef *spi)    { return (spi->SR & SPI_SR_BSY)  ? 1 : 0; }

/* 说明：判断是否发生溢出错误 */
static inline int spi_is_ovr(SPI_TypeDef *spi)     { return (spi->SR & SPI_SR_OVR)  ? 1 : 0; }

/* 说明：判断是否发生模式故障 */
static inline int spi_is_modf(SPI_TypeDef *spi)    { return (spi->SR & SPI_SR_MODF) ? 1 : 0; }

/* 说明：判断是否发生 CRC 错误 */
static inline int spi_is_crcerr(SPI_TypeDef *spi)  { return (spi->SR & SPI_SR_CRCERR) ? 1 : 0; }

/* ==================== DMA ==================== */

#ifdef DRV_USING_DMA

/* 说明：使能 SPI 的 DMA 发送和/或接收请求 */
void spi_enable_dma(spi_dev_t *dev, uint8_t tx_en, uint8_t rx_en);

/* 说明：禁止 SPI 的 DMA 发送与接收请求 */
void spi_disable_dma(spi_dev_t *dev);

#endif /* DRV_USING_DMA */

/* ==================== 配置辅助 ==================== */

/* 说明：动态修改 SPI 波特率分频系数（会短暂禁用再使能 SPE） */
void spi_set_prescaler(spi_dev_t *dev, uint8_t prescaler);

/* 说明：使能硬件 CRC 并设置多项式 */
void spi_enable_crc(spi_dev_t *dev, uint16_t polynomial);

/* 说明：禁止硬件 CRC */
void spi_disable_crc(spi_dev_t *dev);

/* 说明：使能 TI 模式帧格式 */
void spi_enable_ti_mode(spi_dev_t *dev);

/* 说明：禁止 TI 模式帧格式 */
void spi_disable_ti_mode(spi_dev_t *dev);

/* 说明：设置软件 NSS 管理（enable 非零则开启 SSM + SSI） */
void spi_set_software_nss(spi_dev_t *dev, uint8_t enable);

/* 说明：使能双向模式并设置为发送方向 */
void spi_enable_bidirectional_tx(spi_dev_t *dev);

/* 说明：使能双向模式并设置为接收方向 */
void spi_enable_bidirectional_rx(spi_dev_t *dev);

#endif /* DRV_USING_SPI */
#endif /* __DRV_SPI_H__ */
