#include "drv_spi.h"

#ifdef DRV_USING_SPI

#include "drv_rcc.h"
#include "drv_gpio.h"

/* ==================== 时钟接口 ==================== */

void spi_clk_enable(SPI_TypeDef *spi)
{
    if (spi == NULL)
    {
        return;
    }

#ifdef DRV_USING_RCC
    rcc_enable_spi_clk(spi);
#else
    if (spi == SPI1)
    {
        RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    }
    else if (spi == SPI2)
    {
        RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    }
    else if (spi == SPI3)
    {
        RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
    }
    else
    {
        return;
    }

    /* 读回寄存器以确保写入生效 */
    (void)RCC->APB2ENR;
    (void)RCC->APB1ENR;
#endif
}

/* ==================== 初始化与使能 ==================== */

void spi_init(spi_dev_t *dev)
{
    uint32_t cr1 = 0U;

    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }

    /* 使能 SPI 外设时钟 */
    spi_clk_enable(dev->instance);

    /* 若配置了片选引脚，初始化为推挽输出并拉高 */
    if (dev->cs_port != NULL && dev->cs_pin <= 15U)
    {
        gpio_init_output_pp(dev->cs_port, dev->cs_pin);
        spi_cs_high(dev);
    }

    /* 先关闭 SPI */
    dev->instance->CR1 &= ~SPI_CR1_SPE;

    /* 配置为主机模式 */
    cr1 |= SPI_CR1_MSTR;

    /* 波特率分频 */
    cr1 |= ((uint32_t)(dev->prescaler & 0x07U) << 3U);

    /* 时钟极性 */
    if (dev->cpol)
    {
        cr1 |= SPI_CR1_CPOL;
    }
    /* 时钟相位 */
    if (dev->cpha)
    {
        cr1 |= SPI_CR1_CPHA;
    }
    /* 16 位数据帧 */
    if (dev->data_16bit)
    {
        cr1 |= SPI_CR1_DFF;
    }
    /* 低位先发 */
    if (dev->lsb_first)
    {
        cr1 |= SPI_CR1_LSBFIRST;
    }

    /* 软件 NSS 管理 */
    cr1 |= SPI_CR1_SSM;
    cr1 |= SPI_CR1_SSI;

    dev->instance->CR1 = cr1;
    dev->instance->CR2 = 0U;

    /* 使能 SPI */
    dev->instance->CR1 |= SPI_CR1_SPE;
}

void spi_deinit(spi_dev_t *dev)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }
    dev->instance->CR1 &= ~SPI_CR1_SPE;
    dev->instance->CR1 = 0U;
    dev->instance->CR2 = 0U;
}

void spi_enable(spi_dev_t *dev)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }
    dev->instance->CR1 |= SPI_CR1_SPE;
}

void spi_disable(spi_dev_t *dev)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }
    dev->instance->CR1 &= ~SPI_CR1_SPE;
}

/* ==================== 阻塞式收发 ==================== */

void spi_wait_idle(spi_dev_t *dev)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }
    while ((dev->instance->SR & SPI_SR_BSY) != 0U)
    {
        /* 等待总线空闲 */
    }
}

uint8_t spi_txrx_byte(spi_dev_t *dev, uint8_t tx)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return 0U;
    }

    /* 等待发送缓冲区空 */
    while ((dev->instance->SR & SPI_SR_TXE) == 0U)
    {
        /* 等待 */
    }
    *((__IO uint8_t *)&dev->instance->DR) = tx;

    /* 等待接收缓冲区非空 */
    while ((dev->instance->SR & SPI_SR_RXNE) == 0U)
    {
        /* 等待 */
    }
    return *((__IO uint8_t *)&dev->instance->DR);
}

uint16_t spi_txrx_halfword(spi_dev_t *dev, uint16_t tx)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return 0U;
    }

    /* 等待发送缓冲区空 */
    while ((dev->instance->SR & SPI_SR_TXE) == 0U)
    {
        /* 等待 */
    }
    dev->instance->DR = tx;

    /* 等待接收缓冲区非空 */
    while ((dev->instance->SR & SPI_SR_RXNE) == 0U)
    {
        /* 等待 */
    }
    return (uint16_t)dev->instance->DR;
}

void spi_tx_buffer(spi_dev_t *dev, const uint8_t *buf, uint32_t len)
{
    uint32_t i;

    if (dev == NULL || dev->instance == NULL || buf == NULL)
    {
        return;
    }

    for (i = 0U; i < len; i++)
    {
        (void)spi_txrx_byte(dev, buf[i]);
    }
}

void spi_rx_buffer(spi_dev_t *dev, uint8_t *buf, uint32_t len)
{
    uint32_t i;

    if (dev == NULL || dev->instance == NULL || buf == NULL)
    {
        return;
    }

    for (i = 0U; i < len; i++)
    {
        buf[i] = spi_txrx_byte(dev, 0xFFU);
    }
}

