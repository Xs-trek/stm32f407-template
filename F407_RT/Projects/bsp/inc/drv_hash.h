#ifndef __DRV_HASH_H__
#define __DRV_HASH_H__

#include "drv_config.h"

#ifdef DRV_USING_HASH

/*
 * 当前版本定义：HASH 外设驱动（SHA-1 / MD5）
 * 覆盖范围    ：时钟使能、算法初始化、数据写入、摘要读取
 * 限制        ：仅支持 SHA-1（5 字）和 MD5（4 字）两种算法
 */

#include "stm32f4xx.h"

/* ==================== 类型定义 ==================== */

typedef enum
{
    HASH_ALGO_SHA1 = 0,
    HASH_ALGO_MD5  = 1
} hash_algo_t;

/* ==================== 时钟控制 ==================== */

/* 说明：使能 HASH 外设时钟 */
void hash_clk_enable(void);

/* ==================== 初始化 ==================== */

/* 说明：按指定算法初始化 HASH 处理器 */
void hash_init(hash_algo_t algo);

/* ==================== 数据操作 ==================== */

/* 说明：写入一个 32 位数据字（等待 DINIS 标志置位） */
void hash_write(uint32_t data);

/* 说明：启动最终摘要计算（等待 DCIS 标志置位） */
void hash_start_digest(void);

/* 说明：读取摘要字（SHA-1 为 5 字，MD5 为 4 字） */
void hash_read_digest(uint32_t *digest, uint8_t word_count);

#endif /* DRV_USING_HASH */
#endif /* __DRV_HASH_H__ */
