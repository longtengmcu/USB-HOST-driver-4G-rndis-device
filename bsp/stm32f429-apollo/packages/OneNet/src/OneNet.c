/****************************************Copyright (c)****************************************************
**                                       Zhao shimin
**
**                                     Mobile:13810794827
**                                     QQ:283669063
**--------------File Info---------------------------------------------------------------------------------
** File name:           OneNet.c
** Last modified Date:  2020-07-08
** Last Version:        V1.00
** Descriptions:        OneNet Iot Platform access programe 
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
#include <rtthread.h>
#include <rtdevice.h>
#include <mqttclient.h>
#include <tiny_base64.h>
#include <tiny_sha1.h>
#include <cJSON.h>
#include <webclient.h>
#include <OneNet.h>


#define DBG_ENABLE
#define DBG_SECTION_NAME               "OneNet"
#ifdef ONENET_DEBUG
#define DBG_LEVEL                      DBG_LOG
#else
#define DBG_LEVEL                      DBG_INFO
#endif /* WEBCLIENT_DEBUG */
#define DBG_COLOR
#include <rtdbg.h>

ONENET_SESSION *g_onenet = NULL;
/*********************************************************************************************************
** Function name：      OneNet_UrlEncode()
** Descriptions:        convert special char
**                      +           %2B
**                      空格        %20
**                      /           %2F
**                      ?           %3F
**                      %           %25
**                      #           %23
**                      &           %26
**                      =           %3D
**
** input parameters：   sign: input char
** output parameters：  char
** Returned value:      None
*********************************************************************************************************/
static int OneNet_UrlEncode(char *sign)
{

    char sign_t[40] = {0};
    uint32_t  i = 0, j = 0;
    uint32_t  sign_len = strlen(sign);
    
    if((sign == NULL) || (sign_len < 28))
    {
        return -1;
    }    
    
    for(i = 0; i < sign_len; i++)
    {
        sign_t[i] = sign[i];
        sign[i] = 0;
    }
    sign_t[i] = 0;
    
    for(i = 0, j = 0; i < sign_len; i++)
    {
        switch(sign_t[i])
        {
            case '+':
                strcat(sign + j, "%2B"); 
                j += 3;
            break;
            
            case ' ':
                strcat(sign + j, "%20");
                j += 3;
            break;
            
            case '/':
                strcat(sign + j, "%2F");
                j += 3;
            break;
            
            case '?':
                strcat(sign + j, "%3F");
                j += 3;
            break;
            
            case '%':
                strcat(sign + j, "%25");
                j += 3;
            break;
            
            case '#':
                strcat(sign + j, "%23");
                j += 3;
            break;
            
            case '&':
                strcat(sign + j, "%26");
                j += 3;
            break;
            
            case '=':
                strcat(sign + j, "%3D");j += 3;
            break;
            
            default:
                sign[j] = sign_t[i];
                j++;
            break;
        }
    }
    
    sign[j] = 0;
    
    return 0;

}