void spi_txrx_buffer(spi_dev_t *dev, const uint8_t *tx, uint8_t *rx, uint32_t len)
{
    uint32_t i;
    uint8_t tx_data;
    uint8_t rx_data;

    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }

    for (i = 0U; i < len; i++)
    {
        tx_data = (tx != NULL) ? tx[i] : 0xFFU;
        rx_data = spi_txrx_byte(dev, tx_data);
        if (rx != NULL)
        {
            rx[i] = rx_data;
        }
    }
}

void spi_transfer_frame(spi_dev_t *dev,
                         const uint8_t *tx_buf, uint32_t tx_len,
                         uint8_t *rx_buf, uint32_t rx_len)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }

    spi_cs_low(dev);

    /* 先发送 */
    if (tx_buf != NULL && tx_len > 0U)
    {
        spi_tx_buffer(dev, tx_buf, tx_len);
    }

    /* 再接收 */
    if (rx_buf != NULL && rx_len > 0U)
    {
        spi_rx_buffer(dev, rx_buf, rx_len);
    }

    /* 等待总线空闲后释放片选 */
    spi_wait_idle(dev);
    spi_cs_high(dev);
}

/* ==================== 中断控制 ==================== */

void spi_enable_rxne_irq(spi_dev_t *dev)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }
    dev->instance->CR2 |= SPI_CR2_RXNEIE;
}

void spi_disable_rxne_irq(spi_dev_t *dev)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }
    dev->instance->CR2 &= ~SPI_CR2_RXNEIE;
}

void spi_enable_txe_irq(spi_dev_t *dev)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }
    dev->instance->CR2 |= SPI_CR2_TXEIE;
}

void spi_disable_txe_irq(spi_dev_t *dev)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }
    dev->instance->CR2 &= ~SPI_CR2_TXEIE;
}

void spi_enable_err_irq(spi_dev_t *dev)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }
    dev->instance->CR2 |= SPI_CR2_ERRIE;
}

void spi_disable_err_irq(spi_dev_t *dev)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }
    dev->instance->CR2 &= ~SPI_CR2_ERRIE;
}

/* ==================== DMA ==================== */

#ifdef DRV_USING_DMA
void spi_enable_dma(spi_dev_t *dev, uint8_t tx_en, uint8_t rx_en)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }
    if (tx_en)
    {
        dev->instance->CR2 |= SPI_CR2_TXDMAEN;
    }
    if (rx_en)
    {
        dev->instance->CR2 |= SPI_CR2_RXDMAEN;
    }
}

void spi_disable_dma(spi_dev_t *dev)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }
    dev->instance->CR2 &= ~(SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);
}
#endif

/* ==================== 配置辅助 ==================== */

void spi_set_prescaler(spi_dev_t *dev, uint8_t prescaler)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }
    /* 先禁用 SPI，修改分频后再使能 */
    dev->instance->CR1 &= ~SPI_CR1_SPE;
    dev->instance->CR1 &= ~(7U << 3U);
    dev->instance->CR1 |= ((uint32_t)(prescaler & 0x07U) << 3U);
    dev->instance->CR1 |= SPI_CR1_SPE;
    dev->prescaler = (prescaler & 0x07U);
}

void spi_enable_crc(spi_dev_t *dev, uint16_t polynomial)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }
    /* 先禁用 SPI，设置 CRC 多项式后使能 */
    dev->instance->CR1 &= ~SPI_CR1_SPE;
    dev->instance->CRCPR = polynomial;
    dev->instance->CR1 |= SPI_CR1_CRCEN;
    dev->instance->CR1 |= SPI_CR1_SPE;
}

void spi_disable_crc(spi_dev_t *dev)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }
    dev->instance->CR1 &= ~SPI_CR1_CRCEN;
}

void spi_enable_ti_mode(spi_dev_t *dev)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }
    dev->instance->CR2 |= SPI_CR2_FRF;
}

void spi_disable_ti_mode(spi_dev_t *dev)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }
    dev->instance->CR2 &= ~SPI_CR2_FRF;
}

void spi_set_software_nss(spi_dev_t *dev, uint8_t enable)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }
    if (enable)
    {
        dev->instance->CR1 |= (SPI_CR1_SSM | SPI_CR1_SSI);
    }
    else
    {
        dev->instance->CR1 &= ~(SPI_CR1_SSM | SPI_CR1_SSI);
    }
}

void spi_enable_bidirectional_tx(spi_dev_t *dev)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }
    dev->instance->CR1 |= SPI_CR1_BIDIMODE;
    dev->instance->CR1 |= SPI_CR1_BIDIOE;
}

void spi_enable_bidirectional_rx(spi_dev_t *dev)
{
    if (dev == NULL || dev->instance == NULL)
    {
        return;
    }
    dev->instance->CR1 |= SPI_CR1_BIDIMODE;
    dev->instance->CR1 &= ~SPI_CR1_BIDIOE;
}

#endif /* DRV_USING_SPI */
