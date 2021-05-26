/****************************************Copyright (c)****************************************************
**                                       fhqlongteng@163.com                              
**                                       QQ:283669063
**--------------File Info---------------------------------------------------------------------------------
** File name:           zFTP.c
** Last modified Date:  2020-12-01
** Last Version:        V1.00
** Descriptions:        ZFTP协议实现程序 
**
**--------------------------------------------------------------------------------------------------------
** Created by:          Zhao shimin
** Created date:        2020-12-01
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
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>

#include "zFTP.h"


#ifdef PKG_USING_ZFTP

#define DBG_ENABLE
#define DBG_SECTION_NAME "zFTP"
#define DBG_LEVEL        DBG_INFO
#include <rtdbg.h>

/*********************************************************************************************************
** Function name：      zFTP_wait_server_ack()
** Descriptions:        FTP client wait server ack 
** input parameters：   socket, ack_code, wait_time
** output parameters：  None
** Returned value:      RT_EOK, -RT_ERROR
*********************************************************************************************************/
static int zFTP_wait_server_ack(int socket,  rt_uint32_t *ack_code, rt_uint32_t wait_time)
{
    fd_set readset;
    struct timeval timeout;
    char          ftp_recv_buf[100] = {0};
    int            recv_len = 0;
    int            code= 0;
    
    timeout.tv_sec = wait_time/1000;
    timeout.tv_usec = 0;

    /*wait the ack*/

    FD_ZERO(&readset);
    FD_SET(socket, &readset);

    if (select(socket + 1, &readset, RT_NULL, RT_NULL, &timeout) <= 0) 
    {
        LOG_E("select the socket timeout!");
        return -RT_ERROR;
    }

    // Wait and receive the packet back from the server. If n == -1, it failed.
    recv_len = recv(socket, (char*)ftp_recv_buf, sizeof(ftp_recv_buf), 0);

    if (recv_len < 0) 
    {
        LOG_E("reading from socket error!");
        return -RT_ERROR;
    }
    


    if(1 == sscanf(ftp_recv_buf, "%d", &code))
    {
        *ack_code = code;
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }


}

/*********************************************************************************************************
** Function name：      zFTP_send_cmd_wait_server_ack()
** Descriptions:        FTP client send the command and wait the sever ack
** input parameters：   socket, cmd, cmd_len, ack_code, wait_time
** output parameters：  None
** Returned value:      RT_EOK, -RT_ERROR
*********************************************************************************************************/
static int zFTP_send_cmd_wait_server_ack(int socket, char *cmd, rt_uint32_t cmd_len, 
                                                            rt_uint32_t *ack_code, rt_uint32_t wait_time)
{
    fd_set readset;
    struct timeval timeout;
    char          ftp_recv_buf[100] = {0};
    int            len = 0;
    int            code= 0;

    if((RT_NULL == cmd) || (RT_NULL == ack_code))
    {
        return -RT_ERROR;
    }
    
    timeout.tv_sec  = wait_time/1000;
    timeout.tv_usec = 0;

    FD_ZERO(&readset);
    FD_SET(socket, &readset);

    /*发送指令*/
    len = send(socket, (char*)cmd, cmd_len, 0);

    if (len < 0) {
        LOG_E("send cmd error!");
        return -RT_ERROR;
    }
    
    /*等待应答*/
    if (select(socket + 1, &readset, RT_NULL, RT_NULL, &timeout) <= 0) {
        LOG_E("select the socket timeout!");
        return -RT_ETIMEOUT;
    }

    // Wait and receive the packet back from the server. If n == -1, it failed.
    len = recv(socket, (char*)ftp_recv_buf, sizeof(ftp_recv_buf), 0);

    if (len < 0) {
        LOG_E("reading from socket error!");
        return -RT_ERROR;
    }

    if(1 == sscanf(ftp_recv_buf, "%d", &code))
    {
        *ack_code = code;
        return RT_EOK;
    }
    else
    {
        LOG_I("Rcv data:%s", ftp_recv_buf);
        return -RT_ERROR;
    }

}

