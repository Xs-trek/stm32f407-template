#include "drv_flash.h"

#ifdef DRV_USING_FLASH

/* ==================== 内部辅助函数 ==================== */

static flash_status_t flash_get_status(void)
{
    if (FLASH->SR & FLASH_SR_BSY)    { return FLASH_STATUS_BUSY; }
    if (FLASH->SR & FLASH_SR_PGSERR) { return FLASH_STATUS_ERROR_PG; }
    if (FLASH->SR & FLASH_SR_PGPERR) { return FLASH_STATUS_ERROR_PGP; }
    if (FLASH->SR & FLASH_SR_PGAERR) { return FLASH_STATUS_ERROR_PGA; }
    if (FLASH->SR & FLASH_SR_WRPERR) { return FLASH_STATUS_ERROR_WRP; }
    return FLASH_STATUS_OK;
}

static void flash_clear_flags(void)
{
    FLASH->SR = FLASH_SR_PGSERR |
                FLASH_SR_PGPERR |
                FLASH_SR_PGAERR |
                FLASH_SR_WRPERR |
                FLASH_SR_EOP;
}

/* ==================== 访问控制（ACR） ==================== */

void flash_set_latency(uint32_t latency)
{
    uint32_t reg;

    reg = FLASH->ACR;
    reg &= ~FLASH_ACR_LATENCY;
    reg |= (latency & 0x0FU);
    FLASH->ACR = reg;
}

void flash_prefetch_enable(void)
{
    FLASH->ACR |= FLASH_ACR_PRFTEN;
}

void flash_prefetch_disable(void)
{
    FLASH->ACR &= ~FLASH_ACR_PRFTEN;
}

void flash_icache_enable(void)
{
    FLASH->ACR |= FLASH_ACR_ICEN;
}

void flash_icache_disable(void)
{
    FLASH->ACR &= ~FLASH_ACR_ICEN;
}

void flash_dcache_enable(void)
{
    FLASH->ACR |= FLASH_ACR_DCEN;
}

void flash_dcache_disable(void)
{
    FLASH->ACR &= ~FLASH_ACR_DCEN;
}

/* ==================== 解锁 / 锁定 ==================== */

void flash_unlock(void)
{
    if (FLASH->CR & FLASH_CR_LOCK)
    {
        FLASH->KEYR = 0x45670123U;
        FLASH->KEYR = 0xCDEF89ABU;
    }
}

void flash_lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}

/* ==================== 状态 / 等待 ==================== */

flash_status_t flash_wait_done(void)
{
    flash_status_t status;

    do
    {
        status = flash_get_status();
    } while (status == FLASH_STATUS_BUSY);

    return status;
}

/* ==================== 擦除 ==================== */

flash_status_t flash_erase_sector(uint8_t sector)
{
    flash_status_t status;

    if (sector > 11U) { return FLASH_STATUS_ERROR_PG; }

    status = flash_wait_done();
    if (status != FLASH_STATUS_OK) { return status; }

    flash_clear_flags();

    FLASH->CR &= ~(FLASH_CR_SNB | FLASH_CR_PSIZE);
    FLASH->CR |= FLASH_CR_SER;
    FLASH->CR |= ((uint32_t)(sector & 0x0FU) << 3U);
    FLASH->CR |= (2U << 8U);  /* PSIZE = x32 */
    FLASH->CR |= FLASH_CR_STRT;

    status = flash_wait_done();

    FLASH->CR &= ~FLASH_CR_SER;
    FLASH->CR &= ~FLASH_CR_SNB;

    return status;
}

/* ==================== 编程 ==================== */

flash_status_t flash_program_word(uint32_t addr, uint32_t data)
{
    flash_status_t status;

    status = flash_wait_done();
    if (status != FLASH_STATUS_OK) { return status; }

    flash_clear_flags();

    FLASH->CR &= ~(FLASH_CR_PG | FLASH_CR_PSIZE);
    FLASH->CR |= FLASH_CR_PG;
    FLASH->CR |= (2U << 8U);  /* PSIZE = x32 */

    *(__IO uint32_t *)addr = data;

    status = flash_wait_done();

    FLASH->CR &= ~FLASH_CR_PG;

    return status;
}

flash_status_t flash_program_halfword(uint32_t addr, uint16_t data)
{
    flash_status_t status;

    status = flash_wait_done();
    if (status != FLASH_STATUS_OK) { return status; }

    flash_clear_flags();

    FLASH->CR &= ~(FLASH_CR_PG | FLASH_CR_PSIZE);
    FLASH->CR |= FLASH_CR_PG;
    FLASH->CR |= (1U << 8U);  /* PSIZE = x16 */

    *(__IO uint16_t *)addr = data;

    status = flash_wait_done();

    FLASH->CR &= ~FLASH_CR_PG;

    return status;
}

flash_status_t flash_program_byte(uint32_t addr, uint8_t data)
{
    flash_status_t status;

    status = flash_wait_done();
    if (status != FLASH_STATUS_OK) { return status; }

    flash_clear_flags();

    FLASH->CR &= ~(FLASH_CR_PG | FLASH_CR_PSIZE);
    FLASH->CR |= FLASH_CR_PG;
    /* PSIZE = x8 (0)，已在上方清除 */

    *(__IO uint8_t *)addr = data;

    status = flash_wait_done();

    FLASH->CR &= ~FLASH_CR_PG;

    return status;
}

/* ==================== 扇区地址查询 ==================== */

static const uint32_t flash_sector_addr[12] =
{
    0x08000000U, 0x08004000U, 0x08008000U, 0x0800C000U,  /* 16 KB x4  */
    0x08010000U,                                          /* 64 KB x1  */
    0x08020000U, 0x08040000U, 0x08060000U,                /* 128 KB x3 */
    0x08080000U, 0x080A0000U, 0x080C0000U, 0x080E0000U   /* 128 KB x4 */
};

uint32_t flash_get_sector_addr(uint8_t sector)
{
    if (sector > 11U) { return 0U; }
    return flash_sector_addr[sector];
}

/* ==================== Option Bytes ==================== */

void flash_opt_unlock(void)
{
    if (FLASH->OPTCR & FLASH_OPTCR_OPTLOCK)
    {
        FLASH->OPTKEYR = 0x08192A3BU;
        FLASH->OPTKEYR = 0x4C5D6E7FU;
    }
}

void flash_opt_lock(void)
{
    FLASH->OPTCR |= FLASH_OPTCR_OPTLOCK;
}

flash_status_t flash_opt_start(void)
{
    FLASH->OPTCR |= FLASH_OPTCR_OPTSTRT;
    return flash_wait_done();
}

#endif /* DRV_USING_FLASH */
