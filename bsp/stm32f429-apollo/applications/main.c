/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-07-29     Arda.Fu      first implementation
 */
#include <rtthread.h>
#include <board.h>
#include <rtdevice.h>

#include <arpa/inet.h>
#include <netdev.h>
#include <fal.h>
#include <string.h>
#include <stdio.h>
#include "mqttclient.h"
#include "OneNet.h"


#if !defined(SAL_USING_POSIX)
#error "Please enable SAL_USING_POSIX!"
#else
#include <sys/time.h>
#include <sys/select.h>
#endif
#include <sys/socket.h> /* 使用BSD socket，需要包含socket.h头文件 */
#include "netdb.h"

#define DEBUG_TCP_SERVER

#define DBG_TAG               "MAIN"
#ifdef DEBUG_TCP_SERVER
#define DBG_LVL               DBG_LOG
#else
#define DBG_LVL               DBG_INFO /* DBG_ERROR */
#endif
#include <rtdbg.h>

/*系统灯*/
#define  SYS_LED_PIN       57
#define BUFSZ             (1024)

#define FS_PARTITION_NAME "firmware"

static int started = 0;
static int is_running = 0;
static int port = 5000;

/*
 * Structure used for manipulating linger option.
 */
static void tcpserv(void *arg)
{
    int ret;
    char *recv_data; /* 用于接收的指针，后面会做一次动态分配以请求可用内存 */
    int sock, connected, bytes_received;
    struct sockaddr_in server_addr, client_addr;

    struct timeval timeout;
    fd_set readset, readset_c;
    

    socklen_t sin_size = sizeof(struct sockaddr_in);



    recv_data = rt_malloc(BUFSZ + 1); /* 分配接收用的数据缓冲 */
    if (recv_data == RT_NULL)
    {
        LOG_E("No memory");
        return;
    }

    /* 一个socket在使用前，需要预先创建出来，指定SOCK_STREAM为TCP的socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        LOG_E("Create socket error");
        goto __exit;
    }

    /* 初始化服务端地址 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port); /* 服务端工作的端口 */
    server_addr.sin_addr.s_addr = INADDR_ANY;
    rt_memset(&(server_addr.sin_zero), 0x0, sizeof(server_addr.sin_zero));

    /* 绑定socket到服务端地址 */
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        LOG_E("Unable to bind");
        goto __exit;
    }

    /* 在socket上进行监听 */
    if (listen(sock, 10) == -1)
    {
        LOG_E("Listen error");
        goto __exit;
    }

    LOG_I("\nTCPServer Waiting for client on port %d...\n", port);

    started = 1;
    is_running = 1;

    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    while (is_running)
    {
        FD_ZERO(&readset);
        FD_SET(sock, &readset);

        LOG_I("Waiting for a new connection...");

        /* Wait for read or write */
        ret = select(sock + 1, &readset, RT_NULL, RT_NULL, &timeout);
        if (ret == 0)
        {
            
            continue;
        }
        else if(ret < 0)
        {
            LOG_I("accept error from (sock = %d)\n", sock);
            rt_thread_mdelay(1000);
            continue;
        }

        /* 接受一个客户端连接socket的请求，这个函数调用是阻塞式的 */
        connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);
        /* 返回的是连接成功的socket */
        if (connected < 0)
        {
            LOG_E("accept connection failed! errno = %d", errno);
            continue;
        }

        /* 接受返回的client_addr指向了客户端的地址信息 */
        LOG_I("I got a connection from (%s , %d)\n",
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));



        /* 客户端连接的处理 */
        while (is_running)
        {
            FD_ZERO(&readset_c);
            FD_SET(connected, &readset_c);

            /* Wait for read or write */
            ret = select(connected + 1, &readset_c, RT_NULL, RT_NULL, NULL);
            if (ret == 0)
            {
                continue;
            }
            else if(ret < 0)
            {
                rt_thread_mdelay(1000);
                LOG_E("recv error from (sock = %d)\n", connected);
                closesocket(connected);
                connected = -1;
                break;  
            }

            /* 从connected socket中接收数据，接收buffer是1024大小，但并不一定能够收到1024大小的数据 */
            bytes_received = recv(connected, recv_data, BUFSZ, 0);
            if (bytes_received <= 0)
            {
                LOG_E("Received error, close the connect.");
                closesocket(connected);
                connected = -1;
                break;
            }
            else
            {
                /* 有接收到数据，把末端清零 */
                recv_data[bytes_received] = '\0';

                if (strcmp(recv_data, "q") == 0 || strcmp(recv_data, "Q") == 0)
                {
                    /* 如果是首字母是q或Q，关闭这个连接 */
                    LOG_I("Got a 'q' or 'Q', close the connect.");
                    closesocket(connected);
                    connected = -1;
                    break;
                }
                else if (strcmp(recv_data, "exit") == 0)
                {
                    /* 如果接收的是exit，则关闭整个服务端 */
                    closesocket(connected);
                    connected = -1;
                    goto __exit;
                }
                else
                {
                    /* 在控制终端显示收到的数据 */
                    //LOG_D("Received data = %s", recv_data);
                }
            }

            /* 把接收到的数据发送回去connected socket */
            ret = send(connected, recv_data, bytes_received, 0);
            if (ret < 0)
            {
                LOG_E("send error, close the connect.");
                closesocket(connected);
                connected = -1;
                break;
            }
            else if (ret == 0)
            {
                /* 打印send函数返回值为0的警告信息 */
                LOG_W("Send warning, send function return 0.");
            }
        }
    }

