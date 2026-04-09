#include "drv_cryp.h"

#ifdef DRV_USING_CRYP

/* ==================== 时钟使能 ==================== */

/* 使能 CRYP 外设 AHB2 总线时钟，回读寄存器确保写入生效 */
void cryp_clk_enable(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_CRYPEN;
    (void)RCC->AHB2ENR;
}

/* ==================== 初始化 ==================== */

/* 根据算法类型、密钥长度和加解密方向初始化 CRYP 外设 */
void cryp_init(cryp_algo_t algo, cryp_keysize_t keysize, uint8_t decrypt)
{
    uint32_t cr;

    cryp_clk_enable();

    cr = 0;
    cr |= ((uint32_t)(algo & 0x07U) << 3U);
    cr |= ((uint32_t)(keysize & 0x03U) << 8U);  /* KEYSIZE[1:0] */

    if (decrypt)
    {
        cr |= CRYP_CR_ALGODIR;
    }

    CRYP->CR = cr;
    CRYP->CR |= CRYP_CR_CRYPEN;
}

/* ==================== 密钥 / 初始化向量 ==================== */

/* 根据密钥长度将密钥写入对应的 K0~K3 寄存器 */
void cryp_set_key(const uint32_t *key, cryp_keysize_t keysize)
{
    if (key == NULL) { return; }

    if (keysize == CRYP_KEYSIZE_256)
    {
        CRYP->K0LR = key[0];
        CRYP->K0RR = key[1];
        CRYP->K1LR = key[2];
        CRYP->K1RR = key[3];
        CRYP->K2LR = key[4];
        CRYP->K2RR = key[5];
        CRYP->K3LR = key[6];
        CRYP->K3RR = key[7];
    }
    else if (keysize == CRYP_KEYSIZE_192)
    {
        CRYP->K1LR = key[0];
        CRYP->K1RR = key[1];
        CRYP->K2LR = key[2];
        CRYP->K2RR = key[3];
        CRYP->K3LR = key[4];
        CRYP->K3RR = key[5];
    }
    else  /* CRYP_KEYSIZE_128 */
    {
        CRYP->K2LR = key[0];
        CRYP->K2RR = key[1];
        CRYP->K3LR = key[2];
        CRYP->K3RR = key[3];
    }
}

/* 加载 4 个 32 位字的初始化向量到 IV0/IV1 寄存器 */
void cryp_set_iv(const uint32_t *iv)
{
    if (iv == NULL) { return; }

    CRYP->IV0LR = iv[0];
    CRYP->IV0RR = iv[1];
    CRYP->IV1LR = iv[2];
    CRYP->IV1RR = iv[3];
}

/* ==================== 数据读写 ==================== */

/* 等待输入 FIFO 非满（IFEM），写入一个 32 位数据字 */
void cryp_write(uint32_t data)
{
    while ((CRYP->SR & CRYP_SR_IFEM) == 0U)
    {
    }

    CRYP->DIN = data;
}

/* 等待输出 FIFO 非空（OFNE），读取一个 32 位结果字 */
uint32_t cryp_read(void)
{
    while ((CRYP->SR & CRYP_SR_OFNE) == 0U)
    {
    }

    return CRYP->DOUT;
}

/* ==================== 使能 / 禁用 ==================== */

/* 置位 CRYPEN 位，使能 CRYP 处理器 */
void cryp_enable(void)
{
    CRYP->CR |= CRYP_CR_CRYPEN;
}

/* 清除 CRYPEN 位，禁用 CRYP 处理器 */
void cryp_disable(void)
{
    CRYP->CR &= ~CRYP_CR_CRYPEN;
}

#endif /* DRV_USING_CRYP */
