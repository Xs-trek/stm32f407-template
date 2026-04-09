#ifndef __DRV_DMA_H__
#define __DRV_DMA_H__

#include "drv_config.h"

#ifdef DRV_USING_DMA

#include "stm32f4xx.h"

/*
 * 说明：
 * 本头文件提供 STM32F407 DMA 驱动的公开接口。
 *
 * 当前版本语义：
 * 1. 当前版本覆盖 DMA1 / DMA2 的 8 路 stream 基础配置与控制。
 * 2. 当前版本覆盖：
 *    - DMA 时钟使能
 *    - stream/controller/IRQ 查询
 *    - stream 初始化
 *    - 启动 / 停止
 *    - 标志查询 / 清除
 *    - 计数与地址运行时更新
 *    - circular / double buffer / FIFO / burst
 *
 * 限制：
 * 1. 当前版本不提供 PFCTRL、PINCOS、FEIE、DMEIE 等高级配置接口。
 * 2. 当前版本不提供 DMA 控制器与外设的高层联动封装。
 * 3. 当前版本不负责为具体外设选择 stream/channel 组合。
 * 4. 查询型接口仅对合法 DMA stream 实例有意义。
 */

/* ==================== 类型定义 ==================== */

/* 说明：DMA 传输方向。 */
typedef enum
{
    DMA_DIR_P2M = 0,
    DMA_DIR_M2P = 1,
    DMA_DIR_M2M = 2
} dma_dir_t;

/* 说明：DMA 数据宽度。 */
typedef enum
{
    DMA_SIZE_BYTE     = 0,
    DMA_SIZE_HALFWORD = 1,
    DMA_SIZE_WORD     = 2
} dma_size_t;

/* 说明：DMA 优先级。 */
typedef enum
{
    DMA_PRIORITY_LOW       = 0,
    DMA_PRIORITY_MEDIUM    = 1,
    DMA_PRIORITY_HIGH      = 2,
    DMA_PRIORITY_VERY_HIGH = 3
} dma_priority_t;

/* 说明：DMA burst 配置。 */
typedef enum
{
    DMA_BURST_SINGLE = 0,
    DMA_BURST_INC4   = 1,
    DMA_BURST_INC8   = 2,
    DMA_BURST_INC16  = 3
} dma_burst_t;

/* 说明：DMA FIFO 阈值。 */
typedef enum
{
    DMA_FIFO_TH_1_4  = 0,
    DMA_FIFO_TH_HALF = 1,
    DMA_FIFO_TH_3_4  = 2,
    DMA_FIFO_TH_FULL = 3
} dma_fifo_threshold_t;

/*
 * 说明：
 * DMA stream 初始化配置。
 *
 * 当前版本语义：
 * 1. channel 取值范围为 0~7。
 * 2. mem1_addr 仅在 double_buffer != 0 时有效。
 * 3. double_buffer != 0 时，当前版本按循环模式配置。
 * 4. dir = DMA_DIR_M2M 时，当前版本自动按 FIFO mode 配置。
 * 5. mem_burst / periph_burst 任一非 SINGLE 时，当前版本自动按 FIFO mode 配置。
 * 6. 若未进入 FIFO mode，则 fifo_threshold 配置无效。
 *
 * 限制：
 * 1. double_buffer != 0 时，mem1_addr 必须为有效地址。
 * 2. p_size / m_size 不应传入保留值。
 */
typedef struct
{
    DMA_Stream_TypeDef  *stream;
    uint32_t             channel;          /* 0~7 */
    uint32_t             periph_addr;
    uint32_t             mem0_addr;
    uint32_t             mem1_addr;        /* 仅在 double_buffer != 0 时使用 */
    uint16_t             count;
    dma_dir_t            dir;
    dma_size_t           p_size;
    dma_size_t           m_size;
    uint8_t              p_inc;
    uint8_t              m_inc;
    uint8_t              circular;
    uint8_t              double_buffer;
    dma_priority_t       priority;
    uint8_t              use_fifo;
    dma_fifo_threshold_t fifo_threshold;
    dma_burst_t          mem_burst;
    dma_burst_t          periph_burst;
    uint8_t              enable_tc_irq;
    uint8_t              enable_ht_irq;
    uint8_t              enable_te_irq;
} dma_config_t;

/* ==================== 时钟控制 ==================== */

/* 说明：使能指定 DMA 控制器时钟。 */
void dma_clk_enable(DMA_TypeDef *dma);

/* ==================== stream 信息查询 ==================== */

/*
 * 说明：
 * 根据 stream 获取所属 DMA 控制器。
 *
 * 限制：
 * 仅对合法 DMA stream 实例有意义。
 *
 * 当前版本语义：
 * 若 stream 非法，则返回 NULL。
 */
