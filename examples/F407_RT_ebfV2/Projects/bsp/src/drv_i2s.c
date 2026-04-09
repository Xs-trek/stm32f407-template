#include "drv_i2s.h"

#ifdef DRV_USING_I2S

#include "drv_rcc.h"

/* ==================== 内部辅助函数 ==================== */

static void i2s_calc_prescaler(uint32_t audio_freq, uint32_t *i2sdiv, uint32_t *odd)
{
    uint32_t i2s_clk;
    uint32_t total;

    i2s_clk = rcc_get_plli2s_clk();
    if (i2s_clk == 0U) { i2s_clk = 96000000U; }
    if (audio_freq == 0U) { audio_freq = 48000U; }

    total = i2s_clk / (audio_freq * 32U);
    if (total < 4U)   { total = 4U; }
    if (total > 511U) { total = 511U; }

    *i2sdiv = total / 2U;
    *odd    = total & 1U;
}

/* ==================== 初始化 / 反初始化 ==================== */

void i2s_init(const i2s_config_t *cfg)
{
    SPI_TypeDef *spi;
    uint32_t i2scfgr;
    uint32_t i2sdiv;
    uint32_t odd;
    uint32_t i2spr;

    if (cfg == NULL || cfg->instance == NULL) { return; }

    spi = cfg->instance;

    if (spi == SPI2)      { RCC->APB1ENR |= RCC_APB1ENR_SPI2EN; (void)RCC->APB1ENR; }
    else if (spi == SPI3) { RCC->APB1ENR |= RCC_APB1ENR_SPI3EN; (void)RCC->APB1ENR; }

    spi->I2SCFGR &= ~SPI_I2SCFGR_I2SE;

    i2scfgr = SPI_I2SCFGR_I2SMOD;
    i2scfgr |= ((uint32_t)(cfg->mode & 0x03U) << 8U);
    i2scfgr |= ((uint32_t)(cfg->standard & 0x03U) << 4U);
    i2scfgr |= ((uint32_t)(cfg->data_format & 0x03U) << 1U);
    if (cfg->cpol) { i2scfgr |= SPI_I2SCFGR_CKPOL; }

    i2s_calc_prescaler(cfg->audio_freq, &i2sdiv, &odd);

    i2spr = (i2sdiv & 0xFFU) | ((odd & 0x01U) << 8U);
    if (cfg->mclk_output) { i2spr |= SPI_I2SPR_MCKOE; }

    spi->I2SPR   = i2spr;
    spi->I2SCFGR = i2scfgr;
    spi->I2SCFGR |= SPI_I2SCFGR_I2SE;
}

void i2s_deinit(SPI_TypeDef *spi)
{
    if (spi == NULL) { return; }
    spi->I2SCFGR &= ~SPI_I2SCFGR_I2SE;
    spi->I2SCFGR = 0U;
    spi->I2SPR   = 0x0002U;  /* 复位值 */
}

/* ==================== 数据收发 ==================== */

void i2s_send(SPI_TypeDef *spi, uint16_t data)
{
    if (spi == NULL) { return; }
    while ((spi->SR & SPI_SR_TXE) == 0U) { }
    spi->DR = data;
}

uint16_t i2s_recv(SPI_TypeDef *spi)
{
    if (spi == NULL) { return 0U; }
    while ((spi->SR & SPI_SR_RXNE) == 0U) { }
    return (uint16_t)spi->DR;
}

/* ==================== DMA ==================== */

void i2s_enable_dma_tx(SPI_TypeDef *spi)
{
    if (spi == NULL) { return; }
    spi->CR2 |= SPI_CR2_TXDMAEN;
}

void i2s_enable_dma_rx(SPI_TypeDef *spi)
{
    if (spi == NULL) { return; }
    spi->CR2 |= SPI_CR2_RXDMAEN;
}

void i2s_disable_dma(SPI_TypeDef *spi)
{
    if (spi == NULL) { return; }
    spi->CR2 &= ~(SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);
}

#endif /* DRV_USING_I2S */
