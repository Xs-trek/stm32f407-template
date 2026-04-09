#ifndef __F407_RT_CONFIG_H__
#define __F407_RT_CONFIG_H__

/* ================================================================
 * F407_RT_ebfV2 硬件验证例程配置
 * ================================================================ */

/* ========== 芯片与系统 ========== */
#define F407_RT_HSE_MHZ             25U
#define F407_RT_SYSCLK_MHZ          168U

/* ========== RT-Thread Nano ========== */
#define F407_RT_USING_RTTHREAD_NANO 1

/* ========== 基础设施层 ========== */
#define F407_RT_USING_RCC           1
#define F407_RT_USING_GPIO          1
#define F407_RT_USING_NVIC          1
#define F407_RT_USING_SCB           1
#define F407_RT_USING_SYSTICK       1
#define F407_RT_USING_EXTI          1   /* exti_init_gpio_interrupt 用于 KEY1，即使测试 SKIP 也需要编译通过 */
#define F407_RT_USING_SYSCFG        1

#define F407_RT_USING_DMA           1
#define F407_RT_USING_DMA1          0   /* 验证例程 DMA 测试仅用 DMA2 Stream0 M2M */
#define F407_RT_USING_DMA2          1

/* ========== 时钟、电源、Flash ========== */
#define F407_RT_USING_PWR           1
#define F407_RT_USING_PVD           0   /* 验证例程不测试 PVD */
#define F407_RT_USING_FLASH         1

/* ========== 定时器 ========== */
#define F407_RT_USING_TIM           1
#define F407_RT_USING_TIM1          0
#define F407_RT_USING_TIM2          0
#define F407_RT_USING_TIM3          0
#define F407_RT_USING_TIM4          0
#define F407_RT_USING_TIM5          0
#define F407_RT_USING_TIM6          1   /* tim_basic 测试：500ms 定时 */
#define F407_RT_USING_TIM7          0
#define F407_RT_USING_TIM8          0
#define F407_RT_USING_TIM9          0
#define F407_RT_USING_TIM10         1   /* tim_pwm 测试：CH1 → LED_R PF6 */
#define F407_RT_USING_TIM11         0
#define F407_RT_USING_TIM12         0
#define F407_RT_USING_TIM13         0
#define F407_RT_USING_TIM14         0
#define F407_RT_TIM_USING_PWM       1

/* ========== 通信外设 ========== */
#define F407_RT_USING_USART         1
#define F407_RT_USING_USART1        1   /* console 调试串口 */
#define F407_RT_USING_USART2        0
#define F407_RT_USING_USART3        0   /* WiFi ESP8266，验证例程不测试 */
#define F407_RT_USING_UART4         0
#define F407_RT_USING_UART5         0
#define F407_RT_USING_USART6        0

#define F407_RT_USING_SPI           1
#define F407_RT_USING_SPI1          1   /* W25Q128 Flash */
#define F407_RT_USING_SPI2          0
#define F407_RT_USING_SPI3          0

#define F407_RT_USING_I2C           1
#define F407_RT_USING_I2C1          1   /* AT24C02 + MPU6050 + AP3216C */
#define F407_RT_USING_I2C2          0
#define F407_RT_USING_I2C3          0

#define F407_RT_USING_I2S           0
#define F407_RT_USING_I2S2          0
#define F407_RT_USING_I2S3          0

#define F407_RT_USING_CAN           0   /* 无收发器，测试 SKIP，驱动无需编译 */
#define F407_RT_USING_CAN1          0
#define F407_RT_USING_CAN2          0

#define F407_RT_USING_SDIO          0   /* 无 SD 卡，验证例程不测试 */

/* ========== 模拟外设 ========== */
#define F407_RT_USING_ADC           1
#define F407_RT_USING_ADC1          1   /* PB0 电位器 + 温度 + VREFINT */
#define F407_RT_USING_ADC2          0
#define F407_RT_USING_ADC3          0

#define F407_RT_USING_DAC           1   /* PA4 输出 ~1.65V */

/* ========== 看门狗 ========== */
#define F407_RT_USING_IWDG          1
#define F407_RT_USING_WWDG          1

/* ========== 时钟日历 ========== */
#define F407_RT_USING_RTC           1

/* ========== CRC 与安全 ========== */
#define F407_RT_USING_CRC           1
#define F407_RT_USING_RNG           1
#define F407_RT_USING_HASH          0   /* 驱动不可用 */
#define F407_RT_USING_CRYP          0   /* 驱动不可用 */

/* ========== 存储与外部总线 ========== */
#define F407_RT_USING_FSMC          1   /* IS62WV51216 SRAM */

/* ========== 高级外设 ========== */
#define F407_RT_USING_DCMI          0   /* 无摄像头，测试 SKIP，驱动无需编译 */
#define F407_RT_USING_ETH           0   /* 验证例程不测试以太网 */
#define F407_RT_USING_USB_OTG_FS    0   /* 驱动不可用 */
#define F407_RT_USING_USB_OTG_HS    0   /* 驱动不可用 */

/* ========== 调试 ========== */
#define F407_RT_USING_DBGMCU        1

#endif /* __F407_RT_CONFIG_H__ */
