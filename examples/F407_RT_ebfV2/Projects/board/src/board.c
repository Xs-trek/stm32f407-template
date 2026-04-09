#include "board.h"
#include "f407_rt_modules.h"
#include <rtthread.h>
#include <rthw.h>

/*
 * 说明：
 * 本文件为 F407_RT 模板工程的 board 层实现。
 *
 * 当前版本语义：
 * 1. 只做系统级初始化，不含板级资源绑定。
 * 2. 时钟配置参数由 f407_rt_config.h 中的宏决定。
 * 3. 用户如需扩展板级初始化，在 rt_hw_board_init() 末尾添加，
 *    或在应用层线程入口中完成外设初始化。
 */

/* ==================== 时钟配置 ==================== */

/*
 * 说明：
 * 配置系统时钟至目标频率。
 *
 * 当前版本语义：
 * 1. 目标 SYSCLK = 168MHz，APB1 = 42MHz，APB2 = 84MHz。
 * 2. 除 25MHz 外，所有 HSE 配置的 VCO 输入均为 2MHz（HSE / PLLM = 2MHz）。
 * 3. VCO 输出 = 336MHz，PLLP = /2，PLLQ = /7（USB = 48MHz）。
 * 4. Flash Latency = 5WS，适用于 168MHz / 3.3V 供电。
 *
 * 限制：
 * 当前仅支持以下 HSE 频率（MHz）：8 / 12 / 16 / 25。
 * 如需其它 HSE 或目标时钟，在下方添加对应 #elif 分支。
 */
static void SystemClock_Config(void)
{
    /* 步骤 1：启动 HSE 并等待就绪 */
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)) { }

    /* 步骤 2：使能 PWR 时钟，配置电压调节器为 Scale 1（支持 168MHz） */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;

    /* 步骤 3：配置 Flash 访问延迟（168MHz 需要 5WS） */
    FLASH->ACR = FLASH_ACR_LATENCY_5WS
               | FLASH_ACR_PRFTEN
               | FLASH_ACR_ICEN
               | FLASH_ACR_DCEN;

    /* 步骤 4：配置总线分频
     * AHB  = SYSCLK / 1 = 168MHz
     * APB1 = SYSCLK / 4 = 42MHz（最大 42MHz）
     * APB2 = SYSCLK / 2 = 84MHz（最大 84MHz）
     */
    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1
               | RCC_CFGR_PPRE1_DIV4
               | RCC_CFGR_PPRE2_DIV2;

    /* 步骤 5：关闭 PLL 后再配置参数 */
    RCC->CR &= ~RCC_CR_PLLON;
    while (RCC->CR & RCC_CR_PLLRDY) { }

    /*
     * 步骤 6：配置 PLL 参数
     *
     * 公式：
     * VCO_in  = HSE / PLLM         → 目标 2MHz（25MHz HSE 除外）
     * VCO_out = VCO_in × PLLN      → 336MHz
     * SYSCLK  = VCO_out / PLLP     → 168MHz（PLLP=2，编码为 00）
     * USB_CLK = VCO_out / PLLQ     → 48MHz
     *
     * 注意：PLLP 字段编码为 (PLLP/2 - 1)：
     * PLLP=2 → 写 0（00），PLLP=4 → 写 1（01）
     */
#if   (F407_RT_HSE_MHZ == 8U)
    /* PLLM=4, PLLN=168, PLLP=2(00), PLLSRC=HSE, PLLQ=7 */
    RCC->PLLCFGR = (4U   << RCC_PLLCFGR_PLLM_Pos)
                 | (168U << RCC_PLLCFGR_PLLN_Pos)
                 | (0U   << RCC_PLLCFGR_PLLP_Pos)
                 | RCC_PLLCFGR_PLLSRC_HSE
                 | (7U   << RCC_PLLCFGR_PLLQ_Pos);

#elif (F407_RT_HSE_MHZ == 12U)
    /* PLLM=6, PLLN=168, PLLP=2(00), PLLSRC=HSE, PLLQ=7 */
    RCC->PLLCFGR = (6U   << RCC_PLLCFGR_PLLM_Pos)
                 | (168U << RCC_PLLCFGR_PLLN_Pos)
                 | (0U   << RCC_PLLCFGR_PLLP_Pos)
                 | RCC_PLLCFGR_PLLSRC_HSE
                 | (7U   << RCC_PLLCFGR_PLLQ_Pos);

