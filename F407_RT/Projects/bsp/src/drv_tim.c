#include "drv_tim.h"

#ifdef DRV_USING_TIM

#include "drv_rcc.h"

/* ==================== 内部辅助函数 ==================== */

static const uint8_t hpre_shift_lut[16] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 2, 3, 4, 6, 7, 8, 9
};

static const uint8_t ppre_shift_lut[8] = {
    0, 0, 0, 0,
    1, 2, 3, 4
};

static uint32_t tim_get_apb_timer_clock(uint8_t apb_idx)
{
    uint32_t hclk;
    uint32_t ppre;

    hclk = SystemCoreClock >> hpre_shift_lut[(RCC->CFGR >> 4U) & 0x0FU];

    if (apb_idx == 1U)
    {
        ppre = ppre_shift_lut[(RCC->CFGR >> 10U) & 0x07U];
    }
    else
    {
        ppre = ppre_shift_lut[(RCC->CFGR >> 13U) & 0x07U];
    }

    if (ppre == 0U) { return hclk; }

    /* 定时器时钟 = APBx 时钟 × 2（当 APBx 分频系数 != 1 时） */
    return (hclk >> ppre) << 1U;
}

static int tim_is_apb2(TIM_TypeDef *tim)
{
    return (tim == TIM1) || (tim == TIM8) || (tim == TIM9)
        || (tim == TIM10) || (tim == TIM11);
}

/* ==================== 时钟 ==================== */

void tim_clk_enable(TIM_TypeDef *tim)
{
    if (tim == NULL) { return; }

#ifdef DRV_USING_RCC
    rcc_enable_tim_clk(tim);
#else
    if (tim == TIM1)       { RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; }
    else if (tim == TIM2)  { RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; }
    else if (tim == TIM3)  { RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; }
    else if (tim == TIM4)  { RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; }
    else if (tim == TIM5)  { RCC->APB1ENR |= RCC_APB1ENR_TIM5EN; }
    else if (tim == TIM6)  { RCC->APB1ENR |= RCC_APB1ENR_TIM6EN; }
    else if (tim == TIM7)  { RCC->APB1ENR |= RCC_APB1ENR_TIM7EN; }
    else if (tim == TIM8)  { RCC->APB2ENR |= RCC_APB2ENR_TIM8EN; }
    else if (tim == TIM9)  { RCC->APB2ENR |= RCC_APB2ENR_TIM9EN; }
    else if (tim == TIM10) { RCC->APB2ENR |= RCC_APB2ENR_TIM10EN; }
    else if (tim == TIM11) { RCC->APB2ENR |= RCC_APB2ENR_TIM11EN; }
    else if (tim == TIM12) { RCC->APB1ENR |= RCC_APB1ENR_TIM12EN; }
    else if (tim == TIM13) { RCC->APB1ENR |= RCC_APB1ENR_TIM13EN; }
    else if (tim == TIM14) { RCC->APB1ENR |= RCC_APB1ENR_TIM14EN; }
    else { return; }

    (void)RCC->APB1ENR;
    (void)RCC->APB2ENR;
#endif
}

uint32_t tim_get_clock(TIM_TypeDef *tim)
{
    if (tim == NULL) { return 0U; }
    return tim_get_apb_timer_clock(tim_is_apb2(tim) ? 2U : 1U);
}

IRQn_Type tim_get_irqn(TIM_TypeDef *tim)
{
    if (tim == TIM1)  { return TIM1_UP_TIM10_IRQn; }
    if (tim == TIM2)  { return TIM2_IRQn; }
    if (tim == TIM3)  { return TIM3_IRQn; }
    if (tim == TIM4)  { return TIM4_IRQn; }
    if (tim == TIM5)  { return TIM5_IRQn; }
    if (tim == TIM6)  { return TIM6_DAC_IRQn; }
    if (tim == TIM7)  { return TIM7_IRQn; }
    if (tim == TIM8)  { return TIM8_UP_TIM13_IRQn; }
    if (tim == TIM9)  { return TIM1_BRK_TIM9_IRQn; }
    if (tim == TIM10) { return TIM1_UP_TIM10_IRQn; }
    if (tim == TIM11) { return TIM1_TRG_COM_TIM11_IRQn; }
    if (tim == TIM12) { return TIM8_BRK_TIM12_IRQn; }
    if (tim == TIM13) { return TIM8_UP_TIM13_IRQn; }
    return TIM8_TRG_COM_TIM14_IRQn;
}

/* ==================== 基本定时 ==================== */

