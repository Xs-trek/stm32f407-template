#ifndef __DRV_USB_OTG_HS_H__
#define __DRV_USB_OTG_HS_H__

#include "drv_config.h"

#ifdef DRV_USING_USB_OTG_HS

#include "stm32f4xx.h"

/*
 * 说明：
 * 本头文件提供 STM32F407 USB OTG HS 驱动的公共接口。
 *
 * 当前版本定义：
 * 1. 当前版本仅覆盖 USB OTG HS 驱动本身闭合的基础功能。
 * 2. 当前版本覆盖：
 *    - USB OTG HS 内核初始化
 *    - 全局中断使能 / 禁止
 *    - FIFO 配置（RX / TX）
 *    - 设备 / 主机模式切换
 *
 * 限制：
 * 1. 当前版本不含完整 USB 协议栈。
 * 2. 当前版本不含传输管理。
 * 3. 当前版本不含 ULPI 接口详细配置。
 */

/* ==================== 时钟使能 ==================== */

/* 说明：使能 USB OTG HS 及 ULPI 外设时钟。 */
void usb_otg_hs_clk_enable(void);

/* ==================== 内核初始化 ==================== */

/* 说明：初始化 USB OTG HS 内核（软复位）。 */
void usb_otg_hs_init(void);

/* ==================== 全局中断 ==================== */

/* 说明：使能全局中断。 */
void usb_otg_hs_enable_global_irq(void);

/* 说明：禁止全局中断。 */
void usb_otg_hs_disable_global_irq(void);

/* ==================== FIFO 配置 ==================== */

/* 说明：设置 RX FIFO 深度（单位：32 位字）。 */
void usb_otg_hs_set_rx_fifo(uint16_t words);

/* 说明：设置 TX FIFO 起始地址和深度（fifo_index: 0..5）。 */
void usb_otg_hs_set_tx_fifo(uint8_t fifo_index, uint16_t start_addr, uint16_t depth);

/* ==================== 模式切换 ==================== */

/* 说明：强制进入设备模式。 */
void usb_otg_hs_device_init(void);

/* 说明：强制进入主机模式。 */
void usb_otg_hs_host_init(void);

#endif /* DRV_USING_USB_OTG_HS */
#endif /* __DRV_USB_OTG_HS_H__ */