__exit:
    if (recv_data)
    {
        rt_free(recv_data);
        recv_data = RT_NULL;
    }
    if (connected >= 0)
    {
        closesocket(connected);
        connected = -1;
    }
    if (sock >= 0)
    {
        closesocket(sock);
        sock = -1;
    }
    started = 0;
    is_running = 0;

}

static void usage(void)
{
    rt_kprintf("Usage: tcpserver -p <port>\n");
    rt_kprintf("       tcpserver --stop\n");
    rt_kprintf("       tcpserver --help\n");
    rt_kprintf("\n");
    rt_kprintf("Miscellaneous:\n");
    rt_kprintf("  -p           Specify the host port number\n");
    rt_kprintf("  --stop       Stop tcpserver program\n");
    rt_kprintf("  --help       Print help information\n");
}

static void tcpserver_test(int argc, char** argv)
{
    rt_thread_t tid;

    if (argc == 1 || argc > 3)
    {
        LOG_I("Please check the command you entered!\n");
        goto __usage;
    }
    else
    {
        if (rt_strcmp(argv[1], "--help") == 0)
        {
            goto __usage;
        }
        else if (rt_strcmp(argv[1], "--stop") == 0)
        {
            is_running = 0;
            return;
        }
        else if (rt_strcmp(argv[1], "-p") == 0)
        {
            if (started)
            {
                LOG_I("The tcpclient has started!");
                LOG_I("Please stop tcpclient firstly, by: tcpclient --stop");
                return;
            }

            port = atoi(argv[2]);
        }
        else
        {
            goto __usage;
        }
    }

    tid = rt_thread_create("tcp_serv",
        tcpserv, RT_NULL,
        2048, 5, 20);
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }
    return;

__usage:
    usage();
}

#ifdef RT_USING_FINSH
MSH_CMD_EXPORT_ALIAS(tcpserver_test, tcpserver,
    Start a tcp server. Help: tcpserver --help);
#endif

int netdev_set_status_test(int argc, char **argv)
{
    struct netdev *netdev = RT_NULL;

    if (argc != 3)
    {
        rt_kprintf("netdev_set_status [netdev_name] [up/down]   --set network interface device status.\n");
        return -1;
    }

    /* 通过名称获取网卡对象 */
    netdev = netdev_get_by_name((char *)argv[1]);
    if (netdev == RT_NULL)
    {
        rt_kprintf("input network interface name(%s) error.\n", argv[1]);
        return -1;
    }

    /*  设置网卡状态为 up 或 down */
    if (strcmp(argv[2], "up") == 0)
    {
        netdev_set_up(netdev);
        rt_kprintf("set network interface device(%s) up status.\n", argv[1]);
    }
    else if (strcmp(argv[2], "down") == 0)
    {
        netdev_set_down(netdev);
        rt_kprintf("set network interface device(%s) down status.\n", argv[1]);
    }
    else 
    {
        rt_kprintf("netdev_set_status [netdev_name] [up/down].\n");
        return -1;
    }

    return 0;
}
#ifdef FINSH_USING_MSH
#include <finsh.h>
/* 导出命令到 FinSH 控制台 */
MSH_CMD_EXPORT_ALIAS(netdev_set_status_test, netdev_set_status, set network interface device status);
#endif /* FINSH_USING_MSH */