#elif (F407_RT_HSE_MHZ == 16U)
    /* PLLM=8, PLLN=168, PLLP=2(00), PLLSRC=HSE, PLLQ=7 */
    RCC->PLLCFGR = (8U   << RCC_PLLCFGR_PLLM_Pos)
                 | (168U << RCC_PLLCFGR_PLLN_Pos)
                 | (0U   << RCC_PLLCFGR_PLLP_Pos)
                 | RCC_PLLCFGR_PLLSRC_HSE
                 | (7U   << RCC_PLLCFGR_PLLQ_Pos);

#elif (F407_RT_HSE_MHZ == 25U)
    /* PLLM=25, PLLN=336, PLLP=2(00), PLLSRC=HSE, PLLQ=7 */
    RCC->PLLCFGR = (25U  << RCC_PLLCFGR_PLLM_Pos)
                 | (336U << RCC_PLLCFGR_PLLN_Pos)
                 | (0U   << RCC_PLLCFGR_PLLP_Pos)
                 | RCC_PLLCFGR_PLLSRC_HSE
                 | (7U   << RCC_PLLCFGR_PLLQ_Pos);

#else
    #error "F407_RT_HSE_MHZ is not supported. Supported values: 8 / 12 / 16 / 25."
#endif

    /* 步骤 7：启动 PLL 并等待就绪 */
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)) { }

    /* 步骤 8：切换系统时钟源为 PLL */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) { }

    /* 步骤 9：更新 CMSIS 系统时钟变量 */
    SystemCoreClock = 168000000U;
}

/* ==================== SysTick 配置 ==================== */

/*
 * 说明：
 * 配置 SysTick 为 RT-Thread 系统节拍源。
 * 节拍频率由 RT_TICK_PER_SECOND 决定（在 rtconfig.h 中配置）。
 */
static void SysTick_Init(void)
{
    SysTick->LOAD = (SystemCoreClock / RT_TICK_PER_SECOND) - 1U;
    SysTick->VAL  = 0U;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk
                  | SysTick_CTRL_TICKINT_Msk
                  | SysTick_CTRL_ENABLE_Msk;
}

/* ==================== SysTick 中断处理 ==================== */

void SysTick_Handler(void)
{
    rt_interrupt_enter();
    rt_tick_increase();
    rt_interrupt_leave();
}

/* ==================== Console（USART1） ==================== */

static void console_init(void)
{
    /* GPIO：PA9 TX，PA10 RX，AF7 */
    gpio_clk_enable(CONSOLE_TX_PORT);
    gpio_init_af_pp(CONSOLE_TX_PORT, CONSOLE_TX_PIN, CONSOLE_AF);
    gpio_init_af_pp(CONSOLE_RX_PORT, CONSOLE_RX_PIN, CONSOLE_AF);

    /* USART1 */
    usart_clk_enable(CONSOLE_USART);

    usart_config_t cfg;
    cfg.instance      = CONSOLE_USART;
    cfg.baudrate      = CONSOLE_BAUD;
    cfg.word_length   = USART_WORDLEN_8B;
    cfg.stop_bits     = USART_STOP_1;
    cfg.parity        = USART_PARITY_NONE;
    cfg.tx_enable     = 1U;
    cfg.rx_enable     = 1U;
    cfg.oversampling8 = 0U;

    usart_init(&cfg);
    usart_enable(CONSOLE_USART);
}

/*
 * RT-Thread console 输出钩子
 * RT_USING_CONSOLE 启用后，rt_kprintf 最终调用此函数
 */
void rt_hw_console_output(const char *str)
{
    usart_send_string(CONSOLE_USART, str);
}

/* ==================== 板级初始化钩子 ==================== */

void rt_hw_board_init(void)
{
    /* 系统时钟初始化 */
    SystemClock_Config();

    /* SysTick 初始化 */
    SysTick_Init();

	  /* NVIC 优先级分组 Group4（RT-Thread Nano 要求）复位后默认 PRIGROUP=0，必须在此显式设置*/
	  nvic_set_priority_grouping(3U);
	
    /* 动态堆初始化（默认不启用，需在 rtconfig.h 中开启 RT_USING_HEAP） */
#ifdef RT_USING_HEAP
    static uint8_t rt_heap[F407_RT_HEAP_SIZE];
    rt_system_heap_init(rt_heap, rt_heap + sizeof(rt_heap));
#endif

    /*
     * 用户扩展区域：
     * 如需在调度器启动前完成板级外设初始化，在此处添加。
     * 注意：此时 RT-Thread 调度器尚未启动，不可使用 RTOS API。
     */
	  console_init();
}
