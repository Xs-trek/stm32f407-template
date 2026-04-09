#include "drv_usb_otg_fs.h"

#ifdef DRV_USING_USB_OTG_FS

/* ==================== 时钟使能 ==================== */

void usb_otg_fs_clk_enable(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN;
    (void)RCC->AHB2ENR;
}

/* ==================== 内核初始化 ==================== */

void usb_otg_fs_init(void)
{
    usb_otg_fs_clk_enable();

    USB_OTG_FS->GAHBCFG = 0;
    USB_OTG_FS->GUSBCFG = 0;
    USB_OTG_FS->GRSTCTL |= USB_OTG_GRSTCTL_CSRST;
    while (USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_CSRST)
    {
    }
}

/* ==================== 全局中断 ==================== */

void usb_otg_fs_enable_global_irq(void)
{
    USB_OTG_FS->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
}

void usb_otg_fs_disable_global_irq(void)
{
    USB_OTG_FS->GAHBCFG &= ~USB_OTG_GAHBCFG_GINT;
}

/* ==================== FIFO 配置 ==================== */

void usb_otg_fs_set_rx_fifo(uint16_t words)
{
    USB_OTG_FS->GRXFSIZ = words;
}

void usb_otg_fs_set_tx_fifo(uint8_t fifo_index, uint16_t start_addr, uint16_t depth)
{
    if (fifo_index == 0U)
    {
        USB_OTG_FS->DIEPTXF0_HNPTXFSIZ = ((uint32_t)depth << 16U) | start_addr;
    }
    else if (fifo_index <= 3U)
    {
        USB_OTG_FS->DIEPTXF[fifo_index - 1U] = ((uint32_t)depth << 16U) | start_addr;
    }
}

/* ==================== 模式切换 ==================== */

void usb_otg_fs_device_init(void)
{
    USB_OTG_FS->GUSBCFG &= ~USB_OTG_GUSBCFG_FHMOD;
    USB_OTG_FS->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD;
}

void usb_otg_fs_host_init(void)
{
    USB_OTG_FS->GUSBCFG &= ~USB_OTG_GUSBCFG_FDMOD;
    USB_OTG_FS->GUSBCFG |= USB_OTG_GUSBCFG_FHMOD;
}

/* ==================== 端点 / 通道 ==================== */

void usb_otg_fs_ep_set_mps(uint8_t ep_num, uint16_t mps)
{
    if (ep_num > 3U) { return; }

    if (ep_num == 0U)
    {
        USB_OTG_FS_DEVICE->INEP[ep_num].DIEPCTL &= ~USB_OTG_DIEPCTL_MPSIZ;
        USB_OTG_FS_DEVICE->INEP[ep_num].DIEPCTL |= (mps & 0x03U);
    }
    else
    {
        USB_OTG_FS_DEVICE->INEP[ep_num].DIEPCTL &= ~USB_OTG_DIEPCTL_MPSIZ;
        USB_OTG_FS_DEVICE->INEP[ep_num].DIEPCTL |= (mps & 0x7FFU);
    }
}

void usb_otg_fs_hc_set_mps(uint8_t ch_num, uint16_t mps)
{
    if (ch_num > 7U) { return; }

    USB_OTG_FS_HOST->HC[ch_num].HCCHAR &= ~USB_OTG_HCCHAR_MPSIZ;
    USB_OTG_FS_HOST->HC[ch_num].HCCHAR |= (mps & 0x7FFU);
}

void usb_otg_fs_ep_in_enable(uint8_t ep_num)
{
    if (ep_num > 3U) { return; }

    USB_OTG_FS_DEVICE->INEP[ep_num].DIEPCTL |= USB_OTG_DIEPCTL_USBAEP | USB_OTG_DIEPCTL_EPENA;
}

void usb_otg_fs_ep_out_enable(uint8_t ep_num)
{
    if (ep_num > 3U) { return; }

    USB_OTG_FS_DEVICE->OUTEP[ep_num].DOEPCTL |= USB_OTG_DOEPCTL_USBAEP | USB_OTG_DOEPCTL_EPENA;
}

#endif /* DRV_USING_USB_OTG_FS */
