#include "drv_eth.h"

#ifdef DRV_USING_ETH

/* ==================== 内部辅助函数 ==================== */

static void eth_mii_wait_busy(void)
{
    while (ETH->MACMIIAR & ETH_MACMIIAR_MB)
    {
    }
}

/* ==================== 时钟控制 ==================== */

void eth_clk_enable(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_ETHMACEN |
                    RCC_AHB1ENR_ETHMACTXEN |
                    RCC_AHB1ENR_ETHMACRXEN;
    (void)RCC->AHB1ENR;
}

/* ==================== 初始化 ==================== */

void eth_init(const eth_config_t *cfg)
{
    if (cfg == NULL) { return; }

    eth_clk_enable();

    /* 软件复位 DMA */
    ETH->DMABMR |= ETH_DMABMR_SR;
    while (ETH->DMABMR & ETH_DMABMR_SR)
    {
    }

    ETH->MACCR  = 0;
    ETH->MACFFR = 0;
    ETH->DMAOMR = 0;
    ETH->DMAIER = 0;

    if (cfg->full_duplex)      ETH->MACCR  |= ETH_MACCR_DM;
    if (cfg->speed_100m)       ETH->MACCR  |= ETH_MACCR_FES;
    if (cfg->checksum_offload) ETH->MACCR  |= ETH_MACCR_IPCO;
    if (cfg->broadcast_enable == 0U) ETH->MACFFR |= ETH_MACFFR_BFD;
    if (cfg->promiscuous_mode) ETH->MACFFR |= ETH_MACFFR_PM;

    ETH->DMAOMR |= ETH_DMAOMR_RSF | ETH_DMAOMR_TSF;

    if (cfg->rx_interrupt) ETH->DMAIER |= ETH_DMAIER_RIE | ETH_DMAIER_NISE;
    if (cfg->tx_interrupt) ETH->DMAIER |= ETH_DMAIER_TIE | ETH_DMAIER_NISE;
}

/* ==================== 启动 / 停止 ==================== */

void eth_start(void)
{
    ETH->MACCR  |= ETH_MACCR_RE | ETH_MACCR_TE;
    ETH->DMAOMR |= ETH_DMAOMR_SR | ETH_DMAOMR_ST;
}

void eth_stop(void)
{
    ETH->DMAOMR &= ~(ETH_DMAOMR_SR | ETH_DMAOMR_ST);
    ETH->MACCR  &= ~(ETH_MACCR_RE | ETH_MACCR_TE);
}

/* ==================== MAC 地址 ==================== */

void eth_set_mac_addr(uint8_t index, const uint8_t mac[6])
{
    __IO uint32_t *high;
    __IO uint32_t *low;

    if (mac == NULL) { return; }
    if (index > 3U) index = 3U;

    if (index == 0U)
    {
        high = &ETH->MACA0HR;
        low  = &ETH->MACA0LR;
    }
    else if (index == 1U)
    {
        high = &ETH->MACA1HR;
        low  = &ETH->MACA1LR;
    }
    else if (index == 2U)
    {
        high = &ETH->MACA2HR;
        low  = &ETH->MACA2LR;
    }
    else
    {
        high = &ETH->MACA3HR;
        low  = &ETH->MACA3LR;
    }

    *high = ((uint32_t)mac[5] << 8U) | ((uint32_t)mac[4] << 0U);
    *low  = ((uint32_t)mac[3] << 24U) |
            ((uint32_t)mac[2] << 16U) |
            ((uint32_t)mac[1] << 8U)  |
            ((uint32_t)mac[0] << 0U);
}

/* ==================== MII PHY 读写 ==================== */

uint16_t eth_mii_read(uint8_t phy_addr, uint8_t reg_addr)
{
    eth_mii_wait_busy();

    ETH->MACMIIAR = ((uint32_t)(phy_addr & 0x1FU) << 11U) |
                    ((uint32_t)(reg_addr & 0x1FU) << 6U)  |
                    ETH_MACMIIAR_CR_Div102 |
                    ETH_MACMIIAR_MB;

    eth_mii_wait_busy();

    return (uint16_t)(ETH->MACMIIDR & 0xFFFFU);
}

void eth_mii_write(uint8_t phy_addr, uint8_t reg_addr, uint16_t value)
{
    eth_mii_wait_busy();

    ETH->MACMIIDR = value;
    ETH->MACMIIAR = ((uint32_t)(phy_addr & 0x1FU) << 11U) |
                    ((uint32_t)(reg_addr & 0x1FU) << 6U)  |
                    ETH_MACMIIAR_CR_Div102 |
                    ETH_MACMIIAR_MW |
                    ETH_MACMIIAR_MB;

    eth_mii_wait_busy();
}

/* ==================== DMA 中断控制 ==================== */

void eth_resume_rx(void)
{
    ETH->DMARPDR = 0;
}

void eth_resume_tx(void)
{
    ETH->DMATPDR = 0;
}

void eth_enable_rx_irq(void)
{
    ETH->DMAIER |= ETH_DMAIER_RIE | ETH_DMAIER_NISE;
}

void eth_enable_tx_irq(void)
{
    ETH->DMAIER |= ETH_DMAIER_TIE | ETH_DMAIER_NISE;
}

uint32_t eth_get_dma_status(void)
{
    return ETH->DMASR;
}

void eth_clear_dma_status(uint32_t flags)
{
    ETH->DMASR = flags;
}

#endif /* DRV_USING_ETH */
