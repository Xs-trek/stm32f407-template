#include "drv_syscfg.h"

#ifdef DRV_USING_SYSCFG

/* ==================== 时钟控制 ==================== */

void syscfg_clk_enable(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    (void)RCC->APB2ENR;
}

/* ==================== EXTI 端口映射 ==================== */

void syscfg_set_exti_port(uint8_t pin, uint8_t port_code)
{
    uint32_t index;
    uint32_t shift;

    if (pin > 15U) { return; }

    syscfg_clk_enable();

    index = ((uint32_t)pin >> 2U);
    shift = (((uint32_t)pin & 0x03U) << 2U);

    SYSCFG->EXTICR[index] &= ~(0x0FU << shift);
    SYSCFG->EXTICR[index] |= (((uint32_t)port_code & 0x0FU) << shift);
}

#endif /* DRV_USING_SYSCFG */
