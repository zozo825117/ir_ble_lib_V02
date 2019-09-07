#include "blecomm.h"
#include "../inc/comm.h"
#include "../inc/netstack_net.h"
#include "../inc/network_conf.h"
// #include "uart.h"
// #include "delay.h"
#include <string.h>
// #include "memory.h"
// #include "flash.h"
#include "../../os/os.h"
// #include "pwm.h"
// #include "system.h"
#include <stdlib.h>
#include "debug_uart.h"


#define	net_encrypt net_decencrypt

struct netstack_local local_info ;



extern uint8_t var_tx_buf[VAR_TX_BUF_SIZE];
const uint8_t null_mac[6] = {0x00,0x00,0x00,0x00,0x00,0x00};

extern const uint8_t adv_addr[];
// extern uint8_t uart_tx_buf[26];
// extern uint8_t uart_rx_buf[26];
// extern uint8_t uart_rx_index;

// uint8_t send_cnt=0;

static uint8_t net_adv(void);
static uint8_t net_tran(uint8_t flag);
static uint8_t net_con_rsp(void);

void task_net(void);



# if 1

/**
 * 功能：解密
 * 参数：解密数据
 */
static void net_decencrypt(NET_PDU *net_pdu)
{
	uint8_t *ptr;
	uint8_t i;
	if(net_pdu->minor.sm == 0)//判读是否加密
		return;
	ptr = net_pdu->uuid;
	for(i=0;i<19;i++){
		*ptr = *ptr^local_info.pass[i%6];
		ptr++;
	}
	
}


// void net_updat_local_info(void)
// {
// 	local_info.flash_crc_val = flash_crc((uint8_t *)&local_info,sizeof(local_info)-1);
// 	flash_erase_section((u16)LOCAL_INFO_ADDR);
// 	flash_write_buf((u16)LOCAL_INFO_ADDR,(uint8_t *)&local_info,sizeof(local_info));
// 	flash_write_buf((u16)(LOCAL_INFO_ADDR+128),(uint8_t *)&local_info,sizeof(local_info));
// }


// uint8_t net_get_local_info(void)
// {
// 	uint8_t crc = 0;
// 	flash_read_buf((u16)LOCAL_INFO_ADDR,(uint8_t *)&local_info,sizeof(struct netstack_local));
// 	crc = flash_crc((uint8_t *)&local_info,sizeof(struct netstack_local)-1);
// 	if(local_info.flash_crc_val == crc){
// 		return 0;
// 	}else{
// 		flash_read_buf((u16)LOCAL_INFO_ADDR+128,(uint8_t *)&local_info,sizeof(struct netstack_local));
// 		crc = flash_crc((uint8_t *)&local_info,sizeof(struct netstack_local)-1);
// 		if(local_info.flash_crc_val == crc){
// 			return 0;
// 		}
// 	}
	
// 	flash_erase_section((u16)LOCAL_INFO_ADDR);
// 	flash_erase_section((u16)LOCAL_INFO_ADDR+128);
	
// 	return 1;
// }

// void net_set_mac_addr(const uint8_t *addr)
// {
// 	memcpy(local_info.mac_addr,addr,6);
// 	net_updat_local_info();
// }

/**
 * 功能：网络层初始化
 */
uint8_t net_init(void)
{
	
	
	// if(net_get_local_info() == 1){
	// 	ResetNoReadOption();
	// }
	
	if(memcmp(null_mac,local_info.mac_addr,6) == 0)
	{//判断是否是第一次运行
//		INSCON = 0x10;
		// flash_read_mac_addr((u16)(CHIP_ADDR+2),local_info.mac_addr);
//		INSCON = 0x00;
		memcpy(local_info.mac_addr,db_mac,6);
	}

		local_info.company_id[0] = 0x4C;
		local_info.company_id[1] = 0x00;

		local_info.channel = 0;
	  local_info.have_dat = 0;
		local_info.pair_time = 0;

	local_info.net_sta = 0;
	local_info.dst[0] = local_info.mac_addr[0];
	local_info.dst[1] = local_info.mac_addr[1];
	
		memcpy(local_info.device_id,local_info.mac_addr,3);
			
	mac_listen_packet(local_info.channel);//进入接收模式
	os_task_add(100,task_net);//添加任务例程
	return 0;
}

/**
 *功能： 数据接收处理
 */
