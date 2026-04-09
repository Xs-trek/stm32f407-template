#include "drv_crc.h"

#ifdef DRV_USING_CRC

/* ==================== 时钟使能 ==================== */

/* 使能 CRC 外设 AHB1 总线时钟，回读寄存器确保写入生效 */
void crc_clk_enable(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;
    (void)RCC->AHB1ENR;
}

/* ==================== 复位 ==================== */

/* 复位 CRC 数据寄存器为初始值 */
void crc_reset(void)
{
    CRC->CR = CRC_CR_RESET;
}

/* ==================== 单次读写 ==================== */

/* 向 CRC 数据寄存器写入一个 32 位字 */
void crc_write(uint32_t data)
{
    CRC->DR = data;
}

/* 读取当前 CRC 计算结果 */
uint32_t crc_read(void)
{
    return CRC->DR;
}

/* ==================== 批量计算 ==================== */

/* 对连续 32 位字数组执行 CRC-32 计算，返回校验值 */
uint32_t crc_calc32(const uint32_t *data, uint32_t word_count)
{
    uint32_t i;

    if (data == NULL || word_count == 0U)
    {
        return 0U;
    }

    crc_clk_enable();
    crc_reset();

    for (i = 0U; i < word_count; i++)
    {
        crc_write(data[i]);
    }

    return crc_read();
}

#endif /* DRV_USING_CRC */