DMA_TypeDef *dma_get_controller(DMA_Stream_TypeDef *stream);

/*
 * 说明：
 * 获取 stream 索引号（0~7）。
 *
 * 限制：
 * 仅对合法 DMA stream 实例有意义。
 *
 * 当前版本语义：
 * 若 stream 非法，则返回 0。
 */
uint8_t dma_get_stream_index(DMA_Stream_TypeDef *stream);

/*
 * 说明：
 * 获取指定 stream 对应的中断号。
 *
 * 限制：
 * 仅对合法 DMA stream 实例有意义。
 *
 * 当前版本语义：
 * 若 stream 非法，则返回值无效，仅用于轻量防御。
 */
IRQn_Type dma_get_irqn(DMA_Stream_TypeDef *stream);

/* ==================== 初始化 / 启动 / 停止 ==================== */

/*
 * 说明：
 * 对指定 DMA stream 执行初始化。
 *
 * 前提：
 * 1. cfg 非空
 * 2. cfg->stream 为合法 DMA stream 实例
 * 3. 配置参数满足当前版本约束
 *
 * 当前版本语义：
 * 1. 初始化前会先关闭 stream，并等待硬件确认 EN 清零。
 * 2. 初始化时会清除该 stream 对应全部事件标志。
 * 3. 若 dir = DMA_DIR_M2M 或任一 burst 非 SINGLE，则自动按 FIFO mode 配置。
 * 4. 若 double_buffer != 0，则自动按循环模式配置。
 */
void dma_init(const dma_config_t *cfg);

/*
 * 说明：
 * 启动指定 stream。
 *
 * 前提：
 * stream 为合法 DMA stream 实例。
 *
 * 当前版本语义：
 * 启动前会先清除该 stream 对应全部事件标志。
 */
void dma_start(DMA_Stream_TypeDef *stream);

/*
 * 说明：
 * 停止指定 stream。
 *
 * 前提：
 * stream 为合法 DMA stream 实例。
 *
 * 当前版本语义：
 * 清除 EN 位后，等待硬件将 EN 读回为 0。
 */
void dma_stop(DMA_Stream_TypeDef *stream);

/* ==================== 运行时参数更新 ==================== */

/*
 * 说明：
 * 更新要传输的数据项数。
 *
 * 前提：
 * stream 处于 disabled 状态。
 */
void dma_set_count(DMA_Stream_TypeDef *stream, uint16_t count);

/*
 * 说明：
 * 读取当前剩余数据项数。
 */
uint16_t dma_get_count(DMA_Stream_TypeDef *stream);

/*
 * 说明：
 * 更新 memory0 地址。
 *
 * 前提：
 * 满足以下任一条件：
 * 1. stream 处于 disabled 状态
 * 2. 双缓冲模式下，stream 已使能且当前目标为 memory1（CT = 1）
 */
void dma_set_mem0_addr(DMA_Stream_TypeDef *stream, uint32_t addr);

/*
 * 说明：
 * 更新 memory1 地址。
 *
 * 前提：
 * 满足以下任一条件：
 * 1. stream 处于 disabled 状态
 * 2. 双缓冲模式下，stream 已使能且当前目标为 memory0（CT = 0）
 */
void dma_set_mem1_addr(DMA_Stream_TypeDef *stream, uint32_t addr);

/*
 * 说明：
 * 更新外设地址。
 *
 * 前提：
 * stream 处于 disabled 状态。
 */
void dma_set_periph_addr(DMA_Stream_TypeDef *stream, uint32_t addr);

/*
 * 说明：
 * 获取双缓冲模式下当前目标。
 *
 * 当前版本语义：
 * 返回 0 表示当前目标为 memory0，返回 1 表示当前目标为 memory1。
 */
uint8_t dma_get_current_target(DMA_Stream_TypeDef *stream);

/* ==================== 标志管理 ==================== */

/*
 * 说明：
 * 清除指定 stream 对应的 FE / DME / TE / HT / TC 标志。
 */
void dma_clear_flags(DMA_Stream_TypeDef *stream);

/* 说明：读取传输完成标志。 */
uint8_t dma_get_flag_tc(DMA_Stream_TypeDef *stream);

/* 说明：读取半传输标志。 */
uint8_t dma_get_flag_ht(DMA_Stream_TypeDef *stream);

/* 说明：读取传输错误标志。 */
uint8_t dma_get_flag_te(DMA_Stream_TypeDef *stream);

/* 说明：读取 FIFO 错误标志。 */
uint8_t dma_get_flag_fe(DMA_Stream_TypeDef *stream);

/* 说明：读取直接模式错误标志。 */
uint8_t dma_get_flag_dme(DMA_Stream_TypeDef *stream);

#endif /* DRV_USING_DMA */
#endif /* __DRV_DMA_H__ */
