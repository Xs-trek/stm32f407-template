#include "drv_dma.h"

#ifdef DRV_USING_DMA

#include "drv_rcc.h"

/* ==================== 内部辅助函数 ==================== */

/*
 * 说明：
 * DMA LISR/LIFCR 对应 stream0~3，HISR/HIFCR 对应 stream4~7。
 * 每个半区内的标志位基准偏移如下：
 * - stream 0/4 : 0
 * - stream 1/5 : 6
 * - stream 2/6 : 16
 * - stream 3/7 : 22
 *
 * 每类标志相对基准偏移：
 * - FEIF  = 0
 * - DMEIF = 2
 * - TEIF  = 3
 * - HTIF  = 4
 * - TCIF  = 5
 */
static const uint8_t dma_flag_shift_lut[4] = { 0U, 6U, 16U, 22U };

/* 说明：检查 stream 是否为 16 个合法 DMA stream 实例之一。 */
static int dma_stream_valid(DMA_Stream_TypeDef *stream)
{
    return (stream == DMA1_Stream0) ||
           (stream == DMA1_Stream1) ||
           (stream == DMA1_Stream2) ||
           (stream == DMA1_Stream3) ||
           (stream == DMA1_Stream4) ||
           (stream == DMA1_Stream5) ||
           (stream == DMA1_Stream6) ||
           (stream == DMA1_Stream7) ||
           (stream == DMA2_Stream0) ||
           (stream == DMA2_Stream1) ||
           (stream == DMA2_Stream2) ||
           (stream == DMA2_Stream3) ||
           (stream == DMA2_Stream4) ||
           (stream == DMA2_Stream5) ||
           (stream == DMA2_Stream6) ||
           (stream == DMA2_Stream7);
}

/*
 * 说明：
 * 根据 stream 获取控制器与索引。
 *
 * 当前版本语义：
 * - 成功返回 1
 * - 非法 stream 返回 0
 */
static int dma_get_stream_info(DMA_Stream_TypeDef *stream, DMA_TypeDef **dma, uint8_t *idx)
{
    if (!dma_stream_valid(stream)) { return 0; }
    if ((dma == NULL) || (idx == NULL)) { return 0; }

    if (stream == DMA1_Stream0) { *dma = DMA1; *idx = 0U; return 1; }
    if (stream == DMA1_Stream1) { *dma = DMA1; *idx = 1U; return 1; }
    if (stream == DMA1_Stream2) { *dma = DMA1; *idx = 2U; return 1; }
    if (stream == DMA1_Stream3) { *dma = DMA1; *idx = 3U; return 1; }
    if (stream == DMA1_Stream4) { *dma = DMA1; *idx = 4U; return 1; }
    if (stream == DMA1_Stream5) { *dma = DMA1; *idx = 5U; return 1; }
    if (stream == DMA1_Stream6) { *dma = DMA1; *idx = 6U; return 1; }
    if (stream == DMA1_Stream7) { *dma = DMA1; *idx = 7U; return 1; }

    if (stream == DMA2_Stream0) { *dma = DMA2; *idx = 0U; return 1; }
    if (stream == DMA2_Stream1) { *dma = DMA2; *idx = 1U; return 1; }
    if (stream == DMA2_Stream2) { *dma = DMA2; *idx = 2U; return 1; }
    if (stream == DMA2_Stream3) { *dma = DMA2; *idx = 3U; return 1; }
    if (stream == DMA2_Stream4) { *dma = DMA2; *idx = 4U; return 1; }
    if (stream == DMA2_Stream5) { *dma = DMA2; *idx = 5U; return 1; }
    if (stream == DMA2_Stream6) { *dma = DMA2; *idx = 6U; return 1; }
    if (stream == DMA2_Stream7) { *dma = DMA2; *idx = 7U; return 1; }

    return 0;
}

/* 说明：读取 stream 当前是否使能。 */
static int dma_stream_enabled(DMA_Stream_TypeDef *stream)
{
    return ((stream->CR & DMA_SxCR_EN) != 0U);
}

/*
 * 说明：
 * 判断当前配置是否必须使用 FIFO mode。
 *
 * 当前版本语义：
 * - M2M 模式必须使用 FIFO mode
 * - 任一 burst 非 SINGLE 时必须使用 FIFO mode
 */
