#ifndef __DRV_USB_OTG_FS_H__
#define __DRV_USB_OTG_FS_H__

#include "drv_config.h"

#ifdef DRV_USING_USB_OTG_FS

#include "stm32f4xx.h"

/*
 * 说明：
 * 本头文件提供 STM32F407 USB OTG FS 驱动的公共接口。
 *
 * 当前版本定义：
 * 1. 当前版本仅覆盖 USB OTG FS 驱动本身闭合的基础功能。
 * 2. 当前版本覆盖：
 *    - USB OTG FS 内核初始化
 *    - 全局中断使能 / 禁止
 *    - FIFO 配置（RX / TX）
 *    - 设备 / 主机模式切换
 *    - 端点 / 通道 MPS 设置
 *
 * 限制：
 * 1. 当前版本不含完整 USB 协议栈。
 * 2. 当前版本不含传输管理。
 * 3. 当前版本不含 OTG 模式切换。
 */

/* ==================== 时钟使能 ==================== */

/* 说明：使能 USB OTG FS 外设时钟。 */
void usb_otg_fs_clk_enable(void);

/* ==================== 内核初始化 ==================== */

/* 说明：初始化 USB OTG FS 内核（软复位）。 */
void usb_otg_fs_init(void);

/* ==================== 全局中断 ==================== */

/* 说明：使能全局中断。 */
void usb_otg_fs_enable_global_irq(void);

/* 说明：禁止全局中断。 */
void usb_otg_fs_disable_global_irq(void);

/* ==================== FIFO 配置 ==================== */

/* 说明：设置 RX FIFO 深度（单位：32 位字）。 */
void usb_otg_fs_set_rx_fifo(uint16_t words);

/* 说明：设置 TX FIFO 起始地址和深度（fifo_index: 0..3）。 */
void usb_otg_fs_set_tx_fifo(uint8_t fifo_index, uint16_t start_addr, uint16_t depth);

/* ==================== 模式切换 ==================== */

/* 说明：强制进入设备模式。 */
void usb_otg_fs_device_init(void);

/* 说明：强制进入主机模式。 */
void usb_otg_fs_host_init(void);

/* ==================== 端点 / 通道 ==================== */

/* 说明：设置设备 IN 端点最大包大小。 */
void usb_otg_fs_ep_set_mps(uint8_t ep_num, uint16_t mps);

/* 说明：设置主机通道最大包大小。 */
void usb_otg_fs_hc_set_mps(uint8_t ch_num, uint16_t mps);

/* 说明：使能设备 IN 端点。 */
void usb_otg_fs_ep_in_enable(uint8_t ep_num);

/* 说明：使能设备 OUT 端点。 */
void usb_otg_fs_ep_out_enable(uint8_t ep_num);

#endif /* DRV_USING_USB_OTG_FS */
#endif /* __DRV_USB_OTG_FS_H__ */