/*********************************************************************************************************
** Function name：      zFTP_filesize_send_cmd()
** Descriptions:        zFTP send the get filesize command
** input parameters：   无
** output parameters：  无
** Returned value:      无
*********************************************************************************************************/
static int zFTP_filesize_send_cmd(int socket, char *cmd, rt_uint32_t cmd_len, rt_uint32_t *ack_code,  
                                                rt_uint32_t *file_size, rt_uint32_t wait_time)
{
    fd_set readset;
    struct timeval timeout;
    char           ftp_recv_buf[100] = {0};
    int            len = 0;
    int            code= 0;
    int            a= 0;

    if((RT_NULL == ack_code) || (RT_NULL == file_size) ||
       (RT_NULL ==  cmd))
    {
        return -RT_ERROR;
    }

    timeout.tv_sec = wait_time/1000;
    timeout.tv_usec = 0;

    FD_ZERO(&readset);
    FD_SET(socket, &readset);

    /*发送指令*/
    len = send(socket, cmd, cmd_len, 0);

    if (len < 0) {
        LOG_E("send cmd error!");
        return -RT_ERROR;
    }
    
    /*等待应答*/
    if (select(socket + 1, &readset, RT_NULL, RT_NULL, &timeout) <= 0) {
        LOG_E("select the socket timeout!");
        return -RT_ETIMEOUT;
    }

    // Wait and receive the packet back from the server. If n == -1, it failed.
    len = recv(socket, (char*)ftp_recv_buf, sizeof(ftp_recv_buf), 0);

    if (len < 0) {
        LOG_E("reading from socket error!");
        return -RT_ERROR;
    }
    
    /*获取文件大小和应答码*/
    if(2 == sscanf(ftp_recv_buf, "%d %d", &code, &a))
    {
        *ack_code = code;
        *file_size = a;
        return RT_EOK;
    }
    else
    {
        LOG_E("Rcv data:%s", ftp_recv_buf);
        return -RT_ERROR;
    }


}



/*********************************************************************************************************
** Function name：      ftp_pasv_mode()
** Descriptions:        FTP客户端发送命令给服务器进入pasv模式
** input parameters：   无
** output parameters：  无
** Returned value:      无
*********************************************************************************************************/
static int zFTP_pasv_mode(int socket,rt_uint32_t *ack_code,  rt_uint16_t *server_port, rt_uint32_t wait_time)
{
    fd_set readset;
    struct timeval timeout;
    char           ftp_recv_buf[80] = {0};
    int            len = 0;
    int            code= 0;
    int            a,b,c,d,e,f = 0;

    if((RT_NULL == ack_code) || (RT_NULL == server_port))
    {
        return -RT_ERROR;
    }

    timeout.tv_sec = wait_time/1000;
    timeout.tv_usec = 0;

    FD_ZERO(&readset);
    FD_SET(socket, &readset);

    /*发送指令*/
    len = send(socket, "PASV\r\n", strlen("PASV\r\n"), 0);

    if (len < 0) {
        LOG_E("send cmd error!");
        return -RT_ERROR;
    }
    
    /*等待应答*/
    if (select(socket + 1, &readset, RT_NULL, RT_NULL, &timeout) <= 0) {
        LOG_E("select the socket timeout!");
        return -RT_ETIMEOUT;
    }

    // Wait and receive the packet back from the server. If n == -1, it failed.
    len = recv(socket, (char*)ftp_recv_buf, sizeof(ftp_recv_buf), 0);

    if (len < 0) {
        LOG_E("reading from socket error!");
        return -RT_ERROR;
    }
    
    /*跳过非数字字符来取出IP和端口号*/
    if(7 == sscanf(ftp_recv_buf, "%d%*[^(](%d,%d,%d,%d,%d,%d", &code,&a,&b,&c,&d,&e,&f))
    {
        *ack_code = code;
        *server_port= e *256+f;
        return RT_EOK;
    }
    else
    {
        LOG_I("Rcv data:%s", ftp_recv_buf);
        return -RT_ERROR;
    }


}



