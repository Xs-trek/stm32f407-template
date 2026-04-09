#include "drv_adc.h"

#ifdef DRV_USING_ADC

#include "drv_rcc.h"

/* 判断 ADC 实例是否合法 */
static uint8_t adc_is_valid_instance(ADC_TypeDef *adc)
{
    return (uint8_t)((adc == ADC1) || (adc == ADC2) || (adc == ADC3));
}

/* ==================== ADC 公共配置（共享 ADC_CCR） ==================== */

void adc_set_prescaler(adc_prescaler_t prescaler)
{
    ADC->CCR &= ~ADC_CCR_ADCPRE;
    ADC->CCR |= ((uint32_t)(prescaler & 0x03U) << 16U);
}

void adc_enable_temp_vrefint(void)
{
    ADC->CCR |= ADC_CCR_TSVREFE;
}

void adc_disable_temp_vrefint(void)
{
    ADC->CCR &= ~ADC_CCR_TSVREFE;
}

void adc_enable_vbat(void)
{
    ADC->CCR |= ADC_CCR_VBATE;
}

void adc_disable_vbat(void)
{
    ADC->CCR &= ~ADC_CCR_VBATE;
}

/* ==================== 时钟 ==================== */

void adc_clk_enable(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return; }

#ifdef DRV_USING_RCC
    rcc_enable_adc_clk(adc);
#else
    if (adc == ADC1)
    {
        RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    }
    else if (adc == ADC2)
    {
        RCC->APB2ENR |= RCC_APB2ENR_ADC2EN;
    }
    else
    {
        RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;
    }

    (void)RCC->APB2ENR;
#endif
}

/* ==================== 初始化 / 使能 / 关闭 ==================== */

void adc_init(const adc_config_t *cfg)
{
    ADC_TypeDef *adc;

    if (cfg == NULL) { return; }
    if (!adc_is_valid_instance(cfg->instance)) { return; }

    adc = cfg->instance;

    adc_clk_enable(adc);

    /* 关闭 ADC 后再重新配置 */
    adc->CR2 &= ~ADC_CR2_ADON;

    /* 重建基础寄存器配置 */
    adc->CR1   = 0U;
    adc->CR2   = 0U;
    adc->SMPR1 = 0U;
    adc->SMPR2 = 0U;
    adc->SQR1  = 0U;
    adc->SQR2  = 0U;
    adc->SQR3  = 0U;
    adc->JSQR  = 0U;

    /* 分辨率 */
    adc->CR1 |= ((uint32_t)(cfg->resolution & 0x03U) << 24U);

    /* 数据对齐 */
    if (cfg->align == ADC_ALIGN_LEFT)
    {
        adc->CR2 |= ADC_CR2_ALIGN;
    }

    /* 连续模式 */
    if (cfg->continuous)
    {
        adc->CR2 |= ADC_CR2_CONT;
    }

    /* 扫描模式 */
    if (cfg->scan)
    {
        adc->CR1 |= ADC_CR1_SCAN;
    }
}

void adc_enable(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return; }
    adc->CR2 |= ADC_CR2_ADON;
}

void adc_disable(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return; }
    adc->CR2 &= ~ADC_CR2_ADON;
}

/* ==================== 规则组基础配置 ==================== */

void adc_set_sample_time(ADC_TypeDef *adc, uint8_t channel, adc_sample_t sample)
{
    if (!adc_is_valid_instance(adc)) { return; }
    if (channel > ADC_CHANNEL_MAX) { return; }

    if (channel <= 9U)
    {
        uint32_t shift = ((uint32_t)channel * 3U);
        adc->SMPR2 &= ~(7U << shift);
        adc->SMPR2 |= ((uint32_t)(sample & 0x07U) << shift);
    }
    else
    {
        uint32_t shift = (((uint32_t)channel - 10U) * 3U);
        adc->SMPR1 &= ~(7U << shift);
        adc->SMPR1 |= ((uint32_t)(sample & 0x07U) << shift);
    }
}