/*********************************************************************************************************
** Function name：      OneNet_authorization_generate()
** Descriptions:        generate OneNet authorization
** input parameters：   onenet: onenet session data
** output parameters：  None
** Returned value:      0: SUCCESS  <0: FAILE
*********************************************************************************************************/
static void  OneNet_authorization_generate(const  char *product_id,  const char *api_key, const char  *device_name,
                                                     char *authorization, uint32_t author_size)
{
    RT_ASSERT(product_id != NULL);
    RT_ASSERT(api_key);
    RT_ASSERT(authorization);
    
    /*according to now time to compute the authorization*/
    int       olen = 0;
    uint32_t  time_stamp = 0;
    uint8_t   sign_buf[40] = {0};                              //保存签名的Base64编码结果 和 URL编码结果
    uint8_t   hmac_sha1_buf[40] = {0};                         //保存签名
    uint8_t   access_key_base64[40] = {0};                    //保存access_key的Base64编码结合
    uint8_t   string_for_signature[70] = {0};                 //保存string_for_signature，这个是加密的key

    
//----------------------------------------------------将access_key进行Base64解码----------------------------------------------------
    memset(access_key_base64, 0, sizeof(access_key_base64));
    olen = sizeof(access_key_base64);
    tiny_base64_decode(access_key_base64, &olen, (uint8_t *)api_key, strlen(api_key));
    
//----------------------------------------------------计算string_for_signature-----------------------------------------------------
    /*read rtc and get time_stamp */
    time_stamp = time(RT_NULL);

    if(time_stamp < 1593537582)
    {
        /*RTC time stamp is less than 2020-07-01(1593537582) not sync time, so use the 2032-01-01 0:0:0 time stamp*/
        time_stamp = 1956499200;
    }
    else
    {
        time_stamp =  time_stamp + (3600 * 2);    
    }

    
    memset(string_for_signature, 0, sizeof(string_for_signature));

    if(device_name == RT_NULL)
    {
        /*计算产品API访问的验证码, 打印无符号数据使用%u*/
        snprintf((char *)string_for_signature, sizeof(string_for_signature), "%u\n%s\nproducts/%s\n%s", time_stamp, 
                ONENET_AUTHORIZATION_ALGORITHM, product_id, ONENET_AUTHORIZATION_VER);
    }
    else
    {
        /*计算设备访问的验证码,打印无符号数据使用%u*/
        snprintf((char *)string_for_signature, sizeof(string_for_signature), "%u\n%s\nproducts/%s/devices/%s\n%s", time_stamp, 
            ONENET_AUTHORIZATION_ALGORITHM, product_id, device_name, ONENET_AUTHORIZATION_VER);
    }
    
    
//----------------------------------------------------加密-------------------------------------------------------------------------
    memset(hmac_sha1_buf, 0, sizeof(hmac_sha1_buf));
    tiny_sha1_hmac(access_key_base64, olen, string_for_signature, strlen((char *)string_for_signature),
                    hmac_sha1_buf);
 
    
//----------------------------------------------------将加密结果进行Base64编码------------------------------------------------------
    memset(sign_buf, 0, sizeof(sign_buf));
    olen = sizeof(sign_buf);
    tiny_base64_encode(sign_buf, &olen, hmac_sha1_buf, strlen((char *)hmac_sha1_buf));

//----------------------------------------------------将Base64编码结果进行URL编码---------------------------------------------------
    OneNet_UrlEncode((char *)sign_buf);
    
    
//----------------------------------------------------计算Token--------------------------------------------------------------------
    if(device_name == RT_NULL)
    {
        snprintf(authorization, author_size, "version=%s&res=products%%2F%s&et=%u&method=%s&sign=%s", ONENET_AUTHORIZATION_VER, 
                product_id, time_stamp, ONENET_AUTHORIZATION_ALGORITHM, sign_buf);
    }
    else
    {
        snprintf(authorization, author_size, "version=%s&res=products%%2F%s%%2Fdevices%%2F%s&et=%u&method=%s&sign=%s", ONENET_AUTHORIZATION_VER, 
                product_id, device_name, time_stamp, ONENET_AUTHORIZATION_ALGORITHM, sign_buf);
    }
    
}

/*********************************************************************************************************
** Function name：      OneNet_reconnect_handler()
** Descriptions:        OneNet reconnect function, it use to computer the mqtt password
** input parameters：   clien, reconnet_date
** output parameters：  None
** Returned value:      NULL or OneNet session handle
*********************************************************************************************************/
static void OneNet_reconnect_handler(void* client, void* reconnect_data)
{
    ONENET_SESSION *onenet = (ONENET_SESSION *)reconnect_data;


    OneNet_authorization_generate(onenet->product_id, onenet->api_key, onenet->device_name, 
                                  onenet->authorization, sizeof(onenet->authorization));

    //LOG_I("OneNet mqtt reconnect once!\r\n");
  
}

