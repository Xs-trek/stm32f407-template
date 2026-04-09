/**
 * @file    system_stm32f4xx.c
 * @brief   CMSIS system file for STM32F407
 *
 * Target:  STM32F407 (RM0090)
 *
 * SystemCoreClock is set by board.c / SystemClock_Config().
 * SystemInit is intentionally empty; clock initialization is
 * handled by RT-Thread board-level init (rt_hw_board_init).
 */

#include "stm32f4xx.h"

/**
 * @brief  CMSIS required global variable.
 *         Initial value is HSI frequency (16 MHz).
 *         Actual value is updated by SystemClock_Config() in board.c.
 */
uint32_t SystemCoreClock = 16000000U;

/**
 * @brief  SystemInit is called by the startup file before __main.
 *         In this project the startup file does not call SystemInit,
 *         so this function is provided as a weak empty stub to satisfy
 *         any library that may reference it.
 */
__weak void SystemInit(void)
{
}

/**
 * @brief  Updates SystemCoreClock from current register state.
 *         Not used in this project. Provided as a weak empty stub.
 */
__weak void SystemCoreClockUpdate(void)
{
}
