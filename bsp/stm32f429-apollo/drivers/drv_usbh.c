/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2017-10-30     ZYH            the first version
 * 2019-12-19     tyustli           port to stm32 series
 */
#include "drv_usbh.h"
#include "stdio.h"
#include "board.h"

#ifdef  DRV_USBH_USE_HS
#define  STM32_USB_HOST_CHN_NUM     12
#else
#define  STM32_USB_HOST_CHN_NUM     8
#endif

static HCD_HandleTypeDef stm32_hhcd;
static volatile rt_bool_t connect_status = RT_FALSE;
static rt_sem_t   usb_urb_sem[STM32_USB_HOST_CHN_NUM];

void OTG_FS_IRQHandler(void)
{
    rt_interrupt_enter();
    HAL_HCD_IRQHandler(&stm32_hhcd);
    rt_interrupt_leave();
}

void OTG_HS_IRQHandler(void)
{
    rt_interrupt_enter();
    HAL_HCD_IRQHandler(&stm32_hhcd);
    rt_interrupt_leave();
}

void HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd)
{
    uhcd_t hcd = (uhcd_t)hhcd->pData;
    rt_kprintf("time:%dms, connect %d\n", rt_tick_get(), connect_status);
    if (!connect_status)
    {
        connect_status = RT_TRUE;
        RT_DEBUG_LOG(RT_DEBUG_USB, ("usb connected\n"));
        rt_usbh_root_hub_connect_handler(hcd, OTG_FS_PORT);
    }
}

void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef *hhcd)
{
    uhcd_t hcd = (uhcd_t)hhcd->pData;
    rt_kprintf("time:%dms disconnect %d\n", rt_tick_get(),connect_status);
    if (connect_status)
    {
        connect_status = RT_FALSE;
        RT_DEBUG_LOG(RT_DEBUG_USB, ("usb disconnnect\n"));
        rt_usbh_root_hub_disconnect_handler(hcd, OTG_FS_PORT);
    }
}


void HAL_HCD_HC_NotifyURBChange_Callback(HCD_HandleTypeDef *hhcd, uint8_t chnum, HCD_URBStateTypeDef urb_state)
{
    /*  when use the L501 4D0103 usb urb_state is idle, L501 2B0402 no enter zhaoshimin 20211120*/
    if((urb_state != URB_NOTREADY) && (urb_state != URB_IDLE))
    {
        rt_sem_release(usb_urb_sem[chnum]);
    }
}

void HAL_HCD_PortEnabled_Callback(HCD_HandleTypeDef *hhcd)
{
    uhcd_t hcd = (uhcd_t)hhcd->pData;
    USB_OTG_GlobalTypeDef *USBx = hhcd->Instance;
    uint32_t USBx_BASE = (uint32_t)USBx;
    uint32_t hprt0 = 0;
    rt_bool_t isHS = RT_FALSE;
    /*port enable , get the device speed*/
    hprt0 = USBx_HPRT0;
    if (((hprt0 & USB_OTG_HPRT_PSPD) == (HPRT0_PRTSPD_LOW_SPEED << 17)) ||
       ((hprt0 & USB_OTG_HPRT_PSPD) == (HPRT0_PRTSPD_FULL_SPEED << 17)))
    {
        isHS = RT_FALSE;  
    }
    else 
    {
      
        isHS = RT_TRUE;
    }
    rt_usbh_root_hub_port_enable_handler(hcd, OTG_FS_PORT,isHS);

}

void HAL_HCD_PortDisabled_Callback(HCD_HandleTypeDef *hhcd)
{
    uhcd_t hcd = (uhcd_t)hhcd->pData;
    /*port disenable*/
    rt_usbh_root_hub_port_disenable_handler(hcd, OTG_FS_PORT);
}

static rt_err_t drv_reset_port(rt_uint8_t port)
{
    RT_DEBUG_LOG(RT_DEBUG_USB, ("reset port\n"));
    HAL_HCD_ResetPort(&stm32_hhcd);
    return RT_EOK;
}