void adc_set_regular_channel(ADC_TypeDef *adc, uint8_t rank, uint8_t channel)
{
    uint32_t shift;

    if (!adc_is_valid_instance(adc)) { return; }
    if (rank == 0U || rank > ADC_REGULAR_RANK_MAX) { return; }
    if (channel > ADC_CHANNEL_MAX) { return; }

    if (rank <= 6U)
    {
        shift = ((uint32_t)(rank - 1U) * 5U);
        adc->SQR3 &= ~(0x1FU << shift);
        adc->SQR3 |= ((uint32_t)channel << shift);
    }
    else if (rank <= 12U)
    {
        shift = ((uint32_t)(rank - 7U) * 5U);
        adc->SQR2 &= ~(0x1FU << shift);
        adc->SQR2 |= ((uint32_t)channel << shift);
    }
    else
    {
        shift = ((uint32_t)(rank - 13U) * 5U);
        adc->SQR1 &= ~(0x1FU << shift);
        adc->SQR1 |= ((uint32_t)channel << shift);
    }
}

void adc_set_regular_length(ADC_TypeDef *adc, uint8_t length)
{
    if (!adc_is_valid_instance(adc)) { return; }
    if (length == 0U || length > ADC_REGULAR_RANK_MAX) { return; }

    adc->SQR1 &= ~(0x0FU << 20U);
    adc->SQR1 |= ((uint32_t)(length - 1U) << 20U);
}

void adc_set_regular_trigger(ADC_TypeDef *adc, uint8_t extsel, adc_extrig_edge_t edge)
{
    if (!adc_is_valid_instance(adc)) { return; }

    adc->CR2 &= ~(ADC_CR2_EXTSEL | ADC_CR2_EXTEN);
    adc->CR2 |= ((uint32_t)(extsel & 0x0FU) << 24U);
    adc->CR2 |= ((uint32_t)(edge & 0x03U) << 28U);
}

void adc_set_eoc_selection(ADC_TypeDef *adc, uint8_t each_conversion)
{
    if (!adc_is_valid_instance(adc)) { return; }

    if (each_conversion)
    {
        adc->CR2 |= ADC_CR2_EOCS;
    }
    else
    {
        adc->CR2 &= ~ADC_CR2_EOCS;
    }
}

void adc_start(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return; }
    if ((adc->CR2 & ADC_CR2_ADON) == 0U) { return; }

    adc->CR2 |= ADC_CR2_SWSTART;
}

uint16_t adc_read_data(ADC_TypeDef *adc)
{
    uint32_t timeout = 1000000UL;

    if (!adc_is_valid_instance(adc)) { return 0U; }
    if ((adc->CR2 & ADC_CR2_ADON) == 0U) { return 0U; }

    while ((adc->SR & ADC_SR_EOC) == 0U)
    {
        if (timeout == 0U) { return 0U; }
        timeout--;
    }

    return (uint16_t)(adc->DR & 0xFFFFU);
}

uint16_t adc_read_channel(ADC_TypeDef *adc, uint8_t channel, adc_sample_t sample)
{
    uint32_t cr1;
    uint32_t cr2;
    uint32_t smpr1;
    uint32_t smpr2;
    uint32_t sqr1;
    uint32_t sqr2;
    uint32_t sqr3;
    uint16_t value;

    if (!adc_is_valid_instance(adc)) { return 0U; }
    if (channel > ADC_CHANNEL_MAX) { return 0U; }
    if ((adc->CR2 & ADC_CR2_ADON) == 0U) { return 0U; }

    /* 保存现场 */
    cr1   = adc->CR1;
    cr2   = adc->CR2;
    smpr1 = adc->SMPR1;
    smpr2 = adc->SMPR2;
    sqr1  = adc->SQR1;
    sqr2  = adc->SQR2;
    sqr3  = adc->SQR3;

    /* 清理旧状态，尽量保证本 helper 的行为闭合 */
    if ((adc->SR & ADC_SR_OVR) != 0U)
    {
        adc_clear_flag_ovr(adc);
    }

    if ((adc->SR & ADC_SR_EOC) != 0U)
    {
        (void)adc->DR;
    }

    /* 构造单通道、单次、软件触发、无 DMA 的规则组读取路径 */
    adc->CR1 &= ~ADC_CR1_SCAN;
    adc->CR2 &= ~(ADC_CR2_CONT | ADC_CR2_EXTEN | ADC_CR2_DMA | ADC_CR2_DDS);
    adc->CR2 |= ADC_CR2_EOCS;

    adc_set_sample_time(adc, channel, sample);
    adc_set_regular_length(adc, 1U);
    adc_set_regular_channel(adc, 1U, channel);

    adc_start(adc);
    value = adc_read_data(adc);

    /* 恢复现场 */
    adc->CR1   = cr1;
    adc->CR2   = cr2;
    adc->SMPR1 = smpr1;
    adc->SMPR2 = smpr2;
    adc->SQR1  = sqr1;
    adc->SQR2  = sqr2;
    adc->SQR3  = sqr3;

    return value;
}

