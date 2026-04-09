#include "drv_sdio.h"

#ifdef DRV_USING_SDIO

/* ==================== 时钟 ==================== */

void sdio_clk_enable(void)
{
    /* 使能 SDIO 外设时钟 */
    RCC->APB2ENR |= RCC_APB2ENR_SDIOEN;
    /* 读回以确保写入生效 */
    (void)RCC->APB2ENR;
}

/* ==================== 初始化与配置 ==================== */

void sdio_init(const sdio_config_t *cfg)
{
    uint32_t clkcr = 0U;

    if (cfg == NULL)
    {
        return;
    }

    sdio_clk_enable();

    /* 设置时钟分频系数 */
    clkcr |= (cfg->clk_div & 0xFFU);

    /* 时钟边沿选择 */
    if (cfg->clk_edge_rising == 0U)
    {
        clkcr |= SDIO_CLKCR_NEGEDGE;
    }
    /* 旁路分频器 */
    if (cfg->clk_bypass)
    {
        clkcr |= SDIO_CLKCR_BYPASS;
    }
    /* 省电模式 */
    if (cfg->clk_power_save)
    {
        clkcr |= SDIO_CLKCR_PWRSAV;
    }
    /* 硬件流控制 */
    if (cfg->hw_flow_ctrl)
    {
        clkcr |= SDIO_CLKCR_HWFC_EN;
    }

    /* 总线宽度 WIDBUS */
    if (cfg->bus_wide == 4U)
    {
        clkcr |= (1U << 11U);   /* WIDBUS=01: 4 位 */
    }
    else if (cfg->bus_wide == 8U)
    {
        clkcr |= (2U << 11U);   /* WIDBUS=10: 8 位 */
    }

    SDIO->POWER = 0x03U;   /* 电源开启 */
    SDIO->CLKCR = clkcr;
}

void sdio_set_bus_width(uint8_t width)
{
    /* 清除 WIDBUS 位 */
    SDIO->CLKCR &= ~(3U << 11U);

    if (width == 4U)
    {
        SDIO->CLKCR |= (1U << 11U);
    }
    else if (width == 8U)
    {
        SDIO->CLKCR |= (2U << 11U);
    }
}

void sdio_power_on(void)
{
    SDIO->POWER = 0x03U;
}

void sdio_power_off(void)
{
    SDIO->POWER = 0x00U;
}

void sdio_enable_clk_output(void)
{
    SDIO->CLKCR |= SDIO_CLKCR_CLKEN;
}

void sdio_disable_clk_output(void)
{
    SDIO->CLKCR &= ~SDIO_CLKCR_CLKEN;
}

/* ==================== 命令 ==================== */

void sdio_send_cmd(uint8_t cmd_idx, uint32_t arg, sdio_resp_t resp, uint8_t wait_irq)
{
    SDIO->ARG = arg;
    SDIO->CMD = ((uint32_t)(cmd_idx & 0x3FU)) |
                ((uint32_t)(resp & 0x03U) << 6U) |
                ((uint32_t)(wait_irq ? 1U : 0U) << 8U) |
                SDIO_CMD_CPSMEN;
}

uint32_t sdio_get_resp1(void)
{
    return SDIO->RESP1;
}

uint32_t sdio_get_resp2(void)
{
    return SDIO->RESP2;
}

uint32_t sdio_get_resp3(void)
{
    return SDIO->RESP3;
}

uint32_t sdio_get_resp4(void)
{
    return SDIO->RESP4;
}

uint32_t sdio_get_cmd_status(void)
{
    return SDIO->STA & (SDIO_STA_CCRCFAIL |
                        SDIO_STA_CTIMEOUT |
                        SDIO_STA_CMDREND  |
                        SDIO_STA_CMDSENT);
}

/* ==================== 数据传输 ==================== */

void sdio_data_config(uint32_t data_len, uint32_t block_size, uint8_t dir_to_card, uint8_t stream_mode)
{
    uint32_t dctrl = 0U;

    SDIO->DTIMER = 0xFFFFFFFFU;
    SDIO->DLEN   = data_len;

    /* 块大小 */
    dctrl |= ((block_size & 0x0FU) << 4U);

    /* 传输方向：卡到控制器 */
    if (dir_to_card == 0U)
    {
        dctrl |= SDIO_DCTRL_DTDIR;
    }
    /* 流模式 */
    if (stream_mode)
    {
        dctrl |= SDIO_DCTRL_DTMODE;
    }

    /* 使能数据传输 */
    dctrl |= SDIO_DCTRL_DTEN;

    SDIO->DCTRL = dctrl;
}

uint32_t sdio_get_data_status(void)
{
    return SDIO->STA & (SDIO_STA_DCRCFAIL |
                        SDIO_STA_DTIMEOUT |
                        SDIO_STA_TXUNDERR |
                        SDIO_STA_RXOVERR  |
                        SDIO_STA_DATAEND  |
                        SDIO_STA_DBCKEND  |
                        SDIO_STA_STBITERR);
}

void sdio_clear_flags(uint32_t flags)
{
    SDIO->ICR = flags;
}

/* ==================== DMA ==================== */

void sdio_enable_dma(void)
{
    SDIO->DCTRL |= SDIO_DCTRL_DMAEN;
}

void sdio_disable_dma(void)
{
    SDIO->DCTRL &= ~SDIO_DCTRL_DMAEN;
}

/* ==================== 中断 ==================== */

void sdio_enable_irq(uint32_t mask)
{
    SDIO->MASK |= mask;
}

void sdio_disable_irq(uint32_t mask)
{
    SDIO->MASK &= ~mask;
}

#endif /* DRV_USING_SDIO */
