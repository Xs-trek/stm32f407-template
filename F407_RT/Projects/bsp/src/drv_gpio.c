#include "drv_gpio.h"

#ifdef DRV_USING_GPIO

#include "drv_rcc.h"

/* ==================== 内部辅助函数 ==================== */

static uint32_t gpio_pin_shift2(uint8_t pin)
{
    return ((uint32_t)pin << 1U);
}

/* ==================== 时钟控制 ==================== */

void gpio_clk_enable(GPIO_TypeDef *port)
{
    if (port == NULL) { return; }

#ifdef DRV_USING_RCC
    rcc_enable_gpio_clk(port);
#else
    if (port == GPIOA)      { RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; }
    else if (port == GPIOB) { RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; }
    else if (port == GPIOC) { RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; }
    else if (port == GPIOD) { RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; }
    else if (port == GPIOE) { RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; }
    else if (port == GPIOF) { RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN; }
    else if (port == GPIOG) { RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN; }
    else if (port == GPIOH) { RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN; }
    else if (port == GPIOI) { RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN; }
    else { return; }

    (void)RCC->AHB1ENR;
#endif
}

/* ==================== 单引脚配置 ==================== */

void gpio_set_mode(GPIO_TypeDef *port, uint8_t pin, gpio_mode_t mode)
{
    uint32_t shift;

    if (port == NULL || pin > 15U) { return; }

    shift = gpio_pin_shift2(pin);
    port->MODER &= ~(3U << shift);
    port->MODER |= ((uint32_t)mode << shift);
}

void gpio_set_af(GPIO_TypeDef *port, uint8_t pin, uint8_t af_num)
{
    uint32_t idx;
    uint32_t shift;

    if (port == NULL || pin > 15U) { return; }

    idx   = ((uint32_t)pin >> 3U);
    shift = (((uint32_t)pin & 0x07U) << 2U);

    port->AFR[idx] &= ~(0x0FU << shift);
    port->AFR[idx] |= (((uint32_t)af_num & 0x0FU) << shift);
}

void gpio_set_speed(GPIO_TypeDef *port, uint8_t pin, gpio_speed_t speed)
{
    uint32_t shift;

    if (port == NULL || pin > 15U) { return; }

    shift = gpio_pin_shift2(pin);
    port->OSPEEDR &= ~(3U << shift);
    port->OSPEEDR |= ((uint32_t)speed << shift);
}

void gpio_set_pull(GPIO_TypeDef *port, uint8_t pin, gpio_pull_t pull)
{
    uint32_t shift;

    if (port == NULL || pin > 15U) { return; }

    shift = gpio_pin_shift2(pin);
    port->PUPDR &= ~(3U << shift);
    port->PUPDR |= ((uint32_t)pull << shift);
}

void gpio_set_otype(GPIO_TypeDef *port, uint8_t pin, gpio_otype_t otype)
{
    if (port == NULL || pin > 15U) { return; }

    port->OTYPER &= ~(1U << pin);
    port->OTYPER |= ((uint32_t)otype << pin);
}

/* ==================== 掩码配置 ==================== */

void gpio_set_mode_mask(GPIO_TypeDef *port, uint16_t pin_mask, gpio_mode_t mode)
{
    uint8_t pin;
    uint32_t shift;

    if (port == NULL || pin_mask == 0U) { return; }

    for (pin = 0U; pin < 16U; pin++)
    {
        if (pin_mask & (1U << pin))
        {
            shift = gpio_pin_shift2(pin);
            port->MODER &= ~(3U << shift);
            port->MODER |= ((uint32_t)mode << shift);
        }
    }
}

/* ==================== 锁定 ==================== */

int gpio_lock(GPIO_TypeDef *port, uint8_t pin)
{
    uint32_t pin_bit;
    uint32_t lock;

    if (port == NULL || pin > 15U) { return 0; }

    pin_bit = (1U << pin);
    lock    = pin_bit | GPIO_LCKR_LCKK;

    /* RM0090 锁定键写入序列 */
    port->LCKR = lock;          /* WR LCKR[16]=1, LCKR[pin]=1 */
    port->LCKR = pin_bit;       /* WR LCKR[16]=0, LCKR[pin]=1 */
    port->LCKR = lock;          /* WR LCKR[16]=1, LCKR[pin]=1 */
    (void)port->LCKR;           /* RD LCKR */

    return (port->LCKR & GPIO_LCKR_LCKK) ? 1 : 0;
}

/* ==================== 便捷初始化 ==================== */

void gpio_init_output_pp(GPIO_TypeDef *port, uint8_t pin)
{
    gpio_clk_enable(port);
    gpio_set_mode(port, pin, GPIO_MODE_OUTPUT);
    gpio_set_otype(port, pin, GPIO_OTYPE_PP);
    gpio_set_speed(port, pin, GPIO_SPEED_HIGH);
    gpio_set_pull(port, pin, GPIO_PULL_NONE);
}

void gpio_init_output_od(GPIO_TypeDef *port, uint8_t pin, gpio_pull_t pull)
{
    gpio_clk_enable(port);
    gpio_set_mode(port, pin, GPIO_MODE_OUTPUT);
    gpio_set_otype(port, pin, GPIO_OTYPE_OD);
    gpio_set_speed(port, pin, GPIO_SPEED_HIGH);
    gpio_set_pull(port, pin, pull);
}

void gpio_init_af_pp(GPIO_TypeDef *port, uint8_t pin, uint8_t af_num)
{
    gpio_clk_enable(port);
    gpio_set_af(port, pin, af_num);
    gpio_set_mode(port, pin, GPIO_MODE_AF);
    gpio_set_otype(port, pin, GPIO_OTYPE_PP);
    gpio_set_speed(port, pin, GPIO_SPEED_HIGH);
    gpio_set_pull(port, pin, GPIO_PULL_NONE);
}

void gpio_init_af_od(GPIO_TypeDef *port, uint8_t pin, uint8_t af_num, gpio_pull_t pull)
{
    gpio_clk_enable(port);
    gpio_set_af(port, pin, af_num);
    gpio_set_mode(port, pin, GPIO_MODE_AF);
    gpio_set_otype(port, pin, GPIO_OTYPE_OD);
    gpio_set_speed(port, pin, GPIO_SPEED_HIGH);
    gpio_set_pull(port, pin, pull);
}

void gpio_init_input(GPIO_TypeDef *port, uint8_t pin, gpio_pull_t pull)
{
    gpio_clk_enable(port);
    gpio_set_mode(port, pin, GPIO_MODE_INPUT);
    gpio_set_pull(port, pin, pull);
}

void gpio_init_analog(GPIO_TypeDef *port, uint8_t pin)
{
    gpio_clk_enable(port);
    gpio_set_mode(port, pin, GPIO_MODE_ANALOG);
    gpio_set_pull(port, pin, GPIO_PULL_NONE);
}

#endif /* DRV_USING_GPIO */
