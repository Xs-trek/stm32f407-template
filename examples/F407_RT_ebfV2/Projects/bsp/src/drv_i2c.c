#include "drv_i2c.h"

#ifdef DRV_USING_I2C

#include "drv_rcc.h"

/* ==================== 内部：APB1 时钟计算 ==================== */

/* AHB 预分频移位查找表 */
static const uint8_t i2c_hpre_lut[16] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 2, 3, 4, 6, 7, 8, 9
};

/* APB1 预分频移位查找表 */
static const uint8_t i2c_ppre_lut[8] = {
    0, 0, 0, 0,
    1, 2, 3, 4
};

static uint32_t i2c_get_pclk1(void)
{
    uint32_t hclk  = SystemCoreClock >> i2c_hpre_lut[(RCC->CFGR >> 4U) & 0x0FU];
    uint32_t ppre1 = i2c_ppre_lut[(RCC->CFGR >> 10U) & 0x07U];
    return hclk >> ppre1;
}

/* 阻塞等待寄存器指定标志位置位 */
static void i2c_wait_flag_set(__IO uint32_t *reg, uint32_t flag)
{
    while (((*reg) & flag) == 0U)
    {
        /* 等待 */
    }
}

/* ==================== 时钟 ==================== */

void i2c_clk_enable(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return;
    }

#ifdef DRV_USING_RCC
    rcc_enable_i2c_clk(i2c);
#else
    if (i2c == I2C1)
    {
        RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    }
    else if (i2c == I2C2)
    {
        RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
    }
    else if (i2c == I2C3)
    {
        RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
    }
    else
    {
        return;
    }

    /* 读回以确保写入生效 */
    (void)RCC->APB1ENR;
#endif
}

uint32_t i2c_get_clock(I2C_TypeDef *i2c)
{
    (void)i2c;
    return i2c_get_pclk1();
}

IRQn_Type i2c_get_ev_irqn(I2C_TypeDef *i2c)
{
    if (i2c == I2C1)
    {
        return I2C1_EV_IRQn;
    }
    if (i2c == I2C2)
    {
        return I2C2_EV_IRQn;
    }
    return I2C3_EV_IRQn;
}

IRQn_Type i2c_get_er_irqn(I2C_TypeDef *i2c)
{
    if (i2c == I2C1)
    {
        return I2C1_ER_IRQn;
    }
    if (i2c == I2C2)
    {
        return I2C2_ER_IRQn;
    }
    return I2C3_ER_IRQn;
}

/* ==================== 初始化 / 使能 / 禁用 ==================== */

void i2c_init(const i2c_config_t *cfg)
{
    I2C_TypeDef *i2c;
    uint32_t pclk_mhz;
    uint32_t ccr;

    if (cfg == NULL || cfg->instance == NULL)
    {
        return;
    }

    i2c = cfg->instance;
    i2c_clk_enable(i2c);

    /* 先禁用外设再配置 */
    i2c->CR1 &= ~I2C_CR1_PE;

    pclk_mhz = i2c_get_clock(i2c) / 1000000U;
    i2c->CR2 = (pclk_mhz & 0x3FU);

    /* 设置自身地址与地址模式 */
    if (cfg->addr_mode == 0U)
    {
        i2c->OAR1 = ((uint32_t)(cfg->own_address & 0x7FU) << 1U);
    }
    else
    {
        i2c->OAR1 = ((uint32_t)(cfg->own_address & 0x3FFU)) | I2C_OAR1_ADDMODE;
    }

    if (cfg->clock_hz <= 100000U)
    {
        /* 标准模式 */
        i2c->TRISE = pclk_mhz + 1U;
        ccr = i2c_get_clock(i2c) / (cfg->clock_hz << 1U);
        if (ccr < 4U)
        {
            ccr = 4U;
        }
        i2c->CCR = ccr;
    }
    else
    {
        /* 快速模式 */
        i2c->TRISE = ((pclk_mhz * 300U) / 1000U) + 1U;
        i2c->CCR = I2C_CCR_FS;

        if (cfg->duty)
        {
            i2c->CCR |= I2C_CCR_DUTY;
            ccr = i2c_get_clock(i2c) / (cfg->clock_hz * 25U);
        }
        else
        {
            ccr = i2c_get_clock(i2c) / (cfg->clock_hz * 3U);
        }

        if (ccr == 0U)
        {
            ccr = 1U;
        }
        i2c->CCR |= (ccr & 0x0FFFU);
    }

    if (cfg->ack_enable)
    {
        i2c->CR1 |= I2C_CR1_ACK;
    }

    /* 使能外设 */
    i2c->CR1 |= I2C_CR1_PE;
}

void i2c_deinit(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return;
    }
    i2c->CR1   = 0U;
    i2c->CR2   = 0U;
    i2c->OAR1  = 0U;
    i2c->CCR   = 0U;
    i2c->TRISE = 0x0002U;  /* 复位值，参见 RM0090 */
}

void i2c_enable(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return;
    }
    i2c->CR1 |= I2C_CR1_PE;
}

void i2c_disable(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return;
    }
    i2c->CR1 &= ~I2C_CR1_PE;
}

/* ==================== 总线操作 ==================== */

void i2c_start(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return;
    }
    i2c->CR1 |= I2C_CR1_START;
    i2c_wait_flag_set(&i2c->SR1, I2C_SR1_SB);
}

void i2c_stop(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return;
    }
    i2c->CR1 |= I2C_CR1_STOP;
}

