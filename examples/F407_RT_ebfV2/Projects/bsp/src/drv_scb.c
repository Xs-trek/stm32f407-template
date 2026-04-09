#include "drv_scb.h"

#ifdef DRV_USING_SCB

/* ==================== 系统复位 ==================== */

void scb_system_reset(void)
{
    NVIC_SystemReset();
}

/* ==================== 睡眠控制 ==================== */

void scb_set_sleepdeep(uint8_t enable)
{
    if (enable)
    {
        SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    }
    else
    {
        SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
    }
}

void scb_set_sleeponexit(uint8_t enable)
{
    if (enable)
    {
        SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
    }
    else
    {
        SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
    }
}

/* ==================== 向量表 ==================== */

void scb_set_vector_table(uint32_t base, uint32_t offset)
{
    SCB->VTOR = base | (offset & 0x1FFFFF80U);
}

/* ==================== 状态读取 ==================== */

uint32_t scb_get_icsr(void)
{
    return SCB->ICSR;
}

uint32_t scb_get_aircr(void)
{
    return SCB->AIRCR;
}

#endif /* DRV_USING_SCB */
