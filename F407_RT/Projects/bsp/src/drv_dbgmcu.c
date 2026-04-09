#include "drv_dbgmcu.h"

#ifdef DRV_USING_DBGMCU

/* ==================== 设备标识 ==================== */

uint16_t dbgmcu_get_dev_id(void)
{
    return (uint16_t)(DBGMCU->IDCODE & 0x0FFFU);
}

uint16_t dbgmcu_get_rev_id(void)
{
    return (uint16_t)((DBGMCU->IDCODE >> 16U) & 0xFFFFU);
}

/* ==================== APB1 冻结 ==================== */

void dbgmcu_freeze_tim2(void)
{
    DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_TIM2_STOP;
}

void dbgmcu_freeze_tim3(void)
{
    DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_TIM3_STOP;
}

void dbgmcu_freeze_tim4(void)
{
    DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_TIM4_STOP;
}

void dbgmcu_freeze_tim5(void)
{
    DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_TIM5_STOP;
}

void dbgmcu_freeze_iwdg(void)
{
    DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_IWDG_STOP;
}

void dbgmcu_freeze_wwdg(void)
{
    DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_WWDG_STOP;
}

#endif /* DRV_USING_DBGMCU */
