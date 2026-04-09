#include "drv_fsmc.h"

#ifdef DRV_USING_FSMC

/* ==================== 内部辅助函数 ==================== */

static __IO uint32_t *fsmc_get_btcr(uint8_t bank)
{
    return &FSMC_Bank1->BTCR[(uint32_t)bank * 2U];
}

static __IO uint32_t *fsmc_get_bwtr(uint8_t bank)
{
    return &FSMC_Bank1E->BWTR[(uint32_t)bank];
}

/* ==================== 时钟使能 ==================== */

void fsmc_clk_enable(void)
{
    RCC->AHB3ENR |= RCC_AHB3ENR_FSMCEN;
    (void)RCC->AHB3ENR;
}

/* ==================== NOR/SRAM 初始化 ==================== */

void fsmc_norsram_init(const fsmc_norsram_config_t *cfg)
{
    __IO uint32_t *bcr;
    __IO uint32_t *btr;
    __IO uint32_t *bwtr;
    uint32_t reg;

    if (cfg == NULL) { return; }
    if (cfg->bank > 3U) { return; }

    fsmc_clk_enable();

    bcr  = fsmc_get_btcr(cfg->bank);
    btr  = fsmc_get_btcr(cfg->bank) + 1;
    bwtr = fsmc_get_bwtr(cfg->bank);

    reg = 0;

    if (cfg->address_data_mux) reg |= FSMC_BCR1_MUXEN;
    if (cfg->burst_enable)     reg |= FSMC_BCR1_BURSTEN;
    if (cfg->write_enable)     reg |= FSMC_BCR1_WREN;
    if (cfg->extended_mode)    reg |= FSMC_BCR1_EXTMOD;
    if (cfg->wait_enable)      reg |= FSMC_BCR1_WAITEN;

    if (cfg->data_width == 16U)      reg |= FSMC_BCR1_MWID_0;
    else if (cfg->data_width == 32U) reg |= FSMC_BCR1_MWID_1;

    reg |= FSMC_BCR1_MBKEN;
    *bcr = reg;

    *btr = ((uint32_t)(cfg->read_timing_addset & 0x0FU) << 0U) |
           ((uint32_t)(cfg->read_timing_datast & 0xFFU) << 8U);

    *bwtr = ((uint32_t)(cfg->write_timing_addset & 0x0FU) << 0U) |
            ((uint32_t)(cfg->write_timing_datast & 0xFFU) << 8U);
}

#endif /* DRV_USING_FSMC */
