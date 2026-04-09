#ifndef __DRV_SDIO_H__
#define __DRV_SDIO_H__

#include "drv_config.h"

#ifdef DRV_USING_SDIO

/*
 * 说明：
 * 本头文件提供 STM32F407 SDIO 驱动的公共接口。
 *
 * 当前版本定义：
 * 1. SDIO 外设寄存器级操作封装（时钟、电源、总线宽度、命令、数据、DMA、中断）
 * 2. 当前版本覆盖：SDIO 控制器硬件层初始化与基本收发控制
 *
 * 限制：
 * 1. 不包含 SD 卡协议层（CMD0/CMD8/ACMD41 等卡识别与初始化流程）
 * 2. 不包含 DMA 通道配置（仅提供 SDIO 侧 DMA 使能开关）
 * 3. 不包含文件系统或多块传输辅助逻辑
 */

#include "stm32f4xx.h"

/* ==================== 类型定义 ==================== */

typedef enum
{
    SDIO_RESP_NONE  = 0,    /* 无响应 */
    SDIO_RESP_SHORT = 1,    /* 短响应（48 位） */
    SDIO_RESP_LONG  = 3     /* 长响应（136 位） */
} sdio_resp_t;

typedef struct
{
    uint32_t clk_div;           /* 时钟分频系数 CLKDIV 0..255 */
    uint8_t  clk_edge_rising;   /* 1=上升沿采样, 0=下降沿采样 */
    uint8_t  clk_bypass;        /* 旁路分频器 */
    uint8_t  clk_power_save;    /* 省电模式：空闲时关闭时钟输出 */
    uint8_t  bus_wide;          /* 总线宽度：1、4 或 8 */
    uint8_t  hw_flow_ctrl;      /* 硬件流控制 */
} sdio_config_t;

/* ==================== 时钟 ==================== */

/* 说明：使能 SDIO 外设 APB2 时钟 */
void sdio_clk_enable(void);

/* ==================== 初始化与配置 ==================== */

/* 说明：按配置结构体初始化 SDIO 外设 */
void sdio_init(const sdio_config_t *cfg);

/* 说明：动态设置总线宽度（1/4/8 位） */
void sdio_set_bus_width(uint8_t width);

/* 说明：SDIO 电源开启 */
void sdio_power_on(void);

/* 说明：SDIO 电源关闭 */
void sdio_power_off(void);

/* 说明：使能 SDIO 时钟输出至卡 */
void sdio_enable_clk_output(void);

/* 说明：关闭 SDIO 时钟输出 */
void sdio_disable_clk_output(void);

/* ==================== 命令 ==================== */

/* 说明：发送 SDIO 命令（命令索引、参数、响应类型、是否等待中断） */
void     sdio_send_cmd(uint8_t cmd_idx, uint32_t arg, sdio_resp_t resp, uint8_t wait_irq);

/* 说明：读取响应寄存器 1 */
uint32_t sdio_get_resp1(void);

/* 说明：读取响应寄存器 2 */
uint32_t sdio_get_resp2(void);

/* 说明：读取响应寄存器 3 */
uint32_t sdio_get_resp3(void);

/* 说明：读取响应寄存器 4 */
uint32_t sdio_get_resp4(void);

/* 说明：获取命令相关状态标志 */
uint32_t sdio_get_cmd_status(void);

/* ==================== 数据传输 ==================== */

/* 说明：配置数据传输参数（数据长度、块大小、方向、流模式） */
void     sdio_data_config(uint32_t data_len, uint32_t block_size, uint8_t dir_to_card, uint8_t stream_mode);

/* 说明：获取数据传输相关状态标志 */
uint32_t sdio_get_data_status(void);

/* 说明：清除指定状态标志 */
void     sdio_clear_flags(uint32_t flags);

/* ==================== DMA ==================== */

/* 说明：使能 SDIO 侧 DMA 请求 */
void sdio_enable_dma(void);

/* 说明：禁用 SDIO 侧 DMA 请求 */
void sdio_disable_dma(void);

/* ==================== 中断 ==================== */

/* 说明：使能指定中断源（按位掩码） */
void sdio_enable_irq(uint32_t mask);

/* 说明：禁用指定中断源（按位掩码） */
void sdio_disable_irq(uint32_t mask);

#endif /* DRV_USING_SDIO */
#endif /* __DRV_SDIO_H__ */
