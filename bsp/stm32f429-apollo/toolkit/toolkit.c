/****************************************Copyright (c)****************************************************
**                            Beijing  LongTeng electronics Co.,LTD.
**
**                                 QQ:283669063
**                                 MOBILE:13810794827
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           toolkit.c
** Last modified Date:  2017-07-03
** Last Version:        V1.00
** Descriptions:        工具箱函数
**
**--------------------------------------------------------------------------------------------------------
** Created by:          Zhao shimin
** Created date:        2017-07-03
** Version:             V1.00
** Descriptions:        创建程序
**
**--------------------------------------------------------------------------------------------------------
** Modified by:         Zhao shimin
** Modified date:      
** Version:             
** Descriptions:        
*********************************************************************************************************/
#include <rtthread.h>
#include "toolkit.h"

static const rt_uint8_t aucCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40
};

static const rt_uint8_t aucCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 
    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80, 0x40
};

/*********************************************************************************************************
** Function name：      checkCrc16
** Descriptions:        check crc16 compute 
** input parameters：   crc_init, pucFrame, usLen
** output parameters：  None
** Returned value:      CRC16 value
*********************************************************************************************************/
rt_uint16_t checkCrc16(rt_uint16_t crc_init, rt_uint8_t * pucFrame, rt_uint16_t usLen )
{
    rt_uint8_t           ucCRCHi = crc_init >> 8;
    rt_uint8_t           ucCRCLo = crc_init & 0xFF;
    rt_uint8_t           iIndex = 0;

    while( usLen-- )
    {
        iIndex = ucCRCLo ^ *( pucFrame++ );
        ucCRCLo = (rt_uint8_t)( ucCRCHi ^ aucCRCHi[iIndex] );
        ucCRCHi = aucCRCLo[iIndex];
    }
    return (rt_uint16_t)(ucCRCHi << 8 | ucCRCLo);
}

/********************************************************************
* checkSum - 计算checkXor
*
* PARAMETERS: As follow:
*   pBuf    输入指针
*   len     输入长度
* RETURNS: 
*   异或和
* EXAMPLE:
* ...
**********************************************************************/
rt_uint8_t checkXor(const rt_uint8_t *pBuf, rt_uint32_t len)
{
    rt_uint8_t crc=0;
    rt_uint32_t i = 0;
        
    for( i=0; i<len; i++)
    {
        crc ^= pBuf[i];
    }
    return crc;
}


/********************************************************************
* checkSum - 计算checksum
*
* PARAMETERS: As follow:
*   pBuf    输入指针
*   len     输入长度
* RETURNS: 
*   累加和
* EXAMPLE:
* ...
*/
/*lint -save -e818 -e661 -e662*/    
rt_uint8_t checkSum(const rt_uint8_t *pBuf, rt_uint32_t len)
{
    rt_uint8_t crc=0;
    rt_uint32_t i = 0;
        
    for( i=0; i<len; i++)
    {
        crc += pBuf[i];
    }
    return crc;
}
/*lint -restore*/

/********************************************************************
* checkCrc - 计算checkCrc
*
* PARAMETERS: As follow:
*   pBuf    输入指针
*   len     输入长度
* RETURNS: 
*   crc
* EXAMPLE:
* ...
******************************************************************/
rt_uint8_t checkCrc(const rt_uint8_t *pBuf, rt_uint32_t len)
{
    rt_uint8_t crc=0;
    rt_uint32_t i,j;
    for( i=0; i<len; i++)
    {
        crc ^= pBuf[i];
        for( j = 0; j < 8; j++ )
        {           
            if(crc & 0x80)
            {
                crc = (crc << 1) ^ 7;
            }
            else
            {
                crc = crc << 1;
            }
        }
    }
    return crc;
}

/*********************************************************************************************************
** Function name：      hex_data_print()
** Descriptions:        dump hex format data to sonsole device
** input parameters：   无
** output parameters：  无
** Returned value:      无
*********************************************************************************************************/
void hex_data_print(const char *name, const rt_uint8_t *buf, rt_size_t size)
{
#define __is_print(ch)       ((unsigned int)((ch) - ' ') < 127u - ' ')
#define WIDTH_SIZE           32

    rt_size_t i, j;
    rt_tick_t    tick = 0;
    rt_uint32_t  time = 0;
    
    tick = rt_tick_get();
    time = (tick* 1000) / RT_TICK_PER_SECOND;
    
    rt_kprintf("%s time=%d.%ds,len = %d\n", name, time/1000, time%1000, size);

    for (i = 0; i < size; i += WIDTH_SIZE)
    {
        rt_kprintf("[HEX] %s: %04X-%04X: ", name, i, i + WIDTH_SIZE);
        for (j = 0; j < WIDTH_SIZE; j++)
        {
            if (i + j < size)
            {
                rt_kprintf("%02X ", buf[i + j]);
            }
            else
            {
                rt_kprintf("   ");
            }
            if ((j + 1) % 8 == 0)
            {
                rt_kprintf(" ");
            }
        }
        rt_kprintf("  ");
        for (j = 0; j < WIDTH_SIZE; j++)
        {
            if (i + j < size)
            {
                rt_kprintf("%c", __is_print(buf[i + j]) ? buf[i + j] : '.');
            }
        }
        rt_kprintf("\n");
    }
}

