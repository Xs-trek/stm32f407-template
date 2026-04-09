#ifndef __F407_RT_CONFIG_H__
#define __F407_RT_CONFIG_H__

/* ================================================================
 * F407_RT 项目配置中心
 * 所有外设裁剪开关集中在此文件
 * 0 = 禁用，1 = 启用
 * ================================================================ */

/* ========== 芯片与系统 ========== */
#define F407_RT_HSE_MHZ             25U
#define F407_RT_SYSCLK_MHZ          168U

/* ========== RT-Thread Nano ========== */
#define F407_RT_USING_RTTHREAD_NANO 1

/* ========== 基础设施层（建议始终启用） ========== */
#define F407_RT_USING_RCC           1
#define F407_RT_USING_GPIO          1
#define F407_RT_USING_NVIC          1
#define F407_RT_USING_SCB           1
#define F407_RT_USING_SYSTICK       1
#define F407_RT_USING_EXTI          1
#define F407_RT_USING_SYSCFG        1

#define F407_RT_USING_DMA           1
#define F407_RT_USING_DMA1          1
#define F407_RT_USING_DMA2          1

/* ========== 时钟、电源、Flash ========== */
#define F407_RT_USING_PWR           1
#define F407_RT_USING_PVD           1

#define F407_RT_USING_FLASH         1

/* ========== 定时器 ========== */
#define F407_RT_USING_TIM           1
#define F407_RT_USING_TIM1          1
#define F407_RT_USING_TIM2          1
#define F407_RT_USING_TIM3          1
#define F407_RT_USING_TIM4          1
#define F407_RT_USING_TIM5          1
#define F407_RT_USING_TIM6          1
#define F407_RT_USING_TIM7          1
#define F407_RT_USING_TIM8          1
#define F407_RT_USING_TIM9          1
#define F407_RT_USING_TIM10         1
#define F407_RT_USING_TIM11         1
#define F407_RT_USING_TIM12         1
#define F407_RT_USING_TIM13         1
#define F407_RT_USING_TIM14         1
#define F407_RT_TIM_USING_PWM       1

/* ========== 通信外设 ========== */
#define F407_RT_USING_USART         1
#define F407_RT_USING_USART1        1
#define F407_RT_USING_USART2        1
#define F407_RT_USING_USART3        1
#define F407_RT_USING_UART4         1
#define F407_RT_USING_UART5         1
#define F407_RT_USING_USART6        1

#define F407_RT_USING_SPI           1
#define F407_RT_USING_SPI1          1
#define F407_RT_USING_SPI2          1
#define F407_RT_USING_SPI3          1

#define F407_RT_USING_I2C           1
#define F407_RT_USING_I2C1          1
#define F407_RT_USING_I2C2          1
#define F407_RT_USING_I2C3          1

#define F407_RT_USING_I2S           0
#define F407_RT_USING_I2S2          0
#define F407_RT_USING_I2S3          0

#define F407_RT_USING_CAN           1
#define F407_RT_USING_CAN1          1
#define F407_RT_USING_CAN2          1

#define F407_RT_USING_SDIO          1

/* ========== 模拟外设 ========== */
#define F407_RT_USING_ADC           1
#define F407_RT_USING_ADC1          1
#define F407_RT_USING_ADC2          1
#define F407_RT_USING_ADC3          1

#define F407_RT_USING_DAC           1

/* ========== 看门狗 ========== */
#define F407_RT_USING_IWDG          1
#define F407_RT_USING_WWDG          1

/* ========== 时钟日历 ========== */
#define F407_RT_USING_RTC           1

/* ========== CRC 与安全 ========== */
#define F407_RT_USING_CRC           1
#define F407_RT_USING_RNG           1
#define F407_RT_USING_HASH          0
#define F407_RT_USING_CRYP          0

/* ========== 存储与外部总线 ========== */
#define F407_RT_USING_FSMC          1
/* ========== 高级外设 ========== */
#define F407_RT_USING_DCMI          1
#define F407_RT_USING_ETH           1
#define F407_RT_USING_USB_OTG_FS    0
#define F407_RT_USING_USB_OTG_HS    0

/* ========== 调试 ========== */
#define F407_RT_USING_DBGMCU        1

#endif /* __F407_RT_CONFIG_H__ */