/*********************************************************************************************************
** Function name：      OneNet_sub_topic_handle()
** Descriptions:        OneNet topic callback handle function
** input parameters：   clien: mqtt session
**                      msg:   mqtt recv data
** output parameters：  None
** Returned value:      None
*********************************************************************************************************/
static void OneNet_sub_topic_handle(void *client, message_data_t* msg)
{
    char ack_buf[100] = {0};
    char *puid = NULL;
    mqtt_message_t data_msg;
    ONENET_SESSION *onenet = NULL;

    onenet = g_onenet;

    snprintf(ack_buf, sizeof(ack_buf), "$sys/%s/%s/cmd/request", onenet->product_id, onenet->device_name);
    
    if(strstr((char *)msg->topic_name, ack_buf))
    {
        /*get the cmd uid*/
        puid = ((char *)msg->topic_name+ strlen(ack_buf) + 1);

        rt_kprintf("Get cmd uiid:%s\r\n", puid);
        
        snprintf(ack_buf, sizeof(ack_buf), "$sys/%s/%s/cmd/response/%s", onenet->product_id, onenet->device_name,puid);

        memset(&data_msg, 0, sizeof(data_msg));
        data_msg.qos = QOS0;
        data_msg.retained = 0;
        data_msg.id = 101;
        data_msg.payload = "I have receive cmd!";
        data_msg.payloadlen = strlen("I have receive cmd!");

        
        rt_kprintf("ack topic name:%s \r\n", ack_buf);
        mqtt_publish(client, ack_buf, &data_msg);
    }

    
}


/*********************************************************************************************************
** Function name：      OneNet_session_creat()
** Descriptions:        creat the OneNet session struct
** input parameters：   onenet_init  onenet init data
** output parameters：  None
** Returned value:      NULL or OneNet session handle
*********************************************************************************************************/
ONENET_SESSION *OneNet_session_creat(ONENET_INIT *onenet_init)
{
    ONENET_SESSION *session = NULL;

    if(NULL == onenet_init)
    {
        LOG_E("input parameters is NULL!");
        return NULL;
    }

    session = (ONENET_SESSION *)OneNet_malloc(sizeof(ONENET_SESSION));

    if(session == NULL)
    {
        LOG_E("malloc the ONENET_SESSION struct faile!");
        return session;
    }

    memset(session, 0, sizeof(ONENET_SESSION));

    strncpy(session->ip, onenet_init->ip, sizeof(session->ip));
    strncpy(session->device_id, onenet_init->device_id, sizeof(session->device_id));
    strncpy(session->product_id, onenet_init->product_id, sizeof(session->product_id));
    strncpy(session->device_name, onenet_init->device_name, sizeof(session->device_name));
    strncpy(session->api_key, onenet_init->api_key, sizeof(session->api_key));
    strncpy(session->port, onenet_init->port, sizeof(session->port));
    
    
    /*malloc the mqtt struct*/
    mqtt_log_init();
    
    session->mqtt = mqtt_lease();
    
    if(session->mqtt == NULL)
    {
        LOG_E("malloc the mqtt_lease faile!");
        return session;
    }

    mqtt_set_host(session->mqtt, session->ip);
    mqtt_set_port(session->mqtt, session->port);
    mqtt_set_user_name(session->mqtt, session->product_id);
    
    mqtt_set_client_id(session->mqtt, session->device_name);
    mqtt_set_clean_session(session->mqtt, 1);

    /*pass session data into reconnect handler function*/
    mqtt_set_reconnect_handler(session->mqtt, OneNet_reconnect_handler);
    mqtt_set_reconnect_data(session->mqtt, session);

    g_onenet = session;
    return session;
}


