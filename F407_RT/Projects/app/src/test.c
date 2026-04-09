#include "f407_rt_modules.h"

#if defined(DRV_USING_RCC)

static void rcc_compile_smoke(void)
{
    rcc_enable_gpio_clk(GPIOA);
    rcc_enable_dma_clk(DMA1);
    rcc_enable_spi_clk(SPI1);
    rcc_enable_tim_clk(TIM2);
    rcc_enable_usart_clk(USART1);
    rcc_enable_i2c_clk(I2C1);
    rcc_enable_adc_clk(ADC1);
    rcc_enable_can_clk(CAN1);

    (void)rcc_get_sysclk();
    (void)rcc_get_hclk();
    (void)rcc_get_pclk1();
    (void)rcc_get_pclk2();
    (void)rcc_get_timclk1();
    (void)rcc_get_timclk2();
    (void)rcc_get_plli2s_clk();

    (void)rcc_get_reset_flags();
    rcc_clear_reset_flags();
}

#endif

#if defined(DRV_USING_GPIO)

static void gpio_compile_smoke(void)
{
    gpio_clk_enable(GPIOA);

    gpio_set_mode(GPIOA, 0U, GPIO_MODE_OUTPUT);
    gpio_set_af(GPIOA, 1U, 7U);
    gpio_set_speed(GPIOA, 0U, GPIO_SPEED_HIGH);
    gpio_set_pull(GPIOA, 0U, GPIO_PULL_NONE);
    gpio_set_otype(GPIOA, 0U, GPIO_OTYPE_PP);

    gpio_set_mode_mask(GPIOB, 0x000FU, GPIO_MODE_OUTPUT);

    (void)gpio_lock(GPIOA, 0U);

    gpio_init_output_pp(GPIOC, 13U);
    gpio_init_output_od(GPIOC, 14U, GPIO_PULL_UP);
    gpio_init_af_pp(GPIOA, 2U, 7U);
    gpio_init_af_od(GPIOA, 3U, 4U, GPIO_PULL_UP);
    gpio_init_input(GPIOA, 4U, GPIO_PULL_DOWN);
    gpio_init_analog(GPIOA, 5U);

    gpio_set(GPIOC, 13U);
    gpio_reset(GPIOC, 13U);
    gpio_toggle(GPIOC, 13U);
    gpio_write(GPIOC, 13U, 1U);
    (void)gpio_read(GPIOA, 4U);
    (void)gpio_read_output(GPIOC, 13U);

    gpio_set_mask(GPIOB, 0x000FU);
    gpio_reset_mask(GPIOB, 0x000FU);
    (void)gpio_read_port(GPIOB);
}

#endif

#if defined(DRV_USING_ADC)

static void adc_compile_smoke(void)
{
    adc_config_t cfg;

    cfg.instance   = ADC1;
    cfg.resolution = ADC_RES_12BIT;
    cfg.align      = ADC_ALIGN_RIGHT;
    cfg.continuous = 0U;
    cfg.scan       = 0U;

    adc_set_prescaler(ADC_PRESCALER_DIV2);
    adc_enable_temp_vrefint();
    adc_enable_vbat();

    adc_init(&cfg);
    adc_enable(ADC1);

    adc_set_sample_time(ADC1, 0U, ADC_SAMPLE_15CYCLES);
    adc_set_regular_length(ADC1, 1U);
    adc_set_regular_channel(ADC1, 1U, 0U);
    adc_set_regular_trigger(ADC1, 0U, ADC_EXTRIG_NONE);
    adc_set_eoc_selection(ADC1, 1U);

    adc_enable_eoc_irq(ADC1);
    adc_disable_eoc_irq(ADC1);

    adc_enable_dma(ADC1);
    adc_disable_dma(ADC1);
    adc_enable_dma_continuous(ADC1);
    adc_disable_dma_continuous(ADC1);

    adc_enable_ovr_irq(ADC1);
    adc_disable_ovr_irq(ADC1);

    adc_set_analog_watchdog_channel(ADC1, 0U);
    adc_set_analog_watchdog_threshold(ADC1, 0x0100U, 0x0F00U);
    adc_enable_awd_irq(ADC1);
    adc_disable_awd_irq(ADC1);
    adc_enable_analog_watchdog(ADC1);
    adc_disable_analog_watchdog(ADC1);

    (void)adc_get_flag_eoc(ADC1);
    (void)adc_get_flag_ovr(ADC1);
    (void)adc_get_flag_awd(ADC1);

    adc_clear_flag_eoc(ADC1);
    adc_clear_flag_ovr(ADC1);
    adc_clear_flag_awd(ADC1);

    adc_start(ADC1);
    (void)adc_read_data(ADC1);
    (void)adc_read_channel(ADC1, 0U, ADC_SAMPLE_15CYCLES);

    adc_disable(ADC1);
}

