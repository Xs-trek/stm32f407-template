#ifndef __DRV_USART_H__
#define __DRV_USART_H__

#include "drv_config.h"

#ifdef DRV_USING_USART

#include "stm32f4xx.h"

/*
 * drv_usart.h - STM32F407 USART 驱动头文件
 *
 * 当前版本覆盖范围：
 *   - 支持 USART1, USART2, USART3, UART4, UART5, USART6
 *   - 支持基本收发（阻塞模式）、中断控制、DMA 使能
 *   - 支持 LIN、IrDA、智能卡、半双工、同步时钟等特殊模式
 *
 * 当前版本限制：
 *   - 不包含 FIFO / 环形缓冲区管理
 *   - 不包含 DMA 通道分配与传输管理（仅提供 CR3 使能位操作）
 *   - 中断服务函数 (ISR) 需由上层自行实现
 *   - 不支持多缓冲区通信（Multiprocessor / Multibuffer）
 */

/* ==================== 类型定义 ==================== */

typedef enum
{
    USART_WORDLEN_8B = 0,
    USART_WORDLEN_9B = 1
} usart_wordlen_t;

typedef enum
{
    USART_STOP_1   = 0,
    USART_STOP_0_5 = 1,
    USART_STOP_2   = 2,
    USART_STOP_1_5 = 3
} usart_stopbits_t;

typedef enum
{
    USART_PARITY_NONE = 0,
    USART_PARITY_EVEN = 1,
    USART_PARITY_ODD  = 2
} usart_parity_t;

typedef struct
{
    USART_TypeDef     *instance;       /* 外设实例指针 */
    uint32_t           baudrate;       /* 波特率 */
    usart_wordlen_t    word_length;    /* 数据位宽 */
    usart_stopbits_t   stop_bits;      /* 停止位 */
    usart_parity_t     parity;         /* 校验位 */
    uint8_t            tx_enable;      /* 发送使能 */
    uint8_t            rx_enable;      /* 接收使能 */
    uint8_t            oversampling8;  /* 8倍过采样使能 */
} usart_config_t;

/* ==================== 时钟操作 ==================== */

/* 说明：使能指定 USART 外设的总线时钟 */
void     usart_clk_enable(USART_TypeDef *usart);

/* 说明：获取指定 USART 外设所在总线的时钟频率（单位 Hz） */
uint32_t usart_get_clock(USART_TypeDef *usart);

/* 说明：获取指定 USART 外设对应的中断号 */
IRQn_Type usart_get_irqn(USART_TypeDef *usart);

/* ==================== 初始化 / 使能 / 禁用 ==================== */

/* 说明：根据配置结构体初始化 USART 外设 */
void usart_init(const usart_config_t *cfg);

/* 说明：复位 USART 外设寄存器至默认值 */
void usart_deinit(USART_TypeDef *usart);

/* 说明：使能 USART 外设（置位 UE） */
void usart_enable(USART_TypeDef *usart);

/* 说明：禁用 USART 外设（清除 UE） */
void usart_disable(USART_TypeDef *usart);

/* ==================== 阻塞式收发 ==================== */

/* 说明：阻塞发送单字节 */
void    usart_send_byte(USART_TypeDef *usart, uint8_t data);

/* 说明：阻塞接收单字节 */
uint8_t usart_recv_byte(USART_TypeDef *usart);

/* 说明：阻塞发送缓冲区数据 */
void    usart_send_buffer(USART_TypeDef *usart, const uint8_t *buf, uint32_t len);

/* 说明：阻塞发送字符串（以 '\0' 结尾） */
void    usart_send_string(USART_TypeDef *usart, const char *str);

/* ==================== 中断控制 ==================== */

/* 说明：使能发送数据寄存器空中断 (TXE) */
void usart_enable_txe_irq(USART_TypeDef *usart);

/* 说明：禁用发送数据寄存器空中断 (TXE) */
void usart_disable_txe_irq(USART_TypeDef *usart);

/* 说明：使能发送完成中断 (TC) */
void usart_enable_tc_irq(USART_TypeDef *usart);

/* 说明：禁用发送完成中断 (TC) */
void usart_disable_tc_irq(USART_TypeDef *usart);

/* 说明：使能接收数据寄存器非空中断 (RXNE) */
void usart_enable_rxne_irq(USART_TypeDef *usart);

