#ifndef __BOARD_H__
#define __BOARD_H__

#include "stm32f4xx.h"
#include "f407_rt_config.h"

/*
 * 说明：
 * 本文件为 F407_RT 模板工程的 board 层公开接口。
 *
 * 当前版本语义：
 * 1. board 层只负责系统级初始化：时钟、SysTick、堆。
 * 2. board 层不包含任何板级资源绑定（LED/KEY/USART 等）。
 * 3. 板级资源绑定由用户在自己的工程中扩展。
 *
 * 时钟配置说明：
 * 1. 在 f407_rt_config.h 中配置以下两个参数：
 *    - F407_RT_HSE_MHZ  ：外部晶振频率，当前支持 8 / 12 / 16 / 25
 *    - F407_RT_SYSCLK_MHZ：目标系统时钟，当前版本固定支持 168
 * 2. 使用其它 HSE 频率或目标时钟时，需在 board.c 的
 *    SystemClock_Config() 中手动添加对应 PLL 参数。
 * 3. 当前所有 HSE 配置均以 168MHz SYSCLK 为目标，
 *    对应 APB1 = 42MHz，APB2 = 84MHz，USB = 48MHz。
 *
 * 堆配置说明：
 * 1. 默认不启用动态堆。
 * 2. 如需动态内存，在 rtconfig.h 中启用 RT_USING_HEAP，
 *    并在 f407_rt_config.h 中配置 F407_RT_HEAP_SIZE。
 */

/* ==================== 堆大小配置 ==================== */

/*
 * 说明：
 * RT-Thread 动态堆大小，单位字节。
 * 仅在 RT_USING_HEAP 启用时生效。
 *
 * 前提：
 * 1. F407_RT_HEAP_SIZE 不得超过可用 SRAM 减去静态分配后的剩余量。
 * 2. STM32F407 片上 SRAM 共 192KB（主 SRAM 128KB + CCM 64KB）。
 * 3. CCM 不能用于 DMA，分配堆时需注意。
 *
 * 当前版本语义：
 * 若未在 f407_rt_config.h 中定义 F407_RT_HEAP_SIZE，
 * 默认使用 4KB 作为保守值。
 */
#ifndef F407_RT_HEAP_SIZE
#define F407_RT_HEAP_SIZE    (32U * 1024U)
#endif

/* ==================== 公开接口 ==================== */

/*
 * 说明：
 * RT-Thread 板级初始化钩子，由内核在启动调度器前调用。
 * 负责完成时钟初始化、SysTick 初始化、堆初始化。
 */
void rt_hw_board_init(void);

/* ==================== LED（低电平点亮） ==================== */

#define LED_R_PORT      GPIOF
#define LED_R_PIN       6U      /* TIM10_CH1 AF3 */
#define LED_G_PORT      GPIOF
#define LED_G_PIN       7U      /* TIM12_CH1 AF9 */
#define LED_B_PORT      GPIOF
#define LED_B_PIN       8U      /* TIM13_CH1 AF9 */

/* LED PWM 复用编号（与 board.h 保持一致，此处重申供外设测试使用） */
#define LED_R_AF        3U      /* TIM10_CH1 AF3 */
#define LED_G_AF        9U      /* TIM12_CH1 AF9 */
#define LED_B_AF        9U      /* TIM13_CH1 AF9 */

/* ==================== KEY ==================== */

#define KEY1_PORT       GPIOA
#define KEY1_PIN        0U      /* 按下高电平 */
#define KEY2_PORT       GPIOC
#define KEY2_PIN        13U     /* 按下低电平 */

/* ==================== USART1 调试串口 ==================== */

#define CONSOLE_USART   USART1
#define CONSOLE_TX_PORT GPIOA
#define CONSOLE_TX_PIN  9U
#define CONSOLE_RX_PORT GPIOA
#define CONSOLE_RX_PIN  10U
#define CONSOLE_AF      7U
#define CONSOLE_BAUD    115200U

/* ==================== USART3 WiFi ESP8266 ==================== */

#define WIFI_USART      USART3
#define WIFI_TX_PORT    GPIOB
#define WIFI_TX_PIN     10U
#define WIFI_RX_PORT    GPIOB
#define WIFI_RX_PIN     11U
#define WIFI_AF         7U

/* ==================== SPI1 Flash W25Q128JV ==================== */

#define FLASH_SPI           SPI1
#define FLASH_CS_PORT       GPIOG
#define FLASH_CS_PIN        6U
#define FLASH_SCK_PORT      GPIOB
#define FLASH_SCK_PIN       3U
#define FLASH_MISO_PORT     GPIOB
#define FLASH_MISO_PIN      4U
#define FLASH_MOSI_PORT     GPIOB
#define FLASH_MOSI_PIN      5U
#define FLASH_SPI_AF        5U

/* ==================== I2C1 传感器总线 ==================== */

#define SENSOR_I2C          I2C1
#define SENSOR_SCL_PORT     GPIOB
#define SENSOR_SCL_PIN      8U
#define SENSOR_SDA_PORT     GPIOB
#define SENSOR_SDA_PIN      9U
#define SENSOR_I2C_AF       4U

/* I2C 设备地址 */
#define AT24C02_ADDR        0x50U
#define MPU6050_ADDR        0x68U
#define AP3216C_ADDR        0x1EU

/* 传感器中断引脚 */
#define MPU6050_INT_PORT    GPIOF
#define MPU6050_INT_PIN     10U
#define AP3216C_INT_PORT    GPIOE
#define AP3216C_INT_PIN     3U

/* ==================== ADC 电位器 ==================== */

#define POT_ADC             ADC1
#define POT_CHANNEL         8U      /* PB0 = ADC1_IN8 */

/* ==================== DAC ==================== */

#define DAC_CH1_PORT        GPIOA
#define DAC_CH1_PIN         4U
#define DAC_CH2_PORT        GPIOA
#define DAC_CH2_PIN         5U

/* ==================== 红外接收 ==================== */

#define IR_PORT             GPIOA
#define IR_PIN              8U

/* ==================== 蜂鸣器（高电平有效，禁止误操作） ==================== */

#define BUZZER_PORT         GPIOG
#define BUZZER_PIN          7U

#endif /* __BOARD_H__ */