#endif

#if defined(DRV_USING_DAC)

static void dac_compile_smoke(void)
{
    uint16_t value;

    dac_clk_enable();

    dac_init(DAC_CHANNEL_1);
    dac_enable_output_buffer(DAC_CHANNEL_1);
    dac_disable_output_buffer(DAC_CHANNEL_1);
    dac_enable_output_buffer(DAC_CHANNEL_1);

    dac_disable_trigger(DAC_CHANNEL_1);
    dac_set_wave(DAC_CHANNEL_1, DAC_WAVE_NONE, 0U);

    dac_write_12r(DAC_CHANNEL_1, 0x0123U);
    dac_write_12l(DAC_CHANNEL_1, 0x0234U);
    dac_write_8r(DAC_CHANNEL_1, 0x56U);

    dac_enable(DAC_CHANNEL_1);
    value = dac_read_output(DAC_CHANNEL_1);
    (void)value;
    dac_disable(DAC_CHANNEL_1);

    dac_init(DAC_CHANNEL_2);

    dac_set_trigger(DAC_CHANNEL_2, DAC_TRIG_SOFTWARE);
    dac_set_wave(DAC_CHANNEL_2, DAC_WAVE_TRIANGLE, 11U);

    dac_enable_dma(DAC_CHANNEL_2);
    dac_disable_dma(DAC_CHANNEL_2);
    dac_enable_dma(DAC_CHANNEL_2);

    dac_enable_dma_underrun_interrupt(DAC_CHANNEL_2);
    dac_disable_dma_underrun_interrupt(DAC_CHANNEL_2);
    dac_enable_dma_underrun_interrupt(DAC_CHANNEL_2);

    (void)dac_get_dma_underrun_flag(DAC_CHANNEL_2);
    dac_clear_dma_underrun_flag(DAC_CHANNEL_2);

    dac_write_12r(DAC_CHANNEL_2, 0x0345U);
    dac_enable(DAC_CHANNEL_2);
    dac_software_trigger(DAC_CHANNEL_2);

    value = dac_read_output(DAC_CHANNEL_2);
    (void)value;

    dac_disable_dma_underrun_interrupt(DAC_CHANNEL_2);
    dac_disable_dma(DAC_CHANNEL_2);
    dac_disable(DAC_CHANNEL_2);

    dac_write_dual_12r(0x0111U, 0x0222U);
}

#endif

#if defined(DRV_USING_DMA)

static void dma_compile_smoke(void)
{
    dma_config_t cfg;
    DMA_TypeDef *dma;
    IRQn_Type irqn;
    uint16_t count;
    uint8_t target;

    cfg.stream            = DMA1_Stream0;
    cfg.channel           = 0U;
    cfg.periph_addr       = 0x40000000U;
    cfg.mem0_addr         = 0x20000000U;
    cfg.mem1_addr         = 0x20000040U;
    cfg.count             = 16U;
    cfg.dir               = DMA_DIR_P2M;
    cfg.p_size            = DMA_SIZE_HALFWORD;
    cfg.m_size            = DMA_SIZE_HALFWORD;
    cfg.p_inc             = 0U;
    cfg.m_inc             = 1U;
    cfg.circular          = 0U;
    cfg.double_buffer     = 0U;
    cfg.priority          = DMA_PRIORITY_HIGH;
    cfg.use_fifo          = 0U;
    cfg.fifo_threshold    = DMA_FIFO_TH_HALF;
    cfg.mem_burst         = DMA_BURST_SINGLE;
    cfg.periph_burst      = DMA_BURST_SINGLE;
    cfg.enable_tc_irq     = 1U;
    cfg.enable_ht_irq     = 1U;
    cfg.enable_te_irq     = 1U;

    dma = dma_get_controller(cfg.stream);
    irqn = dma_get_irqn(cfg.stream);
    (void)dma;
    (void)irqn;
    (void)dma_get_stream_index(cfg.stream);

    dma_clk_enable(DMA1);

    dma_init(&cfg);
    dma_clear_flags(cfg.stream);

    (void)dma_get_flag_tc(cfg.stream);
    (void)dma_get_flag_ht(cfg.stream);
    (void)dma_get_flag_te(cfg.stream);
    (void)dma_get_flag_fe(cfg.stream);
    (void)dma_get_flag_dme(cfg.stream);

    dma_set_count(cfg.stream, 8U);
    count = dma_get_count(cfg.stream);
    (void)count;

    dma_set_periph_addr(cfg.stream, 0x40000004U);
    dma_set_mem0_addr(cfg.stream, 0x20000080U);
    dma_set_mem1_addr(cfg.stream, 0x200000C0U);

    dma_start(cfg.stream);
    target = dma_get_current_target(cfg.stream);
    (void)target;
    dma_stop(cfg.stream);

    cfg.stream            = DMA2_Stream3;
    cfg.channel           = 2U;
    cfg.periph_addr       = 0x40007000U;
    cfg.mem0_addr         = 0x20000100U;
    cfg.mem1_addr         = 0x20000140U;
    cfg.count             = 32U;
    cfg.dir               = DMA_DIR_M2P;
    cfg.p_size            = DMA_SIZE_WORD;
    cfg.m_size            = DMA_SIZE_WORD;
    cfg.p_inc             = 0U;
    cfg.m_inc             = 1U;
    cfg.circular          = 1U;
    cfg.double_buffer     = 1U;
    cfg.priority          = DMA_PRIORITY_VERY_HIGH;
    cfg.use_fifo          = 1U;
    cfg.fifo_threshold    = DMA_FIFO_TH_FULL;
    cfg.mem_burst         = DMA_BURST_INC4;
    cfg.periph_burst      = DMA_BURST_SINGLE;
    cfg.enable_tc_irq     = 1U;
    cfg.enable_ht_irq     = 0U;
    cfg.enable_te_irq     = 1U;

    dma_init(&cfg);
    dma_start(cfg.stream);
    dma_set_mem0_addr(cfg.stream, 0x20000180U);
    dma_set_mem1_addr(cfg.stream, 0x200001C0U);
    dma_stop(cfg.stream);

    cfg.stream            = DMA2_Stream0;
    cfg.channel           = 0U;
    cfg.periph_addr       = 0x20000200U;
    cfg.mem0_addr         = 0x20000240U;
    cfg.mem1_addr         = 0U;
    cfg.count             = 4U;
    cfg.dir               = DMA_DIR_M2M;
    cfg.p_size            = DMA_SIZE_WORD;
    cfg.m_size            = DMA_SIZE_WORD;
    cfg.p_inc             = 1U;
    cfg.m_inc             = 1U;
    cfg.circular          = 0U;
    cfg.double_buffer     = 0U;
    cfg.priority          = DMA_PRIORITY_MEDIUM;
    cfg.use_fifo          = 0U;
    cfg.fifo_threshold    = DMA_FIFO_TH_1_4;
    cfg.mem_burst         = DMA_BURST_SINGLE;
    cfg.periph_burst      = DMA_BURST_SINGLE;
    cfg.enable_tc_irq     = 0U;
    cfg.enable_ht_irq     = 0U;
    cfg.enable_te_irq     = 1U;

    dma_init(&cfg);
}

