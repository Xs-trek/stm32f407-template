#ifndef TEST_PERIPHERALS_H
#define TEST_PERIPHERALS_H

#include "test_runner.h"

/* 软件/寄存器读取 */
test_result_t test_drv_rcc(void);
test_result_t test_drv_scb(void);
test_result_t test_drv_dbgmcu(void);
test_result_t test_drv_crc(void);
test_result_t test_drv_pwr(void);
test_result_t test_drv_syscfg(void);
test_result_t test_drv_nvic(void);
test_result_t test_drv_flash(void);
test_result_t test_drv_rng(void);

/* 硬件 */
test_result_t test_drv_gpio(void);
test_result_t test_drv_usart(void);
test_result_t test_drv_tim_basic(void);
test_result_t test_drv_tim_pwm(void);
test_result_t test_drv_adc(void);
test_result_t test_drv_dac(void);
test_result_t test_drv_dma(void);
test_result_t test_drv_spi(void);
test_result_t test_drv_i2c(void);
test_result_t test_drv_exti(void);
test_result_t test_drv_rtc(void);
test_result_t test_drv_fsmc(void);
test_result_t test_drv_can(void);
test_result_t test_drv_dcmi(void);

/* 看门狗 */
test_result_t test_drv_iwdg(void);
test_result_t test_drv_wwdg(void);

#endif /* TEST_PERIPHERALS_H */
