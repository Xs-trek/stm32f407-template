#ifndef __DRV_TIM_H__
#define __DRV_TIM_H__

#include "drv_config.h"

#ifdef DRV_USING_TIM

#include "stm32f4xx.h"

/*
 * 说明：
 * 本头文件提供 STM32F407 通用/高级定时器驱动的公共接口。
 *
 * 当前版本定义：
 * 1. 当前版本覆盖 TIM1~TIM14 的基础功能。
 * 2. 当前版本覆盖：
 *    - 定时器时钟使能 / 时钟频率查询 / IRQn 查询
 *    - 基本定时配置（PSC / ARR / DIR）
 *    - 频率配置辅助函数
 *    - 启动 / 停止 / 计数器操作（inline）
 *    - 更新中断 / 更新标志
 *    - CC 中断 / CC 标志
 *    - 输出比较 / PWM 配置
 *    - 输入捕获配置
 *    - 编码器模式
 *    - 单脉冲模式
 *    - 主从模式
 *    - 高级定时器（TIM1/TIM8）主输出 / 死区时间
 *
 * 限制：
 * 1. 当前版本不提供 DMA burst 配置接口。
 * 2. 当前版本不提供互补输出通道配置接口。
 * 3. 当前版本不提供刹车输入配置接口。
 * 4. 当前版本不区分 16 位 / 32 位定时器的 ARR 范围检查。
 */

/* ==================== 通道 / 模式常量 ==================== */

#define TIM_CH1                 1U
#define TIM_CH2                 2U
#define TIM_CH3                 3U
#define TIM_CH4                 4U

#define TIM_DIR_UP              0U
#define TIM_DIR_DOWN            1U

#define TIM_OC_MODE_FROZEN      0U
#define TIM_OC_MODE_ACTIVE      1U
#define TIM_OC_MODE_INACTIVE    2U
#define TIM_OC_MODE_TOGGLE      3U
#define TIM_OC_MODE_FORCE_LOW   4U
#define TIM_OC_MODE_FORCE_HIGH  5U
#define TIM_OC_MODE_PWM1        6U
#define TIM_OC_MODE_PWM2        7U

#define TIM_IC_POLARITY_RISING      0U
#define TIM_IC_POLARITY_FALLING     1U
#define TIM_IC_POLARITY_BOTHEDGE    2U

#define TIM_ENCODER_MODE_TI1   1U
#define TIM_ENCODER_MODE_TI2   2U
#define TIM_ENCODER_MODE_TI12  3U

/* ==================== 配置结构体 ==================== */

/* 说明：基本定时配置。 */
typedef struct
{
    TIM_TypeDef *instance;
    uint16_t     prescaler;
    uint32_t     period;
    uint8_t      dir;
} tim_base_config_t;

/* 说明：输出比较配置。 */
typedef struct
{
    uint8_t  mode;
    uint32_t pulse;
    uint8_t  polarity_low;
    uint8_t  preload_enable;
} tim_oc_config_t;

/* 说明：输入捕获配置。 */
typedef struct
{
    uint8_t channel;
    uint8_t polarity;
    uint8_t filter;
    uint8_t prescaler;
} tim_ic_config_t;

/* ==================== 时钟 ==================== */

/* 说明：使能指定定时器时钟。 */
void tim_clk_enable(TIM_TypeDef *tim);

/* 说明：获取指定定时器的输入时钟频率（Hz）。 */
uint32_t tim_get_clock(TIM_TypeDef *tim);

/* 说明：获取指定定时器的更新中断 IRQn。 */
IRQn_Type tim_get_irqn(TIM_TypeDef *tim);

/* ==================== 基本定时 ==================== */

/* 说明：初始化定时器基本配置。 */
void tim_base_init(const tim_base_config_t *cfg);

/*
 * 说明：
 * 根据目标频率自动计算并配置 PSC/ARR。
 *
 * 限制：
 * 简单近似算法，不保证精确匹配。
 */
void tim_config_frequency(TIM_TypeDef *tim, uint32_t freq_hz);