#endif

#if defined(DRV_USING_EXTI)

static void exti_compile_smoke(void)
{
    IRQn_Type irqn;

    exti_syscfg_clk_enable();

    exti_map_gpio(GPIOA, 0U);
    exti_map_gpio(GPIOB, 5U);
    exti_map_gpio(GPIOI, 11U);

    exti_set_trigger(0U, EXTI_TRIGGER_RISING);
    exti_set_trigger(1U, EXTI_TRIGGER_FALLING);
    exti_set_trigger(2U, EXTI_TRIGGER_BOTH);
    exti_set_trigger(16U, EXTI_TRIGGER_RISING);
    exti_set_trigger(22U, EXTI_TRIGGER_FALLING);

    exti_enable_interrupt(0U);
    exti_disable_interrupt(0U);

    exti_enable_event(17U);
    exti_disable_event(17U);

    (void)exti_get_pending(0U);
    exti_clear_pending(0U);
    exti_software_trigger(1U);

    irqn = exti_get_irqn(0U);
    (void)irqn;
    irqn = exti_get_irqn(7U);
    (void)irqn;
    irqn = exti_get_irqn(13U);
    (void)irqn;

    exti_init_gpio_interrupt(GPIOC, 8U, EXTI_TRIGGER_BOTH);
}

#endif

#if defined(DRV_USING_NVIC)

static void nvic_compile_smoke(void)
{
    nvic_set_priority_grouping(4U);
    nvic_set_priority(TIM2_IRQn, 2U, 0U);
    nvic_enable_irq(TIM2_IRQn);
    nvic_disable_irq(TIM2_IRQn);
    nvic_clear_pending(TIM2_IRQn);
    nvic_set_pending(TIM2_IRQn);
    nvic_clear_pending(TIM2_IRQn);
    (void)nvic_get_active(TIM2_IRQn);
}

#endif

#if defined(DRV_USING_SCB)

static void scb_compile_smoke(void)
{
    scb_set_sleepdeep(1U);
    scb_set_sleepdeep(0U);
    scb_set_sleeponexit(1U);
    scb_set_sleeponexit(0U);
    scb_set_vector_table(0x08000000U, 0x0U);
    (void)scb_get_icsr();
    (void)scb_get_aircr();
}

#endif

#if defined(DRV_USING_SYSTICK)

