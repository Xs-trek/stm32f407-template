#ifndef __BOARD_H__
#define __BOARD_H__

#include "stm32f4xx.h"
#include "f407_rt_config.h"

/*
 * 说明：
 * 本文件为 F407_RT 模板工程的 board 层公开接口。
 *
 * 当前版本语义：
 * 1. board 层只负责系统级初始化：时钟、SysTick、堆。
 * 2. board 层不包含任何板级资源绑定（LED/KEY/USART 等）。
 * 3. 板级资源绑定由用户在自己的工程中扩展。
 *
 * 时钟配置说明：
 * 1. 在 f407_rt_config.h 中配置以下两个参数：
 *    - F407_RT_HSE_MHZ  ：外部晶振频率，当前支持 8 / 12 / 16 / 25
 *    - F407_RT_SYSCLK_MHZ：目标系统时钟，当前版本固定支持 168
 * 2. 使用其它 HSE 频率或目标时钟时，需在 board.c 的
 *    SystemClock_Config() 中手动添加对应 PLL 参数。
 * 3. 当前所有 HSE 配置均以 168MHz SYSCLK 为目标，
 *    对应 APB1 = 42MHz，APB2 = 84MHz，USB = 48MHz。
 *
 * 堆配置说明：
 * 1. 默认不启用动态堆。
 * 2. 如需动态内存，在 rtconfig.h 中启用 RT_USING_HEAP，
 *    并在 f407_rt_config.h 中配置 F407_RT_HEAP_SIZE。
 */

/* ==================== 堆大小配置 ==================== */

/*
 * 说明：
 * RT-Thread 动态堆大小，单位字节。
 * 仅在 RT_USING_HEAP 启用时生效。
 *
 * 前提：
 * 1. F407_RT_HEAP_SIZE 不得超过可用 SRAM 减去静态分配后的剩余量。
 * 2. STM32F407 片上 SRAM 共 192KB（主 SRAM 128KB + CCM 64KB）。
 * 3. CCM 不能用于 DMA，分配堆时需注意。
 *
 * 当前版本语义：
 * 若未在 f407_rt_config.h 中定义 F407_RT_HEAP_SIZE，
 * 默认使用 4KB 作为保守值。
 */
#ifndef F407_RT_HEAP_SIZE
#define F407_RT_HEAP_SIZE    (4U * 1024U)
#endif

/* ==================== 公开接口 ==================== */

/*
 * 说明：
 * RT-Thread 板级初始化钩子，由内核在启动调度器前调用。
 * 负责完成时钟初始化、SysTick 初始化、堆初始化。
 */
void rt_hw_board_init(void);

#endif /* __BOARD_H__ */
