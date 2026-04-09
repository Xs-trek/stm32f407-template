#include "board.h"
#include <rtthread.h>

/*
 * 说明：
 * 本文件为 F407_RT 模板工程的应用入口。
 *
 * 当前版本语义：
 * 1. 预配置一个 app 线程作为用户业务入口。
 * 2. 线程栈大小、优先级、时间片通过下方宏配置。
 * 3. 用户在 app_thread_entry() 中填入业务逻辑。
 * 4. 如需多线程，在此处参照 app 线程的方式增加。
 *
 * 启动链说明：
 * Reset_Handler → __main → RT-Thread 劫持
 * → rtthread_startup() → rt_hw_board_init()（board.c）
 * → 调度器启动 → main 线程 → main()
 */

/* ==================== 线程配置 ==================== */

#define APP_THREAD_STACK_SIZE    512U
#define APP_THREAD_PRIORITY      20U
#define APP_THREAD_TIMESLICE     10U

static struct rt_thread app_thread;
static rt_uint8_t app_thread_stack[APP_THREAD_STACK_SIZE];

/* ==================== 线程入口 ==================== */

static void app_thread_entry(void *parameter)
{
    (void)parameter;

    /*
     * 用户业务逻辑入口。
     * 在此处初始化外设、启动任务循环。
     */
    for (;;)
    {
        rt_thread_mdelay(1000);
    }
}

/* ==================== 主函数 ==================== */

int main(void)
{
    rt_thread_init(&app_thread,
                   "app",
                   app_thread_entry,
                   RT_NULL,
                   app_thread_stack,
                   sizeof(app_thread_stack),
                   APP_THREAD_PRIORITY,
                   APP_THREAD_TIMESLICE);

    rt_thread_startup(&app_thread);

    return 0;
}