/* 说明：禁用接收数据寄存器非空中断 (RXNE) */
void usart_disable_rxne_irq(USART_TypeDef *usart);

/* 说明：使能空闲线路检测中断 (IDLE) */
void usart_enable_idle_irq(USART_TypeDef *usart);

/* 说明：禁用空闲线路检测中断 (IDLE) */
void usart_disable_idle_irq(USART_TypeDef *usart);

/* ==================== 状态标志 ==================== */

/* 说明：读取 TXE（发送寄存器空）标志 */
static inline uint8_t usart_get_flag_txe(USART_TypeDef *usart)
{
    return (uint8_t)((usart->SR & USART_SR_TXE) ? 1U : 0U);
}

/* 说明：读取 RXNE（接收寄存器非空）标志 */
static inline uint8_t usart_get_flag_rxne(USART_TypeDef *usart)
{
    return (uint8_t)((usart->SR & USART_SR_RXNE) ? 1U : 0U);
}

/* 说明：读取 TC（发送完成）标志 */
static inline uint8_t usart_get_flag_tc(USART_TypeDef *usart)
{
    return (uint8_t)((usart->SR & USART_SR_TC) ? 1U : 0U);
}

/* 说明：读取 IDLE（空闲线路检测）标志 */
static inline uint8_t usart_get_flag_idle(USART_TypeDef *usart)
{
    return (uint8_t)((usart->SR & USART_SR_IDLE) ? 1U : 0U);
}

/* 说明：读取 ORE（过载错误）标志 */
static inline uint8_t usart_get_flag_ore(USART_TypeDef *usart)
{
    return (uint8_t)((usart->SR & USART_SR_ORE) ? 1U : 0U);
}

/* 说明：读取 PE（校验错误）标志 */
static inline uint8_t usart_get_flag_pe(USART_TypeDef *usart)
{
    return (uint8_t)((usart->SR & USART_SR_PE) ? 1U : 0U);
}

/* 说明：读取 FE（帧错误）标志 */
static inline uint8_t usart_get_flag_fe(USART_TypeDef *usart)
{
    return (uint8_t)((usart->SR & USART_SR_FE) ? 1U : 0U);
}

/* 说明：清除 IDLE 标志（依照 RM0090 需先读 SR 再读 DR） */
void usart_clear_idle_flag(USART_TypeDef *usart);

/* 说明：清除 TC（发送完成）标志 */
static inline void usart_clear_tc_flag(USART_TypeDef *usart)
{
    usart->SR &= ~USART_SR_TC;
}

/* ==================== DMA 控制 ==================== */

/* 说明：使能 USART 的 DMA 发送和/或接收 */
void usart_enable_dma(USART_TypeDef *usart, uint8_t tx_enable, uint8_t rx_enable);

/* 说明：禁用 USART 的 DMA 发送与接收 */
void usart_disable_dma(USART_TypeDef *usart);

/* ==================== 特殊模式 ==================== */

/* 说明：使能 LIN 模式 */
void usart_enable_lin(USART_TypeDef *usart);

/* 说明：禁用 LIN 模式 */
void usart_disable_lin(USART_TypeDef *usart);

/* 说明：使能半双工模式 */
void usart_enable_half_duplex(USART_TypeDef *usart);

/* 说明：禁用半双工模式 */
void usart_disable_half_duplex(USART_TypeDef *usart);

/* 说明：使能 IrDA 模式，low_power 非零时进入低功耗红外模式 */
void usart_enable_irda(USART_TypeDef *usart, uint8_t low_power);

/* 说明：禁用 IrDA 模式 */
void usart_disable_irda(USART_TypeDef *usart);

/* 说明：使能智能卡模式，nackt_enable 非零时使能 NACK 信号 */
void usart_enable_smartcard(USART_TypeDef *usart, uint8_t nackt_enable);

/* 说明：禁用智能卡模式 */
void usart_disable_smartcard(USART_TypeDef *usart);

/* 说明：使能同步时钟输出并配置极性、相位和最后一位时钟脉冲 */
void usart_enable_clock(USART_TypeDef *usart, uint8_t cpol, uint8_t cpha, uint8_t lastbit);

/* 说明：禁用同步时钟输出 */
void usart_disable_clock(USART_TypeDef *usart);

#endif /* DRV_USING_USART */
#endif /* __DRV_USART_H__ */
