/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-02-25 03:36:09
 * @LastEditTime: 2020-05-20 16:24:20
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */

#ifndef _DEFCONFIG_H_
#define _DEFCONFIG_H_

#include "rtconfig.h"

#ifndef KAWAII_MQTT_LOG_LEVEL
    #define KAWAII_MQTT_LOG_LEVEL                          KAWAII_MQTT_LOG_DEBUG_LEVEL   //KAWAII_MQTT_LOG_WARN_LEVEL KAWAII_MQTT_LOG_DEBUG_LEVEL
#endif // !KAWAII_MQTT_LOG_LEVEL

#ifndef KAWAII_MQTT_MAX_PACKET_ID
    #define     KAWAII_MQTT_MAX_PACKET_ID                  (0xFFFF - 1)
#endif // !KAWAII_MQTT_MAX_PACKET_ID

#ifndef KAWAII_MQTT_TOPIC_LEN_MAX
    #define     KAWAII_MQTT_TOPIC_LEN_MAX                  64
#endif // !KAWAII_MQTT_TOPIC_LEN_MAX

#ifndef KAWAII_MQTT_ACK_HANDLER_NUM_MAX
    #define     KAWAII_MQTT_ACK_HANDLER_NUM_MAX            64
#endif // !KAWAII_MQTT_ACK_HANDLER_NUM_MAX

#ifndef KAWAII_MQTT_DEFAULT_BUF_SIZE
    #define     KAWAII_MQTT_DEFAULT_BUF_SIZE               1024
#endif // !KAWAII_MQTT_DEFAULT_BUF_SIZE

#ifndef KAWAII_MQTT_DEFAULT_CMD_TIMEOUT
    #define     KAWAII_MQTT_DEFAULT_CMD_TIMEOUT            4000
#endif // !KAWAII_MQTT_DEFAULT_CMD_TIMEOUT

#ifndef KAWAII_MQTT_MAX_CMD_TIMEOUT
    #define     KAWAII_MQTT_MAX_CMD_TIMEOUT                20000
#endif // !KAWAII_MQTT_MAX_CMD_TIMEOUT

#ifndef KAWAII_MQTT_MIN_CMD_TIMEOUT
    #define     KAWAII_MQTT_MIN_CMD_TIMEOUT                1000
#endif // !KAWAII_MQTT_MIN_CMD_TIMEOUT

#ifndef KAWAII_MQTT_KEEP_ALIVE_INTERVAL
    #define     KAWAII_MQTT_KEEP_ALIVE_INTERVAL            100      // unit: second
#endif // !KAWAII_MQTT_KEEP_ALIVE_INTERVAL

#ifndef KAWAII_MQTT_VERSION
    #define     KAWAII_MQTT_VERSION                        4      // 4 is mqtt 3.1.1
#endif // !KAWAII_MQTT_VERSION

#ifndef KAWAII_MQTT_RECONNECT_DEFAULT_DURATION
    #define     KAWAII_MQTT_RECONNECT_DEFAULT_DURATION     1000
#endif // !KAWAII_MQTT_RECONNECT_DEFAULT_DURATION

#ifndef KAWAII_MQTT_THREAD_STACK_SIZE
    #define     KAWAII_MQTT_THREAD_STACK_SIZE              4096
#endif // !KAWAII_MQTT_THREAD_STACK_SIZE

#ifndef KAWAII_MQTT_THREAD_PRIO
    #define     KAWAII_MQTT_THREAD_PRIO                    5
#endif // !KAWAII_MQTT_THREAD_PRIO

#ifndef KAWAII_MQTT_THREAD_TICK
    #define     KAWAII_MQTT_THREAD_TICK                    50
#endif // !KAWAII_MQTT_THREAD_TICK


#ifdef KAWAII_MQTT_NETWORK_TYPE_TLS

#ifndef KAWAII_MQTT_TLS_HANDSHAKE_TIMEOUT
    #define KAWAII_MQTT_TLS_HANDSHAKE_TIMEOUT  (5 * 1000)
#endif // !KAWAII_MQTT_TLS_HANDSHAKE_TIMEOUT
    
    #include "mbedtls/config.h"
    #include "mbedtls/ssl.h"
    #include "mbedtls/entropy.h"
    #include "mbedtls/net_sockets.h"
    #include "mbedtls/ctr_drbg.h"
    #include "mbedtls/error.h"
    #include "mbedtls/debug.h"
#endif /* KAWAII_MQTT_NETWORK_TYPE_TLS */

#endif /* _DEFCONFIG_H_ */