static rt_err_t drv_vbus_control(rt_uint8_t port, rt_uint8_t power)
{
    RT_DEBUG_LOG(RT_DEBUG_USB, ("vbus control\n"));
    if(power == RT_TRUE)
    {
        /*open the Vbus and power up the usb device*/
#ifdef USBH_USING_CONTROLLABLE_POWER
        rt_pin_write(USBH_POWER_PIN, PIN_HIGH);
#endif    
    }
    else
    {
        /*close the Vbus and power off the usb device*/
#ifdef USBH_USING_CONTROLLABLE_POWER
        rt_pin_write(USBH_POWER_PIN, PIN_LOW);
#endif 
    }
    return RT_EOK;
}

static int drv_pipe_xfer(upipe_t pipe, rt_uint8_t token, void *buffer, int nbytes, int timeouts)
{
    rt_int32_t tick = 0;
    void *pusb_buffer = RT_NULL;
    int   len = 0;
    int   ret = 0;
    HCD_URBStateTypeDef usb_state;

    if(pipe->ep.bEndpointAddress & 0x80)
    {
        /*IN ep*/
        
        /* Because the USB bottom layer will set the receiving buffer to an integer multiple of the maximum packet length, 
           so the length of the receiving buffer should be  converted to an integer multiple of the maximum packet length zhaoshimin 20211119 */
        if((nbytes) && ((nbytes % pipe->ep.wMaxPacketSize) == 0))
        {
            pusb_buffer = buffer;
        }
        else
        {
            if(nbytes == 0)
            {
                len = pipe->ep.wMaxPacketSize;
            }
            else
            {
                len = (nbytes + pipe->ep.wMaxPacketSize - 1) / pipe->ep.wMaxPacketSize;
                len = len * pipe->ep.wMaxPacketSize;
            }

            pusb_buffer = rt_malloc(len);
            if(pusb_buffer == RT_NULL)
            {
                RT_DEBUG_LOG(RT_DEBUG_USB,
                             ("drv_pipe_xfer malloc: %d faile\n", len));
                return -1;
            }
        }
    }
    else
    {
        pusb_buffer = buffer;
    }    

    rt_sem_control(usb_urb_sem[pipe->pipe_index], RT_IPC_CMD_RESET, RT_NULL);     
    HAL_HCD_HC_SubmitRequest(&stm32_hhcd,
                             pipe->pipe_index,
                             (pipe->ep.bEndpointAddress & 0x80) >> 7,
                             pipe->ep.bmAttributes,
                             token,
                             pusb_buffer,
                             nbytes,
                             0);

    if(timeouts == 0)
    {
        tick = RT_WAITING_FOREVER;
    }
    else
    {
        tick = rt_tick_from_millisecond(timeouts);
    }
   
    if(-RT_ETIMEOUT == rt_sem_take(usb_urb_sem[pipe->pipe_index], tick))
    {
        rt_kprintf("sem %d timeout!\n", pipe->pipe_index);
        HAL_HCD_HC_Halt(&stm32_hhcd, pipe->pipe_index);
        ret =  -RT_ETIMEOUT;
        goto __exit;
    }
#ifndef DRV_USBH_USE_HS    
    if((pipe->ep.bEndpointAddress & 0x80) == 0)
    {
        /*OUT EP */
        /*between twice usb out transfer, should add the less 1ms delay. 
         if not next out transfer return NAK zhaoshimin 2021050 */
        rt_thread_mdelay(1);
    }
#endif    
   
    if (!connect_status)
    {
        ret =  -RT_ERROR;
        goto __exit;
    }
    
    usb_state = HAL_HCD_HC_GetURBState(&stm32_hhcd, pipe->pipe_index);
    if (usb_state == URB_NOTREADY)
    {
        RT_DEBUG_LOG(RT_DEBUG_USB, ("nak\n"));
        if (pipe->ep.bmAttributes == USB_EP_ATTR_INT)
        {
            rt_thread_delay((pipe->ep.bInterval * RT_TICK_PER_SECOND / 1000) > 0 ? (pipe->ep.bInterval * RT_TICK_PER_SECOND / 1000) : 1);
        }                 
        
    }
    else if(usb_state == URB_STALL)
    {
        RT_DEBUG_LOG(RT_DEBUG_USB, ("stall\n"));
        pipe->status = UPIPE_STATUS_STALL;
        if (pipe->callback != RT_NULL)
        {
            pipe->callback(pipe);
        }
        ret =  -RT_ERROR;
    }
    else if(usb_state == URB_ERROR)
    {
        RT_DEBUG_LOG(RT_DEBUG_USB, ("error\n"));
        pipe->status = UPIPE_STATUS_ERROR;
        if (pipe->callback != RT_NULL)
        {
            pipe->callback(pipe);
        }
        ret =  -RT_ERROR;
    }
    else if(URB_DONE == usb_state)
    {
        RT_DEBUG_LOG(RT_DEBUG_USB, ("ok\n"));
        pipe->status = UPIPE_STATUS_OK;
        if (pipe->callback != RT_NULL)
        {
            pipe->callback(pipe);
        }
        ret = HAL_HCD_HC_GetXferCount(&stm32_hhcd, pipe->pipe_index);

        if((ret > 0) && (pusb_buffer != buffer) && (pusb_buffer) && (buffer))
        {
            rt_memcpy(buffer, pusb_buffer, nbytes);
            
        }
        
    }
    else
    {
        RT_DEBUG_LOG(RT_DEBUG_USB, ("usb status:%d\n", usb_state));
        
        HAL_HCD_HC_Halt(&stm32_hhcd, pipe->pipe_index);
        ret =  -RT_ERROR;
    }

__exit:
    if((pusb_buffer != buffer) && (pusb_buffer))
    {
        rt_free(pusb_buffer);
        pusb_buffer = RT_NULL;
    }    
    return ret;
}

