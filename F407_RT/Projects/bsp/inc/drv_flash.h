#ifndef __DRV_FLASH_H__
#define __DRV_FLASH_H__

#include "drv_config.h"

#ifdef DRV_USING_FLASH

#include "stm32f4xx.h"

/*
 * 说明：
 * 本头文件提供 STM32F407 内部 Flash 驱动的公共接口。
 *
 * 当前版本定义：
 * 1. 当前版本覆盖：
 *    - Flash 访问控制（等待周期 / 预取 / 指令缓存 / 数据缓存）
 *    - Flash 解锁 / 锁定
 *    - Flash 等待操作完成
 *    - 扇区擦除（0~11）
 *    - 编程（字 / 半字 / 字节）
 *    - 扇区地址查询
 *    - Option Bytes 解锁 / 锁定 / 启动
 *
 * 限制：
 * 1. 当前版本不提供 mass erase 接口。
 * 2. 当前版本不提供读保护等级配置接口。
 * 3. flash_wait_done() 为无超时阻塞等待。
 */

/* ==================== 类型定义 ==================== */

/* 说明：Flash 操作状态。 */
typedef enum
{
    FLASH_STATUS_OK = 0,
    FLASH_STATUS_BUSY,
    FLASH_STATUS_ERROR_PG,
    FLASH_STATUS_ERROR_PGP,
    FLASH_STATUS_ERROR_PGA,
    FLASH_STATUS_ERROR_WRP,
} flash_status_t;

/* ==================== 访问控制（ACR） ==================== */

/* 说明：设置 Flash 读取等待周期（0~15）。 */
void flash_set_latency(uint32_t latency);

/* 说明：使能指令预取缓冲区。 */
void flash_prefetch_enable(void);

/* 说明：禁用指令预取缓冲区。 */
void flash_prefetch_disable(void);

/* 说明：使能指令缓存。 */
void flash_icache_enable(void);

/* 说明：禁用指令缓存。 */
void flash_icache_disable(void);

/* 说明：使能数据缓存。 */
void flash_dcache_enable(void);

/* 说明：禁用数据缓存。 */
void flash_dcache_disable(void);

/* ==================== 解锁 / 锁定 ==================== */

/* 说明：解锁 Flash 控制寄存器。 */
void flash_unlock(void);

/* 说明：锁定 Flash 控制寄存器。 */
void flash_lock(void);

/* ==================== 状态 / 等待 ==================== */

/*
 * 说明：
 * 阻塞等待 Flash 操作完成。
 *
 * 限制：
 * 当前版本无超时保护。
 */
flash_status_t flash_wait_done(void);

/* ==================== 擦除 ==================== */

/*
 * 说明：
 * 擦除指定扇区（0~11，STM32F407）。
 *
 * 前提：
 * Flash 已解锁。
 */
flash_status_t flash_erase_sector(uint8_t sector);

/* ==================== 编程 ==================== */

/* 说明：编程一个 32 位字。前提：Flash 已解锁。 */
flash_status_t flash_program_word(uint32_t addr, uint32_t data);

/* 说明：编程一个 16 位半字。前提：Flash 已解锁。 */
flash_status_t flash_program_halfword(uint32_t addr, uint16_t data);

/* 说明：编程一个字节。前提：Flash 已解锁。 */
flash_status_t flash_program_byte(uint32_t addr, uint8_t data);

/* ==================== 扇区地址查询 ==================== */

/* 说明：获取指定扇区的基地址。 */
uint32_t flash_get_sector_addr(uint8_t sector);

/* ==================== Option Bytes ==================== */

/* 说明：解锁 Option Byte 控制。 */
void flash_opt_unlock(void);

/* 说明：锁定 Option Byte 控制。 */
void flash_opt_lock(void);

/* 说明：启动 Option Byte 编程。 */
flash_status_t flash_opt_start(void);

#endif /* DRV_USING_FLASH */
#endif /* __DRV_FLASH_H__ */
