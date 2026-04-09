#include "drv_exti.h"

#ifdef DRV_USING_EXTI

/* ==================== 内部辅助函数 ==================== */

/* 说明：检查 EXTI line 是否在当前版本支持范围内。 */
static int exti_line_valid(uint8_t line)
{
    return (line <= EXTI_LINE_MAX);
}

/* 说明：检查 GPIO EXTI line 是否在 0~15 范围内。 */
static int exti_gpio_line_valid(uint8_t pin)
{
    return (pin <= EXTI_GPIO_MAX);
}

/* 说明：检查触发方式参数是否合法。 */
static int exti_trigger_valid(exti_trigger_t trigger)
{
    return (trigger == EXTI_TRIGGER_RISING) ||
           (trigger == EXTI_TRIGGER_FALLING) ||
           (trigger == EXTI_TRIGGER_BOTH);
}

/* 说明：将 GPIO 端口实例转换为 SYSCFG EXTICR 端口编码。 */
static uint32_t exti_port_code(GPIO_TypeDef *port)
{
    if (port == GPIOA) { return 0U; }
    if (port == GPIOB) { return 1U; }
    if (port == GPIOC) { return 2U; }
    if (port == GPIOD) { return 3U; }
    if (port == GPIOE) { return 4U; }
    if (port == GPIOF) { return 5U; }
    if (port == GPIOG) { return 6U; }
    if (port == GPIOH) { return 7U; }
    if (port == GPIOI) { return 8U; }

    return 0xFFU;
}

/*
 * 说明：
 * 检查 GPIO 映射组合是否合法。
 *
 * 当前版本语义：
 * 1. 仅允许 GPIOA~GPIOI
 * 2. pin 仅允许 0~15
 * 3. STM32F407 上 PI12~PI15 不允许映射为 EXTI 输入源
 */
static int exti_gpio_route_valid(GPIO_TypeDef *port, uint8_t pin)
{
    if (port == NULL) { return 0; }
    if (!exti_gpio_line_valid(pin)) { return 0; }
    if (exti_port_code(port) > 8U) { return 0; }

    if ((port == GPIOI) && (pin >= 12U))
    {
        return 0;
    }

    return 1;
}

/* ==================== SYSCFG 时钟 ==================== */

void exti_syscfg_clk_enable(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    (void)RCC->APB2ENR;
}

/* ==================== GPIO 到 EXTI 映射 ==================== */

void exti_map_gpio(GPIO_TypeDef *port, uint8_t pin)
{
    uint32_t code;
    uint32_t index;
    uint32_t shift;

    if (!exti_gpio_route_valid(port, pin)) { return; }

    code = exti_port_code(port);

    exti_syscfg_clk_enable();

    index = ((uint32_t)pin >> 2U);
    shift = (((uint32_t)pin & 0x03U) << 2U);

    SYSCFG->EXTICR[index] &= ~(0x0FU << shift);
    SYSCFG->EXTICR[index] |= (code << shift);
}

/* ==================== 触发配置 ==================== */

void exti_set_trigger(uint8_t line, exti_trigger_t trigger)
{
    uint32_t mask;

    if (!exti_line_valid(line)) { return; }
    if (!exti_trigger_valid(trigger)) { return; }

    mask = (1U << line);

    EXTI->RTSR &= ~mask;
    EXTI->FTSR &= ~mask;

    if ((trigger == EXTI_TRIGGER_RISING) || (trigger == EXTI_TRIGGER_BOTH))
    {
        EXTI->RTSR |= mask;
    }

    if ((trigger == EXTI_TRIGGER_FALLING) || (trigger == EXTI_TRIGGER_BOTH))
    {
        EXTI->FTSR |= mask;
    }
}

/* ==================== 中断 / 事件屏蔽 ==================== */

void exti_enable_interrupt(uint8_t line)
{
    if (!exti_line_valid(line)) { return; }

    EXTI->IMR |= (1U << line);
}

void exti_disable_interrupt(uint8_t line)
{
    if (!exti_line_valid(line)) { return; }

    EXTI->IMR &= ~(1U << line);
}

void exti_enable_event(uint8_t line)
{
    if (!exti_line_valid(line)) { return; }

    EXTI->EMR |= (1U << line);
}

void exti_disable_event(uint8_t line)
{
    if (!exti_line_valid(line)) { return; }

    EXTI->EMR &= ~(1U << line);
}

/* ==================== 挂起 / 软件触发 ==================== */

uint8_t exti_get_pending(uint8_t line)
{
    if (!exti_line_valid(line)) { return 0U; }

    return (uint8_t)((EXTI->PR >> line) & 0x01U);
}

void exti_clear_pending(uint8_t line)
{
    if (!exti_line_valid(line)) { return; }

    EXTI->PR = (1U << line);
}

void exti_software_trigger(uint8_t line)
{
    if (!exti_line_valid(line)) { return; }

    EXTI->SWIER |= (1U << line);
}

/* ==================== IRQn 查询 ==================== */

IRQn_Type exti_get_irqn(uint8_t pin)
{
    if (!exti_gpio_line_valid(pin))
    {
        return EXTI_INVALID_IRQN;
    }

    switch (pin)
    {
    case 0U: return EXTI0_IRQn;
    case 1U: return EXTI1_IRQn;
    case 2U: return EXTI2_IRQn;
    case 3U: return EXTI3_IRQn;
    case 4U: return EXTI4_IRQn;
    default:
        if (pin <= 9U)
        {
            return EXTI9_5_IRQn;
        }
        else
        {
            return EXTI15_10_IRQn;
        }
    }
}

/* ==================== 轻量初始化辅助 ==================== */

void exti_init_gpio_interrupt(GPIO_TypeDef *port, uint8_t pin, exti_trigger_t trigger)
{
    if (!exti_gpio_route_valid(port, pin)) { return; }
    if (!exti_trigger_valid(trigger)) { return; }

    exti_map_gpio(port, pin);
    exti_set_trigger(pin, trigger);
    exti_enable_interrupt(pin);
}

#endif /* DRV_USING_EXTI */
