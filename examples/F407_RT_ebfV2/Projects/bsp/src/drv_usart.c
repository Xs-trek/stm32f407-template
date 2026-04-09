#include "drv_usart.h"

#ifdef DRV_USING_USART

#include "drv_rcc.h"

/* ==================== 内部：APB 总线时钟计算 ==================== */

/* AHB 预分频查找表 */
static const uint8_t usart_hpre_lut[16] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 2, 3, 4, 6, 7, 8, 9
};

/* APB 预分频查找表 */
static const uint8_t usart_ppre_lut[8] = {
    0, 0, 0, 0,
    1, 2, 3, 4
};

/* 根据 APB 总线索引获取外设时钟频率 */
static uint32_t usart_get_pclk(uint8_t apb_idx)
{
    uint32_t hclk;
    uint32_t ppre;

    hclk = SystemCoreClock >> usart_hpre_lut[(RCC->CFGR >> 4U) & 0x0FU];

    if (apb_idx == 1U)
    {
        ppre = usart_ppre_lut[(RCC->CFGR >> 10U) & 0x07U];
    }
    else
    {
        ppre = usart_ppre_lut[(RCC->CFGR >> 13U) & 0x07U];
    }

    return hclk >> ppre;
}

/* 判断 USART 外设是否挂载在 APB2 总线上 */
static int usart_is_apb2(USART_TypeDef *usart)
{
    return (usart == USART1) || (usart == USART6);
}

/* 根据外设时钟、波特率和过采样模式计算 BRR 寄存器值 */
static uint32_t usart_calc_brr(uint32_t pclk, uint32_t baud, uint8_t over8)
{
    uint32_t div;
    uint32_t mantissa;
    uint32_t fraction;

    if (baud == 0U || pclk == 0U) { return 0U; }

    if (over8)
    {
        div = (25U * pclk) / (2U * baud);
        mantissa = div / 100U;
        fraction = (((div - (mantissa * 100U)) * 8U) + 50U) / 100U;
        if (fraction > 7U) { mantissa += 1U; fraction = 0U; }
        return (mantissa << 4U) | (fraction & 0x07U);
    }

    div = (25U * pclk) / (4U * baud);
    mantissa = div / 100U;
    fraction = (((div - (mantissa * 100U)) * 16U) + 50U) / 100U;
    if (fraction > 15U) { mantissa += 1U; fraction = 0U; }
    return (mantissa << 4U) | (fraction & 0x0FU);
}

/* ==================== 时钟操作 ==================== */

void usart_clk_enable(USART_TypeDef *usart)
{
    if (usart == NULL) { return; }

#ifdef DRV_USING_RCC
    rcc_enable_usart_clk(usart);
#else
    if      (usart == USART1) { RCC->APB2ENR |= RCC_APB2ENR_USART1EN; }
    else if (usart == USART2) { RCC->APB1ENR |= RCC_APB1ENR_USART2EN; }
    else if (usart == USART3) { RCC->APB1ENR |= RCC_APB1ENR_USART3EN; }
    else if (usart == UART4)  { RCC->APB1ENR |= RCC_APB1ENR_UART4EN;  }
    else if (usart == UART5)  { RCC->APB1ENR |= RCC_APB1ENR_UART5EN;  }
    else if (usart == USART6) { RCC->APB2ENR |= RCC_APB2ENR_USART6EN; }
    else { return; }

    /* 回读以确保时钟使能生效 */
    (void)RCC->APB1ENR;
    (void)RCC->APB2ENR;
#endif
}

uint32_t usart_get_clock(USART_TypeDef *usart)
{
    if (usart == NULL) { return 0U; }
    return usart_get_pclk(usart_is_apb2(usart) ? 2U : 1U);
}

IRQn_Type usart_get_irqn(USART_TypeDef *usart)
{
    if (usart == USART1) { return USART1_IRQn; }
    if (usart == USART2) { return USART2_IRQn; }
    if (usart == USART3) { return USART3_IRQn; }
    if (usart == UART4)  { return UART4_IRQn;  }
    if (usart == UART5)  { return UART5_IRQn;  }
    return USART6_IRQn;
}

/* ==================== 初始化 / 使能 / 禁用 ==================== */