static int dma_need_fifo_mode(const dma_config_t *cfg)
{
    if (cfg->dir == DMA_DIR_M2M) { return 1; }

    if (cfg->mem_burst != DMA_BURST_SINGLE) { return 1; }
    if (cfg->periph_burst != DMA_BURST_SINGLE) { return 1; }

    return (cfg->use_fifo != 0U) ? 1 : 0;
}

/* 说明：检查 dir 参数是否合法。 */
static int dma_dir_valid(dma_dir_t dir)
{
    return (dir == DMA_DIR_P2M) ||
           (dir == DMA_DIR_M2P) ||
           (dir == DMA_DIR_M2M);
}

/* 说明：检查 size 参数是否合法。 */
static int dma_size_valid(dma_size_t size)
{
    return (size == DMA_SIZE_BYTE) ||
           (size == DMA_SIZE_HALFWORD) ||
           (size == DMA_SIZE_WORD);
}

/* 说明：检查 priority 参数是否合法。 */
static int dma_priority_valid(dma_priority_t priority)
{
    return ((uint32_t)priority <= (uint32_t)DMA_PRIORITY_VERY_HIGH);
}

/* 说明：检查 burst 参数是否合法。 */
static int dma_burst_valid(dma_burst_t burst)
{
    return ((uint32_t)burst <= (uint32_t)DMA_BURST_INC16);
}

/* 说明：检查 FIFO threshold 参数是否合法。 */
static int dma_fifo_threshold_valid(dma_fifo_threshold_t threshold)
{
    return ((uint32_t)threshold <= (uint32_t)DMA_FIFO_TH_FULL);
}

/* 说明：读取指定标志。 */
static uint8_t dma_read_flag(DMA_Stream_TypeDef *stream, uint8_t flag_offset)
{
    DMA_TypeDef *dma;
    uint8_t idx;
    uint32_t shift;

    if (!dma_get_stream_info(stream, &dma, &idx)) { return 0U; }

    shift = dma_flag_shift_lut[idx & 0x03U] + flag_offset;

    if (idx <= 3U)
    {
        return (uint8_t)((dma->LISR >> shift) & 0x01U);
    }
    else
    {
        return (uint8_t)((dma->HISR >> shift) & 0x01U);
    }
}

/* ==================== 时钟控制 ==================== */

void dma_clk_enable(DMA_TypeDef *dma)
{
    if (dma == NULL) { return; }

#ifdef DRV_USING_RCC
    rcc_enable_dma_clk(dma);
#else
    if (dma == DMA1)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    }
    else if (dma == DMA2)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    }
    else
    {
        return;
    }

    (void)RCC->AHB1ENR;
#endif
}

/* ==================== stream 信息查询 ==================== */

DMA_TypeDef *dma_get_controller(DMA_Stream_TypeDef *stream)
{
    DMA_TypeDef *dma;
    uint8_t idx;

    if (!dma_get_stream_info(stream, &dma, &idx)) { return NULL; }

    return dma;
}

uint8_t dma_get_stream_index(DMA_Stream_TypeDef *stream)
{
    DMA_TypeDef *dma;
    uint8_t idx;

    if (!dma_get_stream_info(stream, &dma, &idx)) { return 0U; }

    return idx;
}

IRQn_Type dma_get_irqn(DMA_Stream_TypeDef *stream)
{
    static const IRQn_Type dma1_irqn[8] = {
        DMA1_Stream0_IRQn, DMA1_Stream1_IRQn, DMA1_Stream2_IRQn, DMA1_Stream3_IRQn,
        DMA1_Stream4_IRQn, DMA1_Stream5_IRQn, DMA1_Stream6_IRQn, DMA1_Stream7_IRQn
    };
    static const IRQn_Type dma2_irqn[8] = {
        DMA2_Stream0_IRQn, DMA2_Stream1_IRQn, DMA2_Stream2_IRQn, DMA2_Stream3_IRQn,
        DMA2_Stream4_IRQn, DMA2_Stream5_IRQn, DMA2_Stream6_IRQn, DMA2_Stream7_IRQn
    };

    DMA_TypeDef *dma;
    uint8_t idx;

    if (!dma_get_stream_info(stream, &dma, &idx)) { return (IRQn_Type)0; }

    return (dma == DMA2) ? dma2_irqn[idx] : dma1_irqn[idx];
}

/* ==================== 标志管理 ==================== */

