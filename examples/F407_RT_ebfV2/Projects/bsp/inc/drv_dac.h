#ifndef __DRV_DAC_H__
#define __DRV_DAC_H__

#include "drv_config.h"

#ifdef DRV_USING_DAC

#include "stm32f4xx.h"

/*
 * 说明：
 * 本头文件提供 STM32F407 DAC 驱动的公开接口。
 *
 * 当前版本语义：
 * 1. 当前版本按“单 DAC 外设本地闭合能力集”提供接口。
 * 2. 当前版本覆盖：
 *    - DAC 时钟
 *    - 通道使能/关闭
 *    - 输出缓冲控制
 *    - 触发配置/关闭/软件触发
 *    - 噪声波/三角波配置
 *    - 数据写入与输出读回
 *    - DAC 本地 DMA 控制
 *    - DMA 下溢标志/中断
 *    - 默认初始化辅助函数
 *
 * 限制：
 * 1. PA4/PA5 的 GPIO 模拟模式配置不属于本驱动范围。
 * 2. TIM 触发源配置不属于本驱动范围。
 * 3. DMA 控制器的 Stream/Channel/传输配置不属于本驱动范围。
 * 4. 当前版本不提供 DAC+TIM+DMA 的高层联动封装。
 */

/* ==================== 类型定义 ==================== */

/* 说明：DAC 通道选择。 */
typedef enum
{
    DAC_CHANNEL_1 = 1,
    DAC_CHANNEL_2 = 2
} dac_channel_t;

/*
 * 说明：
 * DAC 触发源选择，枚举值与 TSELx 编码一一对应。
 *
 * 当前版本语义：
 * “无触发”不作为一个独立触发源枚举提供，
 * 如需无触发模式，请调用 dac_disable_trigger()。
 */
typedef enum
{
    DAC_TRIG_TIM6      = 0,
    DAC_TRIG_TIM8      = 1,
    DAC_TRIG_TIM7      = 2,
    DAC_TRIG_TIM5      = 3,
    DAC_TRIG_TIM2      = 4,
    DAC_TRIG_TIM4      = 5,
    DAC_TRIG_EXTI9     = 6,
    DAC_TRIG_SOFTWARE  = 7
} dac_trigger_t;

/* 说明：DAC 波形生成模式。 */
typedef enum
{
    DAC_WAVE_NONE     = 0,
    DAC_WAVE_NOISE    = 1,
    DAC_WAVE_TRIANGLE = 2
} dac_wave_t;

/* ==================== 时钟控制 ==================== */

/* 说明：使能 DAC 外设时钟。 */
void dac_clk_enable(void);

/* ==================== 通道使能控制 ==================== */

/* 说明：使能指定 DAC 通道。 */
void dac_enable(dac_channel_t channel);

/* 说明：关闭指定 DAC 通道。 */
void dac_disable(dac_channel_t channel);

/* ==================== 输出缓冲控制 ==================== */

/* 说明：使能输出缓冲。 */
void dac_enable_output_buffer(dac_channel_t channel);

/* 说明：关闭输出缓冲。 */
void dac_disable_output_buffer(dac_channel_t channel);

/* ==================== 触发控制 ==================== */

/*
 * 说明：
 * 配置并使能指定通道的触发源。
 *
 * 前提：
 * 1. channel 合法
 * 2. trig 合法
 * 3. 当前通道处于关闭状态
 *
 * 限制：
 * 根据 RM，ENx = 1 时不能修改 TSELx。
 *
 * 当前版本语义：
 * 若通道已使能，本函数直接返回，不修改寄存器。
 */
void dac_set_trigger(dac_channel_t channel, dac_trigger_t trig);

/*
 * 说明：
 * 关闭指定通道触发功能。
 *
 * 当前版本语义：
 * 1. 本函数仅清除 TENx，不清除 TSELx。
 * 2. TENx = 0 时，写入 DAC_DHRx 的数据会自动转移到 DAC_DORx。
 */
