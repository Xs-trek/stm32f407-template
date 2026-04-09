#include "drv_rng.h"

#ifdef DRV_USING_RNG

/* ==================== 时钟使能 ==================== */

/* 使能 RNG 外设 AHB2 总线时钟，回读寄存器确保写入生效 */
void rng_clk_enable(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;
    (void)RCC->AHB2ENR;
}

/* ==================== 初始化 / 反初始化 ==================== */

/* 使能时钟并开启 RNG 外设 */
void rng_init(void)
{
    rng_clk_enable();
    RNG->CR |= RNG_CR_RNGEN;
}

/* 关闭 RNG 外设 */
void rng_deinit(void)
{
    RNG->CR &= ~RNG_CR_RNGEN;
}

/* ==================== 数据获取 ==================== */

/* 阻塞等待数据就绪，返回 32 位随机数 */
uint32_t rng_get(void)
{
    while ((RNG->SR & RNG_SR_DRDY) == 0U) { /* 等待数据就绪 */ }
    return RNG->DR;
}

/* ==================== 状态查询 ==================== */

/* 查询数据就绪标志，就绪返回 1，否则返回 0 */
uint8_t rng_data_ready(void)
{
    return (uint8_t)((RNG->SR & RNG_SR_DRDY) ? 1U : 0U);
}

/* 检查种子错误（SECS）和时钟错误（CECS），有错误返回 1 */
uint8_t rng_check_error(void)
{
    return (uint8_t)((RNG->SR & (RNG_SR_SECS | RNG_SR_CECS)) ? 1U : 0U);
}

#endif /* DRV_USING_RNG */
