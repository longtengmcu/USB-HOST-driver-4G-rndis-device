/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 00:42:16
 * @LastEditTime: 2020-04-27 23:35:05
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _MQTT_ERROR_H_
#define _MQTT_ERROR_H_

typedef enum mqtt_error {
    KAWAII_MQTT_SSL_CERT_ERROR                                     = -0x001C,      /* cetr parse failed */
    KAWAII_MQTT_SOCKET_FAILED_ERROR                                = -0x001B,      /* socket fd failed */
    KAWAII_MQTT_SOCKET_UNKNOWN_HOST_ERROR                          = -0x001A,      /* socket unknown host ip or domain */ 
    KAWAII_MQTT_SET_PUBLISH_DUP_FAILED_ERROR                       = -0x0019,      /* mqtt publish packet set udp bit failed */
    KAWAII_MQTT_CLEAN_SESSION_ERROR                                = -0x0018,      /* mqtt clean session error */
    KAWAII_MQTT_ACK_NODE_IS_EXIST_ERROR                            = -0x0017,      /* mqtt ack list is exist ack node */
    KAWAII_MQTT_ACK_HANDLER_NUM_TOO_MUCH_ERROR                     = -0x0016,      /* mqtt ack handler number is too much */
    KAWAII_MQTT_RESUBSCRIBE_ERROR                                  = -0x0015,      /* mqtt resubscribe error */
    KAWAII_MQTT_SUBSCRIBE_ERROR                                    = -0x0014,      /* mqtt subscribe error */
    KAWAII_MQTT_SEND_PACKET_ERROR                                  = -0x0013,      /* mqtt send a packet */
    KAWAII_MQTT_SERIALIZE_PUBLISH_ACK_PACKET_ERROR                 = -0x0012,      /* mqtt serialize publish ack packet error */
    KAWAII_MQTT_PUBLISH_PACKET_ERROR                               = -0x0011,      /* mqtt publish packet error */
    KAWAII_MQTT_RECONNECT_TIMEOUT_ERROR                            = -0x0010,      /* mqtt try reconnect, but timeout */
    KAWAII_MQTT_SUBSCRIBE_NOT_ACK_ERROR                            = -0x000F,      /* mqtt subscribe, but not ack */
    KAWAII_MQTT_NOT_CONNECT_ERROR                                  = -0x000E,      /* mqtt not connect */
    KAWAII_MQTT_SUBSCRIBE_ACK_PACKET_ERROR                         = -0x000D,      /* mqtt subscribe, but ack packet error */
    KAWAII_MQTT_UNSUBSCRIBE_ACK_PACKET_ERROR                       = -0x000C,      /* mqtt unsubscribe, but ack packet error */
    KAWAII_MQTT_PUBLISH_ACK_PACKET_ERROR                           = -0x000B,      /* mqtt pubilsh ack packet error */
    KAWAII_MQTT_PUBLISH_ACK_TYPE_ERROR                             = -0x000A,      /* mqtt pubilsh ack type error */
    KAWAII_MQTT_PUBREC_PACKET_ERROR                                = -0x0009,      /* mqtt pubrec packet error */
    KAWAII_MQTT_BUFFER_TOO_SHORT_ERROR                             = -0x0008,      /* mqtt buffer too short */
    KAWAII_MQTT_NOTHING_TO_READ_ERROR                              = -0x0007,      /* mqtt nothing to read */
    KAWAII_MQTT_SUBSCRIBE_QOS_ERROR                                = -0x0006,      /* mqtt subsrcibe qos error */
    KAWAII_MQTT_BUFFER_OVERFLOW_ERROR                              = -0x0005,      /* mqtt buffer overflow */
    KAWAII_MQTT_CONNECT_FAILED_ERROR                               = -0x0004,      /* mqtt connect failed */
    KAWAII_MQTT_MEM_NOT_ENOUGH_ERROR                               = -0x0003,      /* mqtt memory not enough */
    KAWAII_MQTT_NULL_VALUE_ERROR                                   = -0x0002,      /* mqtt value is null */
    KAWAII_MQTT_FAILED_ERROR                                       = -0x0001,      /* failed */
    KAWAII_MQTT_SUCCESS_ERROR                                      = 0x0000        /* success */
} mqtt_error_t;

#define RETURN_ERROR(x) { return x; }

#endif
