#ifndef __DRV_CAN_H__
#define __DRV_CAN_H__

#include "drv_config.h"

#ifdef DRV_USING_CAN

/*
 * ============================================================
 *  drv_can.h - STM32F407 bxCAN 寄存器级驱动
 * ============================================================
 *  当前版本功能范围：
 *    - CAN1 / CAN2 时钟使能与中断号查询
 *    - 初始化（波特率、模式、自动管理选项）
 *    - 28 组硬件过滤器配置（16 位 / 32 位，掩码 / 列表）
 *    - 三邮箱发送、双 FIFO 接收
 *    - FIFO / 发送 / 错误 / 总线关闭 / 唤醒中断开关
 *    - 睡眠进入与退出
 *    - ESR / TSR / RFxR 状态读取
 *
 *  已知限制：
 *    - 不支持 TTCAN（时间触发）模式
 *    - 不支持 CAN FD
 *    - 过滤器仅通过 CAN1 主控寄存器操作，不做从属起始组配置
 *    - 无 DMA 收发支持
 * ============================================================
 */

#include "stm32f4xx.h"

/* ==================== 类型定义 ==================== */

/* CAN 初始化配置结构体 */
typedef struct
{
    CAN_TypeDef *instance;
    uint16_t     prescaler;      /* 预分频值 1..1024 */
    uint8_t      mode;           /* 0=正常, 1=回环, 2=静默, 3=静默+回环 */
    uint8_t      sjw;            /* 同步跳转宽度 1..4 */
    uint8_t      bs1;            /* 时间段1  1..16 */
    uint8_t      bs2;            /* 时间段2  1..8  */
    uint8_t      auto_bus_off;   /* 自动离线管理 */
    uint8_t      auto_wake_up;   /* 自动唤醒 */
    uint8_t      auto_retransmit;/* 自动重发 */
} can_config_t;

/* CAN 数据帧结构体 */
typedef struct
{
    uint32_t std_id;             /* 标准帧 ID */
    uint32_t ext_id;             /* 扩展帧 ID */
    uint8_t  ide;                /* 帧类型：0=标准帧, 1=扩展帧 */
    uint8_t  rtr;                /* 远程帧标志 */
    uint8_t  dlc;                /* 数据长度 0..8 */
    uint8_t  data[8];            /* 数据域 */
} can_frame_t;

/* CAN 过滤器配置结构体 */
typedef struct
{
    uint8_t  bank;               /* 过滤器组号 0..27 */
    uint8_t  fifo;               /* 关联 FIFO：0 或 1 */
    uint8_t  scale_32bit;        /* 位宽：0=16 位, 1=32 位 */
    uint8_t  mode_mask;          /* 模式：1=掩码模式, 0=标识符列表 */
    uint8_t  enable;             /* 使能标志 */
    uint32_t id_high;            /* ID / 标识符高 16 位 */
    uint32_t id_low;             /* ID / 标识符低 16 位 */
    uint32_t mask_high;          /* 掩码高 16 位 */
    uint32_t mask_low;           /* 掩码低 16 位 */
} can_filter_config_t;

/* ==================== 时钟与中断号 ==================== */

/* 说明：使能 CAN 外设时钟（CAN2 会同时使能 CAN1 时钟） */
void can_clk_enable(CAN_TypeDef *canx);

/* 说明：获取发送完成中断号 */
IRQn_Type can_get_tx_irqn(CAN_TypeDef *canx);

/* 说明：获取 FIFO0 接收中断号 */
IRQn_Type can_get_rx0_irqn(CAN_TypeDef *canx);

/* 说明：获取 FIFO1 接收中断号 */
IRQn_Type can_get_rx1_irqn(CAN_TypeDef *canx);

/* 说明：获取错误与状态变化中断号 */
IRQn_Type can_get_sce_irqn(CAN_TypeDef *canx);

/* ==================== 初始化 ==================== */

/* 说明：按配置结构体初始化 CAN 控制器（波特率、模式等） */
void can_init(const can_config_t *cfg);

/* 说明：配置指定过滤器组（位宽、模式、ID/掩码、FIFO 关联） */
void can_filter_init(const can_filter_config_t *cfg);

/* ==================== 发送与接收 ==================== */

/* 说明：将帧写入空闲邮箱并请求发送，返回邮箱号（0/1/2）或 0xFF 表示无空闲 */
uint8_t can_send(CAN_TypeDef *canx, const can_frame_t *frame);

/* 说明：从指定 FIFO 读取一帧并释放邮箱 */
void    can_recv(CAN_TypeDef *canx, uint8_t fifo, can_frame_t *frame);

/* 说明：中止指定邮箱的发送 */
void    can_abort_tx(CAN_TypeDef *canx, uint8_t mailbox);

/* ==================== 中断使能与禁止 ==================== */

/* 说明：使能 / 禁止 FIFO0 接收中断 */
void can_enable_fifo0_irq(CAN_TypeDef *canx);
void can_disable_fifo0_irq(CAN_TypeDef *canx);

/* 说明：使能 / 禁止 FIFO1 接收中断 */
void can_enable_fifo1_irq(CAN_TypeDef *canx);
void can_disable_fifo1_irq(CAN_TypeDef *canx);

/* 说明：使能 / 禁止发送邮箱空中断 */
void can_enable_tx_irq(CAN_TypeDef *canx);
void can_disable_tx_irq(CAN_TypeDef *canx);

/* 说明：使能 / 禁止错误中断 */
void can_enable_err_irq(CAN_TypeDef *canx);
void can_disable_err_irq(CAN_TypeDef *canx);

/* 说明：使能 / 禁止总线关闭中断 */
void can_enable_busoff_irq(CAN_TypeDef *canx);
void can_disable_busoff_irq(CAN_TypeDef *canx);

/* 说明：使能 / 禁止唤醒中断 */
void can_enable_wakeup_irq(CAN_TypeDef *canx);
void can_disable_wakeup_irq(CAN_TypeDef *canx);

/* ==================== 睡眠管理 ==================== */

/* 说明：进入睡眠模式 */
void can_enter_sleep(CAN_TypeDef *canx);

/* 说明：退出睡眠模式 */
void can_exit_sleep(CAN_TypeDef *canx);

/* ==================== 状态查询 ==================== */

/* 说明：读取错误状态寄存器 ESR */
uint32_t can_get_esr(CAN_TypeDef *canx);

/* 说明：读取发送状态寄存器 TSR */
uint32_t can_get_tsr(CAN_TypeDef *canx);

/* 说明：读取 FIFO0 状态寄存器 RF0R */
uint32_t can_get_rf0r(CAN_TypeDef *canx);

/* 说明：读取 FIFO1 状态寄存器 RF1R */
uint32_t can_get_rf1r(CAN_TypeDef *canx);

/* 说明：查询 FIFO0 中挂起的报文数 */
static inline uint8_t can_fifo0_pending(CAN_TypeDef *canx)
{
    return (uint8_t)(canx->RF0R & CAN_RF0R_FMP0);
}

/* 说明：查询 FIFO1 中挂起的报文数 */
static inline uint8_t can_fifo1_pending(CAN_TypeDef *canx)
{
    return (uint8_t)(canx->RF1R & CAN_RF1R_FMP1);
}

#endif /* DRV_USING_CAN */
#endif /* __DRV_CAN_H__ */
