/****************************************Copyright (c)****************************************************
**                                       Zhao shimin
**
**                                     Mobile:13810794827
**                                     QQ:283669063
**--------------File Info---------------------------------------------------------------------------------
** File name:           OneNet.h
** Last modified Date:  2020-07-08
** Last Version:        V1.00
** Descriptions:        OneNet Iot Platform access header
**
**--------------------------------------------------------------------------------------------------------
** Created by:          Zhao shimin
** Created date:        2020-07-08
** Version:             V1.00
** Descriptions:        
**
**
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
**
** Rechecked by:
*********************************************************************************************************/
#ifndef __ONE_NET_H_
#define  __ONE_NET_H_

/*********************************************************************************************************
*  ¡¡¡¡¡¡¡¡¡¡¡¡define onenet micro
*********************************************************************************************************/
#ifndef  ONENET_TOPIC_LEN
#define   ONENET_TOPIC_LEN           100
#endif

#ifndef  ONENET_AUTHORIZATION_LEN
#define   ONENET_AUTHORIZATION_LEN   150
#endif

#ifndef  ONENET_IP_LEN
#define   ONENET_IP_LEN               20
#endif

#ifndef  ONENET_DEVICE_ID_LEN
#define   ONENET_DEVICE_ID_LEN       20
#endif

#ifndef  ONENET_PRODUCT_ID_LEN
#define   ONENET_PRODUCT_ID_LEN      8
#endif

#ifndef  ONENET_PRODUCT_NAME_LEN
#define   ONENET_PRODUCT_NAME_LEN     20
#endif

#ifndef  ONENET_API_KEY_LEN
#define   ONENET_API_KEY_LEN          50
#endif

#define ONENET_DEVICE_REGISTER_URI      "http://api.heclouds.com/mqtt/v1/devices/reg"

#define ONENET_AUTHORIZATION_VER         "2018-10-31"
#define ONENET_AUTHORIZATION_ALGORITHM  "sha1"

/*********************************************************************************************************
*  ¡¡¡¡¡¡¡¡¡¡¡¡define the onenet topic num
*********************************************************************************************************/
typedef enum ONENET_TOPIC_NUM_ENUM {
    ONENET_TOPIC_DATAPOINT = 0,
    ONENET_TOPIC_CMD        = 1,
    ONENET_TOPIC_IMAGE      = 2,
    ONENET_TOPIC_MAX        = 3
} ONENET_TOPIC_NUM;

/*********************************************************************************************************
*  ¡¡¡¡¡¡¡¡¡¡¡¡define the onenet session struct
*********************************************************************************************************/
typedef struct  ONENET_INIT_STRUCT
{
    char *ip;
    char *port;
    char *device_id;                                                       /* device id                */
    char *product_id;
    char *device_name;
    char *api_key;
}ONENET_INIT;

/*********************************************************************************************************
*  ¡¡¡¡¡¡¡¡¡¡¡¡define the onenet session struct
*********************************************************************************************************/
typedef struct  ONENET_SESSION_STRUCT
{
    char ip[ONENET_IP_LEN];
    char  port[6];
    char  device_id[ONENET_DEVICE_ID_LEN];                                  /* device id                */
    char  product_id[ONENET_PRODUCT_ID_LEN];
    char  device_name[ONENET_PRODUCT_NAME_LEN];
    char  api_key[ONENET_API_KEY_LEN];
    char  authorization[ONENET_AUTHORIZATION_LEN];
    mqtt_client_t *mqtt;
    char  topic[ONENET_TOPIC_MAX][ONENET_TOPIC_LEN];
    
}ONENET_SESSION;   

/*********************************************************************************************************
*  ¡¡¡¡¡¡¡¡¡¡¡¡define the onenet pakage port interface, it use micro define
*********************************************************************************************************/
#define   OneNet_malloc       rt_malloc
#define   OneNet_free         rt_free

/*********************************************************************************************************
** Function name£º      OneNet_device_register()
** Descriptions:        register a device to onenet platform
** input parameters£º   pid and access_key:  product id, access key 
** output parameters£º  device_id:  device id buf, 
**                      api_key: device api key
**
** Returned value:      0: SUCCESS  <0: FAILE
*********************************************************************************************************/
int OneNet_device_register(const char *pid, const char *access_key, const char *device_name,
                                   char *device_id, uint32_t device_id_size,
                                   char *api_key,   uint32_t api_key_size);

/*********************************************************************************************************
** Function name£º      OneNet_session_creat()
** Descriptions:        creat the OneNet session struct
** input parameters£º   onenet_init  onenet init data
** output parameters£º  None
** Returned value:      NULL or OneNet session handle
*********************************************************************************************************/
ONENET_SESSION *OneNet_session_creat(ONENET_INIT *onenet_init);

/*********************************************************************************************************
** Function name£º      OneNet_connect()
** Descriptions:        connect to OneNet platform
** input parameters£º   onenet: onenet session data
** output parameters£º  None
** Returned value:      0: SUCCESS  <0: FAILE
*********************************************************************************************************/
int OneNet_connect(ONENET_SESSION *onenet);

/*********************************************************************************************************
** Function name£º      OneNet_disconnect()
** Descriptions:        disconnect from OneNet platform
** input parameters£º   onenet  onenet session data
** output parameters£º  None
** Returned value:      0: SUCCESS  <0: FAILE
*********************************************************************************************************/
int OneNet_disconnect(ONENET_SESSION *onenet);

/*********************************************************************************************************
** Function name£º      OneNet_disconnect()
** Descriptions:        disconnect from OneNet platform
** input parameters£º   onenet  onenet session data
** output parameters£º  None
** Returned value:      0: SUCCESS  <0: FAILE
*********************************************************************************************************/
int OneNet_topic_subscribe(ONENET_SESSION *onenet, ONENET_TOPIC_NUM topic_id);

/*********************************************************************************************************
** Function name£º      OneNet_session_delete()
** Descriptions:        delet the OneNet session struct
**                      Must first call OneNet_disconnect to free mqtt connection
** input parameters£º   onenet  onenet session data pointer
**
** output parameters£º  None
** Returned value:      0:SUCCESS  <0: FAILE
*********************************************************************************************************/
int  OneNet_session_delete(ONENET_SESSION *onenet);

/*********************************************************************************************************
** Function name£º      OneNet_session_send()
** Descriptions:        send data to onenet platform
**                      
** input parameters£º   onenet  onenet session data pointer
**
** output parameters£º  None
** Returned value:      0:SUCCESS  <0: FAILE
*********************************************************************************************************/
int  OneNet_session_send(ONENET_SESSION *onenet, char *send_data, uint32_t send_len);

#endif

