#ifndef __DRV_RTC_H__
#define __DRV_RTC_H__

#include "drv_config.h"

#ifdef DRV_USING_RTC

/*
 * 当前版本覆盖范围：
 *   - RTC 初始化（LSI / LSE 时钟源选择、预分频配置）
 *   - 日历时间与日期的读写（BCD 编解码）
 *   - 闹钟 A / B 的配置、使能、中断与标志清除
 *   - 唤醒定时器的配置、使能、中断与标志清除
 *   - 备份寄存器（BKP0 ~ BKP19）的读写
 *
 * 已知限制：
 *   - 不支持 RTC 校准（平滑校准 / 粗调校准）
 *   - 不支持时间戳（Timestamp）功能
 *   - 不支持入侵检测（Tamper）功能
 *   - 不支持参考时钟输入
 *   - LSI 分频系数按典型值 32 kHz 设定，实际精度受芯片个体偏差影响
 *   - 年份仅保存低两位（0~99），世纪信息需由应用层自行管理
 */

#include "stm32f4xx.h"

/* ==================== 时钟源宏定义 ==================== */

#define RTC_CLOCK_LSI  0U
#define RTC_CLOCK_LSE  1U

/* ==================== 数据结构 ==================== */

/* RTC 配置结构体 */
typedef struct
{
    uint8_t clock_source;  /* RTC_CLOCK_LSI 或 RTC_CLOCK_LSE */
} rtc_config_t;

/* 时间结构体 */
typedef struct
{
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} rtc_time_t;

/* 日期结构体 */
typedef struct
{
    uint8_t year;       /* 0..99 */
    uint8_t month;      /* 1..12 */
    uint8_t date;       /* 1..31 */
    uint8_t week_day;   /* 1..7  */
} rtc_date_t;

/* 闹钟结构体 */
typedef struct
{
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint8_t mask;               /* bit0=MSK1..bit3=MSK4 */
    uint8_t date_weekday_sel;   /* 0=按日期, 1=按星期 */
    uint8_t date_weekday;
} rtc_alarm_t;

/* ==================== 初始化接口 ==================== */

/* 说明：使能 PWR 时钟（APB1） */
void rtc_pwr_clk_enable(void);

/* 说明：使能备份域访问（PWR_CR.DBP） */
void rtc_backup_access_enable(void);

/* 说明：根据 cfg 选择时钟源并初始化 RTC，若 RTC 已使能则仅做同步等待 */
void rtc_init(const rtc_config_t *cfg);

/* ==================== 时间 / 日期接口 ==================== */

/* 说明：设置 RTC 时间（时/分/秒） */
void rtc_set_time(const rtc_time_t *time);

/* 说明：读取 RTC 当前时间（时/分/秒） */
void rtc_get_time(rtc_time_t *time);

/* 说明：设置 RTC 日期（年/月/日/星期） */
void rtc_set_date(const rtc_date_t *date);

/* 说明：读取 RTC 当前日期（年/月/日/星期） */
void rtc_get_date(rtc_date_t *date);

/* ==================== 闹钟接口 ==================== */

/* 说明：配置闹钟 A 的匹配条件（不使能，需另行调用 enable） */
void rtc_set_alarm_a(const rtc_alarm_t *alarm);

/* 说明：配置闹钟 B 的匹配条件（不使能，需另行调用 enable） */
void rtc_set_alarm_b(const rtc_alarm_t *alarm);

/* 说明：使能闹钟 A */
void rtc_enable_alarm_a(void);

/* 说明：禁用闹钟 A */
void rtc_disable_alarm_a(void);

/* 说明：使能闹钟 B */
void rtc_enable_alarm_b(void);

/* 说明：禁用闹钟 B */
void rtc_disable_alarm_b(void);

/* 说明：使能闹钟 A 中断 */
void rtc_enable_alarm_a_irq(void);

/* 说明：使能闹钟 B 中断 */
void rtc_enable_alarm_b_irq(void);

/* 说明：清除闹钟 A 标志位 */
void rtc_clear_alarm_a_flag(void);

/* 说明：清除闹钟 B 标志位 */
void rtc_clear_alarm_b_flag(void);

/* ==================== 唤醒定时器接口 ==================== */

/* 说明：设置唤醒定时器重装载值及时钟选择 */
void rtc_set_wakeup_reload(uint16_t reload, uint8_t clock_sel);

/* 说明：使能唤醒定时器 */
void rtc_enable_wakeup(void);

/* 说明：禁用唤醒定时器 */
void rtc_disable_wakeup(void);

/* 说明：使能唤醒定时器中断 */
void rtc_enable_wakeup_irq(void);

/* 说明：清除唤醒定时器标志位 */
void rtc_clear_wakeup_flag(void);

/* ==================== 备份寄存器接口 ==================== */

/* 说明：向指定备份寄存器（0~19）写入 32 位数据 */
void     rtc_write_backup(uint8_t index, uint32_t value);

/* 说明：从指定备份寄存器（0~19）读取 32 位数据 */
uint32_t rtc_read_backup(uint8_t index);

#endif /* DRV_USING_RTC */
#endif /* __DRV_RTC_H__ */
