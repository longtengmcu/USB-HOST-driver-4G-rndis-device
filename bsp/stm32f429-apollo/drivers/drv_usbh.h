/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2017-12-12     ZYH               the first version
 * 2019-12-19     tyustli           port to stm32 series
 */
#ifndef __DRV_USBH_H__
#define __DRV_USBH_H__
#include <rtthread.h>

#define OTG_FS_PORT 1

#define USBH_USING_CONTROLLABLE_POWER
#ifdef  USBH_USING_CONTROLLABLE_POWER

#define USBH_POWER_PIN       46                    /*PB0  46*/
#endif

//#define DRV_USBH_USE_HS

int stm_usbh_register(void);

#endif

/************* end of file ************/
