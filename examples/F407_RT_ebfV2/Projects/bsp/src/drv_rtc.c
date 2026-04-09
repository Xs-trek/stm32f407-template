#include "drv_rtc.h"

#ifdef DRV_USING_RTC

/* ==================== 超时常量 ==================== */

/*
 * RTC 操作超时循环次数。
 * rtc_wait_sync 在 APB1 168MHz 下，
 * 2 个 LSI 周期约 62μs，对应约 10000 个 APB 周期，
 * 取 100000 作为保守上限。
 */
#define RTC_TIMEOUT_SYNC     100000UL
#define RTC_TIMEOUT_INITF    100000UL
#define RTC_TIMEOUT_ALRAWF   100000UL
#define RTC_TIMEOUT_ALRBWF   100000UL
#define RTC_TIMEOUT_WUTWF    100000UL
#define RTC_TIMEOUT_LSI      100000UL
#define RTC_TIMEOUT_LSE      10000000UL   /* LSE 起振最慢，给更多时间 */

/* ==================== 内部 BCD 编解码 ==================== */

static uint8_t rtc_bcd_to_bin(uint8_t bcd)
{
    return (uint8_t)(((bcd >> 4U) * 10U) + (bcd & 0x0FU));
}

static uint8_t rtc_bin_to_bcd(uint8_t bin)
{
    return (uint8_t)(((bin / 10U) << 4U) | (bin % 10U));
}

/* ==================== 内部操作辅助 ==================== */

/*
 * 等待寄存器同步。
 * 返回 0 表示成功，1 表示超时。
 */
static uint8_t rtc_wait_sync(void)
{
    uint32_t timeout = RTC_TIMEOUT_SYNC;

    RTC->ISR &= ~RTC_ISR_RSF;
    while ((RTC->ISR & RTC_ISR_RSF) == 0U)
    {
        if (timeout == 0U) { return 1U; }
        timeout--;
    }
    return 0U;
}

static void rtc_write_protect_disable(void)
{
    RTC->WPR = 0xCAU;
    RTC->WPR = 0x53U;
}

static void rtc_write_protect_enable(void)
{
    RTC->WPR = 0xFFU;
}

/*
 * 进入初始化模式。
 * 返回 0 表示成功，1 表示超时。
 */
static uint8_t rtc_enter_init_mode(void)
{
    uint32_t timeout = RTC_TIMEOUT_INITF;

    rtc_write_protect_disable();
    RTC->ISR |= RTC_ISR_INIT;
    while ((RTC->ISR & RTC_ISR_INITF) == 0U)
    {
        if (timeout == 0U)
        {
            rtc_write_protect_enable();
            return 1U;
        }
        timeout--;
    }
    return 0U;
}

static void rtc_exit_init_mode(void)
{
    RTC->ISR &= ~RTC_ISR_INIT;
    rtc_write_protect_enable();
}

static __IO uint32_t *rtc_backup_reg_addr(uint8_t index)
{
    return &RTC->BKP0R + index;
}

/* ==================== 初始化 ==================== */

void rtc_pwr_clk_enable(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    (void)RCC->APB1ENR;
}

void rtc_backup_access_enable(void)
{
    rtc_pwr_clk_enable();
    PWR->CR |= PWR_CR_DBP;
}

/*
 * 初始化 RTC。
 * 返回前完成寄存器同步等待，带超时保护。
 * 若超时，函数直接返回，不卡死。
 */
