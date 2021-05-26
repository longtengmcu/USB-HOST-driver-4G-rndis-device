/****************************************Copyright (c)****************************************************
**                                    fhqlongteng@163.com                              
**                                     QQ:283669063
**--------------File Info---------------------------------------------------------------------------------
** File name:           zFTP.h
** Last modified Date:  2020-12-01
** Last Version:        V1.00
** Descriptions:        FTP通信协议头文件 
**
**--------------------------------------------------------------------------------------------------------
** Created by:          Zhao shimin
** Created date:        2020-12-01
** Version:             V1.00
** Descriptions:        
**
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
**
** Rechecked by:
*********************************************************************************************************/
#ifndef __zFTP_H_
#define  __zFTP_H_


/*********************************************************************************************************
*  　　　　　　定义FTP的客户端的存储的缓冲区长度
*********************************************************************************************************/
#ifndef  ZFTP_USER_NAME_LEN
#define   ZFTP_USER_NAME_LEN     20
#endif

#ifndef  ZFTP_PASSWORD_LEN
#define   ZFTP_PASSWORD_LEN      20
#endif

#ifndef  ZFTP_SERVER_IP_LEN
#define   ZFTP_SERVER_IP_LEN       20
#endif

#define  ZFTP_SERVER_CONTROL_PORT        "21"
#define  ZFTP_SERVER_DATA_PORT           "22"

/*FTP communicate command timeout, unit:ms  */
#define  ZFTP_CMD_TIMEOUT                 2000


#define  ZFTP_FILE_DATA_BUF_SIZE        400

typedef  int (*down_callback)(void * handle, char *file_name, rt_uint8_t *buf, rt_uint32_t len, rt_uint32_t file_pos, rt_uint32_t total_len);
typedef  int (*up_callback)(void * handle, char *file_name, rt_uint8_t *buf, rt_uint32_t len, rt_uint32_t file_pos, rt_uint32_t *read_len, rt_uint32_t *total_len);

/*********************************************************************************************************
*  　　　　　　定义zFTP通信管理数据结构
*********************************************************************************************************/
typedef struct zftp_client_struct
{
    char           user_name[ZFTP_USER_NAME_LEN];
    char           password[ZFTP_PASSWORD_LEN];
    char           server_ip[ZFTP_SERVER_IP_LEN];
    char           data_port[6];
    char           control_port[6];
    int            control_socket;                                             /* FTP控制socket    */
    int            data_socket;                                                /* FTP数据socket    */
    down_callback  download_write;
    up_callback    upload_read;
    void           *user_data;
    int            errorno;
   
}zftp_client;

/*********************************************************************************************************
** Function name：      zFTP_login()
** Descriptions:        ftp login server
** input parameters：   user_name, password, server_ip
** output parameters：  None 
** Returned value:      zftp_client
*********************************************************************************************************/
zftp_client *zFTP_login(char *user_name, char *password, char *server_ip);

/*********************************************************************************************************
** Function name：      zFTP_set_callback()
** Descriptions:        set the file download and upload callback function
** input parameters：   ftp, dw_write, up_read
** output parameters：  None
** Returned value:      RT_EOK
*********************************************************************************************************/
int zFTP_set_callback(zftp_client *ftp, down_callback  dw_write, up_callback up_read);

/*********************************************************************************************************
** Function name：      zFTP_get_filesize()
** Descriptions:        get the file size from FTP server
** input parameters：   ftp, path, file_name
** output parameters：  file_size
** Returned value:      zftp_client
*********************************************************************************************************/
int zFTP_get_filesize(zftp_client *ftp, char *file_name, rt_uint32_t *file_size);

/*********************************************************************************************************
** Function name：      zFTP_change_path()
** Descriptions:        change the path of FTP server
** input parameters：   ftp, path, path
** output parameters：  file_size
** Returned value:      zftp_client
*********************************************************************************************************/
int zFTP_change_path(zftp_client *ftp, char *path);

/*********************************************************************************************************
** Function name：      zFTP_download_file()
** Descriptions:        download the file from FTP server
** input parameters：   ftp, path, file_name
** output parameters：  file_size
** Returned value:      zftp_client
*********************************************************************************************************/
int zFTP_download_file(zftp_client *ftp, char *file_name);

/*********************************************************************************************************
** Function name：      zFTP_upload_file()
** Descriptions:        upload the file to FTP server
** input parameters：   ftp, path, file_name
** output parameters：  file_size
** Returned value:      zftp_client
*********************************************************************************************************/
int zFTP_upload_file(zftp_client *ftp, char *file_name);

/*********************************************************************************************************
** Function name：      zFTP_quit()
** Descriptions:        ftp logout and realse the zftp_client
** input parameters：   ftp
** output parameters：  None 
** Returned value:      RT_EOK
*********************************************************************************************************/
int zFTP_quit(zftp_client *ftp);


#endif