uint8_t net_input(uint8_t *src_addr,NET_PDU *net_pdu,uint8_t len)
{
	static uint8_t last_net_sta;
	//判读是否加密
	net_decencrypt(net_pdu);
	
	if((net_pdu->major[0] != local_info.dst[0] || net_pdu->major[1] != local_info.dst[1]) && memcmp(local_info.mac_addr,net_pdu->uuid,6) != 0)
		return 0;

//	net_pdu->len = len;

	//2 判断是否为控制指令
	Debug_Print("oper=%d\r\n",net_pdu->minor.oper);
	// Debug_Print("net_sta=%d\r\n",local_info.net_sta);
	switch(net_pdu->minor.oper){
		case 0x01://连接请求
			if(local_info.net_sta == 0){
				//发送连接响应
				local_info.net_sta = 1;
				local_info.dst[0] = net_pdu->major[0];
				local_info.dst[1] = net_pdu->major[1];
				memcpy(local_info.pass,&net_pdu->uuid[6],6);
				local_info.sn = 0;
				local_info.nesn = 0;
				local_info.pair_time = 1;
			}
		break;
		case 0x03://断开连接
			if(local_info.net_sta == 2){
					bleState = BLE_STATE_DISCONNECTED;
					cyBle_eventHandlerFlag |= CYBLE_CALLBACK;
					eventCode = BLE_EVT_GAP_DEVICE_DISCONNECTED;
					memcpy((uint8_t *)eventParam, local_info.dst,sizeof(local_info.dst));
				}
			local_info.net_sta = 0;
			

		break;
		case 0x04://数据传输
			/**
		   * 	role		sn      nesn
		   *   m       0       0
		   *   s       0       1
		   *   m       1       1
		   *   s			 1       0
		   *   m			 0			 0
		   *	 s			 0       1
		   *   m       1       1
		   *   s       1       0
		   */
			if(local_info.net_sta != 0){
				local_info.net_sta = 2;
			  if(net_pdu->minor.sn == local_info.sn){
			  	Debug_Print("sn=%d\r\n",local_info.sn);
					local_info.nesn = net_pdu->minor.nesn;
					local_info.sn = local_info.sn?0:1;
					local_info.pair_time = 1;
					if(sending == 1){
						sending++;
					}else if(sending == 2){
								// send_cnt = 0;
								sending = 0;
								// uart_rx_index = 0;
					}
				
					//send receive event
					if(net_pdu->uuid[0]>0){
						cyBle_eventHandlerFlag |= CYBLE_CALLBACK;
						eventCode = BLE_EVT_GATTS_PREP_RECEIVE_DATA;
						memcpy((uint8_t *)eventParam, net_pdu->uuid,net_pdu->uuid[0]+1);
					}
				}
			  if(last_net_sta != local_info.net_sta){
			  	sending = 0;
			  	bleState = BLE_STATE_CONNECTED;
					cyBle_eventHandlerFlag |= CYBLE_CALLBACK;
					eventCode = BLE_EVT_GAP_DEVICE_CONNECTED;
					memcpy((uint8_t *)eventParam, local_info.dst,sizeof(local_info.dst));
				}
			}
			
		break;
	}
	last_net_sta = local_info.net_sta;
	return 0;
}

#endif
uint8_t inde=0;
/**
 * 功能：设备广播
 * 
 */
static uint8_t net_adv(void)
{
	uint8_t i=0;
 	NET_PDU *pdu;
	pdu = (NET_PDU *)&var_tx_buf[17];
	pdu->minor.rfu = local_info.seq++;
	pdu->minor.sn = 0;
	pdu->minor.nesn = 0;
	pdu->minor.sm = 0;
	// pdu->major[0] = 0X00;
	// pdu->major[1] = 0X00;
	pdu->major[0] = 0X53;
	pdu->major[1] = 0X53;
	pdu->minor.oper = 0x00;
	memset(pdu->uuid,0,16);
	memcpy(pdu->uuid,db_name,strlen((char *)db_name));
	memcpy(&pdu->uuid[10],local_info.mac_addr,6);
	if(bleState == BLE_STATE_DISCONNECTED || bleState == BLE_STATE_STOPPED ||
			bleState == BLE_STATE_INITIALIZING){
		sending = 0;
    bleState = BLE_STATE_ADVERTISING;
	  cyBle_eventHandlerFlag |= CYBLE_CALLBACK;
	  eventCode = BLE_EVT_GAPP_ADVERTISEMENT_START_STOP;
	  eventParam[0] = 1;
  }
	mac_pack_packet(local_info.mac_addr,0);
	mac_load_packet();
	for(i=0;i<3;i++){
			mac_send_packet(i);
	}
	mac_listen_packet(local_info.channel);//进入接收模式 
	return 0;
}