/*********************************************************************************************************
** Function name：      OneNet_connect()
** Descriptions:        connect to OneNet platform
** input parameters：   onenet: onenet session data
** output parameters：  None
** Returned value:      0: SUCCESS  <0: FAILE
*********************************************************************************************************/
int OneNet_connect(ONENET_SESSION *onenet)
{
    RT_ASSERT(onenet != NULL);
    
    /*computer onenet mqtt password*/
    OneNet_authorization_generate(onenet->product_id, onenet->api_key, onenet->device_name, onenet->authorization, sizeof(onenet->authorization));
    
    /*onenet password max len = 114*/
    mqtt_set_password(onenet->mqtt, onenet->authorization);

    return mqtt_connect(onenet->mqtt);

}

/*********************************************************************************************************
** Function name：      OneNet_disconnect()
** Descriptions:        disconnect from OneNet platform
** input parameters：   onenet  onenet session data
** output parameters：  None
** Returned value:      0: SUCCESS  <0: FAILE
*********************************************************************************************************/
int OneNet_disconnect(ONENET_SESSION *onenet)
{
    RT_ASSERT(onenet != NULL);
    
    return mqtt_disconnect(onenet->mqtt);
}


/*********************************************************************************************************
** Function name：      OneNet_disconnect()
** Descriptions:        disconnect from OneNet platform
** input parameters：   onenet  onenet session data
** output parameters：  None
** Returned value:      0: SUCCESS  <0: FAILE
*********************************************************************************************************/
int OneNet_topic_subscribe(ONENET_SESSION *onenet, ONENET_TOPIC_NUM topic_id)
{
    RT_ASSERT(onenet != NULL);
    
    switch(topic_id)
    {
    case ONENET_TOPIC_DATAPOINT:
        snprintf(onenet->topic[topic_id], ONENET_TOPIC_LEN - 1, "$sys/%s/%s/dp/post/json/+", onenet->product_id, onenet->device_name);
        break;

    case ONENET_TOPIC_CMD:
        snprintf(onenet->topic[topic_id], ONENET_TOPIC_LEN - 1, "$sys/%s/%s/cmd/#", onenet->product_id, onenet->device_name);
        break;   
    default:
        LOG_E("Not support this topic id:%d!", topic_id);
        return -1;
    }

    return mqtt_subscribe(onenet->mqtt, onenet->topic[topic_id], QOS0, OneNet_sub_topic_handle);
 
}

/*********************************************************************************************************
** Function name：      OneNet_session_delete()
** Descriptions:        delet the OneNet session struct
**                      Must first call OneNet_disconnect to free mqtt connection
** input parameters：   onenet  onenet session data pointer
**
** output parameters：  None
** Returned value:      0:SUCCESS  <0: FAILE
*********************************************************************************************************/
int  OneNet_session_delete(ONENET_SESSION *onenet)
{
    int  ret = 0;
    
    RT_ASSERT(onenet != NULL);

    ret = mqtt_release(onenet->mqtt);

    if(ret != KAWAII_MQTT_SUCCESS_ERROR)
    {
        return ret;
    }

    onenet->mqtt = NULL;

    OneNet_free(onenet);
    onenet = NULL;

    return KAWAII_MQTT_SUCCESS_ERROR;

    
}

