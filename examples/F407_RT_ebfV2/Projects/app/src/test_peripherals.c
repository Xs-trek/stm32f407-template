/*
 * test_peripherals.c — 所有外设测试函数实现
 *
 * 依据：
 * - 所有 API 均来自实际 drv_*.h 头文件，无臆测
 * - 单字节 I2C 读写：i2c_mem_write / i2c_mem_read
 * - EXTI 测试因自动化不使用按键，标记 SKIP
 * - CRYP / HASH / USB 驱动不可用，标记 SKIP
 */

#include <rtthread.h>
#include "board.h"
#include "f407_rt_modules.h"
#include "test_runner.h"
#include "test_peripherals.h"

/* ================================================================== */
/*  1. drv_rcc                                                         */
/* ================================================================== */
test_result_t test_drv_rcc(void)
{
    const char   *MOD = "drv_rcc";
    test_result_t res = TEST_PASS;

    uint32_t sysclk = rcc_get_sysclk();
    uint32_t hclk   = rcc_get_hclk();
    uint32_t pclk1  = rcc_get_pclk1();
    uint32_t pclk2  = rcc_get_pclk2();

    if (sysclk != 168000000UL)
    {
        res = TEST_FAIL;
    }

    TEST_LOG(res, MOD, "sysclk=%u hclk=%u pclk1=%u pclk2=%u",
             (unsigned)sysclk, (unsigned)hclk,
             (unsigned)pclk1,  (unsigned)pclk2);

    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  2. drv_scb                                                         */
/* ================================================================== */
test_result_t test_drv_scb(void)
{
    const char   *MOD = "drv_scb";
    test_result_t res = TEST_PASS;

    uint32_t cpuid = SCB->CPUID;

    /* Cortex-M4: Implementer=0x41, PartNo=0xC24 */
    if ((cpuid & 0xFF00FFF0UL) != 0x4100C240UL)
    {
        res = TEST_FAIL;
    }

    TEST_LOG(res, MOD, "SCB->CPUID=0x%08X", (unsigned)cpuid);
    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  3. drv_dbgmcu                                                      */
/* ================================================================== */
test_result_t test_drv_dbgmcu(void)
{
    const char   *MOD = "drv_dbgmcu";
    test_result_t res = TEST_PASS;

    uint16_t dev_id = dbgmcu_get_dev_id();
    uint16_t rev_id = dbgmcu_get_rev_id();

    /* STM32F407 DEV_ID = 0x413 */
    if (dev_id != 0x413U)
    {
        res = TEST_FAIL;
    }

    TEST_LOG(res, MOD, "DEV_ID=0x%03X REV_ID=0x%04X",
             (unsigned)dev_id, (unsigned)rev_id);

    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  4. drv_crc                                                         */
/* ================================================================== */
test_result_t test_drv_crc(void)
{
    const char   *MOD = "drv_crc";
    test_result_t res = TEST_PASS;

    static const uint32_t data[] = {
        0x12345678UL, 0x9ABCDEF0UL,
        0x00000001UL, 0xDEADBEEFUL
    };
    const uint32_t len = sizeof(data) / sizeof(data[0]);

    crc_clk_enable();

    crc_reset();
    uint32_t crc1 = crc_calc32(data, len);

    crc_reset();
    uint32_t crc2 = crc_calc32(data, len);

    if (crc1 != crc2 || crc1 == 0xFFFFFFFFUL)
    {
        res = TEST_FAIL;
    }

    TEST_LOG(res, MOD, "CRC32=0x%08X consistent=%s",
             (unsigned)crc1, (crc1 == crc2) ? "yes" : "no");

    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  5. drv_pwr — reset flags 位于 RCC                                 */
/* ================================================================== */
test_result_t test_drv_pwr(void)
{
    const char   *MOD = "drv_pwr";
    test_result_t res = TEST_PASS;

    uint32_t flags = rcc_get_reset_flags();

    TEST_LOG(res, MOD,
             "RCC_CSR=0x%08X POR=%u PIN=%u SFT=%u IWDG=%u WWDG=%u LPWR=%u",
             (unsigned)flags,
             (unsigned)((flags >> 27) & 1U),
             (unsigned)((flags >> 26) & 1U),
             (unsigned)((flags >> 28) & 1U),
             (unsigned)((flags >> 29) & 1U),
             (unsigned)((flags >> 30) & 1U),
             (unsigned)((flags >> 31) & 1U));

    rcc_clear_reset_flags();

    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  6. drv_syscfg — IO 补偿单元，直接操作寄存器                        */
/* ================================================================== */
test_result_t test_drv_syscfg(void)
{
    const char   *MOD = "drv_syscfg";
    test_result_t res = TEST_PASS;

    syscfg_clk_enable();

    /* 使能补偿单元：CMPCR bit0 */
    SYSCFG->CMPCR |= SYSCFG_CMPCR_CMP_PD;

    /* 等待 READY bit8，超时 1000ms */
    uint32_t timeout = 1000U;
    while (!(SYSCFG->CMPCR & SYSCFG_CMPCR_READY) && timeout > 0U)
    {
        rt_thread_mdelay(1U);
        timeout--;
    }

    if (timeout == 0U)
    {
        res = TEST_TIMEOUT;
    }

    TEST_LOG(res, MOD, "IO compensation %s CMPCR=0x%08X",
             (res == TEST_PASS) ? "ready" : "timeout",
             (unsigned)SYSCFG->CMPCR);

    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  7. drv_nvic — 通过 scb_get_aircr 读优先级分组                      */
/* ================================================================== */
test_result_t test_drv_nvic(void)
{
    const char   *MOD = "drv_nvic";
    test_result_t res = TEST_PASS;

    uint32_t aircr    = scb_get_aircr();
    uint32_t prigroup = (aircr >> 8U) & 0x07U;

    /* RT-Thread Nano 默认 Group4 → PRIGROUP=3 */
    if (prigroup != 3U)
    {
        res = TEST_FAIL;
    }

    TEST_LOG(res, MOD, "PRIGROUP=%u (expect 3 = Group4)",
             (unsigned)prigroup);

    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  8. drv_flash — 读芯片 Flash size 寄存器和 UID 寄存器               */
/* ================================================================== */
test_result_t test_drv_flash(void)
{
    const char   *MOD = "drv_flash";
    test_result_t res = TEST_PASS;

    /* Flash size 寄存器地址：F407 = 0x1FFF7A22，单位 KB */
    uint16_t flash_kb = *((volatile uint16_t *)0x1FFF7A22UL);

    /* STM32F407ZGT6 = 1024 KB */
    if (flash_kb != 1024U)
    {
        res = TEST_FAIL;
    }

    /* UID 寄存器：96bit，起始地址 0x1FFF7A10 */
    uint32_t uid0 = *((volatile uint32_t *)0x1FFF7A10UL);
    uint32_t uid1 = *((volatile uint32_t *)0x1FFF7A14UL);
    uint32_t uid2 = *((volatile uint32_t *)0x1FFF7A18UL);

    TEST_LOG(res, MOD, "Flash=%uKB UID=%08X-%08X-%08X",
             (unsigned)flash_kb,
             (unsigned)uid0, (unsigned)uid1, (unsigned)uid2);

    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  9. drv_rng                                                         */
/* ================================================================== */
test_result_t test_drv_rng(void)
{
    const char   *MOD = "drv_rng";
    test_result_t res = TEST_PASS;

    rng_clk_enable();
    rng_init();

    /* 等待数据就绪，超时 100ms */
    uint32_t timeout = 100U;
    while (!rng_data_ready() && timeout > 0U)
    {
        rt_thread_mdelay(1U);
        timeout--;
    }

    if (timeout == 0U || rng_check_error() != 0)
    {
        res = TEST_FAIL;
        TEST_LOG(res, MOD, "RNG not ready or error");
        runner_record(MOD, res);
        return res;
    }

    uint32_t r1 = rng_get();
    uint32_t r2 = rng_get();

    /* 两次连续随机数相同概率极低，相同视为异常 */
    if (r1 == r2)
    {
        res = TEST_FAIL;
    }

    TEST_LOG(res, MOD, "r1=0x%08X r2=0x%08X",
             (unsigned)r1, (unsigned)r2);

    rng_deinit();
    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  10. drv_gpio — LED_R/G 逐个闪烁，LED_B 由心跳线程控制不干扰       */
/* ================================================================== */
test_result_t test_drv_gpio(void)
{
    const char   *MOD = "drv_gpio";
    test_result_t res = TEST_PASS;

    gpio_clk_enable(LED_R_PORT);
    gpio_clk_enable(LED_G_PORT);

    gpio_init_output_pp(LED_R_PORT, LED_R_PIN);
    gpio_init_output_pp(LED_G_PORT, LED_G_PIN);

    /* 熄灭（低电平点亮，高电平熄灭） */
    gpio_set(LED_R_PORT, LED_R_PIN);
    gpio_set(LED_G_PORT, LED_G_PIN);

    /* LED_R 点亮 500ms */
    gpio_reset(LED_R_PORT, LED_R_PIN);
    rt_thread_mdelay(500U);
    gpio_set(LED_R_PORT, LED_R_PIN);

    /* LED_G 点亮 500ms */
    gpio_reset(LED_G_PORT, LED_G_PIN);
    rt_thread_mdelay(500U);
    gpio_set(LED_G_PORT, LED_G_PIN);

    /* 验证 ODR 熄灭状态为高 */
    uint8_t r = gpio_read_output(LED_R_PORT, LED_R_PIN);
    uint8_t g = gpio_read_output(LED_G_PORT, LED_G_PIN);

    if (r != 1U || g != 1U)
    {
        res = TEST_FAIL;
    }

    TEST_LOG(res, MOD,
             "LED_R ODR=%u LED_G ODR=%u (observe R then G blink)",
             (unsigned)r, (unsigned)g);

    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  11. drv_usart                                                      */
/* ================================================================== */
test_result_t test_drv_usart(void)
{
    const char   *MOD = "drv_usart";
    test_result_t res = TEST_PASS;

    /* USART1 已由 board.c 初始化，直接调用 */
    usart_send_string(CONSOLE_USART, "drv_usart test: 0123456789\r\n");

    TEST_LOG(res, MOD, "USART1 send_string OK");
    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  12. drv_tim_basic — TIM6 500ms 定时误差验证                        */
/* ================================================================== */
test_result_t test_drv_tim_basic(void)
{
    const char   *MOD = "drv_tim";
    test_result_t res = TEST_PASS;

    tim_clk_enable(TIM6);

    /*
     * TIM6 挂 APB1，timclk1 = 84MHz
     * PSC=8399 → 84MHz/8400 = 10kHz
     * ARR=4999 → 10kHz/5000 = 2Hz = 500ms
     */
    tim_base_config_t cfg;
    cfg.instance  = TIM6;
    cfg.prescaler = 8399U;
    cfg.period    = 4999U;
    cfg.dir       = TIM_DIR_UP;

    tim_base_init(&cfg);
    tim_clear_update_flag(TIM6);

    rt_tick_t t0 = rt_tick_get();
    tim_start(TIM6);

    /* 轮询 UIF，超时 600ms */
    uint32_t timeout = 600U;
    while (!tim_get_update_flag(TIM6) && timeout > 0U)
    {
        rt_thread_mdelay(1U);
        timeout--;
    }

    tim_stop(TIM6);
    rt_tick_t t1 = rt_tick_get();

    if (timeout == 0U)
    {
        res = TEST_TIMEOUT;
        TEST_LOG(res, MOD, "TIM6 500ms poll timeout");
        runner_record(MOD, res);
        return res;
    }

    uint32_t elapsed_ms = (uint32_t)((t1 - t0) * 1000U / RT_TICK_PER_SECOND);

    /* 允许 ±30ms 误差 */
    if (elapsed_ms < 470U || elapsed_ms > 530U)
    {
        res = TEST_FAIL;
    }

    tim_clear_update_flag(TIM6);

    TEST_LOG(res, MOD, "TIM6 500ms elapsed=%ums", (unsigned)elapsed_ms);
    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  13. drv_tim_pwm — TIM10 CH1 → LED_R PF6 AF3                      */
/* ================================================================== */
test_result_t test_drv_tim_pwm(void)
{
    const char   *MOD = "drv_tim_pwm";
    test_result_t res = TEST_PASS;

    tim_clk_enable(TIM10);

    /*
     * TIM10 挂 APB2，timclk2 = 168MHz
     * PSC=167, ARR=999 → 168MHz/168/1000 = 1kHz
     * 50% duty: CCR1=499
     */
    tim_base_config_t base_cfg;
    base_cfg.instance  = TIM10;
    base_cfg.prescaler = 167U;
    base_cfg.period    = 999U;
    base_cfg.dir       = TIM_DIR_UP;

    tim_base_init(&base_cfg);

    tim_oc_config_t oc_cfg;
    oc_cfg.mode           = TIM_OC_MODE_PWM1;
    oc_cfg.pulse          = 499U;    /* 50% duty */
    oc_cfg.polarity_low   = 0U;      /* 高电平有效 */
    oc_cfg.preload_enable = 1U;

    tim_oc_init(TIM10, 1U, &oc_cfg);

    /* PF6 配置为 TIM10_CH1 AF3 */
    gpio_clk_enable(LED_R_PORT);
    gpio_init_af_pp(LED_R_PORT, LED_R_PIN, LED_R_AF);

    tim_main_output_enable(TIM10);
    tim_generate_update(TIM10);
    tim_start(TIM10);

    rt_thread_mdelay(1000U);    /* 观察 LED_R 半亮 1s */

    /* 改为 10% 占空比 */
    tim_set_compare(TIM10, 1U, 99U);
    rt_thread_mdelay(500U);     /* 观察 LED_R 更暗 */

    /* 停止，恢复引脚为普通输出并熄灭 */
    tim_stop(TIM10);
    gpio_init_output_pp(LED_R_PORT, LED_R_PIN);
    gpio_set(LED_R_PORT, LED_R_PIN);

    TEST_LOG(res, MOD, "TIM10 CH1 1kHz 50%%->10%% (observe LED_R dim)");
    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  14. drv_adc — PB0 电位器 + 温度 CH16 + VREFINT CH17               */
/* ================================================================== */
test_result_t test_drv_adc(void)
{
    const char   *MOD = "drv_adc";
    test_result_t res = TEST_PASS;

    /* PB0 模拟输入 */
    gpio_clk_enable(GPIOB);
    gpio_init_analog(GPIOB, 0U);

	  adc_clk_enable(POT_ADC);
	
    /* ADC 公共预分频：PCLK2=84MHz → DIV4=21MHz（ADC最大36MHz） */
    adc_set_prescaler(ADC_PRESCALER_DIV4);
    adc_enable_temp_vrefint();

    adc_config_t cfg;
    cfg.instance   = POT_ADC;
    cfg.resolution = ADC_RES_12BIT;
    cfg.align      = ADC_ALIGN_RIGHT;
    cfg.continuous = 0U;
    cfg.scan       = 0U;

    adc_init(&cfg);
    adc_enable(POT_ADC);
		
		rt_thread_mdelay(10U);

    /* 采样时间：温度和 VREFINT 要求最低 10us，用 480 周期（@21MHz ≈ 22.8us） */
    uint16_t raw_pot  = adc_read_channel(POT_ADC, POT_CHANNEL, ADC_SAMPLE_56CYCLES);
    uint16_t raw_temp = adc_read_channel(POT_ADC, 16U,         ADC_SAMPLE_480CYCLES);
    uint16_t raw_vref = adc_read_channel(POT_ADC, 17U,         ADC_SAMPLE_480CYCLES);

    /* VREFINT 典型 ~1.21V，12bit 3.3V参考：raw ≈ 1500，允许 1300~1700 */
    if (raw_vref < 1300U || raw_vref > 1700U)
    {
        res = TEST_FAIL;
    }

    uint32_t pot_mv  = (uint32_t)raw_pot  * 3300UL / 4095UL;
    uint32_t vref_mv = (uint32_t)raw_vref * 3300UL / 4095UL;

    /* 温度粗略估算（V25=0.76V, Slope=2.5mV/°C） */
    int32_t vsense_mv = (int32_t)((uint32_t)raw_temp * 3300UL / 4095UL);
    int32_t temp_c    = (760L - vsense_mv) * 10L / 25L + 25L;

		(void)pot_mv;
    (void)vref_mv;
    (void)temp_c;
		
    TEST_LOG(res, MOD,
             "pot=%u(%umV) temp_raw=%u(~%dC) vref=%u(%umV)",
             (unsigned)raw_pot,  (unsigned)pot_mv,
             (unsigned)raw_temp, (int)temp_c,
             (unsigned)raw_vref, (unsigned)vref_mv);

    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  15. drv_dac — PA4 输出 ~1.65V，读回 DOR 验证                       */
/* ================================================================== */
test_result_t test_drv_dac(void)
{
    const char   *MOD = "drv_dac";
    test_result_t res = TEST_PASS;

    /* PA4 模拟模式，避免 GPIO 驱动干扰 DAC 输出 */
    gpio_clk_enable(DAC_CH1_PORT);
    gpio_init_analog(DAC_CH1_PORT, DAC_CH1_PIN);

    dac_clk_enable();
    dac_init(DAC_CHANNEL_1);
    dac_enable(DAC_CHANNEL_1);

    /* 输出 ~1.65V：4095 * 1650 / 3300 = 2047 */
    const uint16_t SET_VAL = 2047U;
    dac_write_12r(DAC_CHANNEL_1, SET_VAL);
    rt_thread_mdelay(10U);

    uint16_t readback = dac_read_output(DAC_CHANNEL_1);

    /* 允许 ±5 LSB */
    int32_t diff = (int32_t)readback - (int32_t)SET_VAL;
    if (diff < -5 || diff > 5)
    {
        res = TEST_FAIL;
    }

    uint32_t out_mv = (uint32_t)readback * 3300UL / 4095UL;
    (void)out_mv;
		
    TEST_LOG(res, MOD, "DAC1 set=%u readback=%u (~%umV)",
             (unsigned)SET_VAL, (unsigned)readback, (unsigned)out_mv);

    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  16. drv_dma — DMA2 Stream0 M2M 搬运 64 字节                       */
/* ================================================================== */
#define DMA_TEST_SIZE 64U

static uint8_t s_dma_src[DMA_TEST_SIZE];
static uint8_t s_dma_dst[DMA_TEST_SIZE];

test_result_t test_drv_dma(void)
{
    const char   *MOD = "drv_dma";
    test_result_t res = TEST_PASS;

    for (uint32_t i = 0U; i < DMA_TEST_SIZE; i++)
    {
        s_dma_src[i] = (uint8_t)(i ^ 0xA5U);
    }
    rt_memset(s_dma_dst, 0, DMA_TEST_SIZE);

    dma_clk_enable(DMA2);

    /*
     * M2M 必须使用 DMA2
     * Stream0, Channel0, M2M, 字节，源和目标均自增
     */
    dma_config_t cfg;
    cfg.stream        = DMA2_Stream0;
    cfg.channel       = 0U;
    cfg.periph_addr   = (uint32_t)s_dma_src;   /* M2M：源地址放 PAR */
    cfg.mem0_addr     = (uint32_t)s_dma_dst;
    cfg.mem1_addr     = 0U;
    cfg.count         = DMA_TEST_SIZE;
    cfg.dir           = DMA_DIR_M2M;
    cfg.p_size        = DMA_SIZE_BYTE;
    cfg.m_size        = DMA_SIZE_BYTE;
    cfg.p_inc         = 1U;
    cfg.m_inc         = 1U;
    cfg.circular      = 0U;
    cfg.double_buffer = 0U;
    cfg.priority      = DMA_PRIORITY_LOW;
    cfg.use_fifo      = 1U;                     /* M2M 强制 FIFO */
    cfg.fifo_threshold= DMA_FIFO_TH_FULL;
    cfg.mem_burst     = DMA_BURST_SINGLE;
    cfg.periph_burst  = DMA_BURST_SINGLE;
    cfg.enable_tc_irq = 0U;
    cfg.enable_ht_irq = 0U;
    cfg.enable_te_irq = 0U;

    dma_init(&cfg);
    dma_start(DMA2_Stream0);

    /* 轮询 TC，超时 100ms */
    uint32_t timeout = 100U;
    while (!dma_get_flag_tc(DMA2_Stream0) && timeout > 0U)
    {
        rt_thread_mdelay(1U);
        timeout--;
    }

    dma_stop(DMA2_Stream0);

    if (timeout == 0U)
    {
        res = TEST_TIMEOUT;
        TEST_LOG(res, MOD, "DMA2 M2M timeout");
        runner_record(MOD, res);
        return res;
    }

    if (rt_memcmp(s_dma_src, s_dma_dst, DMA_TEST_SIZE) != 0)
    {
        res = TEST_FAIL;
    }

    TEST_LOG(res, MOD, "DMA2 Stream0 M2M 64B %s",
             (res == TEST_PASS) ? "match" : "MISMATCH");

    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  17. drv_spi — W25Q128JV JEDEC ID                                  */
/* ================================================================== */
#define W25Q128_JEDEC_ID  0xEF4018UL

static spi_dev_t s_flash_spi;

test_result_t test_drv_spi(void)
{
    const char   *MOD = "drv_spi";
    test_result_t res = TEST_PASS;

    /* GPIO 初始化 */
    gpio_clk_enable(FLASH_SCK_PORT);
    gpio_clk_enable(FLASH_CS_PORT);

    gpio_init_af_pp(FLASH_SCK_PORT,  FLASH_SCK_PIN,  FLASH_SPI_AF);
    gpio_init_af_pp(FLASH_MOSI_PORT, FLASH_MOSI_PIN, FLASH_SPI_AF);
    /* MISO 推挽即可，SPI 主机模式下 MISO 为输入，AF 配置决定方向 */
    gpio_init_af_pp(FLASH_MISO_PORT, FLASH_MISO_PIN, FLASH_SPI_AF);

    /* CS 普通输出，先拉高 */
    gpio_init_output_pp(FLASH_CS_PORT, FLASH_CS_PIN);
    gpio_set(FLASH_CS_PORT, FLASH_CS_PIN);

    /* 构造 spi_dev_t */
    s_flash_spi.instance   = FLASH_SPI;
    s_flash_spi.cs_port    = FLASH_CS_PORT;
    s_flash_spi.cs_pin     = FLASH_CS_PIN;
    s_flash_spi.prescaler  = 1U;    /* fPCLK/4，SPI1 挂 APB2=84MHz → 21MHz */
    s_flash_spi.cpol       = 0U;    /* Mode0 */
    s_flash_spi.cpha       = 0U;
    s_flash_spi.data_16bit = 0U;    /* 8bit */
    s_flash_spi.lsb_first  = 0U;    /* MSB first */

    spi_clk_enable(FLASH_SPI);
    spi_init(&s_flash_spi);
    spi_enable(&s_flash_spi);

    /* 读 JEDEC ID：0x9F + 3字节响应 */
    spi_cs_low(&s_flash_spi);
    spi_txrx_byte(&s_flash_spi, 0x9FU);
    uint8_t m0 = spi_txrx_byte(&s_flash_spi, 0xFFU);
    uint8_t m1 = spi_txrx_byte(&s_flash_spi, 0xFFU);
    uint8_t m2 = spi_txrx_byte(&s_flash_spi, 0xFFU);
    spi_cs_high(&s_flash_spi);

    uint32_t id = ((uint32_t)m0 << 16U) |
                  ((uint32_t)m1 <<  8U) |
                   (uint32_t)m2;

    if (id != W25Q128_JEDEC_ID)
    {
        res = TEST_FAIL;
    }

    TEST_LOG(res, MOD, "W25Q128 ID=0x%06X expect=0x%06X",
             (unsigned)id, (unsigned)W25Q128_JEDEC_ID);

    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  18. drv_i2c — AT24C02 + MPU6050 + AP3216C（单字节读写）           */
/* ================================================================== */
test_result_t test_drv_i2c(void)
{
    const char   *MOD = "drv_i2c";
    test_result_t res = TEST_PASS;

    /* SCL/SDA：开漏 + 上拉 AF4 */
    gpio_clk_enable(SENSOR_SCL_PORT);
    gpio_init_af_od(SENSOR_SCL_PORT, SENSOR_SCL_PIN,
                    SENSOR_I2C_AF, GPIO_PULL_UP);
    gpio_init_af_od(SENSOR_SDA_PORT, SENSOR_SDA_PIN,
                    SENSOR_I2C_AF, GPIO_PULL_UP);

    i2c_clk_enable(SENSOR_I2C);
	
	  i2c_config_t i2c_cfg;
    i2c_cfg.instance    = SENSOR_I2C;
    i2c_cfg.clock_hz    = 400000U;    /* 快速模式 400kHz */
    i2c_cfg.own_address = 0x00U;      /* 主机模式，自身地址不使用 */
    i2c_cfg.addr_mode   = 0U;         /* 7 位地址 */
    i2c_cfg.duty        = 0U;         /* Tlow/Thigh = 2 */
    i2c_cfg.ack_enable  = 1U;

    i2c_init(&i2c_cfg); 
    i2c_enable(SENSOR_I2C);

    /* ---- AT24C02：写地址 0x00 写入 0xA5，读回比较 ---- */
    i2c_mem_write(SENSOR_I2C, AT24C02_ADDR, 0x00U, 0xA5U);
    rt_thread_mdelay(10U);   /* EEPROM 写周期 ≤ 5ms */

    uint8_t rd = i2c_mem_read(SENSOR_I2C, AT24C02_ADDR, 0x00U);
    uint8_t at_ok = (rd == 0xA5U) ? 1U : 0U;

    if (!at_ok) { res = TEST_FAIL; }

    rt_kprintf("  AT24C02: wr=0xA5 rd=0x%02X -> %s\r\n",
               (unsigned)rd, at_ok ? "OK" : "FAIL");

    /* ---- MPU6050：读 WHO_AM_I 寄存器 0x75，期望 0x68 ---- */
    uint8_t who = i2c_mem_read(SENSOR_I2C, MPU6050_ADDR, 0x75U);
    uint8_t mpu_ok = (who == 0x68U) ? 1U : 0U;

    if (!mpu_ok) { res = TEST_FAIL; }

    rt_kprintf("  MPU6050: WHO_AM_I=0x%02X -> %s\r\n",
               (unsigned)who, mpu_ok ? "OK" : "FAIL");

    /* ---- AP3216C：复位 → 待机模式 → 全功能模式 ---- */
    /* 写 System Register(0x00) = 0x04 复位 */
    i2c_mem_write(SENSOR_I2C, AP3216C_ADDR, 0x00U, 0x04U);
    rt_thread_mdelay(10U);

    /* 写入待机模式 0x00 */
    i2c_mem_write(SENSOR_I2C, AP3216C_ADDR, 0x00U, 0x00U);
    rt_thread_mdelay(10U);

    /* 读回，期望 0x00 */
    uint8_t ap_mode = i2c_mem_read(SENSOR_I2C, AP3216C_ADDR, 0x00U);

    /* 写入全功能模式 0x03 */
    i2c_mem_write(SENSOR_I2C, AP3216C_ADDR, 0x00U, 0x03U);
    rt_thread_mdelay(10U);

    /* 读回，期望 0x03 */
    uint8_t ap_readback = i2c_mem_read(SENSOR_I2C, AP3216C_ADDR, 0x00U);

    uint8_t ap_ok = (ap_mode == 0x00U && ap_readback == 0x03U) ? 1U : 0U;
    if (!ap_ok) { res = TEST_FAIL; }

    rt_kprintf("  AP3216C: mode_rd=0x%02X(exp 0x00) active_rd=0x%02X(exp 0x03) -> %s\r\n",
           (unsigned)ap_mode, (unsigned)ap_readback,
           ap_ok ? "OK" : "FAIL");

    TEST_LOG(res, MOD, "AT24C02+MPU6050+AP3216C");
    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  19. drv_exti — 自动化测试不使用按键，SKIP                          */
/* ================================================================== */
test_result_t test_drv_exti(void)
{
    const char   *MOD = "drv_exti";
    test_result_t res = TEST_SKIP;

    TEST_LOG(res, MOD, "automated test: no keypress, skipped");
    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  20. drv_rtc — 设时间，延时 2s，读回秒数验证                        */
/* ================================================================== */
test_result_t test_drv_rtc(void)
{
    const char   *MOD = "drv_rtc";
    test_result_t res = TEST_PASS;

    rtc_pwr_clk_enable();
    rtc_backup_access_enable();

    rtc_config_t rtc_cfg;
    rtc_cfg.clock_source = RTC_CLOCK_LSI;
    rtc_init(&rtc_cfg);

    /* RTC 时钟域稳定需要时间，等待 10ms */
    rt_thread_mdelay(10U);

    rtc_time_t set_t;
    set_t.hours   = 12U;
    set_t.minutes = 0U;
    set_t.seconds = 0U;
    rtc_set_time(&set_t);

    rt_thread_mdelay(2000U);

    rtc_time_t get_t;
    rtc_get_time(&get_t);

    if (get_t.hours != 12U || get_t.seconds < 1U || get_t.seconds > 4U)
    {
        res = TEST_FAIL;
    }

    TEST_LOG(res, MOD, "set 12:00:00 read %02u:%02u:%02u",
             (unsigned)get_t.hours,
             (unsigned)get_t.minutes,
             (unsigned)get_t.seconds);

    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  21. drv_fsmc — IS62WV51216 SRAM 写读验证                          */
/* ================================================================== */
/*
 * 霸天虎 V2：FSMC NE3 → Bank1 Region3 基地址 0x68000000
 * IS62WV51216：16bit 宽，时序参数依据数据手册计算（HCLK=168MHz，周期≈5.95ns）
 */
#define SRAM_BASE_ADDR   0x6C000000UL
#define SRAM_TEST_WORDS  128U   /* 128 × 16bit = 256 字节 */

test_result_t test_drv_fsmc(void)
{
    const char   *MOD = "drv_fsmc";
    test_result_t res = TEST_PASS;

    /* ---- GPIO 初始化（板级，霸天虎V2 FSMC SRAM NE3） ---- */

    /* 使能端口时钟 */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN
                  | RCC_AHB1ENR_GPIOEEN
                  | RCC_AHB1ENR_GPIOFEN
                  | RCC_AHB1ENR_GPIOGEN;
    (void)RCC->AHB1ENR;

    /* 数据线 D0~D15 */
    gpio_init_af_pp(GPIOD, 14U, 12U);   /* D0  */
    gpio_init_af_pp(GPIOD, 15U, 12U);   /* D1  */
    gpio_init_af_pp(GPIOD,  0U, 12U);   /* D2  */
    gpio_init_af_pp(GPIOD,  1U, 12U);   /* D3  */
    gpio_init_af_pp(GPIOE,  7U, 12U);   /* D4  */
    gpio_init_af_pp(GPIOE,  8U, 12U);   /* D5  */
    gpio_init_af_pp(GPIOE,  9U, 12U);   /* D6  */
    gpio_init_af_pp(GPIOE, 10U, 12U);   /* D7  */
    gpio_init_af_pp(GPIOE, 11U, 12U);   /* D8  */
    gpio_init_af_pp(GPIOE, 12U, 12U);   /* D9  */
    gpio_init_af_pp(GPIOE, 13U, 12U);   /* D10 */
    gpio_init_af_pp(GPIOE, 14U, 12U);   /* D11 */
    gpio_init_af_pp(GPIOE, 15U, 12U);   /* D12 */
    gpio_init_af_pp(GPIOD,  8U, 12U);   /* D13 */
    gpio_init_af_pp(GPIOD,  9U, 12U);   /* D14 */
    gpio_init_af_pp(GPIOD, 10U, 12U);   /* D15 */

    /* 地址线 A0~A18 */
    gpio_init_af_pp(GPIOF,  0U, 12U);   /* A0  */
    gpio_init_af_pp(GPIOF,  1U, 12U);   /* A1  */
    gpio_init_af_pp(GPIOF,  2U, 12U);   /* A2  */
    gpio_init_af_pp(GPIOF,  3U, 12U);   /* A3  */
    gpio_init_af_pp(GPIOF,  4U, 12U);   /* A4  */
    gpio_init_af_pp(GPIOF,  5U, 12U);   /* A5  */
    gpio_init_af_pp(GPIOF, 12U, 12U);   /* A6  */
    gpio_init_af_pp(GPIOF, 13U, 12U);   /* A7  */
    gpio_init_af_pp(GPIOF, 14U, 12U);   /* A8  */
    gpio_init_af_pp(GPIOF, 15U, 12U);   /* A9  */
    gpio_init_af_pp(GPIOG,  0U, 12U);   /* A10 */
    gpio_init_af_pp(GPIOG,  1U, 12U);   /* A11 */
    gpio_init_af_pp(GPIOG,  2U, 12U);   /* A12 */
    gpio_init_af_pp(GPIOG,  3U, 12U);   /* A13 */
    gpio_init_af_pp(GPIOG,  4U, 12U);   /* A14 */
    gpio_init_af_pp(GPIOG,  5U, 12U);   /* A15 */
    gpio_init_af_pp(GPIOD, 11U, 12U);   /* A16 */
    gpio_init_af_pp(GPIOD, 12U, 12U);   /* A17 */
    gpio_init_af_pp(GPIOD, 13U, 12U);   /* A18 */

    /* 控制线 */
    gpio_init_af_pp(GPIOD,  4U, 12U);   /* NOE */
    gpio_init_af_pp(GPIOD,  5U, 12U);   /* NWE */
    gpio_init_af_pp(GPIOG, 12U, 12U);   /* NE4 */
    gpio_init_af_pp(GPIOE,  0U, 12U);   /* NBL0 */
    gpio_init_af_pp(GPIOE,  1U, 12U);   /* NBL1 */

    /* ---- FSMC 控制器初始化 ---- */
    fsmc_clk_enable();

    fsmc_norsram_config_t cfg;
    cfg.bank                = 3U;
    cfg.data_width          = 16U;
    cfg.address_data_mux    = 0U;
    cfg.burst_enable        = 0U;
    cfg.write_enable        = 1U;
    cfg.extended_mode       = 1U;
    cfg.wait_enable         = 0U;
    cfg.read_timing_addset  = 1U;
    cfg.read_timing_datast  = 5U;
    cfg.write_timing_addset = 8U;
    cfg.write_timing_datast = 5U;

    fsmc_norsram_init(&cfg);

    /* ---- SRAM 写读验证 ---- */
    volatile uint16_t *sram = (volatile uint16_t *)SRAM_BASE_ADDR;

    /* 写入前先读 word[0]，判断 SRAM 是否有基本响应 */
    uint16_t pre_read = sram[0];
    TEST_LOG(res, MOD, "pre-read sram[0]=0x%04X (0xFFFF=无响应 0x0000=线被拉低)",
             (unsigned)pre_read);

    /* 写入测试数据 */
    uint32_t i;
    for (i = 0U; i < SRAM_TEST_WORDS; i++)
    {
        sram[i] = (uint16_t)(i ^ 0x5A5AU);
    }

    /* 读回校验 */
    for (i = 0U; i < SRAM_TEST_WORDS; i++)
    {
        uint16_t exp = (uint16_t)(i ^ 0x5A5AU);
        uint16_t got = sram[i];

        if (got != exp)
        {
            res = TEST_FAIL;
            TEST_LOG(res, MOD,
                     "SRAM mismatch at word[%u] addr=0x%08X exp=0x%04X got=0x%04X",
                     (unsigned)i,
                     (unsigned)(SRAM_BASE_ADDR + i * 2U),
                     (unsigned)exp,
                     (unsigned)got);
            break;
        }
    }

    if (res == TEST_PASS)
    {
        TEST_LOG(res, MOD, "SRAM 0x%08X %u words R/W OK",
                 (unsigned)SRAM_BASE_ADDR, (unsigned)SRAM_TEST_WORDS);
    }

    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  22. drv_can — 无收发器，SKIP                                       */
/* ================================================================== */
test_result_t test_drv_can(void)
{
    const char   *MOD = "drv_can";
    test_result_t res = TEST_SKIP;

    TEST_LOG(res, MOD, "no CAN transceiver on board, skipped");
    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  23. drv_dcmi — 无摄像头，SKIP                                      */
/* ================================================================== */
test_result_t test_drv_dcmi(void)
{
    const char   *MOD = "drv_dcmi";
    test_result_t res = TEST_SKIP;

    TEST_LOG(res, MOD, "no camera module, skipped");
    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  24. drv_iwdg — 持续喂狗 3s，验证不复位                             */
/* ================================================================== */
test_result_t test_drv_iwdg(void)
{
    const char   *MOD = "drv_iwdg";
    test_result_t res = TEST_PASS;

    /*
     * LSI ~32kHz，prescaler=4 对应 /64
     * 超时 = (499+1) / (32000/64) = 500/500 = 1000ms
     */
    iwdg_config_t cfg;
    cfg.prescaler = 4U;     /* /64 */
    cfg.reload    = 499U;

    iwdg_init(&cfg);        /* 内部启动，无法软件关闭 */

    /* 喂狗 15次 × 200ms = 3s */
    for (uint32_t i = 0U; i < 15U; i++)
    {
        rt_thread_mdelay(200U);
        iwdg_reload();
    }

    TEST_LOG(res, MOD, "IWDG ~1s timeout fed 15x200ms no reset");
    runner_record(MOD, res);
    return res;
}

/* ================================================================== */
/*  25. drv_wwdg — 持续喂狗 3s，验证不复位                             */
/* ================================================================== */
test_result_t test_drv_wwdg(void)
{
    const char   *MOD = "drv_wwdg";
    test_result_t res = TEST_PASS;

    /*
     * PCLK1=42MHz，WWDG 时钟 = 42MHz/4096/8 ≈ 1281Hz，周期 ≈ 0.781ms
     * 计数器 0x7F → 0x40 = 63步，超时 ≈ 63 × 0.781ms ≈ 49ms
     * 窗口 0x5F：计数器递减到 < 0x5F 时才允许喂狗（不能太早）
     * 喂狗间隔 20ms：0x7F 递减 20/0.781 ≈ 26步 → 计数器约 0x69，在窗口内
     */
    wwdg_config_t cfg;
    cfg.prescaler  = 3U;      /* /8 */
    cfg.window     = 0x7FU;
    cfg.counter    = 0x7FU;
    cfg.ewi_enable = 0U;

    wwdg_clk_enable();
    wwdg_init(&cfg);          /* 内部启动，无法软件关闭 */

    /* 喂狗 150次 × 20ms = 3s */
    for (uint32_t i = 0U; i < 150U; i++)
    {
        rt_thread_mdelay(20U);
        wwdg_refresh(0x7FU);
			  iwdg_reload();
    }

    TEST_LOG(res, MOD, "WWDG ~49ms timeout fed 150x20ms no reset");
    runner_record(MOD, res);
    return res;
}