static void systick_compile_smoke(void)
{
    (void)systick_init(168000U);
    systick_set_clock_source(1U);
    systick_enable();
    systick_disable();
    systick_enable_irq();
    systick_disable_irq();
    (void)systick_get_current();
    (void)systick_get_countflag();
}

#endif

#if defined(DRV_USING_SYSCFG)

static void syscfg_compile_smoke(void)
{
    syscfg_clk_enable();
    syscfg_set_exti_port(0U, 0U);
    syscfg_set_exti_port(5U, 1U);
}

#endif

#if defined(DRV_USING_PWR)

static void pwr_compile_smoke(void)
{
    pwr_clk_enable();
    pwr_enable_backup_access();
    pwr_set_pvd_level(5U);
    pwr_enable_pvd();
    (void)pwr_get_pvd_output();
    pwr_disable_pvd();
    pwr_clear_wakeup_flag();
    pwr_clear_standby_flag();
}

#endif

#if defined(DRV_USING_FLASH)

static void flash_compile_smoke(void)
{
    flash_set_latency(5U);
    flash_prefetch_enable();
    flash_prefetch_disable();
    flash_icache_enable();
    flash_icache_disable();
    flash_dcache_enable();
    flash_dcache_disable();

    flash_unlock();
    flash_lock();

    (void)flash_get_sector_addr(0U);
    (void)flash_get_sector_addr(11U);

    flash_opt_unlock();
    flash_opt_lock();
}

#endif

#if defined(DRV_USING_TIM)

static void tim_compile_smoke(void)
{
    tim_base_config_t base_cfg;
    tim_oc_config_t oc_cfg;
    tim_ic_config_t ic_cfg;

    base_cfg.instance  = TIM2;
    base_cfg.prescaler = 83U;
    base_cfg.period    = 999U;
    base_cfg.dir       = TIM_DIR_UP;

    tim_clk_enable(TIM2);
    (void)tim_get_clock(TIM2);
    (void)tim_get_irqn(TIM2);

    tim_base_init(&base_cfg);
    tim_config_frequency(TIM2, 1000U);

    tim_set_counter(TIM2, 0U);
    (void)tim_get_counter(TIM2);
    tim_set_prescaler(TIM2, 83U);
    tim_set_period(TIM2, 999U);
    tim_generate_update(TIM2);

    tim_enable_update_irq(TIM2);
    tim_disable_update_irq(TIM2);
    (void)tim_get_update_flag(TIM2);
    tim_clear_update_flag(TIM2);

    tim_enable_cc_irq(TIM2, TIM_CH1);
    tim_disable_cc_irq(TIM2, TIM_CH1);
    (void)tim_get_cc_flag(TIM2, TIM_CH1);
    tim_clear_cc_flag(TIM2, TIM_CH1);

    oc_cfg.mode = TIM_OC_MODE_PWM1;
    oc_cfg.pulse = 500U;
    oc_cfg.polarity_low = 0U;
    oc_cfg.preload_enable = 1U;
    tim_oc_init(TIM2, TIM_CH1, &oc_cfg);
    tim_set_compare(TIM2, TIM_CH1, 250U);

    ic_cfg.channel = TIM_CH2;
    ic_cfg.polarity = TIM_IC_POLARITY_RISING;
    ic_cfg.filter = 0U;
    ic_cfg.prescaler = 0U;
    tim_ic_init(TIM2, &ic_cfg);
    (void)tim_get_capture(TIM2, TIM_CH2);

    tim_encoder_init(TIM2, TIM_ENCODER_MODE_TI12, 0U, 0U);
    tim_one_pulse_enable(TIM2);
    tim_set_master_mode(TIM2, 0U);
    tim_set_slave_mode(TIM2, 0U, 0U);

    tim_main_output_enable(TIM1);
    tim_main_output_disable(TIM1);
    tim_set_deadtime(TIM1, 0x10U);

    tim_start(TIM2);
    tim_stop(TIM2);
}

#endif

#if defined(DRV_USING_I2S)

static void i2s_compile_smoke(void)
{
    i2s_config_t cfg;

    cfg.instance    = SPI2;
    cfg.mode        = I2S_MODE_MASTER_TX;
    cfg.standard    = I2S_STANDARD_PHILIPS;
    cfg.data_format = I2S_DATA_16B;
    cfg.cpol        = 0U;
    cfg.audio_freq  = 48000U;
    cfg.mclk_output = 1U;

    i2s_init(&cfg);

    i2s_send(SPI2, 0x1234U);
    (void)i2s_recv(SPI2);

    i2s_enable_dma_tx(SPI2);
    i2s_enable_dma_rx(SPI2);
    i2s_disable_dma(SPI2);

    i2s_deinit(SPI2);
}

#endif

#if defined(DRV_USING_USART)