void tim_base_init(const tim_base_config_t *cfg)
{
    TIM_TypeDef *tim;

    if (cfg == NULL || cfg->instance == NULL) { return; }

    tim = cfg->instance;

    tim_clk_enable(tim);

    tim->CR1   = 0U;
    tim->CR2   = 0U;
    tim->SMCR  = 0U;
    tim->DIER  = 0U;
    tim->CCMR1 = 0U;
    tim->CCMR2 = 0U;
    tim->CCER  = 0U;

    tim->PSC = cfg->prescaler;
    tim->ARR = cfg->period;

    if (cfg->dir == TIM_DIR_DOWN)
    {
        tim->CR1 |= TIM_CR1_DIR;
    }

    tim->EGR = TIM_EGR_UG;
}

void tim_config_frequency(TIM_TypeDef *tim, uint32_t freq_hz)
{
    uint32_t tim_clk;
    uint32_t total_div;
    uint32_t psc;
    uint32_t arr;

    if (tim == NULL || freq_hz == 0U) { return; }

    tim_clk = tim_get_clock(tim);
    if (tim_clk == 0U) { return; }

    total_div = tim_clk / freq_hz;
    if (total_div == 0U) { total_div = 1U; }

    psc = 0U;

    if (total_div <= 0x10000U)
    {
        arr = total_div - 1U;
    }
    else
    {
        psc = (total_div / 0x10000U);
        arr = (total_div / (psc + 1U)) - 1U;
        if (arr > 0xFFFFU)
        {
            psc++;
            arr = (total_div / (psc + 1U)) - 1U;
        }
    }

    tim->PSC = (uint16_t)psc;
    tim->ARR = arr;
    tim->EGR = TIM_EGR_UG;
}

/* ==================== 更新中断 / 标志 ==================== */

void tim_enable_update_irq(TIM_TypeDef *tim)
{
    if (tim == NULL) { return; }
    tim->DIER |= TIM_DIER_UIE;
}

void tim_disable_update_irq(TIM_TypeDef *tim)
{
    if (tim == NULL) { return; }
    tim->DIER &= ~TIM_DIER_UIE;
}

/* ==================== CC 中断 / 标志 ==================== */

void tim_enable_cc_irq(TIM_TypeDef *tim, uint8_t channel)
{
    if (tim == NULL || channel < TIM_CH1 || channel > TIM_CH4) { return; }
    tim->DIER |= (1U << channel);
}

void tim_disable_cc_irq(TIM_TypeDef *tim, uint8_t channel)
{
    if (tim == NULL || channel < TIM_CH1 || channel > TIM_CH4) { return; }
    tim->DIER &= ~(1U << channel);
}

uint8_t tim_get_cc_flag(TIM_TypeDef *tim, uint8_t channel)
{
    if (tim == NULL || channel < TIM_CH1 || channel > TIM_CH4) { return 0U; }
    return (uint8_t)((tim->SR & (1U << channel)) ? 1U : 0U);
}

void tim_clear_cc_flag(TIM_TypeDef *tim, uint8_t channel)
{
    if (tim == NULL || channel < TIM_CH1 || channel > TIM_CH4) { return; }
    tim->SR = (uint16_t)~(1U << channel);
}

/* ==================== 输出比较 / PWM ==================== */

