#ifndef __DRV_GPIO_H__
#define __DRV_GPIO_H__

#include "drv_config.h"

#ifdef DRV_USING_GPIO

#include "stm32f4xx.h"

/*
 * 说明：
 * 本头文件提供 STM32F407 GPIO 驱动的公共接口。
 *
 * 当前版本定义：
 * 1. 当前版本仅覆盖 GPIO 驱动本身闭合的基础功能。
 * 2. 当前版本覆盖：
 *    - GPIO 端口时钟使能
 *    - 单引脚模式 / AF / 速度 / 上下拉 / 输出类型配置
 *    - 掩码批量模式配置
 *    - GPIO 锁定序列
 *    - 便捷初始化函数（output_pp / output_od / af_pp / af_od / input / analog）
 *    - 单引脚 I/O（inline：set / reset / toggle / write / read / read_output）
 *    - 掩码 I/O（inline：set_mask / reset_mask / read_port）
 *
 * 限制：
 * 1. 当前版本不提供 EXTI 配置（由 drv_exti 负责）。
 * 2. 当前版本不提供端口级批量 AF 配置。
 * 3. 当前版本不提供位带访问接口。
 */

/* ==================== 类型定义 ==================== */

/* 说明：GPIO 模式。 */
typedef enum
{
    GPIO_MODE_INPUT  = 0,
    GPIO_MODE_OUTPUT = 1,
    GPIO_MODE_AF     = 2,
    GPIO_MODE_ANALOG = 3
} gpio_mode_t;

/* 说明：GPIO 输出速度。 */
typedef enum
{
    GPIO_SPEED_LOW  = 0,
    GPIO_SPEED_MED  = 1,
    GPIO_SPEED_FAST = 2,
    GPIO_SPEED_HIGH = 3
} gpio_speed_t;

/* 说明：GPIO 上下拉。 */
typedef enum
{
    GPIO_PULL_NONE = 0,
    GPIO_PULL_UP   = 1,
    GPIO_PULL_DOWN = 2
} gpio_pull_t;

/* 说明：GPIO 输出类型。 */
typedef enum
{
    GPIO_OTYPE_PP = 0,
    GPIO_OTYPE_OD = 1
} gpio_otype_t;

/* ==================== 时钟控制 ==================== */

/* 说明：使能指定 GPIO 端口时钟。 */
void gpio_clk_enable(GPIO_TypeDef *port);

/* ==================== 单引脚配置 ==================== */

/* 说明：设置引脚模式。 */
void gpio_set_mode(GPIO_TypeDef *port, uint8_t pin, gpio_mode_t mode);

/* 说明：设置引脚复用功能编号（AF0~AF15）。 */
void gpio_set_af(GPIO_TypeDef *port, uint8_t pin, uint8_t af_num);

/* 说明：设置引脚输出速度。 */
void gpio_set_speed(GPIO_TypeDef *port, uint8_t pin, gpio_speed_t speed);

/* 说明：设置引脚上下拉。 */
void gpio_set_pull(GPIO_TypeDef *port, uint8_t pin, gpio_pull_t pull);

/* 说明：设置引脚输出类型。 */
void gpio_set_otype(GPIO_TypeDef *port, uint8_t pin, gpio_otype_t otype);

/* ==================== 掩码配置 ==================== */

/* 说明：按掩码批量设置引脚模式。 */
void gpio_set_mode_mask(GPIO_TypeDef *port, uint16_t pin_mask, gpio_mode_t mode);

/* ==================== 锁定 ==================== */

/*
 * 说明：
 * 执行 GPIO 配置锁定序列。
 *
 * 当前版本定义：
 * 返回 1 表示锁定成功，0 表示锁定序列失败。
 */
int gpio_lock(GPIO_TypeDef *port, uint8_t pin);

/* ==================== 便捷初始化 ==================== */

/* 说明：初始化为推挽输出（高速、无上下拉）。 */
void gpio_init_output_pp(GPIO_TypeDef *port, uint8_t pin);

/* 说明：初始化为开漏输出（高速、可选上下拉）。 */
void gpio_init_output_od(GPIO_TypeDef *port, uint8_t pin, gpio_pull_t pull);

/* 说明：初始化为复用推挽（高速、无上下拉）。 */
void gpio_init_af_pp(GPIO_TypeDef *port, uint8_t pin, uint8_t af_num);

/* 说明：初始化为复用开漏（高速、可选上下拉）。 */
void gpio_init_af_od(GPIO_TypeDef *port, uint8_t pin, uint8_t af_num, gpio_pull_t pull);

/* 说明：初始化为输入（可选上下拉）。 */
void gpio_init_input(GPIO_TypeDef *port, uint8_t pin, gpio_pull_t pull);

/* 说明：初始化为模拟模式（无上下拉）。 */
void gpio_init_analog(GPIO_TypeDef *port, uint8_t pin);

/* ==================== 单引脚 I/O（inline） ==================== */

static inline void gpio_set(GPIO_TypeDef *port, uint8_t pin)
{
    port->BSRR = (1U << pin);
}

static inline void gpio_reset(GPIO_TypeDef *port, uint8_t pin)
{
    port->BSRR = (1U << (pin + 16U));
}

static inline void gpio_toggle(GPIO_TypeDef *port, uint8_t pin)
{
    port->ODR ^= (1U << pin);
}

static inline void gpio_write(GPIO_TypeDef *port, uint8_t pin, uint8_t value)
{
    if (value)
    {
        port->BSRR = (1U << pin);
    }
    else
    {
        port->BSRR = (1U << (pin + 16U));
    }
}

static inline uint8_t gpio_read(GPIO_TypeDef *port, uint8_t pin)
{
    return (uint8_t)((port->IDR >> pin) & 0x01U);
}

static inline uint8_t gpio_read_output(GPIO_TypeDef *port, uint8_t pin)
{
    return (uint8_t)((port->ODR >> pin) & 0x01U);
}

/* ==================== 掩码 I/O（inline） ==================== */

static inline void gpio_set_mask(GPIO_TypeDef *port, uint16_t pin_mask)
{
    port->BSRR = (uint32_t)pin_mask;
}

static inline void gpio_reset_mask(GPIO_TypeDef *port, uint16_t pin_mask)
{
    port->BSRR = ((uint32_t)pin_mask << 16U);
}

static inline uint16_t gpio_read_port(GPIO_TypeDef *port)
{
    return (uint16_t)(port->IDR);
}

#endif /* DRV_USING_GPIO */
#endif /* __DRV_GPIO_H__ */
