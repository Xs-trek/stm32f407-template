#include "drv_can.h"

#ifdef DRV_USING_CAN

/* ==================== 时钟与中断号 ==================== */

/* 使能 CAN 外设时钟 */
void can_clk_enable(CAN_TypeDef *canx)
{
    if (canx == NULL)
    {
        return;
    }

    if (canx == CAN1)
    {
        RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;
    }
    else if (canx == CAN2)
    {
        RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;   /* CAN2 依赖 CAN1 时钟 */
        RCC->APB1ENR |= RCC_APB1ENR_CAN2EN;
    }
    else
    {
        return;
    }

    (void)RCC->APB1ENR;    /* 读回以确保写入生效 */
}

/* 获取发送完成中断号 */
IRQn_Type can_get_tx_irqn(CAN_TypeDef *canx)
{
    return (canx == CAN1) ? CAN1_TX_IRQn : CAN2_TX_IRQn;
}

/* 获取 FIFO0 接收中断号 */
IRQn_Type can_get_rx0_irqn(CAN_TypeDef *canx)
{
    return (canx == CAN1) ? CAN1_RX0_IRQn : CAN2_RX0_IRQn;
}

/* 获取 FIFO1 接收中断号 */
IRQn_Type can_get_rx1_irqn(CAN_TypeDef *canx)
{
    return (canx == CAN1) ? CAN1_RX1_IRQn : CAN2_RX1_IRQn;
}

/* 获取错误与状态变化中断号 */
IRQn_Type can_get_sce_irqn(CAN_TypeDef *canx)
{
    return (canx == CAN1) ? CAN1_SCE_IRQn : CAN2_SCE_IRQn;
}

/* ==================== 初始化 ==================== */

/* 按配置结构体初始化 CAN 控制器 */
void can_init(const can_config_t *cfg)
{
    CAN_TypeDef *canx;

    if (cfg == NULL || cfg->instance == NULL)
    {
        return;
    }

    canx = cfg->instance;
    can_clk_enable(canx);

    /* 进入初始化模式 */
    canx->MCR |= CAN_MCR_INRQ;
    while ((canx->MSR & CAN_MSR_INAK) == 0U)
    {
        /* 等待进入初始化模式 */
    }

    canx->MCR = CAN_MCR_INRQ;

    if (cfg->auto_bus_off)
    {
        canx->MCR |= CAN_MCR_ABOM;
    }
    if (cfg->auto_wake_up)
    {
        canx->MCR |= CAN_MCR_AWUM;
    }
    if (cfg->auto_retransmit == 0U)
    {
        canx->MCR |= CAN_MCR_NART;
    }

    canx->BTR = ((uint32_t)(cfg->mode & 0x03U) << 30U) |
                ((uint32_t)((cfg->sjw  - 1U) & 0x03U)  << 24U) |
                ((uint32_t)((cfg->bs1  - 1U) & 0x0FU)  << 16U) |
                ((uint32_t)((cfg->bs2  - 1U) & 0x07U)  << 20U) |
                ((uint32_t)((cfg->prescaler - 1U) & 0x03FFU));

    /* 退出初始化模式 */
    canx->MCR &= ~CAN_MCR_INRQ;
    while ((canx->MSR & CAN_MSR_INAK) != 0U)
    {
        /* 等待退出初始化模式 */
    }
}

/* 配置指定过滤器组 */
void can_filter_init(const can_filter_config_t *cfg)
{
    uint32_t bit;

    if (cfg == NULL || cfg->bank > 27U)
    {
        return;
    }

    bit = (1U << cfg->bank);

    /* 进入过滤器初始化模式 */
    CAN1->FMR |= CAN_FMR_FINIT;

    /* 先禁用该过滤器组 */
    CAN1->FA1R &= ~bit;

    /* 设置位宽：32 位或 16 位 */
    if (cfg->scale_32bit)
    {
        CAN1->FS1R |= bit;
    }
    else
    {
        CAN1->FS1R &= ~bit;
    }

    /* 设置模式：掩码模式或标识符列表模式 */
    if (cfg->mode_mask)
    {
        CAN1->FM1R &= ~bit;    /* 掩码模式 */
    }
    else
    {
        CAN1->FM1R |= bit;     /* 标识符列表模式 */
    }

    /* 关联 FIFO */
    if (cfg->fifo == 0U)
    {
        CAN1->FFA1R &= ~bit;
    }
    else
    {
        CAN1->FFA1R |= bit;
    }

    /* 写入 ID 与掩码寄存器 */
    CAN1->sFilterRegister[cfg->bank].FR1 =
        ((cfg->id_high & 0xFFFFU) << 16U) | (cfg->id_low & 0xFFFFU);
    CAN1->sFilterRegister[cfg->bank].FR2 =
        ((cfg->mask_high & 0xFFFFU) << 16U) | (cfg->mask_low & 0xFFFFU);

    /* 按需使能过滤器组 */
    if (cfg->enable)
    {
        CAN1->FA1R |= bit;
    }

    /* 退出过滤器初始化模式 */
    CAN1->FMR &= ~CAN_FMR_FINIT;
}

