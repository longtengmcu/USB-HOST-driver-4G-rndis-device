/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-27 03:25:58
 * @LastEditTime: 2020-06-17 15:26:54
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _MQTT_LOG_H_
#define _MQTT_LOG_H_

#include "mqtt_defconfig.h"

#define KAWAII_MQTT_LOG_BASE_LEVEL      (0)
#define KAWAII_MQTT_LOG_ERR_LEVEL       (KAWAII_MQTT_LOG_BASE_LEVEL + 1)
#define KAWAII_MQTT_LOG_WARN_LEVEL      (KAWAII_MQTT_LOG_ERR_LEVEL + 1)
#define KAWAII_MQTT_LOG_INFO_LEVEL      (KAWAII_MQTT_LOG_WARN_LEVEL + 1)
#define KAWAII_MQTT_LOG_DEBUG_LEVEL     (KAWAII_MQTT_LOG_INFO_LEVEL + 1)

#ifdef KAWAII_MQTT_LOG_IS_SALOF
    #include "salof.h"

    #define KAWAII_MQTT_LOG_D(fmt, ...)   SALOF_LOG_DEBUG(fmt, ##__VA_ARGS__)
    #define KAWAII_MQTT_LOG_I(fmt, ...)   SALOF_LOG_INFO(fmt, ##__VA_ARGS__)
    #define KAWAII_MQTT_LOG_W(fmt, ...)   SALOF_LOG_WARN(fmt, ##__VA_ARGS__)
    #define KAWAII_MQTT_LOG_E(fmt, ...)   SALOF_LOG_ERR(fmt, ##__VA_ARGS__)
    #define mqtt_log_init   salof_init
#else
    #include <stdio.h>
    
#if KAWAII_MQTT_LOG_LEVEL < KAWAII_MQTT_LOG_DEBUG_LEVEL
    #define KAWAII_MQTT_LOG_D(fmt, ...)
#else
    #define KAWAII_MQTT_LOG_D(fmt, ...)        { rt_kprintf(fmt, ##__VA_ARGS__); rt_kprintf("\n");}
#endif

#if KAWAII_MQTT_LOG_LEVEL < KAWAII_MQTT_LOG_INFO_LEVEL
    #define KAWAII_MQTT_LOG_I(fmt, ...)
#else
    #define KAWAII_MQTT_LOG_I(fmt, ...)        { rt_kprintf(fmt, ##__VA_ARGS__); rt_kprintf("\n");}
#endif

#if KAWAII_MQTT_LOG_LEVEL < KAWAII_MQTT_LOG_WARN_LEVEL
    #define KAWAII_MQTT_LOG_W(fmt, ...)
#else
    #define KAWAII_MQTT_LOG_W(fmt, ...)        { rt_kprintf(fmt, ##__VA_ARGS__); rt_kprintf("\n");}
#endif

#if KAWAII_MQTT_LOG_LEVEL < KAWAII_MQTT_LOG_ERR_LEVEL
    #define KAWAII_MQTT_LOG_E(fmt, ...)
#else
    #define KAWAII_MQTT_LOG_E(fmt, ...)        { rt_kprintf(fmt, ##__VA_ARGS__); rt_kprintf("\n");}
#endif

#if KAWAII_MQTT_LOG_LEVEL < KAWAII_MQTT_LOG_BASE_LEVEL
    #define KAWAII_MQTT_LOG(fmt, ...)
#else
    #define KAWAII_MQTT_LOG(fmt, ...)          { rt_kprintf(fmt, ##__VA_ARGS__); rt_kprintf("\n");}
#endif

    #define mqtt_log_init()
#endif

#endif /* _LOG_H_ */
