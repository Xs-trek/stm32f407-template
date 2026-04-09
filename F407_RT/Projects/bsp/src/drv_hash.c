#include "drv_hash.h"

#ifdef DRV_USING_HASH

/* ==================== 时钟控制 ==================== */

/* 说明：使能 HASH 外设时钟 */
void hash_clk_enable(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_HASHEN;
    (void)RCC->AHB2ENR;
}

/* ==================== 初始化 ==================== */

/* 说明：按指定算法初始化 HASH 处理器 */
void hash_init(hash_algo_t algo)
{
    hash_clk_enable();

    HASH->CR = HASH_CR_INIT;
    if (algo == HASH_ALGO_MD5)
    {
        HASH->CR |= HASH_CR_ALGO;
    }
}

/* ==================== 数据操作 ==================== */

/* 说明：写入一个 32 位数据字（等待 DINIS 标志置位） */
void hash_write(uint32_t data)
{
    while ((HASH->SR & HASH_SR_DINIS) == 0U)
    {
    }

    HASH->DIN = data;
}

/* 说明：启动最终摘要计算（等待 DCIS 标志置位） */
void hash_start_digest(void)
{
    HASH->STR |= HASH_STR_DCAL;
    while ((HASH->SR & HASH_SR_DCIS) == 0U)
    {
    }
}

/* 说明：读取摘要字（SHA-1 为 5 字，MD5 为 4 字） */
void hash_read_digest(uint32_t *digest, uint8_t word_count)
{
    uint8_t i;

    if (digest == NULL) { return; }
    if (word_count > 5U) { word_count = 5U; }  /* SHA-1 最大 = 5 字 */

    for (i = 0; i < word_count; i++)
    {
        digest[i] = *(&HASH->HR0 + i);
    }
}

#endif /* DRV_USING_HASH */