/* ==================== 发送与接收 ==================== */

/* 将帧写入空闲邮箱并请求发送 */
uint8_t can_send(CAN_TypeDef *canx, const can_frame_t *frame)
{
    uint8_t  mailbox;
    uint32_t tir;
    uint32_t tdlr;
    uint32_t tdhr;

    if (canx == NULL || frame == NULL)
    {
        return 0xFFU;
    }

    /* 查找空闲发送邮箱 */
    if (canx->TSR & CAN_TSR_TME0)
    {
        mailbox = 0U;
    }
    else if (canx->TSR & CAN_TSR_TME1)
    {
        mailbox = 1U;
    }
    else if (canx->TSR & CAN_TSR_TME2)
    {
        mailbox = 2U;
    }
    else
    {
        return 0xFFU;   /* 无空闲邮箱 */
    }

    /* 组装仲裁段寄存器 */
    tir = 0U;

    if (frame->ide == 0U)
    {
        tir |= ((frame->std_id & 0x7FFU) << 21U);
    }
    else
    {
        tir |= ((frame->ext_id & 0x1FFFFFFFU) << 3U);
        tir |= CAN_TI0R_IDE;
    }

    if (frame->rtr)
    {
        tir |= CAN_TI0R_RTR;
    }

    /* 组装数据段寄存器 */
    tdlr = ((uint32_t)frame->data[0])       |
           ((uint32_t)frame->data[1] << 8U)  |
           ((uint32_t)frame->data[2] << 16U) |
           ((uint32_t)frame->data[3] << 24U);
    tdhr = ((uint32_t)frame->data[4])       |
           ((uint32_t)frame->data[5] << 8U)  |
           ((uint32_t)frame->data[6] << 16U) |
           ((uint32_t)frame->data[7] << 24U);

    /* 写入邮箱寄存器并请求发送 */
    canx->sTxMailBox[mailbox].TDTR = (frame->dlc & 0x0FU);
    canx->sTxMailBox[mailbox].TDLR = tdlr;
    canx->sTxMailBox[mailbox].TDHR = tdhr;
    canx->sTxMailBox[mailbox].TIR  = tir | CAN_TI0R_TXRQ;

    return mailbox;
}

/* 从指定 FIFO 读取一帧并释放邮箱 */
void can_recv(CAN_TypeDef *canx, uint8_t fifo, can_frame_t *frame)
{
    CAN_FIFOMailBox_TypeDef *mb;
    uint32_t rir;
    uint32_t rdtr;
    uint32_t rdlr;
    uint32_t rdhr;

    if (canx == NULL || frame == NULL || fifo > 1U)
    {
        return;
    }

    mb   = &canx->sFIFOMailBox[fifo];
    rir  = mb->RIR;
    rdtr = mb->RDTR;
    rdlr = mb->RDLR;
    rdhr = mb->RDHR;

    /* 解析帧类型 */
    frame->ide = (uint8_t)((rir & CAN_RI0R_IDE) ? 1U : 0U);
    frame->rtr = (uint8_t)((rir & CAN_RI0R_RTR) ? 1U : 0U);

    if (frame->ide == 0U)
    {
        frame->std_id = (rir >> 21U) & 0x7FFU;
        frame->ext_id = 0U;
    }
    else
    {
        frame->ext_id = (rir >> 3U) & 0x1FFFFFFFU;
        frame->std_id = 0U;
    }

    /* 提取数据长度与数据域 */
    frame->dlc = (uint8_t)(rdtr & 0x0FU);
    frame->data[0] = (uint8_t)(rdlr);
    frame->data[1] = (uint8_t)(rdlr >> 8U);
    frame->data[2] = (uint8_t)(rdlr >> 16U);
    frame->data[3] = (uint8_t)(rdlr >> 24U);
    frame->data[4] = (uint8_t)(rdhr);
    frame->data[5] = (uint8_t)(rdhr >> 8U);
    frame->data[6] = (uint8_t)(rdhr >> 16U);
    frame->data[7] = (uint8_t)(rdhr >> 24U);

    /* 释放 FIFO 邮箱 */
    if (fifo == 0U)
    {
        canx->RF0R |= CAN_RF0R_RFOM0;
    }
    else
    {
        canx->RF1R |= CAN_RF1R_RFOM1;
    }
}

