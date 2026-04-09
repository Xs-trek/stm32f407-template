#include <rtthread.h>
#include "test_runner.h"

static test_record_t s_records[TEST_MAX_ITEMS];
static uint32_t      s_count = 0U;
static runner_feed_hook_t s_feed_hook = RT_NULL;

void runner_set_feed_hook(runner_feed_hook_t hook)
{
    s_feed_hook = hook;
}

void runner_init(void)
{
    s_count = 0U;
    rt_memset(s_records, 0, sizeof(s_records));
}

void runner_record(const char *name, test_result_t result)
{
    if (s_count >= TEST_MAX_ITEMS)
    {
        rt_kprintf("[WARN] runner: buffer full!\r\n");
        return;
    }
    s_records[s_count].name   = name;
    s_records[s_count].result = result;
    s_count++;
}

void runner_print_summary(void)
{
    uint32_t pass = 0U, fail = 0U, skip = 0U, tout = 0U;
    uint32_t i;

    rt_kprintf("\r\n");
    rt_kprintf("================================================\r\n");
    rt_kprintf("  TEST SUMMARY\r\n");
    rt_kprintf("================================================\r\n");

    for (i = 0U; i < s_count; i++)
    {
        /* 直接用三目运算符，不声明局部 tag 变量 */
        rt_kprintf("  %s %s\r\n",
                   (s_records[i].result == TEST_PASS)    ? "[PASS]" :
                   (s_records[i].result == TEST_FAIL)    ? "[FAIL]" :
                   (s_records[i].result == TEST_TIMEOUT) ? "[TOUT]" : "[SKIP]",
                   s_records[i].name);

        if      (s_records[i].result == TEST_PASS)    { pass++; }
        else if (s_records[i].result == TEST_FAIL)    { fail++; }
        else if (s_records[i].result == TEST_TIMEOUT) { tout++; }
        else                                          { skip++; }
				
				if (s_feed_hook != RT_NULL) { s_feed_hook(); }
    }

    rt_kprintf("------------------------------------------------\r\n");
    rt_kprintf("  Total=%u PASS=%u FAIL=%u SKIP=%u TIMEOUT=%u\r\n",
               (unsigned)s_count, (unsigned)pass,
               (unsigned)fail,    (unsigned)skip,
               (unsigned)tout);
    rt_kprintf("================================================\r\n");
							 
		if (s_feed_hook != RT_NULL) { s_feed_hook(); }
}
