#ifndef __DRV_ETH_H__
#define __DRV_ETH_H__

#include "drv_config.h"

#ifdef DRV_USING_ETH

#include "stm32f4xx.h"

/*
 * 说明：
 * 本头文件提供 STM32F407 ETH 驱动的公共接口。
 *
 * 当前版本定义：
 * 1. 当前版本仅覆盖 ETH 驱动本身闭合的基础功能。
 * 2. 当前版本覆盖：
 *    - ETH MAC / DMA 基本配置
 *    - MAC 地址
 *    - MII PHY 读写
 *    - DMA 中断控制
 *
 * 限制：
 * 1. 当前版本不含描述符链表管理。
 * 2. 当前版本不含收发缓冲区管理。
 * 3. 当前版本不含 PTP/IEEE 1588 支持。
 */

/* ==================== 类型定义 ==================== */

typedef struct
{
    uint8_t  full_duplex;       /* 1 = 全双工 */
    uint8_t  speed_100m;        /* 1 = 100 Mbit/s, 0 = 10 Mbit/s */
    uint8_t  checksum_offload;  /* 1 = IP 校验和卸载 */
    uint8_t  broadcast_enable;  /* 1 = 接收广播帧 */
    uint8_t  promiscuous_mode;  /* 1 = 混杂模式 */
    uint8_t  rx_interrupt;      /* 1 = 使能 RX DMA 中断 */
    uint8_t  tx_interrupt;      /* 1 = 使能 TX DMA 中断 */
} eth_config_t;

/* ==================== 时钟控制 ==================== */

/* 说明：使能 ETH MAC / TX / RX 外设时钟。 */
void eth_clk_enable(void);

/* ==================== 初始化 ==================== */

/* 说明：初始化 ETH MAC 和 DMA。 */
void eth_init(const eth_config_t *cfg);

/* ==================== 启动 / 停止 ==================== */

/* 说明：使能 MAC TX/RX 并启动 DMA。 */
void eth_start(void);

/* 说明：停止 MAC TX/RX 和 DMA。 */
void eth_stop(void);

/* ==================== MAC 地址 ==================== */

/* 说明：设置指定索引（0~3）的 MAC 地址。 */
void eth_set_mac_addr(uint8_t index, const uint8_t mac[6]);

/* ==================== MII PHY 读写 ==================== */

/* 说明：通过 MII 读取 PHY 寄存器。 */
uint16_t eth_mii_read(uint8_t phy_addr, uint8_t reg_addr);

/* 说明：通过 MII 写入 PHY 寄存器。 */
void eth_mii_write(uint8_t phy_addr, uint8_t reg_addr, uint16_t value);

/* ==================== DMA 中断控制 ==================== */

/* 说明：恢复 DMA 接收（发出轮询请求）。 */
void eth_resume_rx(void);

/* 说明：恢复 DMA 发送（发出轮询请求）。 */
void eth_resume_tx(void);

/* 说明：使能 DMA 接收中断。 */
void eth_enable_rx_irq(void);

/* 说明：使能 DMA 发送中断。 */
void eth_enable_tx_irq(void);

/* 说明：获取 DMA 状态寄存器值。 */
uint32_t eth_get_dma_status(void);

/* 说明：清除 DMA 状态标志（写 1 清除）。 */
void eth_clear_dma_status(uint32_t flags);

#endif /* DRV_USING_ETH */
#endif /* __DRV_ETH_H__ */