void dma_clear_flags(DMA_Stream_TypeDef *stream)
{
    DMA_TypeDef *dma;
    uint8_t idx;
    uint32_t shift;
    uint32_t clear_mask;

    if (!dma_get_stream_info(stream, &dma, &idx)) { return; }

    shift = dma_flag_shift_lut[idx & 0x03U];

    /*
     * 清除 FE / DME / TE / HT / TC 标志：
     * bit0 + bit2 + bit3 + bit4 + bit5 = 0x3D
     */
    clear_mask = (0x3DU << shift);

    if (idx <= 3U)
    {
        dma->LIFCR = clear_mask;
    }
    else
    {
        dma->HIFCR = clear_mask;
    }
}

uint8_t dma_get_flag_tc(DMA_Stream_TypeDef *stream)
{
    return dma_read_flag(stream, 5U);
}

uint8_t dma_get_flag_ht(DMA_Stream_TypeDef *stream)
{
    return dma_read_flag(stream, 4U);
}

uint8_t dma_get_flag_te(DMA_Stream_TypeDef *stream)
{
    return dma_read_flag(stream, 3U);
}

uint8_t dma_get_flag_fe(DMA_Stream_TypeDef *stream)
{
    return dma_read_flag(stream, 0U);
}

uint8_t dma_get_flag_dme(DMA_Stream_TypeDef *stream)
{
    return dma_read_flag(stream, 2U);
}

/* ==================== 初始化 / 启动 / 停止 ==================== */

void dma_init(const dma_config_t *cfg)
{
    DMA_Stream_TypeDef *stream;
    DMA_TypeDef *dma;
    uint8_t idx;
    uint32_t cr;
    uint32_t fcr;
    uint8_t use_fifo;

    if (cfg == NULL) { return; }
    if (cfg->stream == NULL) { return; }
    if (!dma_get_stream_info(cfg->stream, &dma, &idx)) { return; }

    if (cfg->channel > 7U) { return; }
    if (!dma_dir_valid(cfg->dir)) { return; }
    if (!dma_size_valid(cfg->p_size)) { return; }
    if (!dma_size_valid(cfg->m_size)) { return; }
    if (!dma_priority_valid(cfg->priority)) { return; }
    if (!dma_burst_valid(cfg->mem_burst)) { return; }
    if (!dma_burst_valid(cfg->periph_burst)) { return; }
    if (!dma_fifo_threshold_valid(cfg->fifo_threshold)) { return; }

    if ((cfg->double_buffer != 0U) && (cfg->mem1_addr == 0U)) { return; }

    stream = cfg->stream;

    dma_clk_enable(dma);

    /* 说明：重新配置前先关闭 stream，并等待 EN 读回为 0。 */
    stream->CR &= ~DMA_SxCR_EN;
    while ((stream->CR & DMA_SxCR_EN) != 0U) { }

    dma_clear_flags(stream);

    use_fifo = (uint8_t)dma_need_fifo_mode(cfg);

    cr = 0U;
    cr |= ((cfg->channel & 0x07U) << 25U);
    cr |= ((uint32_t)(cfg->dir & 0x03U) << 6U);
    cr |= ((uint32_t)(cfg->p_size & 0x03U) << 11U);
    cr |= ((uint32_t)(cfg->m_size & 0x03U) << 13U);
    cr |= ((uint32_t)(cfg->priority & 0x03U) << 16U);
    cr |= ((uint32_t)(cfg->mem_burst & 0x03U) << 23U);
    cr |= ((uint32_t)(cfg->periph_burst & 0x03U) << 21U);

    if (cfg->p_inc != 0U)         { cr |= DMA_SxCR_PINC; }
    if (cfg->m_inc != 0U)         { cr |= DMA_SxCR_MINC; }
    if (cfg->circular != 0U)      { cr |= DMA_SxCR_CIRC; }
    if (cfg->double_buffer != 0U) { cr |= DMA_SxCR_DBM; }
    if (cfg->enable_tc_irq != 0U) { cr |= DMA_SxCR_TCIE; }
    if (cfg->enable_ht_irq != 0U) { cr |= DMA_SxCR_HTIE; }
    if (cfg->enable_te_irq != 0U) { cr |= DMA_SxCR_TEIE; }

    /*
     * 说明：
     * DBM 模式下，CIRC 在 EN=1 后会被硬件强制置 1。
     * 当前版本直接在初始化阶段显式按循环模式配置。
     */
    if (cfg->double_buffer != 0U)
    {
        cr |= DMA_SxCR_CIRC;
    }

    fcr = 0U;
    if (use_fifo != 0U)
    {
        fcr |= DMA_SxFCR_DMDIS;
        fcr |= ((uint32_t)cfg->fifo_threshold & 0x03U);
    }

    /*
     * 说明：
     * 先清 stream 配置寄存器，再写入本次配置。
     * 所有受保护位均在 EN=0 时编程。
     */
    stream->CR   = 0U;
    stream->NDTR = cfg->count;
    stream->PAR  = cfg->periph_addr;
    stream->M0AR = cfg->mem0_addr;
    stream->M1AR = cfg->mem1_addr;
    stream->FCR  = fcr;
    stream->CR   = cr;
}

