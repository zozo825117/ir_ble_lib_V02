#ifndef __NETSTACK_MAC_H__
#define __NETSTACK_MAC_H__

#include "comm.h"


enum SYNC_WORD{
	SYNC_WORD_BLE=0x00,
	SYNC_WORD_G24=0x01
};


#define HEAD_TYPE_MASK	0x07

#define CHANNEL_MASK		0x03
//head type
typedef enum{
	TYPE_RESERVED,     //ѣ´
	TYPE_UNICAST,     //եҥ
	TYPE_MULTICAST,		//ةҥ
	TYPE_BROADCAST,		//ڣҥ
}PKT_TYPE;


/*
 * Different length in a packet
 */
#define PREAMBLE_LEN            (1)
#define ACCESS_ADDRESS_LEN      (4)
#define PDU_HEADER_LEN          (2)
#define MAX_PDU_PAYLOAD_LEN     (37)
#define CRC_LEN                 (3)
#define MAX_DATA_WHITENING_LEN  (PDU_HEADER_LEN + MAX_PDU_PAYLOAD_LEN + CRC_LEN)
#define DEV_ADDR_LEN            (6)

typedef struct{
	uint8_t oper;
	uint8_t sn:1;
	uint8_t nesn:1;
	uint8_t sm:1;
	uint8_t rfu:5;
}MINOR;

typedef struct{
	uint8_t uuid[16];
	uint8_t major[2];
	MINOR minor;
	uint8_t tx_power;
	uint8_t len;
}NET_PDU;


/* PDU header of packets in ADV channels */
typedef struct _ADV_PDU_HEADER {
    uint8_t rx_add: 1;
    uint8_t tx_add: 1;
    uint8_t rfu0: 2;
    uint8_t type: 4;
    uint8_t length: 6;
    uint8_t rfu1: 2;
} ADV_PDU_HEADER;


/* PDU header fields */
#define PDU_HDR_TXADDR_RANDOM       BIT6
#define PDU_HDR_RXADDR_RANDOM       BIT7
#define PDU_HDR_TYPE_MASK           (0x0F)
#define PDU_HDR_TYPE_ADV_IND        (0)
#define PDU_HDR_TYPE_ADV_DIRECT_IND  (1)
#define PDU_HDR_TYPE_ADV_NONCONN_IND (2)
#define PDU_HDR_TYPE_SCAN_REQ       (3)
#define PDU_HDR_TYPE_SCAN_RSP       (4)
#define PDU_HDR_TYPE_CONN_REQ       (5)
#define PDU_HDR_TYPE_ADV_SCAN_IND   (6)
#define PDU_HDR_LENGTH_MASK         (0x3F)


 #define IBEACON_PDU_LEN     (6 + 27) // 6-byte ADVA, 30-byte IBEACON
 #define EXP_LEN             (1 + IBEACON_PDU_LEN) // 1-byte Length
 


/* ADV_IND PDU */
typedef struct _ADV_IND_PDU {
    union {
        ADV_PDU_HEADER bits;
        uint8_t bytes[2];
    } header;
    uint8_t mac_addr[DEV_ADDR_LEN];
   	uint8_t adv_flag[3];
		uint8_t len;
		uint8_t type;
		uint8_t company_id[2];
		uint8_t beacon_type;
		uint8_t beacon_len;
		uint8_t dat[27];
} ADV_IND_PDU;


/* An entire BLE packet plus certain attributes */
typedef struct _BLE_PKT {
    ADV_IND_PDU adv_ind;
    uint8_t crc[CRC_LEN];
    /* Packet length (from preamble to the end of crc). Not part of a real BLE packet */
    uint8_t len;
		uint8_t rssi;
} BLE_PKT;


uint8_t mac_init(void);
void mac_pack_packet(uint8_t *addr,uint8_t len);
void mac_load_packet(void);
void mac_send_packet(uint8_t channel);
void mac_listen_packet(uint8_t channel);
uint8_t mac_read_packet(void);
uint8_t mac_packet_deal(void);
void mac_process(void);

//uint8_t crc_check(uint8_t *dat);

//void mac_set_updat_model(uint8_t *dst);
void mac_set_power(uint8_t power);

#endif





