#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include <stdint.h>

typedef enum {
    TEST_PASS    = 0,
    TEST_FAIL    = 1,
    TEST_SKIP    = 2,
    TEST_TIMEOUT = 3,
} test_result_t;

typedef struct {
    const char   *name;
    test_result_t result;
} test_record_t;

#define TEST_MAX_ITEMS   32U

void runner_init(void);
void runner_record(const char *name, test_result_t result);
void runner_print_summary(void);
typedef void (*runner_feed_hook_t)(void);
void runner_set_feed_hook(runner_feed_hook_t hook);

/*
 * TEST_LOG — 不引入局部变量，直接用三目运算符内联，消除 ARMCC V5 warning
 */
#define TEST_LOG(res, mod, fmt, ...)                                        \
    rt_kprintf("%s %-14s : " fmt "\r\n",                                    \
               ((res) == TEST_PASS)    ? "[PASS]" :                         \
               ((res) == TEST_FAIL)    ? "[FAIL]" :                         \
               ((res) == TEST_TIMEOUT) ? "[TOUT]" : "[SKIP]",              \
               (mod), ##__VA_ARGS__)

#endif /* TEST_RUNNER_H */
