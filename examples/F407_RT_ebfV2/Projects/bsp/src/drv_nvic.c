#include "drv_nvic.h"

#ifdef DRV_USING_NVIC

/* ==================== 优先级分组 ==================== */

void nvic_set_priority_grouping(uint32_t group)
{
    NVIC_SetPriorityGrouping(group);
}

/* ==================== 优先级设置 ==================== */

void nvic_set_priority(IRQn_Type irqn, uint32_t preempt_prio, uint32_t sub_prio)
{
    uint32_t priority_group;
    uint32_t encoded;

    priority_group = NVIC_GetPriorityGrouping();
    encoded = NVIC_EncodePriority(priority_group, preempt_prio, sub_prio);
    NVIC_SetPriority(irqn, encoded);
}

/* ==================== 使能 / 禁用 ==================== */

void nvic_enable_irq(IRQn_Type irqn)
{
    NVIC_EnableIRQ(irqn);
}

void nvic_disable_irq(IRQn_Type irqn)
{
    NVIC_DisableIRQ(irqn);
}

/* ==================== 挂起状态 ==================== */

void nvic_clear_pending(IRQn_Type irqn)
{
    NVIC_ClearPendingIRQ(irqn);
}

void nvic_set_pending(IRQn_Type irqn)
{
    NVIC_SetPendingIRQ(irqn);
}

/* ==================== 活动状态 ==================== */

uint32_t nvic_get_active(IRQn_Type irqn)
{
    return NVIC_GetActive(irqn);
}

#endif /* DRV_USING_NVIC */
