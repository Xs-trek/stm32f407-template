#include "drv_rcc.h"

#ifdef DRV_USING_RCC

/* ==================== 内部辅助函数 ==================== */

static uint32_t rcc_get_hse_hz(void)
{
    return (uint32_t)(F407_RT_HSE_MHZ * 1000000U);
}

static const uint8_t ahb_presc_table[16] = {0,0,0,0,0,0,0,0,1,2,3,4,6,7,8,9};
static const uint8_t apb_presc_table[8]  = {0,0,0,0,1,2,3,4};

static uint32_t rcc_ahb_presc_shift(uint32_t hpre)
{
    return ahb_presc_table[hpre & 0x0FU];
}

static uint32_t rcc_apb_presc_shift(uint32_t ppre)
{
    return apb_presc_table[ppre & 0x07U];
}

/* ==================== 外设时钟使能 ==================== */

void rcc_enable_gpio_clk(GPIO_TypeDef *port)
{
    if (port == GPIOA)      { RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; }
    else if (port == GPIOB) { RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; }
    else if (port == GPIOC) { RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; }
    else if (port == GPIOD) { RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; }
    else if (port == GPIOE) { RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; }
    else if (port == GPIOF) { RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN; }
    else if (port == GPIOG) { RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN; }
    else if (port == GPIOH) { RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN; }
    else if (port == GPIOI) { RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN; }
    else { return; }

    (void)RCC->AHB1ENR;
}

void rcc_enable_dma_clk(DMA_TypeDef *dma)
{
    if (dma == DMA1)      { RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN; }
    else if (dma == DMA2) { RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN; }
    else { return; }

    (void)RCC->AHB1ENR;
}

void rcc_enable_spi_clk(SPI_TypeDef *spi)
{
    if (spi == SPI1)      { RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; }
    else if (spi == SPI2) { RCC->APB1ENR |= RCC_APB1ENR_SPI2EN; }
    else if (spi == SPI3) { RCC->APB1ENR |= RCC_APB1ENR_SPI3EN; }
    else { return; }

    (void)RCC->APB2ENR;
    (void)RCC->APB1ENR;
}

void rcc_enable_tim_clk(TIM_TypeDef *tim)
{
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

    (void)RCC->APB2ENR;
    (void)RCC->APB1ENR;
}

void rcc_enable_usart_clk(USART_TypeDef *usart)
{
    if (usart == USART1)      { RCC->APB2ENR |= RCC_APB2ENR_USART1EN; }
    else if (usart == USART2) { RCC->APB1ENR |= RCC_APB1ENR_USART2EN; }
    else if (usart == USART3) { RCC->APB1ENR |= RCC_APB1ENR_USART3EN; }
    else if (usart == UART4)  { RCC->APB1ENR |= RCC_APB1ENR_UART4EN; }
    else if (usart == UART5)  { RCC->APB1ENR |= RCC_APB1ENR_UART5EN; }
    else if (usart == USART6) { RCC->APB2ENR |= RCC_APB2ENR_USART6EN; }
    else { return; }

    (void)RCC->APB2ENR;
    (void)RCC->APB1ENR;
}

void rcc_enable_i2c_clk(I2C_TypeDef *i2c)
{
    if (i2c == I2C1)      { RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; }
    else if (i2c == I2C2) { RCC->APB1ENR |= RCC_APB1ENR_I2C2EN; }
    else if (i2c == I2C3) { RCC->APB1ENR |= RCC_APB1ENR_I2C3EN; }
    else { return; }

    (void)RCC->APB1ENR;
}

void rcc_enable_adc_clk(ADC_TypeDef *adc)
{
    if (adc == ADC1)      { RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; }
    else if (adc == ADC2) { RCC->APB2ENR |= RCC_APB2ENR_ADC2EN; }
    else if (adc == ADC3) { RCC->APB2ENR |= RCC_APB2ENR_ADC3EN; }
    else { return; }

    (void)RCC->APB2ENR;
}

void rcc_enable_can_clk(CAN_TypeDef *can)
{
    if (can == CAN1)      { RCC->APB1ENR |= RCC_APB1ENR_CAN1EN; }
    else if (can == CAN2) { RCC->APB1ENR |= RCC_APB1ENR_CAN2EN; }
    else { return; }

    (void)RCC->APB1ENR;
}

