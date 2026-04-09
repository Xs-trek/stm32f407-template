#include "drv_dcmi.h"

#ifdef DRV_USING_DCMI

/* ==================== 时钟使能 ==================== */

void dcmi_clk_enable(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_DCMIEN;
    (void)RCC->AHB2ENR;
}

/* ==================== 初始化 ==================== */

void dcmi_init(const dcmi_config_t *cfg)
{
    uint32_t cr = 0;

    if (cfg == NULL) { return; }

    dcmi_clk_enable();

    cr |= ((uint32_t)(cfg->capture_rate & 0x03U) << 8U);
    cr |= ((uint32_t)(cfg->extended_data_mode & 0x03U) << 10U);

    if (cfg->hsync_polarity)         cr |= DCMI_CR_HSPOL;
    if (cfg->vsync_polarity)         cr |= DCMI_CR_VSPOL;
    if (cfg->pixel_clock_polarity)   cr |= DCMI_CR_PCKPOL;
    if (cfg->capture_mode_continuous) cr |= DCMI_CR_CM;

    DCMI->CR = cr;
}

/* ==================== 控制接口 ==================== */

void dcmi_start(void)
{
    DCMI->CR |= DCMI_CR_CAPTURE;
    DCMI->CR |= DCMI_CR_ENABLE;
}

void dcmi_stop(void)
{
    DCMI->CR &= ~DCMI_CR_CAPTURE;
    DCMI->CR &= ~DCMI_CR_ENABLE;
}

void dcmi_enable(void)
{
    DCMI->CR |= DCMI_CR_ENABLE;
}

void dcmi_disable(void)
{
    DCMI->CR &= ~DCMI_CR_ENABLE;
}

#endif /* DRV_USING_DCMI */