void tim_oc_init(TIM_TypeDef *tim, uint8_t channel, const tim_oc_config_t *cfg)
{
    uint32_t mode;

    if (tim == NULL || cfg == NULL) { return; }

    mode = ((uint32_t)(cfg->mode & 0x07U) << 4U);

    switch (channel)
    {
    case TIM_CH1:
        tim->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_OC1M | TIM_CCMR1_OC1PE);
        tim->CCMR1 |= mode;
        if (cfg->preload_enable) { tim->CCMR1 |= TIM_CCMR1_OC1PE; }
        if (cfg->polarity_low) { tim->CCER |= TIM_CCER_CC1P; }
        else { tim->CCER &= ~TIM_CCER_CC1P; }
        tim->CCR1 = cfg->pulse;
        tim->CCER |= TIM_CCER_CC1E;
        break;

    case TIM_CH2:
        tim->CCMR1 &= ~(TIM_CCMR1_CC2S | TIM_CCMR1_OC2M | TIM_CCMR1_OC2PE);
        tim->CCMR1 |= (mode << 8U);
        if (cfg->preload_enable) { tim->CCMR1 |= TIM_CCMR1_OC2PE; }
        if (cfg->polarity_low) { tim->CCER |= TIM_CCER_CC2P; }
        else { tim->CCER &= ~TIM_CCER_CC2P; }
        tim->CCR2 = cfg->pulse;
        tim->CCER |= TIM_CCER_CC2E;
        break;

    case TIM_CH3:
        tim->CCMR2 &= ~(TIM_CCMR2_CC3S | TIM_CCMR2_OC3M | TIM_CCMR2_OC3PE);
        tim->CCMR2 |= mode;
        if (cfg->preload_enable) { tim->CCMR2 |= TIM_CCMR2_OC3PE; }
        if (cfg->polarity_low) { tim->CCER |= TIM_CCER_CC3P; }
        else { tim->CCER &= ~TIM_CCER_CC3P; }
        tim->CCR3 = cfg->pulse;
        tim->CCER |= TIM_CCER_CC3E;
        break;

    case TIM_CH4:
        tim->CCMR2 &= ~(TIM_CCMR2_CC4S | TIM_CCMR2_OC4M | TIM_CCMR2_OC4PE);
        tim->CCMR2 |= (mode << 8U);
        if (cfg->preload_enable) { tim->CCMR2 |= TIM_CCMR2_OC4PE; }
        if (cfg->polarity_low) { tim->CCER |= TIM_CCER_CC4P; }
        else { tim->CCER &= ~TIM_CCER_CC4P; }
        tim->CCR4 = cfg->pulse;
        tim->CCER |= TIM_CCER_CC4E;
        break;

    default:
        return;
    }

    tim->CR1 |= TIM_CR1_ARPE;
    tim->EGR = TIM_EGR_UG;
}

void tim_set_compare(TIM_TypeDef *tim, uint8_t channel, uint32_t value)
{
    if (tim == NULL) { return; }

    switch (channel)
    {
    case TIM_CH1: tim->CCR1 = value; break;
    case TIM_CH2: tim->CCR2 = value; break;
    case TIM_CH3: tim->CCR3 = value; break;
    case TIM_CH4: tim->CCR4 = value; break;
    default: break;
    }
}

/* ==================== 输入捕获 ==================== */

void tim_ic_init(TIM_TypeDef *tim, const tim_ic_config_t *cfg)
{
    if (tim == NULL || cfg == NULL) { return; }

    switch (cfg->channel)
    {
    case TIM_CH1:
        tim->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_IC1F | TIM_CCMR1_IC1PSC);
        tim->CCMR1 |= 1U;
        tim->CCMR1 |= ((uint32_t)(cfg->filter & 0x0FU) << 4U);
        tim->CCMR1 |= ((uint32_t)(cfg->prescaler & 0x03U) << 2U);
        tim->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
        if (cfg->polarity == TIM_IC_POLARITY_FALLING)
        {
            tim->CCER |= TIM_CCER_CC1P;
        }
        else if (cfg->polarity == TIM_IC_POLARITY_BOTHEDGE)
        {
            tim->CCER |= (TIM_CCER_CC1P | TIM_CCER_CC1NP);
        }
        tim->CCER |= TIM_CCER_CC1E;
        break;

    case TIM_CH2:
        tim->CCMR1 &= ~(TIM_CCMR1_CC2S | TIM_CCMR1_IC2F | TIM_CCMR1_IC2PSC);
        tim->CCMR1 |= (1U << 8U);
        tim->CCMR1 |= ((uint32_t)(cfg->filter & 0x0FU) << 12U);
        tim->CCMR1 |= ((uint32_t)(cfg->prescaler & 0x03U) << 10U);
        tim->CCER &= ~(TIM_CCER_CC2P | TIM_CCER_CC2NP);
        if (cfg->polarity == TIM_IC_POLARITY_FALLING)
        {
            tim->CCER |= TIM_CCER_CC2P;
        }
        else if (cfg->polarity == TIM_IC_POLARITY_BOTHEDGE)
        {
            tim->CCER |= (TIM_CCER_CC2P | TIM_CCER_CC2NP);
        }
        tim->CCER |= TIM_CCER_CC2E;
        break;

    case TIM_CH3:
        tim->CCMR2 &= ~(TIM_CCMR2_CC3S | TIM_CCMR2_IC3F | TIM_CCMR2_IC3PSC);
        tim->CCMR2 |= 1U;
        tim->CCMR2 |= ((uint32_t)(cfg->filter & 0x0FU) << 4U);
        tim->CCMR2 |= ((uint32_t)(cfg->prescaler & 0x03U) << 2U);
        tim->CCER &= ~(TIM_CCER_CC3P | TIM_CCER_CC3NP);
        if (cfg->polarity == TIM_IC_POLARITY_FALLING)
        {
            tim->CCER |= TIM_CCER_CC3P;
        }
        else if (cfg->polarity == TIM_IC_POLARITY_BOTHEDGE)
        {
            tim->CCER |= (TIM_CCER_CC3P | TIM_CCER_CC3NP);
        }
        tim->CCER |= TIM_CCER_CC3E;
        break;

    case TIM_CH4:
        tim->CCMR2 &= ~(TIM_CCMR2_CC4S | TIM_CCMR2_IC4F | TIM_CCMR2_IC4PSC);
        tim->CCMR2 |= (1U << 8U);
        tim->CCMR2 |= ((uint32_t)(cfg->filter & 0x0FU) << 12U);
        tim->CCMR2 |= ((uint32_t)(cfg->prescaler & 0x03U) << 10U);
        tim->CCER &= ~(TIM_CCER_CC4P | TIM_CCER_CC4NP);
        if (cfg->polarity == TIM_IC_POLARITY_FALLING)
        {
            tim->CCER |= TIM_CCER_CC4P;
        }
        else if (cfg->polarity == TIM_IC_POLARITY_BOTHEDGE)
        {
            tim->CCER |= (TIM_CCER_CC4P | TIM_CCER_CC4NP);
        }
        tim->CCER |= TIM_CCER_CC4E;
        break;

    default:
        break;
    }
}