void usart_init(const usart_config_t *cfg)
{
    USART_TypeDef *usart;
    uint32_t cr1;
    uint32_t cr2;
    uint32_t pclk;

    if (cfg == NULL || cfg->instance == NULL) { return; }

    usart = cfg->instance;

    usart_clk_enable(usart);
    usart->CR1 &= ~USART_CR1_UE;

    cr1 = 0U;
    cr2 = 0U;

    if (cfg->word_length == USART_WORDLEN_9B)
    {
        cr1 |= USART_CR1_M;
    }

    if (cfg->parity == USART_PARITY_EVEN)
    {
        cr1 |= USART_CR1_PCE;
    }
    else if (cfg->parity == USART_PARITY_ODD)
    {
        cr1 |= (USART_CR1_PCE | USART_CR1_PS);
    }

    if (cfg->tx_enable)
    {
        cr1 |= USART_CR1_TE;
    }
    if (cfg->rx_enable)
    {
        cr1 |= USART_CR1_RE;
    }
    if (cfg->oversampling8)
    {
        cr1 |= USART_CR1_OVER8;
    }

    cr2 |= ((uint32_t)(cfg->stop_bits & 0x03U) << 12U);

    pclk = usart_get_clock(usart);
    usart->BRR = usart_calc_brr(pclk, cfg->baudrate, cfg->oversampling8);
    usart->CR2 = cr2;
    usart->CR3 = 0U;
    usart->CR1 = cr1;
    usart->CR1 |= USART_CR1_UE;
}

void usart_deinit(USART_TypeDef *usart)
{
    if (usart == NULL) { return; }
    usart->CR1 = 0U;
    usart->CR2 = 0U;
    usart->CR3 = 0U;
}

void usart_enable(USART_TypeDef *usart)
{
    if (usart == NULL) { return; }
    usart->CR1 |= USART_CR1_UE;
}

void usart_disable(USART_TypeDef *usart)
{
    if (usart == NULL) { return; }
    usart->CR1 &= ~USART_CR1_UE;
}

/* ==================== 阻塞式收发 ==================== */

void usart_send_byte(USART_TypeDef *usart, uint8_t data)
{
    if (usart == NULL) { return; }
    while ((usart->SR & USART_SR_TXE) == 0U) { /* 等待 */ }
    usart->DR = data;
}

uint8_t usart_recv_byte(USART_TypeDef *usart)
{
    if (usart == NULL) { return 0U; }
    while ((usart->SR & USART_SR_RXNE) == 0U) { /* 等待 */ }
    return (uint8_t)(usart->DR & 0xFFU);
}

void usart_send_buffer(USART_TypeDef *usart, const uint8_t *buf, uint32_t len)
{
    uint32_t i;

    if (usart == NULL || buf == NULL) { return; }

    for (i = 0U; i < len; i++)
    {
        while ((usart->SR & USART_SR_TXE) == 0U) { /* 等待 */ }
        usart->DR = buf[i];
    }
    while ((usart->SR & USART_SR_TC) == 0U) { /* 等待 */ }
}

void usart_send_string(USART_TypeDef *usart, const char *str)
{
    if (usart == NULL || str == NULL) { return; }

    while (*str != '\0')
    {
        while ((usart->SR & USART_SR_TXE) == 0U) { /* 等待 */ }
        usart->DR = (uint8_t)(*str);
        str++;
    }
    while ((usart->SR & USART_SR_TC) == 0U) { /* 等待 */ }
}

/* ==================== 中断控制 ==================== */

void usart_enable_txe_irq(USART_TypeDef *usart)
{
    if (usart == NULL) { return; }
    usart->CR1 |= USART_CR1_TXEIE;
}

void usart_disable_txe_irq(USART_TypeDef *usart)
{
    if (usart == NULL) { return; }
    usart->CR1 &= ~USART_CR1_TXEIE;
}

void usart_enable_tc_irq(USART_TypeDef *usart)
{
    if (usart == NULL) { return; }
    usart->CR1 |= USART_CR1_TCIE;
}

void usart_disable_tc_irq(USART_TypeDef *usart)
{
    if (usart == NULL) { return; }
    usart->CR1 &= ~USART_CR1_TCIE;
}

void usart_enable_rxne_irq(USART_TypeDef *usart)
{
    if (usart == NULL) { return; }
    usart->CR1 |= USART_CR1_RXNEIE;
}

void usart_disable_rxne_irq(USART_TypeDef *usart)
{
    if (usart == NULL) { return; }
    usart->CR1 &= ~USART_CR1_RXNEIE;
}