/*********************************************************************************************************
** Function name：      zFTP_upload_file_data()
** Descriptions:        zFTP download the file data
** input parameters：   socket, buf, buf_size, wait_time
** output parameters：  rd_len
** Returned value:      RT_EOK, -RT_ERROR
*********************************************************************************************************/
static int zFTP_upload_file_data(zftp_client *ftp, char *file_name, rt_uint32_t wait_time)
{
    struct timeval timeout;
    fd_set   writeset;
    int      socket= -1;
    rt_uint8_t  *file_buf = RT_NULL;
    rt_uint32_t  rd_len = 0, file_pos= 0, total_len = 0;
    
    

    timeout.tv_sec = wait_time/1000;
    timeout.tv_usec = 0;

    
    socket = ftp->data_socket;

    file_buf = rt_malloc(ZFTP_FILE_DATA_BUF_SIZE);
    if(file_buf == RT_NULL)
    {
        LOG_E("Cannot malloc the file data buf!");
        return -RT_ERROR;
    }

    if(ftp->upload_read == RT_NULL)
    {
        return -RT_ERROR;
    }

    while(1)
    {
        if(RT_EOK != ftp->upload_read(ftp, file_name, file_buf, ZFTP_FILE_DATA_BUF_SIZE, file_pos, &rd_len, &total_len))
        {
            rt_free(file_buf);
            return -RT_ERROR;
        }
        FD_ZERO(&writeset);
        FD_SET(socket, &writeset);
        if (select(socket + 1,  RT_NULL, &writeset, RT_NULL, &timeout) <= 0) 
        {
            rt_free(file_buf);
            LOG_E("select data write socket timeout!");
            return -RT_ERROR;
        }

        // Wait and receive the packet back from the server. If n == -1, it failed.
        if(rd_len != send(socket, (char*)file_buf, rd_len, 0))
        {
            rt_free(file_buf);
            LOG_E("reading data socket error!");
            return -RT_ERROR;
        }

        file_pos = file_pos + rd_len;
        
        if(file_pos >= total_len)
        {
            /*upload the file finish!*/
            break;
        }
       
    }

    rt_free(file_buf);
    return RT_EOK;

}



/*********************************************************************************************************
** Function name：      zFTP_download_file_data()
** Descriptions:        zFTP download the file data
** input parameters：   socket, buf, buf_size, wait_time
** output parameters：  rd_len
** Returned value:      RT_EOK, -RT_ERROR
*********************************************************************************************************/
static int zFTP_download_file_data(int socket, rt_uint8_t  *buf, rt_uint32_t buf_size, 
                                    rt_uint32_t *rd_len, rt_uint32_t wait_time)
{
    struct timeval timeout;
    fd_set readset;
    int      recv_len = 0;
    
    FD_ZERO(&readset);

    timeout.tv_sec = wait_time/1000;
    timeout.tv_usec = 0;

    
    FD_SET(socket, &readset);
    if (select(socket + 1, &readset, RT_NULL, RT_NULL, &timeout) <= 0) 
    {
        LOG_E("select data socket timeout!");
        *rd_len = 0;
        return -RT_ERROR;
    }

    // Wait and receive the packet back from the server. If n == -1, it failed.
    recv_len = recv(socket, (char*)buf, buf_size, 0);

    if (recv_len < 0) 
    {
        LOG_E("reading data socket error!");
        return -RT_ERROR;
    }
    else
    {   
        *rd_len = recv_len;
        return RT_EOK;
    }

}

