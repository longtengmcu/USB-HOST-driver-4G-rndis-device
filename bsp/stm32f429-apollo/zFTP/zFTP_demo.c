/****************************************Copyright (c)****************************************************
**                                       fhqlongteng@163.com                              
**                                       QQ:283669063
**--------------File Info---------------------------------------------------------------------------------
** File name:           zFTP_demo.c
** Last modified Date:  2020-12-01
** Last Version:        V1.00
** Descriptions:        zFTP demo
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
#include "zFTP.h"
#include <fal.h>


#ifdef PKG_USING_ZFTP

#define DBG_ENABLE
#define DBG_SECTION_NAME "zFTP_DEMO"
#define DBG_LEVEL        DBG_INFO
#include <rtdbg.h>


/*********************************************************************************************************
*  　　　　　　定义FTP的默认用户名称，服务器地址，端口等
*********************************************************************************************************/
#define FTP_SVR_ADDR    "www.hgjzn.com" // "172.16.253.184"
#define FTP_SVR_PATH    "LT-VC12"
#define FTP_USER_NAME   "123156"
#define FTP_PASSWORD    "1231321"
#define FTP_FILENAME    "LT-VC12_V10.bin"
#define FIRMWARE_FLASH_PARTITION  "firmware"
#define FIRMWARE_STORE_ADDR        0


/*********************************************************************************************************
** Function name：      file_read()
** Descriptions:        read file from flash 
** input parameters：   ftp, buf, len, file_pos, total_len
** output parameters：  None
** Returned value:      RT_OK, RT_ERROR
*********************************************************************************************************/
int file_read(void * handle, char *file_name, rt_uint8_t *buf, rt_uint32_t len, rt_uint32_t file_pos, rt_uint32_t *read_len, rt_uint32_t *total_len)
{
    const struct fal_partition *partition = RT_NULL;

    *total_len = 600 * 1024;

    partition = fal_partition_find(FIRMWARE_FLASH_PARTITION);
    if (partition == RT_NULL)
    {
        LOG_E("Find partition (%s) failed, Cannot save the net para!", FIRMWARE_FLASH_PARTITION);
        return -RT_ERROR;  
    }

    if(file_pos + len <= *total_len)
    {
        *read_len = len;
    }
    else
    {
        *read_len = *total_len - file_pos;
    }

    if(*read_len != fal_partition_read(partition, FIRMWARE_STORE_ADDR + file_pos, buf, *read_len))
    {
        LOG_E("write file error, file_pos = %d", file_pos); 
    }


    return RT_EOK;


}     



/*********************************************************************************************************
** Function name：      file_write()
** Descriptions:        write file into flash 
** input parameters：   ftp, buf, len, file_pos, total_len
** output parameters：  None
** Returned value:      RT_OK, RT_ERROR
*********************************************************************************************************/
int file_write(void * handle, char *file_name, rt_uint8_t *buf, rt_uint32_t len, rt_uint32_t file_pos, rt_uint32_t total_len)
{
    const struct fal_partition *partition = RT_NULL;

    partition = fal_partition_find(FIRMWARE_FLASH_PARTITION);
    if (partition == RT_NULL)
    {
        LOG_E("Find partition (%s) failed, Cannot save the net para!", FIRMWARE_FLASH_PARTITION);
        return -RT_ERROR;  
    }

    if(len != fal_partition_write(partition, FIRMWARE_STORE_ADDR + file_pos, buf, len))
    {
        LOG_E("write file error, file_pos = %d", file_pos); 
    }


    return RT_EOK;


}

/*********************************************************************************************************
** Function name：      file_write()
** Descriptions:        write file into flash 
** input parameters：   ftp, buf, len, file_pos, total_len
** output parameters：  None
** Returned value:      RT_OK, RT_ERROR
*********************************************************************************************************/
int file_erase(rt_uint32_t file_len)
{
    const struct fal_partition *partition = RT_NULL;

    partition = fal_partition_find(FIRMWARE_FLASH_PARTITION);
    if (partition == RT_NULL)
    {
        LOG_E("Find partition (%s) failed, Cannot save the net para!", FIRMWARE_FLASH_PARTITION);
        return -RT_ERROR;  
    }

    if(file_len != fal_partition_erase(partition, FIRMWARE_STORE_ADDR, file_len))
    {
        LOG_E("erase flash error"); 
    }


    return RT_EOK;


}


/*********************************************************************************************************
** Function name：      cmd_ftp_download()
** Descriptions:        ftp download command line
** input parameters：   argc, argv
** output parameters：  None
** Returned value:      RT_OK, RT_ERROR
*********************************************************************************************************/
int cmd_ftp_download(int argc, char **argv)
{
    zftp_client *ftp;
    rt_uint32_t file_size = 0;

    
    
    ftp = zFTP_login(FTP_USER_NAME, FTP_PASSWORD, FTP_SVR_ADDR);

    if(ftp == RT_NULL)
    {
        rt_kprintf("zFTP login fail!\n");
    } 
    else
    {
        zFTP_set_callback(ftp, file_write, file_read);

        zFTP_change_path(ftp, FTP_SVR_PATH);

        if(rt_strcmp(argv[2], "upload") == 0)
        {
            if(RT_EOK == zFTP_upload_file(ftp, argv[1]))
            {
                rt_kprintf("zFTP upload success!\n");
            }
            else
            {
                rt_kprintf("zFTP upload  %s faile!\n", argv[1]);
            }  
        }
        else
        {
            if(RT_EOK == zFTP_get_filesize(ftp, argv[1], &file_size))
            {
              rt_kprintf("zFTP file %s size:%d!\n", argv[1], file_size);
              file_erase(file_size);

              if(RT_EOK == zFTP_download_file(ftp, argv[1]))
              {
                  rt_kprintf("zFTP download success len: %d!\n", file_size);
              }
                    
            }
        }
        
        zFTP_quit(ftp);

    }  
    
    return 0;
}

#ifdef FINSH_USING_MSH
#include <finsh.h>
MSH_CMD_EXPORT_ALIAS(cmd_ftp_download ,ftp,  ftp download file);
#endif


/*********************************************************************************************************
**   End of file
*********************************************************************************************************/
#endif

