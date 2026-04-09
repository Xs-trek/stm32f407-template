#ifndef __DRV_ADC_H__
#define __DRV_ADC_H__

#include "drv_config.h"

#ifdef DRV_USING_ADC

#include "stm32f4xx.h"

/* STM32F407 ADC 通道范围：
 * 0~15  : 外部模拟输入
 * 16    : 温度传感器
 * 17    : VREFINT
 * 18    : VBAT
 */
#define ADC_CHANNEL_MAX          18U
#define ADC_REGULAR_RANK_MAX     16U

/* 分辨率 */
typedef enum
{
    ADC_RES_12BIT = 0,
    ADC_RES_10BIT = 1,
    ADC_RES_8BIT  = 2,
    ADC_RES_6BIT  = 3
} adc_resolution_t;

/* 数据对齐 */
typedef enum
{
    ADC_ALIGN_RIGHT = 0,
    ADC_ALIGN_LEFT  = 1
} adc_align_t;

/* 采样时间 */
typedef enum
{
    ADC_SAMPLE_3CYCLES   = 0,
    ADC_SAMPLE_15CYCLES  = 1,
    ADC_SAMPLE_28CYCLES  = 2,
    ADC_SAMPLE_56CYCLES  = 3,
    ADC_SAMPLE_84CYCLES  = 4,
    ADC_SAMPLE_112CYCLES = 5,
    ADC_SAMPLE_144CYCLES = 6,
    ADC_SAMPLE_480CYCLES = 7
} adc_sample_t;

/* ADC 公共预分频 */
typedef enum
{
    ADC_PRESCALER_DIV2 = 0,
    ADC_PRESCALER_DIV4 = 1,
    ADC_PRESCALER_DIV6 = 2,
    ADC_PRESCALER_DIV8 = 3
} adc_prescaler_t;

/* 规则组外部触发边沿 */
typedef enum
{
    ADC_EXTRIG_NONE    = 0,    /* 禁止外部触发检测，适用于纯软件触发 */
    ADC_EXTRIG_RISING  = 1,
    ADC_EXTRIG_FALLING = 2,
    ADC_EXTRIG_BOTH    = 3
} adc_extrig_edge_t;

/* ADC 基础初始化配置 */
typedef struct
{
    ADC_TypeDef        *instance;
    adc_resolution_t    resolution;
    adc_align_t         align;
    uint8_t             continuous;
    uint8_t             scan;
} adc_config_t;

/* ==================== ADC 公共配置（共享 ADC_CCR） ==================== */

/* 设置 ADC 公共预分频 */
void adc_set_prescaler(adc_prescaler_t prescaler);

/* 使能/关闭温度传感器与 VREFINT 通路 */
void adc_enable_temp_vrefint(void);
void adc_disable_temp_vrefint(void);

/* 使能/关闭 VBAT 通路 */
void adc_enable_vbat(void);
void adc_disable_vbat(void);

/* ==================== 时钟 ==================== */

/* 使能 ADC1/ADC2/ADC3 时钟 */
void adc_clk_enable(ADC_TypeDef *adc);

/* ==================== 初始化 / 使能 / 关闭 ==================== */

/* ADC 基础初始化
 * 说明：
 * 1. 本函数会清除并重建 CR1/CR2/SMPR1/SMPR2/SQR1/SQR2/SQR3/JSQR 的基础配置。
 * 2. 本函数不会自动使能 ADC，需单独调用 adc_enable()。
 * 3. 调用后，上述寄存器中的旧配置会被覆盖。
 */
void adc_init(const adc_config_t *cfg);

/* 使能 / 关闭 ADC
 * 说明：
 * 1. adc_enable() 用于使能并唤醒 ADC（ADON=1）
 * 2. adc_disable() 用于关闭 ADC 并进入掉电状态（ADON=0）
 */
void adc_enable(ADC_TypeDef *adc);
void adc_disable(ADC_TypeDef *adc);

/* ==================== 规则组基础配置 ==================== */

/* 设置通道采样时间 */
void adc_set_sample_time(ADC_TypeDef *adc, uint8_t channel, adc_sample_t sample);

/* 设置规则序列指定 rank 对应的通道 */
void adc_set_regular_channel(ADC_TypeDef *adc, uint8_t rank, uint8_t channel);

/* 设置规则序列长度（1~16） */
void adc_set_regular_length(ADC_TypeDef *adc, uint8_t length);

