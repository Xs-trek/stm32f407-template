#ifndef __DRV_CONFIG_H__
#define __DRV_CONFIG_H__

#include <stddef.h>
#include "f407_rt_config.h"

/* ========== 基础设施层（建议始终启用） ========== */

#if F407_RT_USING_RCC
#define DRV_USING_RCC
#endif

#if F407_RT_USING_GPIO
#define DRV_USING_GPIO
#endif

#if F407_RT_USING_NVIC
#define DRV_USING_NVIC
#endif

#if F407_RT_USING_SCB
#define DRV_USING_SCB
#endif

#if F407_RT_USING_SYSTICK
#define DRV_USING_SYSTICK
#endif

#if F407_RT_USING_EXTI
#define DRV_USING_EXTI
#endif

#if F407_RT_USING_SYSCFG
#define DRV_USING_SYSCFG
#endif

#if F407_RT_USING_DMA
#define DRV_USING_DMA
    #if F407_RT_USING_DMA1
    #define DRV_DMA_USING_DMA1
    #endif
    #if F407_RT_USING_DMA2
    #define DRV_DMA_USING_DMA2
    #endif
#endif

/* ========== 时钟、电源、Flash ========== */

#if F407_RT_USING_PWR
#define DRV_USING_PWR
    #if F407_RT_USING_PVD
    #define DRV_PWR_USING_PVD
    #endif
#endif

#if F407_RT_USING_FLASH
#define DRV_USING_FLASH
#endif

/* ========== 定时器 ========== */

#if F407_RT_USING_TIM
#define DRV_USING_TIM
    #if F407_RT_USING_TIM1
    #define DRV_TIM_USING_TIM1
    #endif
    #if F407_RT_USING_TIM2
    #define DRV_TIM_USING_TIM2
    #endif
    #if F407_RT_USING_TIM3
    #define DRV_TIM_USING_TIM3
    #endif
    #if F407_RT_USING_TIM4
    #define DRV_TIM_USING_TIM4
    #endif
    #if F407_RT_USING_TIM5
    #define DRV_TIM_USING_TIM5
    #endif
    #if F407_RT_USING_TIM6
    #define DRV_TIM_USING_TIM6
    #endif
    #if F407_RT_USING_TIM7
    #define DRV_TIM_USING_TIM7
    #endif
    #if F407_RT_USING_TIM8
    #define DRV_TIM_USING_TIM8
    #endif
    #if F407_RT_USING_TIM9
    #define DRV_TIM_USING_TIM9
    #endif
    #if F407_RT_USING_TIM10
    #define DRV_TIM_USING_TIM10
    #endif
    #if F407_RT_USING_TIM11
    #define DRV_TIM_USING_TIM11
    #endif
    #if F407_RT_USING_TIM12
    #define DRV_TIM_USING_TIM12
    #endif
    #if F407_RT_USING_TIM13
    #define DRV_TIM_USING_TIM13
    #endif
    #if F407_RT_USING_TIM14
    #define DRV_TIM_USING_TIM14
    #endif
    #if F407_RT_TIM_USING_PWM
    #define DRV_TIM_USING_PWM
    #endif
#endif

/* ========== 通信外设 ========== */

#if F407_RT_USING_USART
#define DRV_USING_USART
    #if F407_RT_USING_USART1
    #define DRV_USART_USING_USART1
    #endif
    #if F407_RT_USING_USART2
    #define DRV_USART_USING_USART2
    #endif
    #if F407_RT_USING_USART3
    #define DRV_USART_USING_USART3
    #endif
    #if F407_RT_USING_UART4
    #define DRV_USART_USING_UART4
    #endif
    #if F407_RT_USING_UART5
    #define DRV_USART_USING_UART5
    #endif
    #if F407_RT_USING_USART6
    #define DRV_USART_USING_USART6
    #endif
#endif

#if F407_RT_USING_SPI
#define DRV_USING_SPI
    #if F407_RT_USING_SPI1
    #define DRV_SPI_USING_SPI1
    #endif
    #if F407_RT_USING_SPI2
    #define DRV_SPI_USING_SPI2
    #endif
    #if F407_RT_USING_SPI3
    #define DRV_SPI_USING_SPI3
    #endif
#endif

#if F407_RT_USING_I2C
#define DRV_USING_I2C
    #if F407_RT_USING_I2C1
    #define DRV_I2C_USING_I2C1
    #endif
    #if F407_RT_USING_I2C2
    #define DRV_I2C_USING_I2C2
    #endif
    #if F407_RT_USING_I2C3
    #define DRV_I2C_USING_I2C3
    #endif
#endif

#if F407_RT_USING_I2S
#define DRV_USING_I2S
    #if F407_RT_USING_I2S2
    #define DRV_I2S_USING_I2S2
    #endif
    #if F407_RT_USING_I2S3
    #define DRV_I2S_USING_I2S3
    #endif
#endif

/* ========== 编译互斥检查 ========== */

#if defined(DRV_SPI_USING_SPI2) && defined(DRV_I2S_USING_I2S2)
#error "SPI2 and I2S2 cannot be enabled simultaneously (shared hardware)"
#endif

#if defined(DRV_SPI_USING_SPI3) && defined(DRV_I2S_USING_I2S3)
#error "SPI3 and I2S3 cannot be enabled simultaneously (shared hardware)"
#endif

#if F407_RT_USING_CAN
#define DRV_USING_CAN
    #if F407_RT_USING_CAN1
    #define DRV_CAN_USING_CAN1
    #endif
    #if F407_RT_USING_CAN2
    #define DRV_CAN_USING_CAN2
    #endif
#endif

#if F407_RT_USING_SDIO
#define DRV_USING_SDIO
#endif

/* ========== 模拟外设 ========== */

#if F407_RT_USING_ADC
#define DRV_USING_ADC
    #if F407_RT_USING_ADC1
    #define DRV_ADC_USING_ADC1
    #endif
    #if F407_RT_USING_ADC2
    #define DRV_ADC_USING_ADC2
    #endif
    #if F407_RT_USING_ADC3
    #define DRV_ADC_USING_ADC3
    #endif
#endif

#if F407_RT_USING_DAC
#define DRV_USING_DAC
#endif

/* ========== 看门狗 ========== */

#if F407_RT_USING_IWDG
#define DRV_USING_IWDG
#endif

#if F407_RT_USING_WWDG
#define DRV_USING_WWDG
#endif

/* ========== 时钟日历 ========== */

#if F407_RT_USING_RTC
#define DRV_USING_RTC
#endif

/* ========== CRC 与安全 ========== */

#if F407_RT_USING_CRC
#define DRV_USING_CRC
#endif

#if F407_RT_USING_RNG
#define DRV_USING_RNG
#endif

#if F407_RT_USING_HASH
#define DRV_USING_HASH
#endif

#if F407_RT_USING_CRYP
#define DRV_USING_CRYP
#endif

/* ========== 存储与外部总线 ========== */

#if F407_RT_USING_FSMC
#define DRV_USING_FSMC
#endif

/* ========== 高级外设 ========== */

#if F407_RT_USING_DCMI
#define DRV_USING_DCMI
#endif

#if F407_RT_USING_ETH
#define DRV_USING_ETH
#endif

#if F407_RT_USING_USB_OTG_FS
#define DRV_USING_USB_OTG_FS
#endif

#if F407_RT_USING_USB_OTG_HS
#define DRV_USING_USB_OTG_HS
#endif

/* ========== 调试 ========== */

#if F407_RT_USING_DBGMCU
#define DRV_USING_DBGMCU
#endif

#endif /* __DRV_CONFIG_H__ */