/* ==================== 时钟频率查询 ==================== */

uint32_t rcc_get_sysclk(void)
{
    uint32_t sws;
    uint32_t pllsrc;
    uint32_t pllm;
    uint32_t plln;
    uint32_t pllp;
    uint32_t vco_in;
    uint32_t vco_out;

    sws = (RCC->CFGR >> 2U) & 0x03U;

    if (sws == 0U) { return 16000000U; }
    if (sws == 1U) { return rcc_get_hse_hz(); }

    pllsrc = (RCC->PLLCFGR >> 22U) & 0x01U;
    pllm   = RCC->PLLCFGR & 0x3FU;
    plln   = (RCC->PLLCFGR >> 6U) & 0x1FFU;
    pllp   = ((((RCC->PLLCFGR >> 16U) & 0x03U) + 1U) * 2U);

    if (pllm == 0U) { return 0U; }

    vco_in  = (pllsrc ? rcc_get_hse_hz() : 16000000U) / pllm;
    vco_out = vco_in * plln;

    return vco_out / pllp;
}

uint32_t rcc_get_hclk(void)
{
    uint32_t sysclk = rcc_get_sysclk();
    uint32_t hpre   = (RCC->CFGR >> 4U) & 0x0FU;

    return sysclk >> rcc_ahb_presc_shift(hpre);
}

uint32_t rcc_get_pclk1(void)
{
    uint32_t hclk  = rcc_get_hclk();
    uint32_t ppre1 = (RCC->CFGR >> 10U) & 0x07U;

    return hclk >> rcc_apb_presc_shift(ppre1);
}

uint32_t rcc_get_pclk2(void)
{
    uint32_t hclk  = rcc_get_hclk();
    uint32_t ppre2 = (RCC->CFGR >> 13U) & 0x07U;

    return hclk >> rcc_apb_presc_shift(ppre2);
}

uint32_t rcc_get_timclk1(void)
{
    uint32_t pclk1 = rcc_get_pclk1();
    uint32_t ppre1 = (RCC->CFGR >> 10U) & 0x07U;

    if (rcc_apb_presc_shift(ppre1) == 0U) { return pclk1; }
    return (pclk1 << 1U);
}

uint32_t rcc_get_timclk2(void)
{
    uint32_t pclk2 = rcc_get_pclk2();
    uint32_t ppre2 = (RCC->CFGR >> 13U) & 0x07U;

    if (rcc_apb_presc_shift(ppre2) == 0U) { return pclk2; }
    return (pclk2 << 1U);
}

uint32_t rcc_get_plli2s_clk(void)
{
    uint32_t plli2ssrc;
    uint32_t plli2sm;
    uint32_t plli2sn;
    uint32_t plli2sr;
    uint32_t vco_in;
    uint32_t vco_out;

    /* PLLI2S 与主 PLL 共享 PLLM 和时钟源 */
    plli2ssrc = (RCC->PLLCFGR >> 22U) & 0x01U;
    plli2sm   = RCC->PLLCFGR & 0x3FU;
    plli2sn   = (RCC->PLLI2SCFGR >>  6U) & 0x1FFU;
    plli2sr   = (RCC->PLLI2SCFGR >> 28U) & 0x07U;

    if ((plli2sm == 0U) || (plli2sr == 0U)) { return 0U; }

    vco_in  = (plli2ssrc ? rcc_get_hse_hz() : 16000000U) / plli2sm;
    vco_out = vco_in * plli2sn;

    return vco_out / plli2sr;
}

/* ==================== 复位标志 ==================== */

uint32_t rcc_get_reset_flags(void)
{
    return RCC->CSR & (RCC_CSR_LPWRRSTF |
                       RCC_CSR_WWDGRSTF |
                       RCC_CSR_WDGRSTF  |
                       RCC_CSR_SFTRSTF  |
                       RCC_CSR_PORRSTF  |
                       RCC_CSR_PADRSTF  |
                       RCC_CSR_BORRSTF);
}

void rcc_clear_reset_flags(void)
{
    RCC->CSR |= RCC_CSR_RMVF;
}

#endif /* DRV_USING_RCC */