/*********************************************************************************************************
** Function name：      zFTP_download_send_cmd()
** Descriptions:        zFTP send the download file command
** input parameters：   无
** output parameters：  无
** Returned value:      无
*********************************************************************************************************/
static int zFTP_download_send_cmd(int socket, char *cmd, rt_uint32_t cmd_len, rt_uint32_t *ack_code,  
                                                rt_uint32_t *file_size, rt_uint32_t wait_time)
{
    fd_set readset;
    struct timeval timeout;
    char           ftp_recv_buf[100] = {0};
    int            len = 0;
    int            code= 0;
    int            a= 0;

    if((RT_NULL == ack_code) || (RT_NULL == file_size) ||
       (RT_NULL ==  cmd))
    {
        return -RT_ERROR;
    }

    timeout.tv_sec = wait_time/1000;
    timeout.tv_usec = 0;

    FD_ZERO(&readset);
    FD_SET(socket, &readset);

    /*发送指令*/
    len = send(socket, cmd, cmd_len, 0);

    if (len < 0) {
        LOG_E("send cmd error!");
        return -RT_ERROR;
    }
    
    /*等待应答*/
    if (select(socket + 1, &readset, RT_NULL, RT_NULL, &timeout) <= 0) {
        LOG_E("select the socket timeout!");
        return -RT_ETIMEOUT;
    }

    // Wait and receive the packet back from the server. If n == -1, it failed.
    len = recv(socket, (char*)ftp_recv_buf, sizeof(ftp_recv_buf), 0);

    if (len < 0) {
        LOG_E("reading from socket error!");
        return -RT_ERROR;
    }
    
    /*获取文件大小和应答码*/
    if(2 == sscanf(ftp_recv_buf, "%d%*[^(](%d", &code,&a))
    {
        *ack_code = code;
        *file_size = a;
        return RT_EOK;
    }
    else
    {
        LOG_E("Rcv data:%s", ftp_recv_buf);
        return -RT_ERROR;
    }


}


/*********************************************************************************************************
** Function name：      zFTP_quit()
** Descriptions:        ftp logout and realse the zftp_client
** input parameters：   ftp
** output parameters：  None 
** Returned value:      RT_EOK
*********************************************************************************************************/
int zFTP_quit(zftp_client *ftp)
{
    rt_uint32_t  ack_code = 0;
    char        cmd_buf[50] = {0};

    RT_ASSERT(ftp);
    
    /*退出登陆*/
    strcpy(cmd_buf, "QUIT\r\n");
    if((RT_EOK != zFTP_send_cmd_wait_server_ack(ftp->control_socket, cmd_buf, strlen(cmd_buf), &ack_code, ZFTP_CMD_TIMEOUT)) ||
       (ack_code != 221))
    {   
        LOG_E("Server logout error, ack_code:%d", ack_code);
       
    }

    closesocket(ftp->control_socket);
    ftp->control_socket = -1;

    rt_free(ftp);

    return RT_EOK;
}


/*********************************************************************************************************
** Function name：      zFTP_upload_file()
** Descriptions:        upload the file to FTP server
** input parameters：   ftp, path, file_name
** output parameters：  file_size
** Returned value:      zftp_client
*********************************************************************************************************/
int zFTP_upload_file(zftp_client *ftp, char *file_name)
{
    char cmd_buf[50] = {0};
    rt_uint32_t  ack_code = 0;
    int ret = RT_EOK, fd = 0;
    struct addrinfo hints, *addr_list = RT_NULL, *cur = RT_NULL;
    rt_uint16_t    port = 0;
   
    RT_ASSERT(ftp);


    /*Enter the pasv mode */
    if(RT_EOK != zFTP_pasv_mode(ftp->control_socket, &ack_code, &port, ZFTP_CMD_TIMEOUT) || (ack_code != 227))
    {
        LOG_E("Enter pasv mode error, ack_code:%d", ack_code);
        ret = -RT_ERROR;
        goto __exit;
    }


    /*creat the data socket link */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    
    sprintf(ftp->data_port, "%d", port);

    /* Do name resolution with both IPv6 and IPv4 */
    if (getaddrinfo(ftp->server_ip, ftp->data_port, &hints, &addr_list) != RT_EOK) 
    {
        goto __exit;
    }
    
    for (cur = addr_list; cur != NULL; cur = cur->ai_next) 
    {
        fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if (fd < 0) 
        {
            continue;
        }

        if (connect(fd, cur->ai_addr, cur->ai_addrlen) == 0) 
        {
            ftp->data_socket = fd;
            break;
        }

        closesocket(fd);
        
    }
    freeaddrinfo(addr_list);

    if(ftp->data_socket < 0)
    {
        ret = -RT_ERROR;
        goto __exit;
    }

    /*send download the file command*/
    sprintf(cmd_buf, "STOR %s\r\n", file_name);
    if((RT_EOK != zFTP_send_cmd_wait_server_ack(ftp->control_socket, cmd_buf, strlen(cmd_buf), &ack_code, ZFTP_CMD_TIMEOUT)) || 
      (ack_code != 150))
    {   
        LOG_E("upload file error, ack_code:%d", ack_code);
        ret = -RT_ERROR;
        goto __exit;
    }


    /* read the file send to FTP server */
    if(RT_EOK == zFTP_upload_file_data(ftp,  file_name, ZFTP_CMD_TIMEOUT))
    {
        
    }
    else
    {
        ret = -RT_ERROR;
        goto __exit;
    }

    /*close the data socket*/
    if(ftp->data_socket >= 0)
    {
        closesocket(ftp->data_socket);
        ftp->data_socket = -1;
    }
        
    

    /*waite the upload file finish */
    if((RT_EOK != zFTP_wait_server_ack(ftp->control_socket, &ack_code, ZFTP_CMD_TIMEOUT)) || (ack_code != 226))
    {
        LOG_E("Server not ack download complete, ack_code:%d", ack_code); 
    }

    ret = RT_EOK;
    
__exit:
    if(ftp->data_socket >= 0)
    {
        closesocket(ftp->data_socket);
        ftp->data_socket = -1;
    }

    return ret;

}



