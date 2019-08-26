#ifndef __NETSTACK_NET_H__
#define __NETSTACK_NET_H__

#include "comm.h"

struct netstack_local{
	uint8_t mac_addr[6];				//本地MAC地址
	uint8_t pass[6];						//密匙
	uint8_t company_id[2];        //公司id
	uint8_t channel;						//信道
	uint8_t device_id[3];				//组ID
	uint8_t seq;								//序列号
	uint8_t nid;								//网络ID
	uint8_t net_sta;						//网络状态
	uint8_t dst[2];
	uint8_t have_dat;						//有数据
	uint8_t sn;
	uint8_t nesn;
	uint8_t pair_time;
	uint8_t flash_crc_val;
};


struct netstack_rx_flag{
	uint8_t src[2];
	uint8_t seq;
};

uint8_t net_init(void);
void net_set_mac_addr(const uint8_t *addr);
uint8_t net_input(uint8_t *src_addr,NET_PDU *net_pdu,uint8_t len);
void net_updat_local_info(void);
#endif




