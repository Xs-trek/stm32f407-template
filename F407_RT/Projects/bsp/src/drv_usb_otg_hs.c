#include "drv_usb_otg_hs.h"

#ifdef DRV_USING_USB_OTG_HS

/* ==================== 时钟使能 ==================== */

void usb_otg_hs_clk_enable(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_OTGHSEN | RCC_AHB1ENR_OTGHSULPIEN;
    (void)RCC->AHB1ENR;
}

/* ==================== 内核初始化 ==================== */

void usb_otg_hs_init(void)
{
    usb_otg_hs_clk_enable();

    USB_OTG_HS->GAHBCFG = 0;
    USB_OTG_HS->GUSBCFG = 0;
    USB_OTG_HS->GRSTCTL |= USB_OTG_GRSTCTL_CSRST;
    while (USB_OTG_HS->GRSTCTL & USB_OTG_GRSTCTL_CSRST)
    {
    }
}

/* ==================== 全局中断 ==================== */

void usb_otg_hs_enable_global_irq(void)
{
    USB_OTG_HS->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
}

void usb_otg_hs_disable_global_irq(void)
{
    USB_OTG_HS->GAHBCFG &= ~USB_OTG_GAHBCFG_GINT;
}

/* ==================== FIFO 配置 ==================== */

void usb_otg_hs_set_rx_fifo(uint16_t words)
{
    USB_OTG_HS->GRXFSIZ = words;
}

void usb_otg_hs_set_tx_fifo(uint8_t fifo_index, uint16_t start_addr, uint16_t depth)
{
    if (fifo_index == 0U)
    {
        USB_OTG_HS->DIEPTXF0_HNPTXFSIZ = ((uint32_t)depth << 16U) | start_addr;
    }
    else if (fifo_index <= 5U)
    {
        USB_OTG_HS->DIEPTXF[fifo_index - 1U] = ((uint32_t)depth << 16U) | start_addr;
    }
}

/* ==================== 模式切换 ==================== */

void usb_otg_hs_device_init(void)
{
    USB_OTG_HS->GUSBCFG &= ~USB_OTG_GUSBCFG_FHMOD;
    USB_OTG_HS->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD;
}

void usb_otg_hs_host_init(void)
{
    USB_OTG_HS->GUSBCFG &= ~USB_OTG_GUSBCFG_FDMOD;
    USB_OTG_HS->GUSBCFG |= USB_OTG_GUSBCFG_FHMOD;
}

#endif /* DRV_USING_USB_OTG_HS */