/*********************************************************************************************************
** Function name：      zFTP_download_file()
** Descriptions:        download the file from FTP server
** input parameters：   ftp, path, file_name
** output parameters：  file_size
** Returned value:      zftp_client
*********************************************************************************************************/
int zFTP_download_file(zftp_client *ftp, char *file_name)
{
    char cmd_buf[50] = {0};
    rt_uint32_t  ack_code = 0;
    int ret = RT_EOK, fd = 0;
    struct addrinfo hints, *addr_list = RT_NULL, *cur = RT_NULL;
    rt_uint32_t   rd_len = 0;
    rt_uint8_t    *file_buf = RT_NULL;
    rt_uint32_t    file_pos = 0;
    rt_uint16_t    port = 0;
    rt_uint32_t    file_size = 0;
    
    RT_ASSERT(ftp);

    file_buf = rt_malloc(ZFTP_FILE_DATA_BUF_SIZE);
    if(file_buf == RT_NULL)
    {
        LOG_E("Cannot malloc the file data buf!");
        return -RT_ERROR;
    }

    /*Enter the pasv mode */
    if(RT_EOK != zFTP_pasv_mode(ftp->control_socket, &ack_code, &port, ZFTP_CMD_TIMEOUT) || (ack_code != 227))
    {
        LOG_E("Enter pasv mode error, ack_code:%d", ack_code);
        ret = -RT_ERROR;
        goto __exit;
    }


    /*creat the data socket link */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    
    sprintf(ftp->data_port, "%d", port);

    /* Do name resolution with both IPv6 and IPv4 */
    if (getaddrinfo(ftp->server_ip, ftp->data_port, &hints, &addr_list) != RT_EOK) 
    {
        goto __exit;
    }
    
    for (cur = addr_list; cur != NULL; cur = cur->ai_next) 
    {
        fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if (fd < 0) 
        {
            continue;
        }

        if (connect(fd, cur->ai_addr, cur->ai_addrlen) == 0) 
        {
            ftp->data_socket = fd;
            break;
        }

        closesocket(fd);
        
    }
    freeaddrinfo(addr_list);

    if(ftp->data_socket < 0)
    {
        ret = -RT_ERROR;
        goto __exit;
    }

    /*send download the file command*/
    sprintf(cmd_buf, "RETR %s\r\n", file_name);
    if((RT_EOK != zFTP_download_send_cmd(ftp->control_socket,  cmd_buf, strlen(cmd_buf), &ack_code, &file_size, ZFTP_CMD_TIMEOUT)) || 
      (ack_code != 150))
    {   
        LOG_E("Download file error, ack_code:%d", ack_code);
        ret = -RT_ERROR;
        goto __exit;
    }

    /* read the file */
    while(file_size > file_pos)
    {
        if(RT_EOK == zFTP_download_file_data(ftp->data_socket, file_buf, ZFTP_FILE_DATA_BUF_SIZE, &rd_len, ZFTP_CMD_TIMEOUT))
        {
            if((rd_len) && (ftp->download_write))
            {
                ftp->download_write(ftp, file_name, file_buf, rd_len, file_pos, file_size);
            }
        }
        else
        {
            ret = -RT_ERROR;
            goto __exit;
        }
        file_pos = file_pos + rd_len;
    }

    /*waite the download file finish */
    if((RT_EOK != zFTP_wait_server_ack(ftp->control_socket, &ack_code, ZFTP_CMD_TIMEOUT)) || (ack_code != 226))
    {
        LOG_E("Server not ack download complete, ack_code:%d", ack_code); 
    }

    ret = RT_EOK;
    
__exit:
    if(ftp->data_socket >= 0)
    {
        closesocket(ftp->data_socket);
        ftp->data_socket = -1;
    }

    if(file_buf)
    {
        rt_free(file_buf);
    }
    
    return ret;

}