static void usart_compile_smoke(void)
{
    usart_config_t cfg;

    cfg.instance      = USART1;
    cfg.baudrate      = 115200U;
    cfg.word_length   = USART_WORDLEN_8B;
    cfg.stop_bits     = USART_STOP_1;
    cfg.parity        = USART_PARITY_NONE;
    cfg.tx_enable     = 1U;
    cfg.rx_enable     = 1U;
    cfg.oversampling8 = 0U;

    usart_clk_enable(USART1);
    (void)usart_get_clock(USART1);
    (void)usart_get_irqn(USART1);

    usart_init(&cfg);
    usart_enable(USART1);

    usart_send_byte(USART1, 0x55U);
    (void)usart_recv_byte(USART1);
    usart_send_buffer(USART1, (const uint8_t *)"AB", 2U);
    usart_send_string(USART1, "test");

    usart_enable_txe_irq(USART1);
    usart_disable_txe_irq(USART1);
    usart_enable_tc_irq(USART1);
    usart_disable_tc_irq(USART1);
    usart_enable_rxne_irq(USART1);
    usart_disable_rxne_irq(USART1);
    usart_enable_idle_irq(USART1);
    usart_disable_idle_irq(USART1);

    (void)usart_get_flag_txe(USART1);
    (void)usart_get_flag_rxne(USART1);
    (void)usart_get_flag_tc(USART1);
    (void)usart_get_flag_idle(USART1);
    (void)usart_get_flag_ore(USART1);
    (void)usart_get_flag_pe(USART1);
    (void)usart_get_flag_fe(USART1);

    usart_clear_idle_flag(USART1);
    usart_clear_tc_flag(USART1);

    usart_enable_dma(USART1, 1U, 1U);
    usart_disable_dma(USART1);

    usart_enable_lin(USART1);
    usart_disable_lin(USART1);
    usart_enable_half_duplex(USART1);
    usart_disable_half_duplex(USART1);
    usart_enable_irda(USART1, 0U);
    usart_disable_irda(USART1);
    usart_enable_smartcard(USART1, 0U);
    usart_disable_smartcard(USART1);
    usart_enable_clock(USART1, 0U, 0U, 0U);
    usart_disable_clock(USART1);

    usart_disable(USART1);
    usart_deinit(USART1);
}

#endif

#if defined(DRV_USING_SPI)

static void spi_compile_smoke(void)
{
    spi_dev_t dev;

    dev.instance  = SPI1;
    dev.cs_port   = GPIOA;
    dev.cs_pin    = 4U;
    dev.prescaler = 3U;
    dev.cpol      = 0U;
    dev.cpha      = 0U;
    dev.data_16bit = 0U;
    dev.lsb_first = 0U;

    spi_clk_enable(SPI1);

    spi_init(&dev);
    spi_enable(&dev);

    spi_cs_low(&dev);
    spi_cs_high(&dev);

    spi_wait_idle(&dev);
    (void)spi_txrx_byte(&dev, 0xAAU);
    (void)spi_txrx_halfword(&dev, 0x1234U);

    spi_tx_buffer(&dev, (const uint8_t *)"AB", 2U);
    {
        uint8_t rx[2];
        spi_rx_buffer(&dev, rx, 2U);
        (void)rx;
    }
    {
        const uint8_t tx[2] = {0x01U, 0x02U};
        uint8_t rx2[2];
        spi_txrx_buffer(&dev, tx, rx2, 2U);
        (void)rx2;
    }
    spi_transfer_frame(&dev, (const uint8_t *)"A", 1U, (void *)0, 0U);

    spi_enable_rxne_irq(&dev);
    spi_disable_rxne_irq(&dev);
    spi_enable_txe_irq(&dev);
    spi_disable_txe_irq(&dev);
    spi_enable_err_irq(&dev);
    spi_disable_err_irq(&dev);

    (void)spi_is_txe(SPI1);
    (void)spi_is_rxne(SPI1);
    (void)spi_is_busy(SPI1);
    (void)spi_is_ovr(SPI1);
    (void)spi_is_modf(SPI1);
    (void)spi_is_crcerr(SPI1);

#ifdef DRV_USING_DMA
    spi_enable_dma(&dev, 1U, 1U);
    spi_disable_dma(&dev);
#endif

    spi_set_prescaler(&dev, 5U);
    spi_enable_crc(&dev, 0x0007U);
    spi_disable_crc(&dev);
    spi_enable_ti_mode(&dev);
    spi_disable_ti_mode(&dev);
    spi_set_software_nss(&dev, 1U);
    spi_enable_bidirectional_tx(&dev);
    spi_enable_bidirectional_rx(&dev);

    spi_disable(&dev);
    spi_deinit(&dev);
}

#endif

#if defined(DRV_USING_I2C)

