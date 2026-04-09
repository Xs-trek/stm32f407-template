/*
 * main.c  —  F407_RT_ebfV2 硬件验证例程
 */

#include <rtthread.h>
#include "board.h"
#include "f407_rt_modules.h"
#include "test_runner.h"
#include "test_peripherals.h"

#define HEARTBEAT_STK_SIZE   256U
#define HEARTBEAT_PRIORITY   20U
#define HEARTBEAT_TIMESLICE  5U

#define TEST_STK_SIZE        8192U
#define TEST_PRIORITY        10U
#define TEST_TIMESLICE       5U

static void feed_all_wdg(void)
{
    iwdg_reload();
    wwdg_refresh(0x7FU);
}

/* ------------------------------------------------------------------ */
/*  心跳线程：LED_B PF8 每 500ms 翻转                                  */
/* ------------------------------------------------------------------ */
static void heartbeat_thread_entry(void *param)
{
    (void)param;

    gpio_clk_enable(LED_B_PORT);
    gpio_init_output_pp(LED_B_PORT, LED_B_PIN);

    while (1)
    {
        gpio_toggle(LED_B_PORT, LED_B_PIN);
        rt_thread_mdelay(500U);
    }
}

/* ------------------------------------------------------------------ */
/*  测试线程                                                            */
/* ------------------------------------------------------------------ */
static void test_thread_entry(void *param)
{
	  runner_set_feed_hook(feed_all_wdg);
	
    (void)param;

    rt_thread_mdelay(500U);

    rt_kprintf("\r\n");
    rt_kprintf("================================================\r\n");
    rt_kprintf("  F407_RT_ebfV2 Hardware Verification\r\n");
    rt_kprintf("================================================\r\n");

    runner_init();

    /* 软件/寄存器 */
    test_drv_rcc();
    test_drv_scb();
    test_drv_dbgmcu();
    test_drv_crc();
    test_drv_pwr();
    test_drv_syscfg();
    test_drv_nvic();
    test_drv_flash();
    test_drv_rng();

    /* 硬件 */
    test_drv_gpio();
    test_drv_usart();
    test_drv_tim_basic();
    test_drv_tim_pwm();
    test_drv_adc();
    test_drv_dac();
    test_drv_dma();
    test_drv_spi();
    test_drv_i2c();
    test_drv_exti();
    test_drv_rtc();
    test_drv_fsmc();
    test_drv_can();
    test_drv_dcmi();

    /* 看门狗放最后 */
    test_drv_iwdg();
    test_drv_wwdg();

    runner_print_summary();
    rt_kprintf("\r\n[INFO] All tests done.\r\n");

    /* 持续喂狗，防止看门狗复位 */
    while (1)
    {
        iwdg_reload();
        wwdg_refresh(0x7FU);
        rt_thread_mdelay(10U);
    }
}

/* ------------------------------------------------------------------ */
/*  main                                                               */
/* ------------------------------------------------------------------ */
int main(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("heartbeat",
                           heartbeat_thread_entry, RT_NULL,
                           HEARTBEAT_STK_SIZE,
                           HEARTBEAT_PRIORITY,
                           HEARTBEAT_TIMESLICE);
    RT_ASSERT(tid != RT_NULL);
    rt_thread_startup(tid);

    tid = rt_thread_create("test",
                           test_thread_entry, RT_NULL,
                           TEST_STK_SIZE,
                           TEST_PRIORITY,
                           TEST_TIMESLICE);
    RT_ASSERT(tid != RT_NULL);
    rt_thread_startup(tid);

    return 0;
}