/* ==================== DMA ==================== */

void adc_enable_dma(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return; }
    adc->CR2 |= ADC_CR2_DMA;
}

void adc_disable_dma(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return; }
    adc->CR2 &= ~ADC_CR2_DMA;
}

void adc_enable_dma_continuous(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return; }
    adc->CR2 |= ADC_CR2_DDS;
}

void adc_disable_dma_continuous(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return; }
    adc->CR2 &= ~ADC_CR2_DDS;
}

/* ==================== 中断 ==================== */

void adc_enable_eoc_irq(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return; }
    adc->CR1 |= ADC_CR1_EOCIE;
}

void adc_disable_eoc_irq(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return; }
    adc->CR1 &= ~ADC_CR1_EOCIE;
}

void adc_enable_ovr_irq(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return; }
    adc->CR1 |= ADC_CR1_OVRIE;
}

void adc_disable_ovr_irq(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return; }
    adc->CR1 &= ~ADC_CR1_OVRIE;
}

void adc_enable_awd_irq(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return; }
    adc->CR1 |= ADC_CR1_AWDIE;
}

void adc_disable_awd_irq(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return; }
    adc->CR1 &= ~ADC_CR1_AWDIE;
}

/* ==================== 状态标志 ==================== */

uint8_t adc_get_flag_eoc(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return 0U; }
    return (uint8_t)(((adc->SR & ADC_SR_EOC) != 0U) ? 1U : 0U);
}

uint8_t adc_get_flag_ovr(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return 0U; }
    return (uint8_t)(((adc->SR & ADC_SR_OVR) != 0U) ? 1U : 0U);
}

uint8_t adc_get_flag_awd(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return 0U; }
    return (uint8_t)(((adc->SR & ADC_SR_AWD) != 0U) ? 1U : 0U);
}

void adc_clear_flag_eoc(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return; }
    adc->SR &= ~ADC_SR_EOC;
}

void adc_clear_flag_ovr(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return; }
    adc->SR &= ~ADC_SR_OVR;
}

void adc_clear_flag_awd(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return; }
    adc->SR &= ~ADC_SR_AWD;
}

/* ==================== 模拟看门狗（规则组单通道子集） ==================== */

void adc_set_analog_watchdog_channel(ADC_TypeDef *adc, uint8_t channel)
{
    if (!adc_is_valid_instance(adc)) { return; }
    if (channel > ADC_CHANNEL_MAX) { return; }

    adc->CR1 &= ~ADC_CR1_AWDCH;
    adc->CR1 |= ((uint32_t)channel & 0x1FU);
}

void adc_set_analog_watchdog_threshold(ADC_TypeDef *adc, uint16_t low, uint16_t high)
{
    if (!adc_is_valid_instance(adc)) { return; }

    adc->LTR = (low & 0x0FFFU);
    adc->HTR = (high & 0x0FFFU);
}

void adc_enable_analog_watchdog(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return; }

    /* 当前版本只支持：
     * 规则组 + 单通道模拟看门狗
     */
    adc->CR1 &= ~ADC_CR1_JAWDEN;
    adc->CR1 |= (ADC_CR1_AWDSGL | ADC_CR1_AWDEN);
}

void adc_disable_analog_watchdog(ADC_TypeDef *adc)
{
    if (!adc_is_valid_instance(adc)) { return; }
    adc->CR1 &= ~(ADC_CR1_AWDSGL | ADC_CR1_AWDEN | ADC_CR1_JAWDEN);
}

#endif /* DRV_USING_ADC */