/*********************************************************************************************************
** Function name：      OneNet_device_register()
** Descriptions:        register a device to onenet platform
** input parameters：   pid and access_key:  product id, access key 
** output parameters：  device_id:  device id buf, 
**                      api_key: device api key
**
** Returned value:      0: SUCCESS  <0: FAILE
*********************************************************************************************************/
int OneNet_device_register(const char *pid, const char *access_key, const char *device_name,
                                   char *device_id, uint32_t device_id_size,
                                   char *api_key,   uint32_t api_key_size)
{
    char authorization[ONENET_AUTHORIZATION_LEN] = {0};
    char *request = RT_NULL, *header = RT_NULL;
    int   ret = 0;
    cJSON *json_data = RT_NULL;
    char  *p_post = RT_NULL;
    cJSON *data = RT_NULL, *device = RT_NULL;

    RT_ASSERT(pid);
    RT_ASSERT(access_key);
    RT_ASSERT(device_id);
    RT_ASSERT(api_key);

    /*compute the onenet API authorization*/
    OneNet_authorization_generate(pid, access_key, RT_NULL, authorization, sizeof(authorization));

    /*generate the post json data from device name*/
    json_data = cJSON_CreateObject();
    cJSON_AddItemToObject(json_data, "name", cJSON_CreateString(device_name));
    p_post = cJSON_Print(json_data);

    cJSON_Delete(json_data);
    json_data = RT_NULL;
    
    if(p_post == RT_NULL)
    {
        LOG_E("cJSON print failed!");
        return -RT_ERROR; 
    }

    webclient_request_header_add(&header, "Authorization: %s\r\n",  authorization);
    /*after web post, server should close connection! so client can release the socket connetion */
    webclient_request_header_add(&header, "Connection: close\r\n");
    
    if (webclient_request(ONENET_DEVICE_REGISTER_URI, (const char *)header, p_post, (unsigned char **)&request) < 0)
    {
        LOG_E("webclient send post request failed.");
        web_free(header);
        OneNet_free(p_post);
        return -RT_ERROR;
    }

    OneNet_free(p_post);

    /*analysis the device id from request buf*/
    /*{
  "request_id" : "36808a5d-93d8-42cd-8a2c-357e208f7623",
  "code" : "onenet_common_success",
  "code_no" : "000000",
  "message" : null,
  "data" : {
    "device_id" : "604745909",
    "name" : "FHQLONGTENG",
    "pid" : 354804,
    "key" : "DECCw7OmgCJ+qs9dFeAeRrkbpPIv9jySCZkOqEk8TSs="
  }
}
    */
    json_data = cJSON_Parse(request);
    
    if(json_data)
    {
        data = cJSON_GetObjectItem(json_data, "data");
        if(data)
        {
            device = cJSON_GetObjectItem(data, "device_id");

            if((device->type == cJSON_String) && (device_id_size > strlen(device->valuestring)))
            {
                strcpy(device_id ,device->valuestring);
                ret = RT_EOK;
            }
            else
            {
                LOG_E("Not found device_id or device_id buf is small!");
                ret = -RT_ENOMEM;
            }

            device = cJSON_GetObjectItem(data, "key");

            if((device->type == cJSON_String) && (api_key_size > strlen(device->valuestring)))
            {
                strcpy(api_key ,device->valuestring);
                ret = RT_EOK;
            }
            else
            {
                LOG_E("Not found api-key or api-key buf is small!");
                ret = -RT_ENOMEM;
            }
            
        }
        else
        {
            LOG_E("Not found data json!"); 
            ret = -RT_ERROR;
        }
    }
    else
    {
        LOG_E("Not a valid json!");
        ret = -RT_ERROR;
    }
    
   
    cJSON_Delete(json_data);

    if (header)
    {
        web_free(header);
    }

    if (request)
    {
        web_free(request);
    }

    return ret;

}

/*********************************************************************************************************
** Function name：      OneNet_session_send()
** Descriptions:        send data to onenet platform
**                      
** input parameters：   onenet  onenet session data pointer
**
** output parameters：  None
** Returned value:      0:SUCCESS  <0: FAILE
*********************************************************************************************************/
int  OneNet_session_send(ONENET_SESSION *onenet, char *send_data, uint32_t send_len)
{
    char send_topic[ONENET_TOPIC_LEN] = {0};
    mqtt_message_t data_msg;

    RT_ASSERT(onenet);
    RT_ASSERT(send_data);
    
    data_msg.qos =  QOS0;
    data_msg.payloadlen = send_len; 
    data_msg.payload = send_data;
    /* onenet publish msg head bit retained must be 0*/
    data_msg.retained = 0;
    data_msg.id = 100;

    snprintf(send_topic, sizeof(send_topic), "$sys/%s/%s/dp/post/json", onenet->product_id, onenet->device_name);
   
    return mqtt_publish(onenet->mqtt, send_topic, &data_msg);

}

/*********************************************************************************************************
**   End of file
*********************************************************************************************************/


