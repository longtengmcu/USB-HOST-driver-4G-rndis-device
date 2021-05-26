# STM32 USB HOST driver 4G rndis device #

It use RT-Thread 3.1.3 operating system, MCU is the stm32F429, the board is the apollo STM32F429 evk. The 4G module model is L501 it designed by the Shang Hai mobiletek.

## Overview ##

The USB host driver framework is modified according to RT thread's USB host framework. The function of enumerating USB manager's combined devices is added. The driver of 4G rndis device is added. The host driver is modified in a large area to increase the stability and reliability of the driver.

## Software function ##

* All functions of rndis protocol are realized
* The driver of 4G rndis device is realized
* Ethernet packet communication based on 4G rndis device
* The virtual serial port driver of 4G rndis device is not driven due to the limitation of the number of host channels. 
* The AT instruction interaction interface of 4G module can be realized through its own hardware serial port.
* In the main function, the IP address of the 4G network card of the device is a fixed IP address, which is related to the 4G module. For example, the IP address of the L501 module is 192.168.1. X network segment.


## Command line ##
* msh />ifconfig
* network interface device: u0 (Default)
* MTU: 1500
* MAC: ac 0c 29 a3 9b 6d 
* FLAGS: UP LINK_UP INTERNET_UP DHCP_DISABLE ETHARP BROADCAST
* ip address: 192.168.0.101
* gw address: 192.168.0.1
* net mask  : 255.255.255.0
* dns server #0: 8.8.8.8
* dns server #1: 0.0.0.0
* msh />ping www.sina.com
* 60 bytes from 111.13.103.37 icmp_seq=0 ttl=56 time=49 ms
* 60 bytes from 111.13.103.37 icmp_seq=1 ttl=56 time=41 ms
* 60 bytes from 111.13.103.37 icmp_seq=2 ttl=56 time=41 ms
* 60 bytes from 111.13.103.37 icmp_seq=3 ttl=56 time=31 ms


## Stability ##
Stable network communication for three days, no more testing


## Future optimized functions ##
* Keep alive timeout, by cutting off the USB host power supply, disconnect the USB device, 
* Add the USB host channel dynamic allocation release function, 
* Add the at command interface initialization 4G module, monitor the 4G module networking operation function
* Optimize USB host protocol framework to support more kinds of USB devices

## reference material
[STM32 USB主机传输中断过程](https://blog.csdn.net/fhqlongteng/article/details/116898748?spm=1001.2014.3001.5501)
[STM32 USB主机通信连接中断过程](https://blog.csdn.net/fhqlongteng/article/details/116897096?spm=1001.2014.3001.5501)

## Author
* fhqlongteng@163.com in Beijing China


