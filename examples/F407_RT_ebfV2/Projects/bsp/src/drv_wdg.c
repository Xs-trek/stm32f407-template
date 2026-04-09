#include "drv_wdg.h"

#ifdef DRV_USING_IWDG

/* ==================== 内部函数 ==================== */

/* 等待预分频与重装载寄存器写入完成 */
static void iwdg_wait_ready(void)
{
    uint32_t timeout = 100000UL;

    while ((IWDG->SR & (IWDG_SR_PVU | IWDG_SR_RVU)) != 0U)
    {
        if (timeout == 0U) { return; }
        timeout--;
    }
}

/* ==================== 公共接口 ==================== */

/* 根据配置结构体初始化并启动 IWDG */
void iwdg_init(const iwdg_config_t *cfg)
{
    if (cfg == NULL)
    {
        return;
    }

    IWDG->KR  = 0x5555U;   /* 允许写入 PR / RLR */
    IWDG->PR  = (cfg->prescaler & 0x07U);
    IWDG->RLR = (cfg->reload & 0x0FFFU);

    iwdg_wait_ready();

    IWDG->KR = 0xAAAAU;    /* 重装载计数器 */
    IWDG->KR = 0xCCCCU;    /* 启动 IWDG */
}

/* 喂狗，重装载 IWDG 计数器 */
void iwdg_reload(void)
{
    IWDG->KR = 0xAAAAU;
}

#endif /* DRV_USING_IWDG */
