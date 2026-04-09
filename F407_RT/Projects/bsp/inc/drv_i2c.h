#ifndef __DRV_I2C_H__
#define __DRV_I2C_H__

#include "drv_config.h"

#ifdef DRV_USING_I2C

/*
 * ==========================================================================
 *  drv_i2c  --  STM32F407 I2C 外设寄存器级驱动
 * ==========================================================================
 *
 *  当前版本覆盖范围：
 *    - 支持 I2C1 / I2C2 / I2C3 三路外设
 *    - 标准模式（SCL <= 100 kHz）与快速模式（SCL <= 400 kHz）
 *    - 7 位地址与 10 位地址
 *    - ACK / POS 控制
 *    - DMA 使能 / LAST 位控制
 *    - 事件 / 缓冲 / 错误 中断开关
 *    - 内联状态标志查询与清除
 *    - 单字节寄存器便捷读写（阻塞轮询方式）
 *
 *  已知限制：
 *    - 不支持多字节连续（burst）传输
 *    - 不支持从机模式
 *    - 标志等待无超时机制，总线异常时会死等
 *    - 所有总线操作均为阻塞（轮询）方式
 */

#include "stm32f4xx.h"

/* ==================== 配置结构体 ==================== */

typedef struct
{
    I2C_TypeDef *instance;
    uint32_t     clock_hz;     /* SCL 频率：<=100000 标准模式, <=400000 快速模式 */
    uint16_t     own_address;
    uint8_t      addr_mode;    /* 0: 7 位地址, 1: 10 位地址 */
    uint8_t      duty;         /* 仅快速模式: 0 = Tlow/Thigh=2, 1 = 16/9 */
    uint8_t      ack_enable;
} i2c_config_t;

/* ==================== 时钟 ==================== */

/* 说明：使能指定 I2C 外设的 APB1 总线时钟 */
void     i2c_clk_enable(I2C_TypeDef *i2c);

/* 说明：获取 I2C 外设所在 APB1 总线的时钟频率（Hz） */
uint32_t i2c_get_clock(I2C_TypeDef *i2c);

/* 说明：获取指定 I2C 外设的事件中断号 */
IRQn_Type i2c_get_ev_irqn(I2C_TypeDef *i2c);

/* 说明：获取指定 I2C 外设的错误中断号 */
IRQn_Type i2c_get_er_irqn(I2C_TypeDef *i2c);

/* ==================== 初始化 / 使能 / 禁用 ==================== */

/* 说明：根据配置结构体初始化 I2C 外设（含时钟使能、CCR、TRISE 等） */
void i2c_init(const i2c_config_t *cfg);

/* 说明：将 I2C 外设寄存器恢复为复位值 */
void i2c_deinit(I2C_TypeDef *i2c);

/* 说明：置位 PE 位，使能 I2C 外设 */
void i2c_enable(I2C_TypeDef *i2c);

/* 说明：清除 PE 位，禁用 I2C 外设 */
void i2c_disable(I2C_TypeDef *i2c);

/* ==================== 总线操作 ==================== */

/* 说明：产生起始条件并等待 SB 标志置位 */
void    i2c_start(I2C_TypeDef *i2c);

/* 说明：产生停止条件 */
void    i2c_stop(I2C_TypeDef *i2c);

/* 说明：发送从机地址（支持 7 位 / 10 位），read=1 为读方向 */
void    i2c_send_addr(I2C_TypeDef *i2c, uint16_t addr, uint8_t addr_mode, uint8_t read);

/* 说明：发送一个字节数据，等待 TXE 和 BTF */
void    i2c_send_byte(I2C_TypeDef *i2c, uint8_t data);

/* 说明：接收一个字节并回复 ACK */
uint8_t i2c_recv_byte_ack(I2C_TypeDef *i2c);

/* 说明：接收一个字节并回复 NACK，随后产生停止条件 */
uint8_t i2c_recv_byte_nack(I2C_TypeDef *i2c);

/* ==================== 应答 / POS 控制 ==================== */

/* 说明：使能 ACK 应答 */
void i2c_enable_ack(I2C_TypeDef *i2c);

/* 说明：禁用 ACK 应答 */
void i2c_disable_ack(I2C_TypeDef *i2c);

/* 说明：使能 POS 位（用于双字节接收流程） */
void i2c_enable_pos(I2C_TypeDef *i2c);

/* 说明：禁用 POS 位 */
void i2c_disable_pos(I2C_TypeDef *i2c);

/* ==================== DMA 控制 ==================== */