int netdev_set_dhcp_status_test(int argc, char **argv)
{
    struct netdev *netdev = RT_NULL;

    if (argc != 3)
    {
        rt_kprintf("netdev_set_dhcp_status [netdev_name] [up/down]   --set network interface device dhcp.\n");
        return -1;
    }

    /* 通过名称获取网卡对象 */
    netdev = netdev_get_by_name(argv[1]);
    if (netdev == RT_NULL)
    {
        rt_kprintf("input network interface name(%s) error.\n", argv[1]);
        return -1;
    }

    /*  设置网卡状态为 up 或 down */
    if (strcmp(argv[2], "enable") == 0)
    {
        netdev_dhcp_enabled(netdev, RT_TRUE);
        rt_kprintf("set network interface device(%s) DHCP enable status.\n", argv[1]);
    }
    else if (strcmp(argv[2], "disable") == 0)
    {
        netdev_dhcp_enabled(netdev, RT_FALSE);
        rt_kprintf("set network interface device(%s) DHCP disable status.\n", argv[1]);
    }
    else 
    {
        rt_kprintf("netdev_set_dhcp_status [netdev_name] [enable/disable].\n");
        return -1;
    }

    return 0;
}
#ifdef FINSH_USING_MSH
#include <finsh.h>
/* 导出命令到 FinSH 控制台 */
MSH_CMD_EXPORT_ALIAS(netdev_set_dhcp_status_test, netdev_set_dhcp, set network interface device dhcp status);
#endif /* FINSH_USING_MSH */



int main(void)
{
    struct netdev *netdev = RT_NULL;
    ip_addr_t addr;
    const struct fal_flash_dev *flash_dev = RT_NULL;
    const struct fal_partition *partition = RT_NULL;
    
    char  data_point_buf[50] = {0};
    int ret = 0;
    uint32_t  i = 0;
    uint32_t  send_len = 0;



    ONENET_SESSION *onenet= RT_NULL;
    ONENET_INIT    init;
    char device_id[ONENET_DEVICE_ID_LEN] = {0};
    char api_key[ONENET_API_KEY_LEN] = {0};
    char *access_key = "12315642165";

    

    netdev = netdev_get_by_name("u0");
    if (netdev == RT_NULL)
    {
        rt_kprintf("input network interface name u0 error.\n");
        
    }
    else
    {  

        /*关闭DHCP功能，设备静态IP地址*/
        netdev_dhcp_enabled(netdev, RT_FALSE);

        /* set IP address */
        
        inet_aton("192.168.0.101", &addr);
        netdev_set_ipaddr(netdev, &addr);

        inet_aton("192.168.0.1", &addr);
        netdev_set_gw(netdev, &addr);

        inet_aton("255.255.255.0", &addr);
        netdev_set_netmask(netdev, &addr);

        inet_aton("8.8.8.8", &addr);
        netdev_set_dns_server(netdev, 0, &addr);
    }
    
    
    /* user app entry */
    rt_pin_mode(SYS_LED_PIN, PIN_MODE_OUTPUT);


    /* Fal软件包初始化 */
    fal_init();
    
    partition = fal_partition_find(FS_PARTITION_NAME);
    if (partition == RT_NULL)
    {
        LOG_E("Find partition (%s) failed!", FS_PARTITION_NAME);
        
    }

    flash_dev = fal_flash_device_find(partition->flash_name);
    if (flash_dev == RT_NULL)
    {
        LOG_E("Find flash device (%s) failed!", partition->flash_name);
        
    }
    
    LOG_I("Flash device : %s   "
      "Flash size : %dK   "
      "Partition : %s   "
      "Size: %dKB", 
       partition->flash_name, 
       flash_dev->len/1024,
       partition->name,
       partition->len/1024);

    
      
    
    init.ip = "183.230.40.96";
    init.port = "1883";
    init.device_name = "FHQLONGTENG";
    init.product_id = "354804";
    init.device_id = device_id;
    init.api_key = api_key;
    rt_thread_mdelay(5000);
   
       
    
    for(;;)
    {  
        if(RT_EOK != OneNet_device_register(init.product_id, access_key, init.device_name, device_id, sizeof(device_id), api_key, sizeof(api_key)))
        {
            LOG_E("Can not register device to platform!");
            rt_thread_mdelay(1000);
        }
        else
        {
            break;
        }  
    }

    onenet = OneNet_session_creat(&init);

    for(;;)
    {
        
        if(OneNet_connect(onenet) != RT_EOK)
        {
            rt_thread_mdelay(1000);
            rt_kprintf("mqtt connect fail : %d!\r\n", ret);
        }
        else
        {
            break;
        }
    }    
   

    OneNet_topic_subscribe(onenet, ONENET_TOPIC_DATAPOINT);
    OneNet_topic_subscribe(onenet, ONENET_TOPIC_CMD);

    
    while(1)
    {
        /*延时1秒*/
        rt_pin_write(SYS_LED_PIN, PIN_LOW);
        rt_thread_mdelay(500);
        rt_pin_write(SYS_LED_PIN, PIN_HIGH);
        rt_thread_mdelay(500);

        /*上传数据*/
        send_len = snprintf(data_point_buf, sizeof(data_point_buf), "{\"id\":%d, \"dp\":{\"csq\":[{\"v\":%d}]}}", i, i); 
        i++;
        
        
        OneNet_session_send(onenet, data_point_buf, send_len);
        

        rt_thread_mdelay(59000);
    }    
        

}
