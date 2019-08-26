#ifndef __NETWORK_CONF_H__
#define __NETWORK_CONF_H__

#include "comm.h"

//#include "uart.h"
//网路配置文件
extern struct netstack_local local_info;

#define LOCAL_INFO_ADDR 0x3C00    //本地信息存储地址
#define NETSTACK_GROUP_ADDR			(LOCAL_INFO_ADDR+256)  //组成员地址
#define LOCAL_NAME_ADDR 				(NETSTACK_GROUP_ADDR+128)

#define NETSTACK_GROUP_MAX			15   //最大组数

#define CHIP_ADDR 							0x0100	


//xdata 基地址
#define VAR_BASE_X			0x00

//本地信息结构体变量地址和大小
#define VAR_LOCAL_INFO_X			VAR_BASE_X
#define VAR_LOCAL_INTO_SIZE		0x28



#define VAR_TX_BEACON_OFFSET	 			0x08
//#define VAR_TX_BEACON_SIZE				 0x22

//data 基地址
#define VAR_BASE_D						0x10

//接收缓存地址和大小
#define VAR_RX_BUF_D					VAR_BASE_D
#define VAR_RX_BUF_SIZE				0x3F

//发送缓存地址和大小
#define VAR_TX_BUF_D					(VAR_BASE_D+VAR_RX_BUF_SIZE)
#define VAR_TX_BUF_SIZE				0x3F



#endif