void dac_disable_trigger(dac_channel_t channel);

/*
 * 说明：
 * 对指定通道发起一次软件触发。
 *
 * 前提：
 * 1. 已使能触发（TENx = 1）
 * 2. 触发源已配置为 DAC_TRIG_SOFTWARE
 *
 * 当前版本语义：
 * 本函数仅执行 SWTRIGx 写 1 触发，不检查当前触发模式组合。
 */
void dac_software_trigger(dac_channel_t channel);

/* ==================== 波形生成控制 ==================== */

/*
 * 说明：
 * 配置指定通道的噪声波/三角波生成参数。
 *
 * 参数说明：
 * mask_amp 为 4 位编码。
 *
 * 当前版本语义：
 * 1. 当 wave = DAC_WAVE_NONE 时，清除波形相关配置。
 * 2. mask_amp 取值 0~15 全部允许。
 * 3. 按 RM，11~15 均等价到最大掩码/最大振幅。
 *
 * 限制：
 * 波形生成功能仅在 TENx = 1 时有意义。
 */
void dac_set_wave(dac_channel_t channel, dac_wave_t wave, uint8_t mask_amp);

/* ==================== 数据写入 ==================== */

/* 说明：12 位右对齐写入。 */
void dac_write_12r(dac_channel_t channel, uint16_t value);

/* 说明：12 位左对齐写入。 */
void dac_write_12l(dac_channel_t channel, uint16_t value);

/* 说明：8 位右对齐写入。 */
void dac_write_8r(dac_channel_t channel, uint8_t value);

/*
 * 说明：
 * 双通道同时写入，格式为 12 位右对齐。
 *
 * 当前版本语义：
 * 当前版本仅提供 dual 12-bit right-aligned 写入接口。
 */
void dac_write_dual_12r(uint16_t val_ch1, uint16_t val_ch2);

/* ==================== 输出读回 ==================== */

/* 说明：读取指定通道当前 DAC 输出寄存器值。 */
uint16_t dac_read_output(dac_channel_t channel);

/* ==================== DMA 控制 ==================== */

/* 说明：使能指定通道 DAC 侧 DMA。 */
void dac_enable_dma(dac_channel_t channel);

/* 说明：关闭指定通道 DAC 侧 DMA。 */
void dac_disable_dma(dac_channel_t channel);

/* 说明：使能指定通道 DMA 下溢中断。 */
void dac_enable_dma_underrun_interrupt(dac_channel_t channel);

/* 说明：关闭指定通道 DMA 下溢中断。 */
void dac_disable_dma_underrun_interrupt(dac_channel_t channel);

/* 说明：读取指定通道 DMA 下溢标志。返回 1 表示置位，0 表示未置位。 */
int dac_get_dma_underrun_flag(dac_channel_t channel);

/*
 * 说明：
 * 清除指定通道 DMA 下溢标志。
 *
 * 当前版本语义：
 * 按 RM，DMAUDRx 采用“写 1 清零”。
 */
void dac_clear_dma_underrun_flag(dac_channel_t channel);

/* ==================== 默认初始化辅助 ==================== */

/*
 * 说明：
 * 对指定通道执行确定性默认初始化。
 *
 * 当前版本语义：
 * 1. 使能 DAC 外设时钟
 * 2. 关闭该通道
 * 3. 关闭触发，并清除触发源选择位
 * 4. 关闭波形生成
 * 5. 关闭 DAC 侧 DMA
 * 6. 关闭 DMA 下溢中断
 * 7. 默认使能输出缓冲
 * 8. 清除 DMA 下溢标志
 *
 * 限制：
 * 本函数不会自动使能 DAC 通道。
 */
void dac_init(dac_channel_t channel);

#endif /* DRV_USING_DAC */
#endif /* __DRV_DAC_H__ */