static void i2c_compile_smoke(void)
{
    i2c_config_t cfg;

    cfg.instance   = I2C1;
    cfg.clock_hz   = 100000U;
    cfg.own_address = 0x00U;
    cfg.addr_mode  = 0U;
    cfg.duty       = 0U;
    cfg.ack_enable = 1U;

    i2c_clk_enable(I2C1);
    (void)i2c_get_clock(I2C1);
    (void)i2c_get_ev_irqn(I2C1);
    (void)i2c_get_er_irqn(I2C1);

    i2c_init(&cfg);
    i2c_enable(I2C1);

    i2c_enable_ack(I2C1);
    i2c_disable_ack(I2C1);
    i2c_enable_pos(I2C1);
    i2c_disable_pos(I2C1);

    i2c_enable_dma(I2C1);
    i2c_disable_dma(I2C1);
    i2c_enable_dma_last(I2C1);
    i2c_disable_dma_last(I2C1);

    i2c_enable_evt_irq(I2C1);
    i2c_disable_evt_irq(I2C1);
    i2c_enable_buf_irq(I2C1);
    i2c_disable_buf_irq(I2C1);
    i2c_enable_err_irq(I2C1);
    i2c_disable_err_irq(I2C1);

    (void)i2c_is_sb(I2C1);
    (void)i2c_is_addr(I2C1);
    (void)i2c_is_btf(I2C1);
    (void)i2c_is_txe(I2C1);
    (void)i2c_is_rxne(I2C1);
    (void)i2c_is_busy(I2C1);
    (void)i2c_is_af(I2C1);
    (void)i2c_is_berr(I2C1);
    (void)i2c_is_arlo(I2C1);
    (void)i2c_is_ovr(I2C1);

    i2c_clear_af(I2C1);
    i2c_clear_berr(I2C1);
    i2c_clear_arlo(I2C1);
    i2c_clear_ovr(I2C1);

    (void)i2c_get_sr1(I2C1);
    (void)i2c_get_sr2(I2C1);

    i2c_disable(I2C1);
    i2c_deinit(I2C1);
}

#endif

#if defined(DRV_USING_CAN)

static void can_compile_smoke(void)
{
    can_config_t cfg;
    can_filter_config_t fcfg;
    can_frame_t frame;

    cfg.instance        = CAN1;
    cfg.prescaler       = 6U;
    cfg.mode            = 0U;
    cfg.sjw             = 1U;
    cfg.bs1             = 7U;
    cfg.bs2             = 4U;
    cfg.auto_bus_off    = 1U;
    cfg.auto_wake_up    = 0U;
    cfg.auto_retransmit = 1U;

    can_clk_enable(CAN1);
    (void)can_get_tx_irqn(CAN1);
    (void)can_get_rx0_irqn(CAN1);
    (void)can_get_rx1_irqn(CAN1);
    (void)can_get_sce_irqn(CAN1);

    can_init(&cfg);

    fcfg.bank       = 0U;
    fcfg.fifo       = 0U;
    fcfg.scale_32bit = 1U;
    fcfg.mode_mask  = 1U;
    fcfg.enable     = 1U;
    fcfg.id_high    = 0U;
    fcfg.id_low     = 0U;
    fcfg.mask_high  = 0U;
    fcfg.mask_low   = 0U;
    can_filter_init(&fcfg);

    frame.std_id = 0x100U;
    frame.ext_id = 0U;
    frame.ide    = 0U;
    frame.rtr    = 0U;
    frame.dlc    = 2U;
    frame.data[0] = 0xAAU;
    frame.data[1] = 0xBBU;
    (void)can_send(CAN1, &frame);
    can_recv(CAN1, 0U, &frame);
    can_abort_tx(CAN1, 0U);

    can_enable_fifo0_irq(CAN1);
    can_disable_fifo0_irq(CAN1);
    can_enable_fifo1_irq(CAN1);
    can_disable_fifo1_irq(CAN1);
    can_enable_tx_irq(CAN1);
    can_disable_tx_irq(CAN1);
    can_enable_err_irq(CAN1);
    can_disable_err_irq(CAN1);
    can_enable_busoff_irq(CAN1);
    can_disable_busoff_irq(CAN1);
    can_enable_wakeup_irq(CAN1);
    can_disable_wakeup_irq(CAN1);

    can_enter_sleep(CAN1);
    can_exit_sleep(CAN1);

    (void)can_get_esr(CAN1);
    (void)can_get_tsr(CAN1);
    (void)can_get_rf0r(CAN1);
    (void)can_get_rf1r(CAN1);
    (void)can_fifo0_pending(CAN1);
    (void)can_fifo1_pending(CAN1);
}

#endif

#if defined(DRV_USING_SDIO)