/**
 * 功能：连接响应
 * 
 */
static uint8_t net_con_rsp(void)
{
	uint8_t i=0;
	NET_PDU *pdu;
	pdu = (NET_PDU *)&var_tx_buf[17];
	pdu->minor.rfu = local_info.seq++;
	pdu->minor.sn = 0;
	pdu->minor.nesn = 0;
	pdu->minor.sm = 1;
	pdu->major[0] = local_info.dst[0];
	pdu->major[1] = local_info.dst[1];
	pdu->minor.oper = 0x02;
	pdu->len = 0;
	net_encrypt(pdu);
	mac_pack_packet(local_info.mac_addr,0);

	mac_load_packet();
	for(i=0;i<3;i++){
		mac_send_packet(i);
	}
	mac_listen_packet(local_info.channel);//进入接收模式 
	if(local_info.pair_time>=pairTimeOut){
		local_info.net_sta = 0;
		bleState = BLE_STATE_DISCONNECTED;
		cyBle_eventHandlerFlag |= CYBLE_CALLBACK;
		eventCode = BLE_EVT_TIMEOUT;
		memcpy((uint8_t *)eventParam, local_info.dst,sizeof(local_info.dst));
		return 0;
	}
	// if(local_info.pair_time>0){
	// 	local_info.pair_time++;
	// }
	return 0;
}

/**
 * 功能：透传
 * 
 */
static uint8_t net_tran(uint8_t flag)
{
  uint8_t i=0;
  static uint8_t last_sending;
	NET_PDU *pdu;
	pdu = (NET_PDU *)&var_tx_buf[17];
	Debug_Print("time=%d\r\n",local_info.pair_time);
	//判断是否超时
	if(local_info.pair_time>=pairTimeOut){
		local_info.net_sta = 0;
		bleState = BLE_STATE_DISCONNECTED;
		cyBle_eventHandlerFlag |= CYBLE_CALLBACK;
		eventCode = BLE_EVT_TIMEOUT;
		memcpy((uint8_t *)eventParam, local_info.dst,sizeof(local_info.dst));

		return 0;
	}
	// if(local_info.pair_time>0){
	// 	local_info.pair_time++;
	// }
	if(sending == 0 || sending == 1){//发送空包
//	  	pdu->minor.rfu = local_info.seq++;
			pdu->minor.sn = local_info.nesn;
			pdu->minor.nesn = local_info.sn;
		  pdu->minor.sm = 1;
		  pdu->major[0] = local_info.dst[0];
			pdu->major[1] = local_info.dst[1];
		  pdu->minor.oper = 0x04;
		  pdu->uuid[0] = 0;
		  net_encrypt(pdu);
		  mac_pack_packet(local_info.mac_addr,1);
		
	}else if(sending == 2){

			if(last_sending != sending){
				Debug_Print("send data\r\n");
				cyBle_eventHandlerFlag |= CYBLE_CALLBACK;
			  eventCode = BLE_EVT_GATTC_HANDLE_VALUE_NTF;
			  memcpy((uint8_t *)eventParam,send_buf,sizeof(send_buf));
			}

			
			pdu->minor.sn = local_info.nesn;
			pdu->minor.nesn = local_info.sn;
			pdu->minor.sm = 1;
			pdu->major[0] = local_info.dst[0];
			pdu->major[1] = local_info.dst[1];
			pdu->minor.oper = 0x04;
			memcpy(&pdu->uuid[0],send_buf,sizeof(send_buf));
			// send_cnt++;
			net_encrypt(pdu);
			mac_pack_packet(local_info.mac_addr,1);

	}
	mac_load_packet();
	for(i=0;i<6;i++){
		mac_send_packet(i%3);
	}
	mac_listen_packet(local_info.channel);//进入接收模式 
	last_sending = sending;
	return 0;
}

/**
 * 功能：网络层进程
 * 参数：无
 * 返回：无
 */
void task_net(void)
{
	if(local_info.net_sta == 0){
		net_adv();
		os_task_add(12,task_net);
	}
	else if(local_info.net_sta == 1){
		net_con_rsp();
		os_task_add(30,task_net);
	}else if(local_info.net_sta == 2){
		net_tran(local_info.have_dat);
		os_task_add(30,task_net);
	}
	
}