void usart_enable_idle_irq(USART_TypeDef *usart)
{
    if (usart == NULL) { return; }
    usart->CR1 |= USART_CR1_IDLEIE;
}

void usart_disable_idle_irq(USART_TypeDef *usart)
{
    if (usart == NULL) { return; }
    usart->CR1 &= ~USART_CR1_IDLEIE;
}

/* ==================== 标志位清除 ==================== */

void usart_clear_idle_flag(USART_TypeDef *usart)
{
    volatile uint32_t tmp;

    if (usart == NULL) { return; }
    /* RM0090: 先读 SR 再读 DR 以清除 IDLE 标志 */
    tmp = usart->SR;
    tmp = usart->DR;
    (void)tmp;
}

/* ==================== DMA 控制 ==================== */

void usart_enable_dma(USART_TypeDef *usart, uint8_t tx_enable, uint8_t rx_enable)
{
    if (usart == NULL) { return; }
    if (tx_enable)
    {
        usart->CR3 |= USART_CR3_DMAT;
    }
    if (rx_enable)
    {
        usart->CR3 |= USART_CR3_DMAR;
    }
}

void usart_disable_dma(USART_TypeDef *usart)
{
    if (usart == NULL) { return; }
    usart->CR3 &= ~(USART_CR3_DMAT | USART_CR3_DMAR);
}

/* ==================== 特殊模式 ==================== */

void usart_enable_lin(USART_TypeDef *usart)
{
    if (usart == NULL) { return; }
    usart->CR2 |= USART_CR2_LINEN;
}

void usart_disable_lin(USART_TypeDef *usart)
{
    if (usart == NULL) { return; }
    usart->CR2 &= ~USART_CR2_LINEN;
}

void usart_enable_half_duplex(USART_TypeDef *usart)
{
    if (usart == NULL) { return; }
    usart->CR3 |= USART_CR3_HDSEL;
}

void usart_disable_half_duplex(USART_TypeDef *usart)
{
    if (usart == NULL) { return; }
    usart->CR3 &= ~USART_CR3_HDSEL;
}

void usart_enable_irda(USART_TypeDef *usart, uint8_t low_power)
{
    if (usart == NULL) { return; }
    usart->CR3 |= USART_CR3_IREN;
    if (low_power)
    {
        usart->CR3 |= USART_CR3_IRLP;
    }
    else
    {
        usart->CR3 &= ~USART_CR3_IRLP;
    }
}

void usart_disable_irda(USART_TypeDef *usart)
{
    if (usart == NULL) { return; }
    usart->CR3 &= ~(USART_CR3_IREN | USART_CR3_IRLP);
}

void usart_enable_smartcard(USART_TypeDef *usart, uint8_t nackt_enable)
{
    if (usart == NULL) { return; }
    usart->CR3 |= USART_CR3_SCEN;
    if (nackt_enable)
    {
        usart->CR3 |= USART_CR3_NACK;
    }
    else
    {
        usart->CR3 &= ~USART_CR3_NACK;
    }
}

void usart_disable_smartcard(USART_TypeDef *usart)
{
    if (usart == NULL) { return; }
    usart->CR3 &= ~(USART_CR3_SCEN | USART_CR3_NACK);
}

void usart_enable_clock(USART_TypeDef *usart, uint8_t cpol, uint8_t cpha, uint8_t lastbit)
{
    if (usart == NULL) { return; }

    usart->CR2 |= USART_CR2_CLKEN;

    if (cpol)
    {
        usart->CR2 |= USART_CR2_CPOL;
    }
    else
    {
        usart->CR2 &= ~USART_CR2_CPOL;
    }

    if (cpha)
    {
        usart->CR2 |= USART_CR2_CPHA;
    }
    else
    {
        usart->CR2 &= ~USART_CR2_CPHA;
    }

    if (lastbit)
    {
        usart->CR2 |= USART_CR2_LBCL;
    }
    else
    {
        usart->CR2 &= ~USART_CR2_LBCL;
    }
}

void usart_disable_clock(USART_TypeDef *usart)
{
    if (usart == NULL) { return; }
    usart->CR2 &= ~(USART_CR2_CLKEN | USART_CR2_CPOL | USART_CR2_CPHA | USART_CR2_LBCL);
}

#endif /* DRV_USING_USART */