/* 说明：启动定时器。 */
static inline void tim_start(TIM_TypeDef *tim)
{
    tim->CR1 |= TIM_CR1_CEN;
}

/* 说明：停止定时器。 */
static inline void tim_stop(TIM_TypeDef *tim)
{
    tim->CR1 &= ~TIM_CR1_CEN;
}

/* ==================== 计数器操作（inline） ==================== */

static inline void tim_set_counter(TIM_TypeDef *tim, uint32_t value)
{
    tim->CNT = value;
}

static inline uint32_t tim_get_counter(TIM_TypeDef *tim)
{
    return tim->CNT;
}

static inline void tim_set_prescaler(TIM_TypeDef *tim, uint16_t psc)
{
    tim->PSC = psc;
}

static inline void tim_set_period(TIM_TypeDef *tim, uint32_t arr)
{
    tim->ARR = arr;
}

static inline void tim_generate_update(TIM_TypeDef *tim)
{
    tim->EGR = TIM_EGR_UG;
}

/* ==================== 更新中断 / 标志 ==================== */

/* 说明：使能更新中断。 */
void tim_enable_update_irq(TIM_TypeDef *tim);

/* 说明：禁用更新中断。 */
void tim_disable_update_irq(TIM_TypeDef *tim);

static inline void tim_clear_update_flag(TIM_TypeDef *tim)
{
    tim->SR = (uint16_t)~TIM_SR_UIF;
}

static inline uint8_t tim_get_update_flag(TIM_TypeDef *tim)
{
    return (uint8_t)((tim->SR & TIM_SR_UIF) ? 1U : 0U);
}

/* ==================== CC 中断 / 标志 ==================== */

void    tim_enable_cc_irq(TIM_TypeDef *tim, uint8_t channel);
void    tim_disable_cc_irq(TIM_TypeDef *tim, uint8_t channel);
uint8_t tim_get_cc_flag(TIM_TypeDef *tim, uint8_t channel);
void    tim_clear_cc_flag(TIM_TypeDef *tim, uint8_t channel);

/* ==================== 输出比较 / PWM ==================== */

/* 说明：初始化指定通道的输出比较配置。 */
void tim_oc_init(TIM_TypeDef *tim, uint8_t channel, const tim_oc_config_t *cfg);

/* 说明：设置指定通道的比较值。 */
void tim_set_compare(TIM_TypeDef *tim, uint8_t channel, uint32_t value);

/* ==================== 输入捕获 ==================== */

/* 说明：初始化输入捕获通道。 */
void tim_ic_init(TIM_TypeDef *tim, const tim_ic_config_t *cfg);

/* 说明：读取指定通道的捕获值。 */
uint32_t tim_get_capture(TIM_TypeDef *tim, uint8_t channel);

/* ==================== 编码器 ==================== */

/* 说明：配置编码器模式。 */
void tim_encoder_init(TIM_TypeDef *tim, uint8_t mode, uint8_t ic1_polarity, uint8_t ic2_polarity);

/* ==================== 单脉冲 ==================== */

/* 说明：使能单脉冲模式。 */
void tim_one_pulse_enable(TIM_TypeDef *tim);

/* ==================== 主从模式 ==================== */

/* 说明：设置主模式输出（MMS）。 */
void tim_set_master_mode(TIM_TypeDef *tim, uint8_t mode);

/* 说明：设置从模式和触发源。 */
void tim_set_slave_mode(TIM_TypeDef *tim, uint8_t mode, uint8_t trigger);

/* ==================== 高级定时器（TIM1/TIM8） ==================== */

/* 说明：使能主输出（MOE）。仅对 TIM1/TIM8 有效。 */
void tim_main_output_enable(TIM_TypeDef *tim);

/* 说明：禁用主输出（MOE）。仅对 TIM1/TIM8 有效。 */
void tim_main_output_disable(TIM_TypeDef *tim);

/* 说明：设置死区时间。仅对 TIM1/TIM8 有效。 */
void tim_set_deadtime(TIM_TypeDef *tim, uint8_t deadtime);

#endif /* DRV_USING_TIM */
#endif /* __DRV_TIM_H__ */
