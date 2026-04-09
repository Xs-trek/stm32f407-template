#include "drv_wwdg.h"

#ifdef DRV_USING_WWDG

/* ==================== 公共接口 ==================== */

/* 使能 WWDG 外设的 APB1 总线时钟 */
void wwdg_clk_enable(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_WWDGEN;
    (void)RCC->APB1ENR;    /* 回读确保时钟已生效 */
}

/* 根据配置结构体初始化并启动 WWDG */
void wwdg_init(const wwdg_config_t *cfg)
{
    if (cfg == NULL)
    {
        return;
    }

    wwdg_clk_enable();

    WWDG->CFR = ((uint32_t)(cfg->window & 0x7FU)) |
                ((uint32_t)(cfg->prescaler & 0x03U) << 7U);

    if (cfg->ewi_enable)
    {
        WWDG->CFR |= WWDG_CFR_EWI;
    }

    WWDG->CR = WWDG_CR_WDGA | (uint32_t)(cfg->counter & 0x7FU);
}

/* 喂狗，刷新 WWDG 计数器 */
void wwdg_refresh(uint8_t counter)
{
    WWDG->CR = WWDG_CR_WDGA | (uint32_t)(counter & 0x7FU);
}

/* 清除早期唤醒中断 (EWI) 标志位 */
void wwdg_clear_ewi_flag(void)
{
    WWDG->SR = 0U;
}

#endif /* DRV_USING_WWDG */