void rtc_init(const rtc_config_t *cfg)
{
    uint32_t timeout;

    if (cfg == NULL) { return; }

    rtc_backup_access_enable();

    if (cfg->clock_source == RTC_CLOCK_LSE)
    {
        RCC->BDCR |= RCC_BDCR_LSEON;
        timeout = RTC_TIMEOUT_LSE;
        while ((RCC->BDCR & RCC_BDCR_LSERDY) == 0U)
        {
            if (timeout == 0U) { return; }
            timeout--;
        }
    }
    else  /* LSI */
    {
        RCC->CSR |= RCC_CSR_LSION;
        timeout = RTC_TIMEOUT_LSI;
        while ((RCC->CSR & RCC_CSR_LSIRDY) == 0U)
        {
            if (timeout == 0U) { return; }
            timeout--;
        }
    }

    if ((RCC->BDCR & RCC_BDCR_RTCEN) == 0U)
    {
        /* 复位备份域 */
        RCC->BDCR |= RCC_BDCR_BDRST;
        RCC->BDCR &= ~RCC_BDCR_BDRST;

        if (cfg->clock_source == RTC_CLOCK_LSE)
        {
            RCC->BDCR |= RCC_BDCR_LSEON;
            timeout = RTC_TIMEOUT_LSE;
            while ((RCC->BDCR & RCC_BDCR_LSERDY) == 0U)
            {
                if (timeout == 0U) { return; }   /* LSE 未就绪，直接返回 */
                timeout--;
            }

            RCC->BDCR &= ~RCC_BDCR_RTCSEL;
            RCC->BDCR |= RCC_BDCR_RTCSEL_0;
            RCC->BDCR |= RCC_BDCR_RTCEN;

            if (rtc_enter_init_mode() != 0U) { return; }
            RTC->PRER = (127U << 16U) | 255U;
            rtc_exit_init_mode();
        }
        else
        {
            RCC->CSR |= RCC_CSR_LSION;
            timeout = RTC_TIMEOUT_LSI;
            while ((RCC->CSR & RCC_CSR_LSIRDY) == 0U)
            {
                if (timeout == 0U) { return; }   /* LSI 未就绪，直接返回 */
                timeout--;
            }

            RCC->BDCR &= ~RCC_BDCR_RTCSEL;
            RCC->BDCR |= RCC_BDCR_RTCSEL_1;
            RCC->BDCR |= RCC_BDCR_RTCEN;

            if (rtc_enter_init_mode() != 0U) { return; }
            RTC->PRER = (127U << 16U) | 249U;
            rtc_exit_init_mode();
        }
    }

    /* 等待同步，超时直接返回，不卡死 */
    (void)rtc_wait_sync();
}

/* ==================== 时间 / 日期 ==================== */

void rtc_set_time(const rtc_time_t *time)
{
    if (time == NULL) { return; }
    if (rtc_enter_init_mode() != 0U) { return; }

    RTC->TR = ((uint32_t)rtc_bin_to_bcd(time->hours)   << 16U) |
              ((uint32_t)rtc_bin_to_bcd(time->minutes)  <<  8U) |
              ((uint32_t)rtc_bin_to_bcd(time->seconds));
    rtc_exit_init_mode();

    (void)rtc_wait_sync();
}

void rtc_get_time(rtc_time_t *time)
{
    uint32_t tr;

    if (time == NULL) { return; }

    (void)rtc_wait_sync();

    tr = RTC->TR;
    (void)RTC->DR;
    time->hours   = rtc_bcd_to_bin((uint8_t)((tr >> 16U) & 0x3FU));
    time->minutes = rtc_bcd_to_bin((uint8_t)((tr >>  8U) & 0x7FU));
    time->seconds = rtc_bcd_to_bin((uint8_t)(tr & 0x7FU));
}

void rtc_set_date(const rtc_date_t *date)
{
    if (date == NULL) { return; }
    if (rtc_enter_init_mode() != 0U) { return; }

    RTC->DR = ((uint32_t)(date->week_day & 0x07U)     << 13U) |
              ((uint32_t)rtc_bin_to_bcd(date->year)    << 16U) |
              ((uint32_t)rtc_bin_to_bcd(date->month)   <<  8U) |
              ((uint32_t)rtc_bin_to_bcd(date->date));
    rtc_exit_init_mode();
		
    (void)rtc_wait_sync();
}

void rtc_get_date(rtc_date_t *date)
{
    uint32_t dr;

    if (date == NULL) { return; }

    dr = RTC->DR;
    date->week_day = (uint8_t)((dr >> 13U) & 0x07U);
    date->year     = rtc_bcd_to_bin((uint8_t)((dr >> 16U) & 0xFFU));
    date->month    = rtc_bcd_to_bin((uint8_t)((dr >>  8U) & 0x1FU));
    date->date     = rtc_bcd_to_bin((uint8_t)(dr & 0x3FU));
}

/* ==================== 闹钟 ==================== */

static uint32_t rtc_build_alrm(const rtc_alarm_t *alarm)
{
    uint32_t alrmr = 0U;
    alrmr |= ((uint32_t)rtc_bin_to_bcd(alarm->hours)   << 16U);
    alrmr |= ((uint32_t)rtc_bin_to_bcd(alarm->minutes) <<  8U);
    alrmr |= ((uint32_t)rtc_bin_to_bcd(alarm->seconds));
    if (alarm->mask & 0x01U) { alrmr |= (1U <<  7U); }
    if (alarm->mask & 0x02U) { alrmr |= (1U << 15U); }
    if (alarm->mask & 0x04U) { alrmr |= (1U << 23U); }
    if (alarm->mask & 0x08U) { alrmr |= (1U << 31U); }
    if (alarm->date_weekday_sel) { alrmr |= RTC_ALRMAR_WDSEL; }
    alrmr |= ((uint32_t)rtc_bin_to_bcd(alarm->date_weekday) << 24U);
    return alrmr;
}