void i2c_send_addr(I2C_TypeDef *i2c, uint16_t addr, uint8_t addr_mode, uint8_t read)
{
    volatile uint32_t tmp;

    if (i2c == NULL)
    {
        return;
    }

    if (addr_mode == 0U)
    {
        /* 7 位地址 */
        i2c->DR = (uint8_t)(((addr & 0x7FU) << 1U) | (read ? 1U : 0U));
        i2c_wait_flag_set(&i2c->SR1, I2C_SR1_ADDR);

        /* 读 SR1 + SR2 清除 ADDR 标志 */
        tmp = i2c->SR1;
        tmp = i2c->SR2;
        (void)tmp;
    }
    else
    {
        /* 10 位地址：头字节 + 低字节 */
        i2c->DR = (uint8_t)(0xF0U | ((addr >> 7U) & 0x06U));
        i2c_wait_flag_set(&i2c->SR1, I2C_SR1_ADD10);

        i2c->DR = (uint8_t)(addr & 0xFFU);
        i2c_wait_flag_set(&i2c->SR1, I2C_SR1_ADDR);

        /* 读 SR1 + SR2 清除 ADDR 标志 */
        tmp = i2c->SR1;
        tmp = i2c->SR2;
        (void)tmp;

        if (read)
        {
            /* 10 位地址读方向需要重发起始条件和头字节 */
            i2c->CR1 |= I2C_CR1_START;
            i2c_wait_flag_set(&i2c->SR1, I2C_SR1_SB);
            i2c->DR = (uint8_t)(0xF0U | ((addr >> 7U) & 0x06U) | 1U);
            i2c_wait_flag_set(&i2c->SR1, I2C_SR1_ADDR);

            tmp = i2c->SR1;
            tmp = i2c->SR2;
            (void)tmp;
        }
    }
}

void i2c_send_byte(I2C_TypeDef *i2c, uint8_t data)
{
    if (i2c == NULL)
    {
        return;
    }
    i2c_wait_flag_set(&i2c->SR1, I2C_SR1_TXE);
    i2c->DR = data;
    i2c_wait_flag_set(&i2c->SR1, I2C_SR1_BTF);
}

uint8_t i2c_recv_byte_ack(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return 0U;
    }
    i2c->CR1 |= I2C_CR1_ACK;
    i2c_wait_flag_set(&i2c->SR1, I2C_SR1_RXNE);
    return (uint8_t)i2c->DR;
}

uint8_t i2c_recv_byte_nack(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return 0U;
    }
    i2c->CR1 &= ~I2C_CR1_ACK;
    i2c_stop(i2c);
    i2c_wait_flag_set(&i2c->SR1, I2C_SR1_RXNE);
    return (uint8_t)i2c->DR;
}

/* ==================== 应答 / POS 控制 ==================== */

void i2c_enable_ack(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return;
    }
    i2c->CR1 |= I2C_CR1_ACK;
}

void i2c_disable_ack(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return;
    }
    i2c->CR1 &= ~I2C_CR1_ACK;
}

void i2c_enable_pos(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return;
    }
    i2c->CR1 |= I2C_CR1_POS;
}

void i2c_disable_pos(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return;
    }
    i2c->CR1 &= ~I2C_CR1_POS;
}

/* ==================== DMA 控制 ==================== */

void i2c_enable_dma(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return;
    }
    i2c->CR2 |= I2C_CR2_DMAEN;
}

void i2c_disable_dma(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return;
    }
    i2c->CR2 &= ~I2C_CR2_DMAEN;
}

void i2c_enable_dma_last(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return;
    }
    i2c->CR2 |= I2C_CR2_LAST;
}

void i2c_disable_dma_last(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return;
    }
    i2c->CR2 &= ~I2C_CR2_LAST;
}

/* ==================== 中断控制 ==================== */

void i2c_enable_evt_irq(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return;
    }
    i2c->CR2 |= I2C_CR2_ITEVTEN;
}

void i2c_disable_evt_irq(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return;
    }
    i2c->CR2 &= ~I2C_CR2_ITEVTEN;
}

void i2c_enable_buf_irq(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return;
    }
    i2c->CR2 |= I2C_CR2_ITBUFEN;
}

void i2c_disable_buf_irq(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return;
    }
    i2c->CR2 &= ~I2C_CR2_ITBUFEN;
}

void i2c_enable_err_irq(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return;
    }
    i2c->CR2 |= I2C_CR2_ITERREN;
}

void i2c_disable_err_irq(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return;
    }
    i2c->CR2 &= ~I2C_CR2_ITERREN;
}

/* ==================== 状态寄存器读取 ==================== */

uint32_t i2c_get_sr1(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return 0U;
    }
    return i2c->SR1;
}

uint32_t i2c_get_sr2(I2C_TypeDef *i2c)
{
    if (i2c == NULL)
    {
        return 0U;
    }
    return i2c->SR2;
}

/* ==================== 便捷接口：单字节寄存器读写 ==================== */

void i2c_mem_write(I2C_TypeDef *i2c, uint8_t dev_addr, uint8_t reg, uint8_t data)
{
    if (i2c == NULL)
    {
        return;
    }
    i2c_start(i2c);
    i2c_send_addr(i2c, dev_addr, 0U, 0);
    i2c_send_byte(i2c, reg);
    i2c_send_byte(i2c, data);
    i2c_stop(i2c);
}

uint8_t i2c_mem_read(I2C_TypeDef *i2c, uint8_t dev_addr, uint8_t reg)
{
    if (i2c == NULL)
    {
        return 0U;
    }

    /* 写阶段：发送寄存器地址 */
    i2c_start(i2c);
    i2c_send_addr(i2c, dev_addr, 0U, 0);
    i2c_send_byte(i2c, reg);

    /* 读阶段：重复起始 + 读一个字节 */
    i2c_start(i2c);
    i2c_send_addr(i2c, dev_addr, 0U, 1);
    return i2c_recv_byte_nack(i2c);
}

#endif /* DRV_USING_I2C */