uint32_t tim_get_capture(TIM_TypeDef *tim, uint8_t channel)
{
    if (tim == NULL) { return 0U; }

    switch (channel)
    {
    case TIM_CH1: return tim->CCR1;
    case TIM_CH2: return tim->CCR2;
    case TIM_CH3: return tim->CCR3;
    case TIM_CH4: return tim->CCR4;
    default: return 0U;
    }
}

/* ==================== 编码器 ==================== */

void tim_encoder_init(TIM_TypeDef *tim, uint8_t mode, uint8_t ic1_polarity, uint8_t ic2_polarity)
{
    if (tim == NULL) { return; }

    tim->SMCR &= ~TIM_SMCR_SMS;
    tim->SMCR |= ((uint32_t)(mode & 0x03U));

    /* CC1S=01 (TI1), CC2S=01 (TI2) */
    tim->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_CC2S);
    tim->CCMR1 |= 1U;
    tim->CCMR1 |= (1U << 8U);

    tim->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC2P);
    if (ic1_polarity) { tim->CCER |= TIM_CCER_CC1P; }
    if (ic2_polarity) { tim->CCER |= TIM_CCER_CC2P; }

    tim->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC2E);
}

/* ==================== 单脉冲 ==================== */

void tim_one_pulse_enable(TIM_TypeDef *tim)
{
    if (tim == NULL) { return; }
    tim->CR1 |= TIM_CR1_OPM;
}

/* ==================== 主从模式 ==================== */

void tim_set_master_mode(TIM_TypeDef *tim, uint8_t mode)
{
    if (tim == NULL) { return; }
    tim->CR2 &= ~TIM_CR2_MMS;
    tim->CR2 |= ((uint32_t)(mode & 0x07U) << 4U);
}

void tim_set_slave_mode(TIM_TypeDef *tim, uint8_t mode, uint8_t trigger)
{
    if (tim == NULL) { return; }
    tim->SMCR &= ~(TIM_SMCR_SMS | TIM_SMCR_TS);
    tim->SMCR |= ((uint32_t)(mode & 0x07U));
    tim->SMCR |= ((uint32_t)(trigger & 0x07U) << 4U);
}

/* ==================== 高级定时器（TIM1/TIM8） ==================== */

void tim_main_output_enable(TIM_TypeDef *tim)
{
    if (tim == NULL) { return; }
    if (tim == TIM1 || tim == TIM8)
    {
        tim->BDTR |= TIM_BDTR_MOE;
    }
}

void tim_main_output_disable(TIM_TypeDef *tim)
{
    if (tim == NULL) { return; }
    if (tim == TIM1 || tim == TIM8)
    {
        tim->BDTR &= ~TIM_BDTR_MOE;
    }
}

void tim_set_deadtime(TIM_TypeDef *tim, uint8_t deadtime)
{
    if (tim == NULL) { return; }
    if (tim == TIM1 || tim == TIM8)
    {
        tim->BDTR &= ~TIM_BDTR_DTG;
        tim->BDTR |= deadtime;
    }
}

#endif /* DRV_USING_TIM */