static void sdio_compile_smoke(void)
{
    sdio_config_t cfg;

    cfg.clk_div          = 0x76U;
    cfg.clk_edge_rising  = 1U;
    cfg.clk_bypass       = 0U;
    cfg.clk_power_save   = 0U;
    cfg.bus_wide         = 1U;
    cfg.hw_flow_ctrl     = 0U;

    sdio_clk_enable();
    sdio_init(&cfg);

    sdio_power_on();
    sdio_enable_clk_output();
    sdio_set_bus_width(4U);

    sdio_send_cmd(0U, 0U, SDIO_RESP_NONE, 0U);
    (void)sdio_get_resp1();
    (void)sdio_get_resp2();
    (void)sdio_get_resp3();
    (void)sdio_get_resp4();
    (void)sdio_get_cmd_status();

    sdio_data_config(512U, 9U, 0U, 0U);
    (void)sdio_get_data_status();
    sdio_clear_flags(0xFFFFFFFFU);

    sdio_enable_dma();
    sdio_disable_dma();

    sdio_enable_irq(0x01U);
    sdio_disable_irq(0x01U);

    sdio_disable_clk_output();
    sdio_power_off();
}

#endif

#if defined(DRV_USING_IWDG)

static void iwdg_compile_smoke(void)
{
    iwdg_config_t cfg;

    cfg.prescaler = 4U;
    cfg.reload    = 0x0FFFU;

    iwdg_init(&cfg);
    iwdg_reload();
}

#endif

#if defined(DRV_USING_WWDG)

static void wwdg_compile_smoke(void)
{
    wwdg_config_t cfg;

    cfg.prescaler  = 3U;
    cfg.window     = 0x50U;
    cfg.counter    = 0x7FU;
    cfg.ewi_enable = 1U;

    wwdg_clk_enable();
    wwdg_init(&cfg);
    wwdg_refresh(0x7FU);
    wwdg_clear_ewi_flag();
}

#endif

#if defined(DRV_USING_CRC)

static void crc_compile_smoke(void)
{
    uint32_t data[2];

    data[0] = 0x12345678U;
    data[1] = 0x9ABCDEF0U;

    crc_clk_enable();
    crc_reset();
    crc_write(0x01020304U);
    (void)crc_read();
    (void)crc_calc32(data, 2U);
}

#endif

#if defined(DRV_USING_RNG)

static void rng_compile_smoke(void)
{
    rng_clk_enable();
    rng_init();

    (void)rng_data_ready();
    (void)rng_check_error();
    (void)rng_get();

    rng_deinit();
}

#endif

#if defined(DRV_USING_RTC)

static void rtc_compile_smoke(void)
{
    rtc_config_t cfg;
    rtc_time_t time;
    rtc_date_t date;
    rtc_alarm_t alarm;

    cfg.clock_source = RTC_CLOCK_LSI;

    rtc_pwr_clk_enable();
    rtc_backup_access_enable();
    rtc_init(&cfg);

    time.hours   = 12U;
    time.minutes = 30U;
    time.seconds = 0U;
    rtc_set_time(&time);
    rtc_get_time(&time);

    date.year     = 26U;
    date.month    = 4U;
    date.date     = 7U;
    date.week_day = 2U;
    rtc_set_date(&date);
    rtc_get_date(&date);

    alarm.hours            = 12U;
    alarm.minutes          = 30U;
    alarm.seconds          = 10U;
    alarm.mask             = 0U;
    alarm.date_weekday_sel = 0U;
    alarm.date_weekday     = 7U;
    rtc_set_alarm_a(&alarm);
    rtc_enable_alarm_a();
    rtc_enable_alarm_a_irq();
    rtc_clear_alarm_a_flag();
    rtc_disable_alarm_a();

    rtc_set_alarm_b(&alarm);
    rtc_enable_alarm_b();
    rtc_enable_alarm_b_irq();
    rtc_clear_alarm_b_flag();
    rtc_disable_alarm_b();

    rtc_set_wakeup_reload(0x0800U, 0U);
    rtc_enable_wakeup();
    rtc_enable_wakeup_irq();
    rtc_clear_wakeup_flag();
    rtc_disable_wakeup();

    rtc_write_backup(0U, 0xDEADBEEFU);
    (void)rtc_read_backup(0U);
}

#endif

#if defined(DRV_USING_HASH)

static void hash_compile_smoke(void)
{
    uint32_t digest[5];

    hash_clk_enable();
    hash_init(HASH_ALGO_SHA1);

    hash_write(0x12345678U);
    hash_start_digest();
    hash_read_digest(digest, 5U);

    hash_init(HASH_ALGO_MD5);
    hash_write(0xAABBCCDDU);
    hash_start_digest();
    hash_read_digest(digest, 4U);
}

#endif

#if defined(DRV_USING_CRYP)