/* 中止指定邮箱的发送 */
void can_abort_tx(CAN_TypeDef *canx, uint8_t mailbox)
{
    if (canx == NULL || mailbox > 2U)
    {
        return;
    }
    canx->TSR |= (CAN_TSR_ABRQ0 << (mailbox * 8U));
}

/* ==================== 中断使能与禁止 ==================== */

void can_enable_fifo0_irq(CAN_TypeDef *canx)
{
    if (canx == NULL)
    {
        return;
    }
    canx->IER |= CAN_IER_FMPIE0;
}

void can_disable_fifo0_irq(CAN_TypeDef *canx)
{
    if (canx == NULL)
    {
        return;
    }
    canx->IER &= ~CAN_IER_FMPIE0;
}

void can_enable_fifo1_irq(CAN_TypeDef *canx)
{
    if (canx == NULL)
    {
        return;
    }
    canx->IER |= CAN_IER_FMPIE1;
}

void can_disable_fifo1_irq(CAN_TypeDef *canx)
{
    if (canx == NULL)
    {
        return;
    }
    canx->IER &= ~CAN_IER_FMPIE1;
}

void can_enable_tx_irq(CAN_TypeDef *canx)
{
    if (canx == NULL)
    {
        return;
    }
    canx->IER |= CAN_IER_TMEIE;
}

void can_disable_tx_irq(CAN_TypeDef *canx)
{
    if (canx == NULL)
    {
        return;
    }
    canx->IER &= ~CAN_IER_TMEIE;
}

void can_enable_err_irq(CAN_TypeDef *canx)
{
    if (canx == NULL)
    {
        return;
    }
    canx->IER |= CAN_IER_ERRIE;
}

void can_disable_err_irq(CAN_TypeDef *canx)
{
    if (canx == NULL)
    {
        return;
    }
    canx->IER &= ~CAN_IER_ERRIE;
}

void can_enable_busoff_irq(CAN_TypeDef *canx)
{
    if (canx == NULL)
    {
        return;
    }
    canx->IER |= CAN_IER_BOFIE;
}

void can_disable_busoff_irq(CAN_TypeDef *canx)
{
    if (canx == NULL)
    {
        return;
    }
    canx->IER &= ~CAN_IER_BOFIE;
}

void can_enable_wakeup_irq(CAN_TypeDef *canx)
{
    if (canx == NULL)
    {
        return;
    }
    canx->IER |= CAN_IER_WKUIE;
}

void can_disable_wakeup_irq(CAN_TypeDef *canx)
{
    if (canx == NULL)
    {
        return;
    }
    canx->IER &= ~CAN_IER_WKUIE;
}

/* ==================== 睡眠管理 ==================== */

/* 进入睡眠模式 */
void can_enter_sleep(CAN_TypeDef *canx)
{
    if (canx == NULL)
    {
        return;
    }
    canx->MCR |= CAN_MCR_SLEEP;
}

/* 退出睡眠模式 */
void can_exit_sleep(CAN_TypeDef *canx)
{
    if (canx == NULL)
    {
        return;
    }
    canx->MCR &= ~CAN_MCR_SLEEP;
}

/* ==================== 状态查询 ==================== */

uint32_t can_get_esr(CAN_TypeDef *canx)
{
    if (canx == NULL)
    {
        return 0U;
    }
    return canx->ESR;
}

uint32_t can_get_tsr(CAN_TypeDef *canx)
{
    if (canx == NULL)
    {
        return 0U;
    }
    return canx->TSR;
}

uint32_t can_get_rf0r(CAN_TypeDef *canx)
{
    if (canx == NULL)
    {
        return 0U;
    }
    return canx->RF0R;
}

uint32_t can_get_rf1r(CAN_TypeDef *canx)
{
    if (canx == NULL)
    {
        return 0U;
    }
    return canx->RF1R;
}

#endif /* DRV_USING_CAN */