void dma_start(DMA_Stream_TypeDef *stream)
{
    DMA_TypeDef *dma;
    uint8_t idx;

    if (!dma_get_stream_info(stream, &dma, &idx)) { return; }

    /* 说明：启动前先清除该 stream 对应事件标志。 */
    dma_clear_flags(stream);
    stream->CR |= DMA_SxCR_EN;
}

void dma_stop(DMA_Stream_TypeDef *stream)
{
    DMA_TypeDef *dma;
    uint8_t idx;

    if (!dma_get_stream_info(stream, &dma, &idx)) { return; }

    stream->CR &= ~DMA_SxCR_EN;
    while ((stream->CR & DMA_SxCR_EN) != 0U) { }
}

/* ==================== 运行时参数更新 ==================== */

void dma_set_count(DMA_Stream_TypeDef *stream, uint16_t count)
{
    DMA_TypeDef *dma;
    uint8_t idx;

    if (!dma_get_stream_info(stream, &dma, &idx)) { return; }
    if (dma_stream_enabled(stream)) { return; }

    stream->NDTR = count;
}

uint16_t dma_get_count(DMA_Stream_TypeDef *stream)
{
    DMA_TypeDef *dma;
    uint8_t idx;

    if (!dma_get_stream_info(stream, &dma, &idx)) { return 0U; }

    return (uint16_t)stream->NDTR;
}

void dma_set_mem0_addr(DMA_Stream_TypeDef *stream, uint32_t addr)
{
    DMA_TypeDef *dma;
    uint8_t idx;
    uint32_t cr;

    if (!dma_get_stream_info(stream, &dma, &idx)) { return; }

    cr = stream->CR;

    /*
     * 允许写 M0AR 的条件：
     * 1. EN = 0
     * 2. EN = 1 且 DBM = 1 且 CT = 1
     */
    if ((cr & DMA_SxCR_EN) == 0U)
    {
        stream->M0AR = addr;
        return;
    }

    if (((cr & DMA_SxCR_DBM) != 0U) && ((cr & DMA_SxCR_CT) != 0U))
    {
        stream->M0AR = addr;
    }
}

void dma_set_mem1_addr(DMA_Stream_TypeDef *stream, uint32_t addr)
{
    DMA_TypeDef *dma;
    uint8_t idx;
    uint32_t cr;

    if (!dma_get_stream_info(stream, &dma, &idx)) { return; }

    cr = stream->CR;

    /*
     * 允许写 M1AR 的条件：
     * 1. EN = 0
     * 2. EN = 1 且 DBM = 1 且 CT = 0
     */
    if ((cr & DMA_SxCR_EN) == 0U)
    {
        stream->M1AR = addr;
        return;
    }

    if (((cr & DMA_SxCR_DBM) != 0U) && ((cr & DMA_SxCR_CT) == 0U))
    {
        stream->M1AR = addr;
    }
}

void dma_set_periph_addr(DMA_Stream_TypeDef *stream, uint32_t addr)
{
    DMA_TypeDef *dma;
    uint8_t idx;

    if (!dma_get_stream_info(stream, &dma, &idx)) { return; }
    if (dma_stream_enabled(stream)) { return; }

    stream->PAR = addr;
}

uint8_t dma_get_current_target(DMA_Stream_TypeDef *stream)
{
    DMA_TypeDef *dma;
    uint8_t idx;

    if (!dma_get_stream_info(stream, &dma, &idx)) { return 0U; }

    return (uint8_t)(((stream->CR & DMA_SxCR_CT) != 0U) ? 1U : 0U);
}

#endif /* DRV_USING_DMA */
