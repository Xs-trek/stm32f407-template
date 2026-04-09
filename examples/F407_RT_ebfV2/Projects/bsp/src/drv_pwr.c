#include "drv_pwr.h"

#ifdef DRV_USING_PWR

/* ==================== 时钟 / 备份域 ==================== */

void pwr_clk_enable(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    (void)RCC->APB1ENR;
}

void pwr_enable_backup_access(void)
{
    pwr_clk_enable();
    PWR->CR |= PWR_CR_DBP;
}

/* ==================== PVD ==================== */

void pwr_set_pvd_level(uint8_t level)
{
    pwr_clk_enable();
    PWR->CR &= ~PWR_CR_PLS;
    PWR->CR |= ((uint32_t)(level & 0x07U) << 5U);
}

void pwr_enable_pvd(void)
{
    pwr_clk_enable();
    PWR->CR |= PWR_CR_PVDE;
}

void pwr_disable_pvd(void)
{
    PWR->CR &= ~PWR_CR_PVDE;
}

uint8_t pwr_get_pvd_output(void)
{
    return (uint8_t)((PWR->CSR & PWR_CSR_PVDO) ? 1U : 0U);
}

/* ==================== 低功耗模式 ==================== */

void pwr_enter_sleep(void)
{
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
    __WFI();
}

void pwr_enter_stop(void)
{
    pwr_clk_enable();
    PWR->CR &= ~PWR_CR_PDDS;
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    __WFI();
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
}

void pwr_enter_standby(void)
{
    pwr_clk_enable();
    PWR->CR |= PWR_CR_PDDS;
    PWR->CR |= PWR_CR_CWUF;
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    __WFI();
}

/* ==================== 标志清除 ==================== */

void pwr_clear_wakeup_flag(void)
{
    PWR->CR |= PWR_CR_CWUF;
}

void pwr_clear_standby_flag(void)
{
    PWR->CR |= PWR_CR_CSBF;
}

#endif /* DRV_USING_PWR */
