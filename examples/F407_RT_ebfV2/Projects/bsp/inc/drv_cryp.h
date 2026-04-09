#ifndef __DRV_CRYP_H__
#define __DRV_CRYP_H__

/*
 * drv_cryp.h - STM32F407 CRYP 驱动头文件
 *
 * 当前版本定义：
 *   - 算法枚举 cryp_algo_t（DES/TDES/AES 各模式）
 *   - 密钥长度枚举 cryp_keysize_t（128/192/256 位）
 *
 * 覆盖范围：
 *   - CRYP 外设时钟使能
 *   - CRYP 算法及密钥长度初始化
 *   - 密钥和初始化向量（IV）加载
 *   - 32 位数据读写（阻塞等待 FIFO 标志）
 *   - CRYP 处理器使能与禁用
 *
 * 限制：
 *   - 仅支持 DES/TDES/AES 的 ECB、CBC、CTR 模式
 *   - 数据读写为阻塞调用，FIFO 未就绪时将一直等待
 *   - 无 DMA 及中断模式支持，仅 CPU 轮询
 */

#include "drv_config.h"

#ifdef DRV_USING_CRYP

#include "stm32f4xx.h"

/* ==================== 类型定义 ==================== */

typedef enum
{
    CRYP_ALGO_DES_ECB  = 0,
    CRYP_ALGO_DES_CBC  = 1,
    CRYP_ALGO_TDES_ECB = 2,
    CRYP_ALGO_TDES_CBC = 3,
    CRYP_ALGO_AES_ECB  = 4,
    CRYP_ALGO_AES_CBC  = 5,
    CRYP_ALGO_AES_CTR  = 6,
    CRYP_ALGO_AES_KEY  = 7
} cryp_algo_t;

typedef enum
{
    CRYP_KEYSIZE_128 = 0,  /* AES-128, 4 words */
    CRYP_KEYSIZE_192 = 1,  /* AES-192, 6 words */
    CRYP_KEYSIZE_256 = 2   /* AES-256, 8 words */
} cryp_keysize_t;

/* ==================== API 声明 ==================== */

/* 说明：使能 CRYP 外设的 AHB2 总线时钟 */
void cryp_clk_enable(void);

/* 说明：根据算法类型和密钥长度初始化 CRYP 外设 */
void cryp_init(cryp_algo_t algo, cryp_keysize_t keysize, uint8_t decrypt);

/* 说明：加载密钥（根据密钥长度选择起始寄存器） */
void cryp_set_key(const uint32_t *key, cryp_keysize_t keysize);

/* 说明：加载初始化向量（4 个 32 位字） */
void cryp_set_iv(const uint32_t *iv);

/* 说明：写入一个 32 位数据字（等待 IFEM 标志） */
void cryp_write(uint32_t data);

/* 说明：读取一个 32 位结果字（等待 OFNE 标志） */
uint32_t cryp_read(void);

/* 说明：使能 CRYP 处理器 */
void cryp_enable(void);

/* 说明：禁用 CRYP 处理器 */
void cryp_disable(void);

#endif /* DRV_USING_CRYP */
#endif /* __DRV_CRYP_H__ */
