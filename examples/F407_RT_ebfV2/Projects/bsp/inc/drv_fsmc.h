#ifndef __DRV_FSMC_H__
#define __DRV_FSMC_H__

#include "drv_config.h"

#ifdef DRV_USING_FSMC

#include "stm32f4xx.h"

/*
 * 说明：
 * 本头文件提供 STM32F407 FSMC 驱动的公共接口。
 *
 * 当前版本定义：
 * 1. 当前版本仅覆盖 FSMC NOR/SRAM Bank 配置。
 * 2. 当前版本覆盖：
 *    - FSMC 外设时钟使能
 *    - NOR/SRAM Bank 初始化（读写时序、数据宽度、地址数据复用等）
 *
 * 限制：
 * 1. 仅覆盖 NOR/SRAM 模式。
 * 2. 不含 NAND/PCCARD 支持。
 */

/* ==================== 类型定义 ==================== */

/* 说明：FSMC NOR/SRAM Bank 配置结构体。 */
typedef struct
{
    uint8_t  bank;                /* NOR/SRAM Bank 编号 0..3 */
    uint8_t  data_width;          /* 数据宽度：8、16 或 32 */
    uint8_t  address_data_mux;    /* 1 = 地址/数据复用 */
    uint8_t  burst_enable;        /* 突发使能 */
    uint8_t  write_enable;        /* 写使能 */
    uint8_t  extended_mode;       /* 1 = 使用独立写时序 */
    uint8_t  wait_enable;         /* 等待使能 */
    uint8_t  write_timing_addset; /* 写时序地址建立时间 */
    uint8_t  write_timing_datast; /* 写时序数据保持时间 */
    uint8_t  read_timing_addset;  /* 读时序地址建立时间 */
    uint8_t  read_timing_datast;  /* 读时序数据保持时间 */
} fsmc_norsram_config_t;

/* ==================== 时钟使能 ==================== */

/* 说明：使能 FSMC 外设时钟。 */
void fsmc_clk_enable(void);

/* ==================== NOR/SRAM 初始化 ==================== */

/* 说明：初始化一个 NOR/SRAM Bank。 */
void fsmc_norsram_init(const fsmc_norsram_config_t *cfg);

#endif /* DRV_USING_FSMC */
#endif /* __DRV_FSMC_H__ */
