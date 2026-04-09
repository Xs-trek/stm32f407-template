#include "drv_systick.h"

#ifdef DRV_USING_SYSTICK

/* ==================== 初始化 ==================== */

uint32_t systick_init(uint32_t ticks)
{
    return SysTick_Config(ticks);
}

/* ==================== 时钟源 ==================== */

void systick_set_clock_source(uint32_t source)
{
    if (source)
    {
        SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
    }
    else
    {
        SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;
    }
}

/* ==================== 使能 / 禁用 ==================== */

void systick_enable(void)
{
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void systick_disable(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

void systick_enable_irq(void)
{
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}

void systick_disable_irq(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}

/* ==================== 状态 ==================== */

uint32_t systick_get_current(void)
{
    return SysTick->VAL;
}

uint32_t systick_get_countflag(void)
{
    return (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk);
}

#endif /* DRV_USING_SYSTICK */
