#include "drv_dac.h"

#ifdef DRV_USING_DAC

/* ==================== 内部辅助函数 ==================== */

/*
 * 说明：
 * DAC 两个通道在 CR 中采用固定偏移布局：
 * - 通道1使用基准位
 * - 通道2相对通道1偏移 16 位
 */
static uint32_t dac_cr_offset(dac_channel_t channel)
{
    return (channel == DAC_CHANNEL_2) ? 16U : 0U;
}

/* 说明：检查通道参数是否合法。 */
static int dac_channel_valid(dac_channel_t channel)
{
    return (channel == DAC_CHANNEL_1 || channel == DAC_CHANNEL_2);
}

/* 说明：检查触发源参数是否合法。 */
static int dac_trigger_valid(dac_trigger_t trig)
{
    return ((uint32_t)trig <= (uint32_t)DAC_TRIG_SOFTWARE);
}

/* 说明：检查波形模式参数是否合法。 */
static int dac_wave_valid(dac_wave_t wave)
{
    return (wave == DAC_WAVE_NONE ||
            wave == DAC_WAVE_NOISE ||
            wave == DAC_WAVE_TRIANGLE);
}

/* 说明：读取指定通道当前是否处于使能状态。 */
static int dac_is_enabled(dac_channel_t channel)
{
    return ((DAC->CR & (DAC_CR_EN1 << dac_cr_offset(channel))) != 0U);
}

/* 说明：获取软件触发位掩码。 */
static uint32_t dac_swtrig_mask(dac_channel_t channel)
{
    if (channel == DAC_CHANNEL_1)
    {
        return DAC_SWTRIGR_SWTRIG1;
    }
    else if (channel == DAC_CHANNEL_2)
    {
        return DAC_SWTRIGR_SWTRIG2;
    }

    return 0U;
}

/* 说明：获取 DMA 下溢标志位掩码。 */
static uint32_t dac_dmaudr_flag_mask(dac_channel_t channel)
{
    if (channel == DAC_CHANNEL_1)
    {
        return DAC_SR_DMAUDR1;
    }
    else if (channel == DAC_CHANNEL_2)
    {
        return DAC_SR_DMAUDR2;
    }

    return 0U;
}

/* ==================== 时钟控制 ==================== */

void dac_clk_enable(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
    (void)RCC->APB1ENR;
}

/* ==================== 通道使能控制 ==================== */

void dac_enable(dac_channel_t channel)
{
    if (!dac_channel_valid(channel)) { return; }

    DAC->CR |= (DAC_CR_EN1 << dac_cr_offset(channel));
}

void dac_disable(dac_channel_t channel)
{
    if (!dac_channel_valid(channel)) { return; }

    DAC->CR &= ~(DAC_CR_EN1 << dac_cr_offset(channel));
}

/* ==================== 输出缓冲控制 ==================== */

void dac_enable_output_buffer(dac_channel_t channel)
{
    if (!dac_channel_valid(channel)) { return; }

    /* 说明：BOFFx = 0 表示使能输出缓冲。 */
    DAC->CR &= ~(DAC_CR_BOFF1 << dac_cr_offset(channel));
}

void dac_disable_output_buffer(dac_channel_t channel)
{
    if (!dac_channel_valid(channel)) { return; }

    DAC->CR |= (DAC_CR_BOFF1 << dac_cr_offset(channel));
}

/* ==================== 触发控制 ==================== */

void dac_set_trigger(dac_channel_t channel, dac_trigger_t trig)
{
    uint32_t offset;

    if (!dac_channel_valid(channel)) { return; }
    if (!dac_trigger_valid(trig)) { return; }

    /*
     * 限制：
     * 按 RM，ENx = 1 时不能修改 TSELx。
     * 当前版本语义：若通道已使能，则直接返回。
     */
    if (dac_is_enabled(channel)) { return; }

    offset = dac_cr_offset(channel);

    /* 先清除原有触发使能与触发源选择。 */
    DAC->CR &= ~((DAC_CR_TSEL1 | DAC_CR_TEN1) << offset);

    /* 再写入新的触发源。 */
    DAC->CR |= (((uint32_t)trig << 3U) << offset);

    /* 最后使能触发。 */
    DAC->CR |= (DAC_CR_TEN1 << offset);
}

void dac_disable_trigger(dac_channel_t channel)
{
    if (!dac_channel_valid(channel)) { return; }

    /*
     * 当前版本语义：
     * 仅清除 TENx，不修改 TSELx。
     */
    DAC->CR &= ~(DAC_CR_TEN1 << dac_cr_offset(channel));
}

void dac_software_trigger(dac_channel_t channel)
{
    uint32_t mask;

    if (!dac_channel_valid(channel)) { return; }

    mask = dac_swtrig_mask(channel);
    if (mask == 0U) { return; }

    /*
     * 说明：
     * SWTRIGx 为“写 1 触发”位，触发后由硬件自动清零。
     * 因此这里直接写入，不做读改写。
     */
    DAC->SWTRIGR = mask;
}

/* ==================== 波形生成控制 ==================== */