/*********************************************************************************************************
** Function name：      zFTP_change_path()
** Descriptions:        change the path of FTP server
** input parameters：   ftp, path, path
** output parameters：  file_size
** Returned value:      zftp_client
*********************************************************************************************************/
int zFTP_change_path(zftp_client *ftp, char *path)
{
    char cmd_buf[50] = {0};
    rt_uint32_t  ack_code = 0;
    
    RT_ASSERT(ftp);
    RT_ASSERT(path);

    /*change path*/

    sprintf(cmd_buf, "CWD %s\r\n", path);
    if((RT_EOK != zFTP_send_cmd_wait_server_ack(ftp->control_socket, cmd_buf, strlen(cmd_buf), &ack_code, ZFTP_CMD_TIMEOUT)) ||
       (ack_code != 250))
    {   
        LOG_E("Switch dir error, ack_code:%d", ack_code);
        return -RT_ERROR;
        
    }
    else
    {
        return RT_EOK;
    }

}



/*********************************************************************************************************
** Function name：      zFTP_get_filesize()
** Descriptions:        get the file size from FTP server
** input parameters：   ftp, path, file_name
** output parameters：  file_size
** Returned value:      zftp_client
*********************************************************************************************************/
int zFTP_get_filesize(zftp_client *ftp, char *file_name, rt_uint32_t *file_size)
{
    char cmd_buf[50] = {0};
    rt_uint32_t  ack_code = 0;
    
    RT_ASSERT(ftp);
    RT_ASSERT(file_name);


    /*get the file size*/
    sprintf(cmd_buf, "SIZE %s\r\n", file_name);
    if((RT_EOK != zFTP_filesize_send_cmd(ftp->control_socket, cmd_buf, strlen(cmd_buf), &ack_code, file_size, ZFTP_CMD_TIMEOUT)) ||
       (ack_code != 213))
    {   
        LOG_E("Get file size error, ack_code:%d", ack_code);
        return  -RT_ERROR;
        
    }
    else
    {
        return RT_EOK;
    }

}

/*********************************************************************************************************
** Function name：      zFTP_set_callback()
** Descriptions:        set the file download and upload callback function
** input parameters：   ftp, dw_write, up_read
** output parameters：  None
** Returned value:      RT_EOK
*********************************************************************************************************/
int zFTP_set_callback(zftp_client *ftp, down_callback  dw_write, up_callback up_read)
{
    RT_ASSERT(ftp);

    ftp->download_write = dw_write;
    ftp->upload_read    = up_read;

    return RT_EOK;
}


