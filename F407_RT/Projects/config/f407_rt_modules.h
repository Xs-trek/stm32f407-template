#ifndef __F407_RT_MODULES_H__
#define __F407_RT_MODULES_H__

#include "f407_rt_config.h"

/* ==================== 基础设施层 ==================== */

#if F407_RT_USING_RCC
#include "drv_rcc.h"
#endif

#if F407_RT_USING_GPIO
#include "drv_gpio.h"
#endif

#if F407_RT_USING_NVIC
#include "drv_nvic.h"
#endif

#if F407_RT_USING_SCB
#include "drv_scb.h"
#endif

#if F407_RT_USING_SYSTICK
#include "drv_systick.h"
#endif

#if F407_RT_USING_EXTI
#include "drv_exti.h"
#endif

#if F407_RT_USING_SYSCFG
#include "drv_syscfg.h"
#endif

#if F407_RT_USING_DMA
#include "drv_dma.h"
#endif

/* ==================== 时钟、电源、Flash ==================== */

#if F407_RT_USING_PWR
#include "drv_pwr.h"
#endif

#if F407_RT_USING_FLASH
#include "drv_flash.h"
#endif

/* ==================== 定时器 ==================== */

#if F407_RT_USING_TIM
#include "drv_tim.h"
#endif

/* ==================== 通信外设 ==================== */

#if F407_RT_USING_USART
#include "drv_usart.h"
#endif

#if F407_RT_USING_SPI
#include "drv_spi.h"
#endif

#if F407_RT_USING_I2C
#include "drv_i2c.h"
#endif

#if F407_RT_USING_I2S
#include "drv_i2s.h"
#endif

#if F407_RT_USING_CAN
#include "drv_can.h"
#endif

#if F407_RT_USING_SDIO
#include "drv_sdio.h"
#endif

/* ==================== 模拟外设 ==================== */

#if F407_RT_USING_ADC
#include "drv_adc.h"
#endif

#if F407_RT_USING_DAC
#include "drv_dac.h"
#endif

/* ==================== 看门狗 ==================== */

#if F407_RT_USING_IWDG
#include "drv_wdg.h"
#endif

#if F407_RT_USING_WWDG
#include "drv_wwdg.h"
#endif

/* ==================== 时钟日历 ==================== */

#if F407_RT_USING_RTC
#include "drv_rtc.h"
#endif

/* ==================== CRC 与安全 ==================== */

#if F407_RT_USING_CRC
#include "drv_crc.h"
#endif

#if F407_RT_USING_RNG
#include "drv_rng.h"
#endif

#if F407_RT_USING_HASH
#include "drv_hash.h"
#endif

#if F407_RT_USING_CRYP
#include "drv_cryp.h"
#endif

/* ==================== 存储与外部总线 ==================== */

#if F407_RT_USING_FSMC
#include "drv_fsmc.h"
#endif

/* ==================== 高级外设 ==================== */

#if F407_RT_USING_DCMI
#include "drv_dcmi.h"
#endif

#if F407_RT_USING_ETH
#include "drv_eth.h"
#endif

#if F407_RT_USING_USB_OTG_FS
#include "drv_usb_otg_fs.h"
#endif

#if F407_RT_USING_USB_OTG_HS
#include "drv_usb_otg_hs.h"
#endif

/* ==================== 调试 ==================== */

#if F407_RT_USING_DBGMCU
#include "drv_dbgmcu.h"
#endif

#endif /* __F407_RT_MODULES_H__ */