#ifndef DRV_USBH_USE_HS
static int drv_pipe_xfer_shell(upipe_t pipe, rt_uint8_t token, void *buffer, int nbytes, int timeouts)
{
    if(pipe->ep.bEndpointAddress & 0x80)
    {
        /*IN EP*/
        return drv_pipe_xfer(pipe, token, buffer, nbytes, timeouts);
    }
    else
    {
        /*OUT EP Subcontracting sending data*/
        rt_size_t remain_size;
        rt_size_t send_size;
        remain_size = nbytes;
        rt_uint8_t * pbuffer = (rt_uint8_t *)buffer;
        do
        {
            send_size = (remain_size > pipe->ep.wMaxPacketSize) ? pipe->ep.wMaxPacketSize : remain_size;
            if(drv_pipe_xfer(pipe, token, pbuffer, send_size, timeouts) == send_size)
            {
                remain_size -= send_size;
                pbuffer += send_size;
            }
            else
            {
                return -1;
            }
        }while(remain_size > 0);

        return nbytes;

    }

}
#endif

static rt_uint16_t pipe_index = 0;
static rt_uint8_t  drv_get_free_pipe_index(void)
{
    rt_uint8_t idx = 0;
    
    for (idx = 0; idx < STM32_USB_HOST_CHN_NUM; idx++)
    {
        if (!(pipe_index & (0x01 << idx)))
        {
            pipe_index |= (0x01 << idx);
            return idx;
        }
    }
    return 0xff;
}

static void drv_free_pipe_index(rt_uint8_t index)
{
    pipe_index &= ~(0x01 << index);
}