/*********************************************************************************************************
** Function name：      zFTP_login()
** Descriptions:        ftp login server
** input parameters：   user_name, password, server_ip
** output parameters：  None 
** Returned value:      zftp_client
*********************************************************************************************************/
zftp_client *zFTP_login(char *user_name, char *password, char *server_ip)
{
    zftp_client  *ftp = RT_NULL;
    int fd = -1;
    rt_uint32_t  ack_code = 0;
    char         cmd_buf[50] = {0};
    struct addrinfo hints, *addr_list = RT_NULL, *cur = RT_NULL;


    if((rt_strlen(user_name) > ZFTP_USER_NAME_LEN) || (rt_strlen(password) > ZFTP_PASSWORD_LEN) ||
       (rt_strlen(server_ip) > ZFTP_SERVER_IP_LEN))
    {
        return RT_NULL;
    }   
    
    /*create a zftp client buf*/
    ftp = rt_malloc(sizeof(zftp_client));

    if(ftp)
    {
        rt_memset(ftp, 0, sizeof(zftp_client));

        ftp->data_socket = -1;
        ftp->control_socket = -1;

        rt_strncpy(ftp->user_name, user_name, rt_strlen(user_name));
        rt_strncpy(ftp->password, password, rt_strlen(password));
        rt_strncpy(ftp->server_ip, server_ip, rt_strlen(server_ip));
        rt_strncpy(ftp->control_port, ZFTP_SERVER_CONTROL_PORT, rt_strlen(ZFTP_SERVER_CONTROL_PORT));
        
        /*creat the control socket link */
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        /* Do name resolution with both IPv6 and IPv4 */
        if (getaddrinfo(ftp->server_ip, ftp->control_port, &hints, &addr_list) != RT_EOK) 
        {
            goto __exit;
        }
        
        for (cur = addr_list; cur != NULL; cur = cur->ai_next) 
        {
            fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
            if (fd < 0) 
            {
                continue;
            }

            if (connect(fd, cur->ai_addr, cur->ai_addrlen) == 0) 
            {
                ftp->control_socket = fd;
                break;
            }

            closesocket(fd);
        }

        freeaddrinfo(addr_list);

        if(ftp->control_socket >= 0)
        {
            /* wait the login success and welcome*/
            if(RT_EOK != zFTP_wait_server_ack(ftp->control_socket, &ack_code, ZFTP_CMD_TIMEOUT) || (ack_code != 220))
            {
                LOG_E("Server not ack welcome, ack_code:%d", ack_code);
                goto __exit;
            }

            /*input the user name */
            sprintf(cmd_buf, "USER %s\r\n", ftp->user_name);
            if((RT_EOK != zFTP_send_cmd_wait_server_ack(ftp->control_socket, cmd_buf, strlen(cmd_buf), &ack_code, ZFTP_CMD_TIMEOUT)) ||
               (ack_code != 331))
            {   
                LOG_E("Login user error, ack_code:%d", ack_code);
                goto __exit;
            }

            /*input the password */
            sprintf(cmd_buf, "PASS %s\r\n", ftp->password);
            if((RT_EOK != zFTP_send_cmd_wait_server_ack(ftp->control_socket, cmd_buf, strlen(cmd_buf), &ack_code, ZFTP_CMD_TIMEOUT)) ||
               (ack_code != 230))
            {   
                LOG_E("Login password error, ack_code:%d", ack_code);
                goto __exit;
            }

            /*change bin mode*/
            strcpy(cmd_buf, "TYPE I\r\n");
            if((RT_EOK != zFTP_send_cmd_wait_server_ack(ftp->control_socket, cmd_buf, strlen(cmd_buf), &ack_code, ZFTP_CMD_TIMEOUT)) ||
               (ack_code != 200))
            {   
                LOG_E("Switch bin mode error, ack_code:%d", ack_code);
                goto __exit;
            }
            
            return ftp;
        }
    }
    else
    {
        LOG_E("Cannot malloc the buf for zftp_client!");
        return RT_NULL;
    }
    
__exit:
    if(ftp->control_socket >= 0)
    {
        closesocket(ftp->control_socket);
    }

    rt_free(ftp);
    ftp = RT_NULL;
    
    return RT_NULL;

}
/*********************************************************************************************************
**   End of file
*********************************************************************************************************/
#endif