/*判断一个字符是不是数字字符或大小写的a~f*/
int HEX_Determine(char data)
{
	if((data<='9')&&(data>='0'))
	{
		return 0;
	}
	if((data<='f')&&(data>='a'))
	{
		return 0;
	}
	if((data<='F')&&(data>='A'))
	{
		return 0;
	}
	return -1;
	
}
/*把一个十六进制字符变成十六进制数(4bit),
调用前应该先调用hex_determine判断是否为十六进制的字符
*/
int Char2Hex(char data)
{
	if((data<='9')&&(data>='0'))
	{
		return (uint8_t)(data-'0');
	}
	if((data<='f')&&(data>='a'))
	{
		return (uint8_t)(10+(data-'a'));
	}
	if((data<='F')&&(data>='A'))
	{
		return (uint8_t)(10+(data-'A'));
	}
	return -1;
}

/*********************************************************************************************************
** Function name：      Parameter_dec_decode()
** Descriptions:        dec parameter string decode dec number, it use comma separated
**                      eg:1,2,3,4,5
** input parameters：   argv
** output parameters：  pData
** Returned value:      无
*********************************************************************************************************/
int Parameter_dec_decode(char* argv, rt_uint32_t *pData, rt_uint32_t pDataSize,rt_uint32_t *pDatalen)
{
	rt_uint8_t i = 0;
	char      *pHead=NULL;
	rt_uint32_t  addr=0;
	rt_uint8_t tempdata=0;

	/*check para*/
	if((argv==NULL) || (pData==NULL) || (pDatalen==NULL))
	{
		return -RT_ERROR;
	}
	
	
	/*n+1逗号就有n个参数","*/	
	pHead=argv;
	while(*pHead!='\0')
	{
		if((*pHead==',')&&((*(pHead+1)==',')||((*(pHead+1)=='\0'))))
		{
			return -RT_ERROR;
		}
		if(*pHead==',')
		{
			pHead++;
			tempdata++;
			continue;
		}

		if((*pHead < '0') || (*pHead > '9'))
		{
            return -RT_ERROR;
		}
		pHead++;
	}

	/*decode the dec para*/
	pHead=argv;
	*pDatalen=tempdata+1;

    if(pDataSize < (tempdata + 1))
    {
        return -RT_ERROR;    
    }
	
	for(i=0; i<= tempdata; i++)
	{
		addr = 0;
		while((*pHead!=',')&&(*pHead!='\0'))
		{
		    addr = addr * 10 + (*pHead - '0');
			pHead++;
		}
		pHead++;
		*(pData+i)=addr;
	}
	
	return RT_EOK;
}


/*********************************************************************************************************
** Function name：      Parameter_dec_decode_with_hyphen()
** Descriptions:        dec parameter string decode dec number,it use comma or hyphen separated
**                      eg:1,2,3-7,10
** input parameters：   argv
** output parameters：  pData
** Returned value:      无
*********************************************************************************************************/
int Parameter_dec_decode_with_hyphen(char* argv, rt_uint32_t *pData, rt_uint32_t pDataSize,rt_uint32_t *pDatalen)
{
	rt_uint32_t i = 0;
	char         *pHead=NULL;
	rt_uint32_t  addr=0;
	rt_uint8_t   num_find = RT_FALSE, hyphen_find = RT_FALSE;
	rt_uint32_t  first_num = 0, second_num = 0;

	/*check para*/
	if((argv==NULL) || (pData==NULL) || (pDatalen==NULL))
	{
		return -RT_ERROR;
	}
    pHead=argv;
    while(*pHead)
    {
        if(!(((*pHead >= '0') && (*pHead <= '9')) || (*pHead == ',')  || (*pHead == '-'))) 
		{
		    /*volid input, return error */
            return -RT_ERROR;
		}
        pHead++;
    
    }  
	
	
	/*n+1逗号就有n个参数","*/	
	pHead=argv;

	*pDatalen = 0;

    while(1)
    {
        

        if((*pHead >= '0') && (*pHead <= '9'))
		{
            addr = addr * 10 + (*pHead - '0');
            num_find = RT_TRUE;
		}
		else if((*pHead == ',') || (*pHead == 0))
		{
		    if(num_find == RT_TRUE)
		    {
                num_find = RT_FALSE;

                if(hyphen_find == RT_TRUE)
                {
                    hyphen_find = RT_FALSE;
                    second_num = addr;
                    
                    if((first_num <= second_num) && ((*pDatalen + (second_num - first_num) + 1) <= pDataSize))
                    {
                        /* find the input like this 1-3*/
                        for(i = first_num; i < second_num +1; i++)
                        {
                            pData[*pDatalen]  = i;  
                            (*pDatalen)++;    
                        }
                        
                    }
                    else
                    {
                        return -RT_ERROR;
                    }
                    
                }
                else
                {
                
        		    if((*pDatalen + 1) <= pDataSize)
        		    {
                        pData[*pDatalen]  = addr;  
                        (*pDatalen)++;
        		    }
        		    else
        		    {
                        return -RT_ERROR;
        		    }
    		    }
		    }

		    if(*pHead == 0)
		    {
                break;
		    }
		    addr = 0;   
		}
		else if(*pHead == '-')
		{
		    if(num_find == RT_TRUE)
		    {
                num_find = RT_FALSE;
                first_num = addr;
                hyphen_find = RT_TRUE;
		    }
		    addr = 0;   
		}

		pHead++;
		

    }

    return RT_EOK;

}