void dac_set_wave(dac_channel_t channel, dac_wave_t wave, uint8_t mask_amp)
{
    uint32_t offset;

    if (!dac_channel_valid(channel)) { return; }
    if (!dac_wave_valid(wave)) { return; }

    offset = dac_cr_offset(channel);

    /* 先清除原有波形类型与掩码/振幅配置。 */
    DAC->CR &= ~((DAC_CR_WAVE1 | DAC_CR_MAMP1) << offset);

    if (wave != DAC_WAVE_NONE)
    {
        DAC->CR |= (((uint32_t)wave << 6U) << offset);
        DAC->CR |= (((uint32_t)(mask_amp & 0x0FU) << 8U) << offset);
    }
}

/* ==================== 数据写入 ==================== */

void dac_write_12r(dac_channel_t channel, uint16_t value)
{
    if (channel == DAC_CHANNEL_1)
    {
        DAC->DHR12R1 = (uint32_t)(value & 0x0FFFU);
    }
    else if (channel == DAC_CHANNEL_2)
    {
        DAC->DHR12R2 = (uint32_t)(value & 0x0FFFU);
    }
}

void dac_write_12l(dac_channel_t channel, uint16_t value)
{
    if (channel == DAC_CHANNEL_1)
    {
        DAC->DHR12L1 = ((uint32_t)(value & 0x0FFFU) << 4U);
    }
    else if (channel == DAC_CHANNEL_2)
    {
        DAC->DHR12L2 = ((uint32_t)(value & 0x0FFFU) << 4U);
    }
}

void dac_write_8r(dac_channel_t channel, uint8_t value)
{
    if (channel == DAC_CHANNEL_1)
    {
        DAC->DHR8R1 = (uint32_t)value;
    }
    else if (channel == DAC_CHANNEL_2)
    {
        DAC->DHR8R2 = (uint32_t)value;
    }
}

void dac_write_dual_12r(uint16_t val_ch1, uint16_t val_ch2)
{
    DAC->DHR12RD = ((uint32_t)(val_ch2 & 0x0FFFU) << 16U)
                 | ((uint32_t)(val_ch1 & 0x0FFFU));
}

/* ==================== 输出读回 ==================== */

uint16_t dac_read_output(dac_channel_t channel)
{
    if (channel == DAC_CHANNEL_1)
    {
        return (uint16_t)(DAC->DOR1 & 0x0FFFU);
    }

    if (channel == DAC_CHANNEL_2)
    {
        return (uint16_t)(DAC->DOR2 & 0x0FFFU);
    }

    return 0U;
}

/* ==================== DMA 控制 ==================== */

void dac_enable_dma(dac_channel_t channel)
{
    if (!dac_channel_valid(channel)) { return; }

    DAC->CR |= (DAC_CR_DMAEN1 << dac_cr_offset(channel));
}

void dac_disable_dma(dac_channel_t channel)
{
    if (!dac_channel_valid(channel)) { return; }

    DAC->CR &= ~(DAC_CR_DMAEN1 << dac_cr_offset(channel));
}

void dac_enable_dma_underrun_interrupt(dac_channel_t channel)
{
    if (!dac_channel_valid(channel)) { return; }

    DAC->CR |= (DAC_CR_DMAUDRIE1 << dac_cr_offset(channel));
}

void dac_disable_dma_underrun_interrupt(dac_channel_t channel)
{
    if (!dac_channel_valid(channel)) { return; }

    DAC->CR &= ~(DAC_CR_DMAUDRIE1 << dac_cr_offset(channel));
}

int dac_get_dma_underrun_flag(dac_channel_t channel)
{
    uint32_t mask;

    if (!dac_channel_valid(channel)) { return 0; }

    mask = dac_dmaudr_flag_mask(channel);
    if (mask == 0U) { return 0; }

    return ((DAC->SR & mask) != 0U) ? 1 : 0;
}

void dac_clear_dma_underrun_flag(dac_channel_t channel)
{
    uint32_t mask;

    if (!dac_channel_valid(channel)) { return; }

    mask = dac_dmaudr_flag_mask(channel);
    if (mask == 0U) { return; }

    /*
     * 说明：
     * DAC_SR.DMAUDRx 采用“写 1 清零”。
     * 当前版本语义：仅写入目标标志位掩码进行清除。
     */
    DAC->SR = mask;
}

/* ==================== 默认初始化辅助 ==================== */

void dac_init(dac_channel_t channel)
{
    uint32_t offset;
    uint32_t clear_mask;

    if (!dac_channel_valid(channel)) { return; }

    dac_clk_enable();

    /*
     * 说明：
     * 为满足 RM 对 TSELx 的约束，初始化时先关闭通道。
     */
    dac_disable(channel);

    offset = dac_cr_offset(channel);

    /*
     * 需要恢复为默认态的控制位：
     * - TSELx
     * - TENx
     * - WAVEx
     * - MAMPx
     * - DMAENx
     * - DMAUDRIEx
     */
    clear_mask = (DAC_CR_TSEL1
                | DAC_CR_TEN1
                | DAC_CR_WAVE1
                | DAC_CR_MAMP1
                | DAC_CR_DMAEN1
                | DAC_CR_DMAUDRIE1) << offset;

    DAC->CR &= ~clear_mask;

    /* 默认使能输出缓冲。 */
    DAC->CR &= ~(DAC_CR_BOFF1 << offset);

    /* 清除可能残留的 DMA 下溢标志。 */
    dac_clear_dma_underrun_flag(channel);
}

#endif /* DRV_USING_DAC */