/* 设置规则组触发源与边沿
 * 说明：
 * 1. extsel 使用硬件编码值，对应 CR2.EXTSEL[3:0]
 * 2. edge = ADC_EXTRIG_NONE 表示关闭外部触发检测，适用于纯软件触发
 */
void adc_set_regular_trigger(ADC_TypeDef *adc, uint8_t extsel, adc_extrig_edge_t edge);

/* 设置 EOC 置位时机
 * each_conversion = 0：整个规则序列结束时置位 EOC（EOCS=0）
 * each_conversion = 1：每次规则转换结束时置位 EOC（EOCS=1）
 */
void adc_set_eoc_selection(ADC_TypeDef *adc, uint8_t each_conversion);

/* 通过软件启动规则组转换
 * 前提：
 * 1. ADC 必须已使能（ADON=1）
 * 2. 若期望纯软件触发，应先将外部触发检测关闭（EXTEN=ADC_EXTRIG_NONE）
 */
void adc_start(ADC_TypeDef *adc);

/* 阻塞读取规则组数据
 * 说明：
 * 1. 本函数等待 EOC 置位
 * 2. 读取 ADC_DR 会清除 EOC
 * 3. 本函数无超时机制
 * 4. 调用者应确保这是当前期望读取的转换结果
 */
uint16_t adc_read_data(ADC_TypeDef *adc);

/* 单通道阻塞读取辅助函数（规则组）
 * 说明：
 * 1. ADC 必须已使能
 * 2. 本函数会临时改写：
 *    - 采样时间
 *    - 规则序列长度
 *    - rank1 通道
 *    - CONT / EXTEN / DMA / DDS / EOCS / SCAN 的相关位
 * 3. 本函数执行完成后会恢复上述寄存器现场
 * 4. 本函数不适用于 ADC 正在后台连续扫描 / DMA 传输 / 活动转换中的场景
 */
uint16_t adc_read_channel(ADC_TypeDef *adc, uint8_t channel, adc_sample_t sample);

/* ==================== DMA ==================== */

/* 使能 / 关闭规则组 DMA */
void adc_enable_dma(ADC_TypeDef *adc);
void adc_disable_dma(ADC_TypeDef *adc);

/* 使能 / 关闭连续 DMA 请求（DDS） */
void adc_enable_dma_continuous(ADC_TypeDef *adc);
void adc_disable_dma_continuous(ADC_TypeDef *adc);

/* ==================== 中断 ==================== */

/* EOC 中断 */
void adc_enable_eoc_irq(ADC_TypeDef *adc);
void adc_disable_eoc_irq(ADC_TypeDef *adc);

/* OVR 中断 */
void adc_enable_ovr_irq(ADC_TypeDef *adc);
void adc_disable_ovr_irq(ADC_TypeDef *adc);

/* 模拟看门狗中断 */
void adc_enable_awd_irq(ADC_TypeDef *adc);
void adc_disable_awd_irq(ADC_TypeDef *adc);

/* ==================== 状态标志 ==================== */

/* 获取状态标志 */
uint8_t adc_get_flag_eoc(ADC_TypeDef *adc);
uint8_t adc_get_flag_ovr(ADC_TypeDef *adc);
uint8_t adc_get_flag_awd(ADC_TypeDef *adc);

/* 清除状态标志
 * 注意：
 * 1. EOC 也可通过读取 ADC_DR 清除
 * 2. 这里提供的是软件清除路径
 */
void adc_clear_flag_eoc(ADC_TypeDef *adc);
void adc_clear_flag_ovr(ADC_TypeDef *adc);
void adc_clear_flag_awd(ADC_TypeDef *adc);

/* ==================== 模拟看门狗（规则组单通道子集） ==================== */

/* 设置模拟看门狗监控通道 */
void adc_set_analog_watchdog_channel(ADC_TypeDef *adc, uint8_t channel);

/* 设置模拟看门狗上下阈值（12 位） */
void adc_set_analog_watchdog_threshold(ADC_TypeDef *adc, uint16_t low, uint16_t high);

/* 使能 / 关闭模拟看门狗
 * 当前版本语义：
 * 1. 仅作用于规则组（AWDEN=1，JAWDEN=0）
 * 2. 仅启用单通道监控（AWDSGL=1）
 * 3. 监控通道由 adc_set_analog_watchdog_channel() 指定
 */
void adc_enable_analog_watchdog(ADC_TypeDef *adc);
void adc_disable_analog_watchdog(ADC_TypeDef *adc);

#endif /* DRV_USING_ADC */
#endif /* __DRV_ADC_H__ */
