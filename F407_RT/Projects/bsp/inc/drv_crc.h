#ifndef __DRV_CRC_H__
#define __DRV_CRC_H__

/*
 * drv_crc.h - STM32F407 CRC 驱动头文件
 *
 * 覆盖范围：
 *   - CRC 外设时钟使能
 *   - CRC 数据寄存器复位
 *   - 单次写入 / 读取 CRC 数据寄存器
 *   - 对连续 32 位字数组执行 CRC-32 计算
 *
 * 限制：
 *   - 仅支持硬件固定的 CRC-32 多项式（0x04C11DB7），不可更改
 *   - 无 DMA 支持，仅 CPU 轮询写入
 *   - 输入数据必须按 32 位字对齐
 */

#include "drv_config.h"

#ifdef DRV_USING_CRC

#include "stm32f4xx.h"

/* ==================== API 声明 ==================== */

/* 说明：使能 CRC 外设的 AHB1 总线时钟 */
void     crc_clk_enable(void);

/* 说明：复位 CRC 数据寄存器为初始值 0xFFFFFFFF */
void     crc_reset(void);

/* 说明：向 CRC 数据寄存器写入一个 32 位字 */
void     crc_write(uint32_t data);

/* 说明：读取当前 CRC 计算结果 */
uint32_t crc_read(void);

/* 说明：对 word_count 个连续 32 位字执行 CRC-32 计算并返回结果 */
uint32_t crc_calc32(const uint32_t *data, uint32_t word_count);

#endif /* DRV_USING_CRC */
#endif /* __DRV_CRC_H__ */