/* 说明：使能 I2C DMA 请求 */
void i2c_enable_dma(I2C_TypeDef *i2c);

/* 说明：禁用 I2C DMA 请求 */
void i2c_disable_dma(I2C_TypeDef *i2c);

/* 说明：置位 LAST 位，DMA 最后一次传输时自动发送 NACK */
void i2c_enable_dma_last(I2C_TypeDef *i2c);

/* 说明：清除 LAST 位 */
void i2c_disable_dma_last(I2C_TypeDef *i2c);

/* ==================== 中断控制 ==================== */

/* 说明：使能事件中断（ITEVTEN） */
void i2c_enable_evt_irq(I2C_TypeDef *i2c);

/* 说明：禁用事件中断 */
void i2c_disable_evt_irq(I2C_TypeDef *i2c);

/* 说明：使能缓冲中断（ITBUFEN） */
void i2c_enable_buf_irq(I2C_TypeDef *i2c);

/* 说明：禁用缓冲中断 */
void i2c_disable_buf_irq(I2C_TypeDef *i2c);

/* 说明：使能错误中断（ITERREN） */
void i2c_enable_err_irq(I2C_TypeDef *i2c);

/* 说明：禁用错误中断 */
void i2c_disable_err_irq(I2C_TypeDef *i2c);

/* ==================== 状态标志（内联） ==================== */

/* 查询标志 */
static inline int i2c_is_sb(I2C_TypeDef *i2c)    { return (i2c->SR1 & I2C_SR1_SB)    ? 1 : 0; }
static inline int i2c_is_addr(I2C_TypeDef *i2c)   { return (i2c->SR1 & I2C_SR1_ADDR)  ? 1 : 0; }
static inline int i2c_is_btf(I2C_TypeDef *i2c)    { return (i2c->SR1 & I2C_SR1_BTF)   ? 1 : 0; }
static inline int i2c_is_txe(I2C_TypeDef *i2c)    { return (i2c->SR1 & I2C_SR1_TXE)   ? 1 : 0; }
static inline int i2c_is_rxne(I2C_TypeDef *i2c)   { return (i2c->SR1 & I2C_SR1_RXNE)  ? 1 : 0; }
static inline int i2c_is_busy(I2C_TypeDef *i2c)   { return (i2c->SR2 & I2C_SR2_BUSY)  ? 1 : 0; }
static inline int i2c_is_af(I2C_TypeDef *i2c)     { return (i2c->SR1 & I2C_SR1_AF)    ? 1 : 0; }
static inline int i2c_is_berr(I2C_TypeDef *i2c)   { return (i2c->SR1 & I2C_SR1_BERR)  ? 1 : 0; }
static inline int i2c_is_arlo(I2C_TypeDef *i2c)   { return (i2c->SR1 & I2C_SR1_ARLO)  ? 1 : 0; }
static inline int i2c_is_ovr(I2C_TypeDef *i2c)    { return (i2c->SR1 & I2C_SR1_OVR)   ? 1 : 0; }

/* 清除标志 */
static inline void i2c_clear_af(I2C_TypeDef *i2c)   { i2c->SR1 = (uint16_t)~I2C_SR1_AF;   }
static inline void i2c_clear_berr(I2C_TypeDef *i2c)  { i2c->SR1 = (uint16_t)~I2C_SR1_BERR; }
static inline void i2c_clear_arlo(I2C_TypeDef *i2c)  { i2c->SR1 = (uint16_t)~I2C_SR1_ARLO; }
static inline void i2c_clear_ovr(I2C_TypeDef *i2c)   { i2c->SR1 = (uint16_t)~I2C_SR1_OVR;  }

/* ==================== 状态寄存器读取 ==================== */

/* 说明：读取 SR1 寄存器原始值 */
uint32_t i2c_get_sr1(I2C_TypeDef *i2c);

/* 说明：读取 SR2 寄存器原始值 */
uint32_t i2c_get_sr2(I2C_TypeDef *i2c);

/* ==================== 便捷接口：单字节寄存器读写 ==================== */

/* 说明：向指定从机的寄存器写入一个字节（阻塞） */
void    i2c_mem_write(I2C_TypeDef *i2c, uint8_t dev_addr, uint8_t reg, uint8_t data);

/* 说明：从指定从机的寄存器读取一个字节（阻塞） */
uint8_t i2c_mem_read(I2C_TypeDef *i2c, uint8_t dev_addr, uint8_t reg);

#endif /* DRV_USING_I2C */
#endif /* __DRV_I2C_H__ */