void rtc_set_alarm_a(const rtc_alarm_t *alarm)
{
    uint32_t timeout;

    if (alarm == NULL) { return; }
    rtc_write_protect_disable();
    RTC->CR &= ~RTC_CR_ALRAE;
    timeout = RTC_TIMEOUT_ALRAWF;
    while ((RTC->ISR & RTC_ISR_ALRAWF) == 0U)
    {
        if (timeout == 0U) { rtc_write_protect_enable(); return; }
        timeout--;
    }
    RTC->ALRMAR = rtc_build_alrm(alarm);
    rtc_write_protect_enable();
}

void rtc_set_alarm_b(const rtc_alarm_t *alarm)
{
    uint32_t timeout;

    if (alarm == NULL) { return; }
    rtc_write_protect_disable();
    RTC->CR &= ~RTC_CR_ALRBE;
    timeout = RTC_TIMEOUT_ALRBWF;
    while ((RTC->ISR & RTC_ISR_ALRBWF) == 0U)
    {
        if (timeout == 0U) { rtc_write_protect_enable(); return; }
        timeout--;
    }
    RTC->ALRMBR = rtc_build_alrm(alarm);
    rtc_write_protect_enable();
}

void rtc_enable_alarm_a(void)  { rtc_write_protect_disable(); RTC->CR |= RTC_CR_ALRAE;  rtc_write_protect_enable(); }
void rtc_disable_alarm_a(void) { rtc_write_protect_disable(); RTC->CR &= ~RTC_CR_ALRAE; rtc_write_protect_enable(); }
void rtc_enable_alarm_b(void)  { rtc_write_protect_disable(); RTC->CR |= RTC_CR_ALRBE;  rtc_write_protect_enable(); }
void rtc_disable_alarm_b(void) { rtc_write_protect_disable(); RTC->CR &= ~RTC_CR_ALRBE; rtc_write_protect_enable(); }
void rtc_enable_alarm_a_irq(void) { rtc_write_protect_disable(); RTC->CR |= RTC_CR_ALRAIE; rtc_write_protect_enable(); }
void rtc_enable_alarm_b_irq(void) { rtc_write_protect_disable(); RTC->CR |= RTC_CR_ALRBIE; rtc_write_protect_enable(); }
void rtc_clear_alarm_a_flag(void) { RTC->ISR = (uint32_t)~RTC_ISR_ALRAF; }
void rtc_clear_alarm_b_flag(void) { RTC->ISR = (uint32_t)~RTC_ISR_ALRBF; }

/* ==================== 唤醒定时器 ==================== */

void rtc_set_wakeup_reload(uint16_t reload, uint8_t clock_sel)
{
    uint32_t timeout;

    rtc_write_protect_disable();
    RTC->CR &= ~RTC_CR_WUTE;
    timeout = RTC_TIMEOUT_WUTWF;
    while ((RTC->ISR & RTC_ISR_WUTWF) == 0U)
    {
        if (timeout == 0U) { rtc_write_protect_enable(); return; }
        timeout--;
    }
    RTC->WUTR  = (reload & 0xFFFFU);
    RTC->CR   &= ~RTC_CR_WUCKSEL;
    RTC->CR   |= (clock_sel & 0x07U);
    rtc_write_protect_enable();
}

void rtc_enable_wakeup(void)  { rtc_write_protect_disable(); RTC->CR |= RTC_CR_WUTE;  rtc_write_protect_enable(); }
void rtc_disable_wakeup(void) { rtc_write_protect_disable(); RTC->CR &= ~RTC_CR_WUTE; rtc_write_protect_enable(); }
void rtc_enable_wakeup_irq(void) { rtc_write_protect_disable(); RTC->CR |= RTC_CR_WUTIE; rtc_write_protect_enable(); }
void rtc_clear_wakeup_flag(void) { RTC->ISR = (uint32_t)~RTC_ISR_WUTF; }

/* ==================== 备份寄存器 ==================== */

void rtc_write_backup(uint8_t index, uint32_t value)
{
    if (index > 19U) { return; }
    rtc_backup_access_enable();
    *rtc_backup_reg_addr(index) = value;
}

uint32_t rtc_read_backup(uint8_t index)
{
    if (index > 19U) { return 0U; }
    return *rtc_backup_reg_addr(index);
}

#endif /* DRV_USING_RTC */