static void cryp_compile_smoke(void)
{
    uint32_t key[4];
    uint32_t iv[4];

    key[0] = 0x01020304U;
    key[1] = 0x05060708U;
    key[2] = 0x090A0B0CU;
    key[3] = 0x0D0E0F10U;

    iv[0] = 0U;
    iv[1] = 0U;
    iv[2] = 0U;
    iv[3] = 0U;

    cryp_clk_enable();
    cryp_init(CRYP_ALGO_AES_ECB, CRYP_KEYSIZE_128, 0U);
    cryp_set_key(key, CRYP_KEYSIZE_128);
    cryp_set_iv(iv);

    cryp_enable();
    cryp_write(0x11223344U);
    (void)cryp_read();
    cryp_disable();
}

#endif

#if defined(DRV_USING_FSMC)

static void fsmc_compile_smoke(void)
{
    fsmc_norsram_config_t cfg;

    cfg.bank                = 0U;
    cfg.data_width          = 16U;
    cfg.address_data_mux    = 0U;
    cfg.burst_enable        = 0U;
    cfg.write_enable        = 1U;
    cfg.extended_mode       = 0U;
    cfg.wait_enable         = 0U;
    cfg.write_timing_addset = 1U;
    cfg.write_timing_datast = 2U;
    cfg.read_timing_addset  = 1U;
    cfg.read_timing_datast  = 2U;

    fsmc_clk_enable();
    fsmc_norsram_init(&cfg);
}

#endif

#if defined(DRV_USING_DCMI)

static void dcmi_compile_smoke(void)
{
    dcmi_config_t cfg;

    cfg.capture_rate            = 0U;
    cfg.hsync_polarity          = 0U;
    cfg.vsync_polarity          = 0U;
    cfg.pixel_clock_polarity    = 1U;
    cfg.capture_mode_continuous = 1U;
    cfg.extended_data_mode      = 0U;

    dcmi_clk_enable();
    dcmi_init(&cfg);

    dcmi_enable();
    dcmi_start();
    dcmi_stop();
    dcmi_disable();
}

#endif

#if defined(DRV_USING_ETH)

static void eth_compile_smoke(void)
{
    eth_config_t cfg;
    uint8_t mac[6];

    cfg.full_duplex      = 1U;
    cfg.speed_100m       = 1U;
    cfg.checksum_offload = 1U;
    cfg.broadcast_enable = 1U;
    cfg.promiscuous_mode = 0U;
    cfg.rx_interrupt     = 1U;
    cfg.tx_interrupt     = 0U;

    eth_clk_enable();
    eth_init(&cfg);

    mac[0] = 0x02U;
    mac[1] = 0x00U;
    mac[2] = 0x00U;
    mac[3] = 0x00U;
    mac[4] = 0x00U;
    mac[5] = 0x01U;
    eth_set_mac_addr(0U, mac);

    (void)eth_mii_read(0U, 0U);
    eth_mii_write(0U, 0U, 0x1234U);

    eth_start();

    eth_resume_rx();
    eth_resume_tx();
    eth_enable_rx_irq();
    eth_enable_tx_irq();
    (void)eth_get_dma_status();
    eth_clear_dma_status(0xFFFFFFFFU);

    eth_stop();
}

#endif

#if defined(DRV_USING_USB_OTG_FS)

static void usb_otg_fs_compile_smoke(void)
{
    usb_otg_fs_clk_enable();
    usb_otg_fs_init();

    usb_otg_fs_enable_global_irq();
    usb_otg_fs_disable_global_irq();

    usb_otg_fs_set_rx_fifo(128U);
    usb_otg_fs_set_tx_fifo(0U, 128U, 64U);
    usb_otg_fs_set_tx_fifo(1U, 192U, 64U);

    usb_otg_fs_device_init();
    usb_otg_fs_host_init();

    usb_otg_fs_ep_set_mps(0U, 0U);
    usb_otg_fs_ep_set_mps(1U, 64U);
    usb_otg_fs_hc_set_mps(0U, 64U);
    usb_otg_fs_ep_in_enable(1U);
    usb_otg_fs_ep_out_enable(1U);
}

#endif

#if defined(DRV_USING_USB_OTG_HS)

static void usb_otg_hs_compile_smoke(void)
{
    usb_otg_hs_clk_enable();
    usb_otg_hs_init();

    usb_otg_hs_enable_global_irq();
    usb_otg_hs_disable_global_irq();

    usb_otg_hs_set_rx_fifo(256U);
    usb_otg_hs_set_tx_fifo(0U, 256U, 128U);
    usb_otg_hs_set_tx_fifo(1U, 384U, 128U);

    usb_otg_hs_device_init();
    usb_otg_hs_host_init();
}

#endif

#if defined(DRV_USING_DBGMCU)

static void dbgmcu_compile_smoke(void)
{
    (void)dbgmcu_get_dev_id();
    (void)dbgmcu_get_rev_id();

    dbgmcu_freeze_tim2();
    dbgmcu_freeze_tim3();
    dbgmcu_freeze_tim4();
    dbgmcu_freeze_tim5();
    dbgmcu_freeze_iwdg();
    dbgmcu_freeze_wwdg();
}

#endif