static rt_err_t drv_open_pipe(upipe_t pipe)
{
    pipe->pipe_index = drv_get_free_pipe_index();
    if(pipe->pipe_index > STM32_USB_HOST_CHN_NUM)
    {
        return -RT_ERROR;
    }
    HAL_HCD_HC_Init(&stm32_hhcd,
                    pipe->pipe_index,
                    pipe->ep.bEndpointAddress,
                    pipe->inst->address,
                    USB_OTG_SPEED_FULL,
                    pipe->ep.bmAttributes,
                    pipe->ep.wMaxPacketSize);
    /* Set DATA0 PID token*/
    if (stm32_hhcd.hc[pipe->pipe_index].ep_is_in)
    {
        stm32_hhcd.hc[pipe->pipe_index].toggle_in = 0;
    }
    else
    {
        stm32_hhcd.hc[pipe->pipe_index].toggle_out = 0;
    }
    return RT_EOK;
}

static rt_err_t drv_close_pipe(upipe_t pipe)
{
    if(pipe->pipe_index < STM32_USB_HOST_CHN_NUM)
    {   
        HAL_HCD_HC_Halt(&stm32_hhcd, pipe->pipe_index);
        drv_free_pipe_index(pipe->pipe_index);
    }
    return RT_EOK;
}

static struct uhcd_ops _uhcd_ops =
{
    drv_reset_port,
#ifdef DRV_USBH_USE_HS
    drv_pipe_xfer,
#else
    drv_pipe_xfer_shell,
#endif    
    drv_open_pipe,
    drv_close_pipe,
    drv_vbus_control,
};

static rt_err_t stm32_hcd_init(rt_device_t device)
{
    rt_uint32_t i = 0;
    char sem_name[10] = {0};
    
    HCD_HandleTypeDef *hhcd = (HCD_HandleTypeDef *)device->user_data;
#ifdef DRV_USBH_USE_HS    
    hhcd->Instance = USB_OTG_HS;
#else
    hhcd->Instance = USB_OTG_FS;
#endif    
    hhcd->Init.Host_channels = STM32_USB_HOST_CHN_NUM;
    hhcd->Init.speed = HCD_SPEED_FULL;
#ifdef DRV_USBH_USE_HS 
    hhcd->Init.dma_enable = ENABLE;
#else
    hhcd->Init.dma_enable = DISABLE;
#endif    
    hhcd->Init.phy_itface = HCD_PHY_EMBEDDED;
    hhcd->Init.Sof_enable = DISABLE;

    for(i = 0; i < hhcd->Init.Host_channels; i++)
    {
        sprintf(sem_name, "urb_sem%d", i);
        usb_urb_sem[i] = rt_sem_create(sem_name, 0, RT_IPC_FLAG_FIFO);
        if(usb_urb_sem[i] == RT_NULL)
        {
            return -RT_ERROR;
        }
    }

    
    RT_ASSERT(HAL_HCD_Init(hhcd) == HAL_OK);
    HAL_HCD_Start(hhcd);
#ifdef USBH_USING_CONTROLLABLE_POWER
    rt_pin_mode(USBH_POWER_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(USBH_POWER_PIN, PIN_LOW);
#endif
    return RT_EOK;
}

int stm_usbh_register(void)
{
    rt_err_t res = -RT_ERROR;

    uhcd_t uhcd = (uhcd_t)rt_malloc(sizeof(struct uhcd));
    if (uhcd == RT_NULL)
    {
        rt_kprintf("uhcd malloc failed\r\n");
        return -RT_ERROR;
    }

    rt_memset((void *)uhcd, 0, sizeof(struct uhcd));

    uhcd->parent.type = RT_Device_Class_USBHost;
    uhcd->parent.init = stm32_hcd_init;
    uhcd->parent.user_data = &stm32_hhcd;

    uhcd->ops = &_uhcd_ops;
    uhcd->num_ports = OTG_FS_PORT;
    stm32_hhcd.pData = uhcd;

    res = rt_device_register(&uhcd->parent, "usbh", RT_DEVICE_FLAG_DEACTIVATE);
    if (res != RT_EOK)
    {
        rt_kprintf("register usb host failed res = %d\r\n", res);
        return -RT_ERROR;
    }

    rt_usb_host_init();

    return RT_EOK;
}
INIT_DEVICE_EXPORT(stm_usbh_register);
