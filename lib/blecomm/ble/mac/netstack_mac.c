#include <string.h>
#include "../../blecomm.h"
#include "../inc/comm.h"
#include "../inc/network_conf.h"
// #include "spi.h"
// #include "delay.h"
// #include "flash.h"


//接收缓存
uint8_t var_rx_buf[VAR_RX_BUF_SIZE];// _at_ VAR_RX_BUF_D;
//发送缓存
uint8_t var_tx_buf[VAR_TX_BUF_SIZE];// _at_ VAR_TX_BUF_D;


#define RF_CHN_TO_PLL_CHN(_x_)  ((_x_) * 2) 

//#define ble_fast_dewhiten   ble_fast_whiten


#define LO(x)	(unsigned char)((x) & 0xff)
#define MID(x)	(unsigned char)(((x) >> 8) & 0xff)
#define HI(x)	(unsigned char)(((x) >> 16) & 0xff)


const uint8_t adv_addr[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};


void net_updat_local_info(void);

/* All BLE channels mapped to PLL ones*/
const uint8_t tpl_channels[] = {
		RF_CHN_TO_PLL_CHN(0),   // CH#37  0   3
    RF_CHN_TO_PLL_CHN(12),	// CH#38  12  25
    RF_CHN_TO_PLL_CHN(39)   // CH#39	39	34
};

const uint8_t tpl_whiten[] = {0xd3,0xb3,0xf3};



const  unsigned long crc24q[256] = {
0x00000000, 0x0000065B, 0x00000CB6, 0x00000AED, 
0x0000196C, 0x00001F37, 0x000015DA, 0x00001381, 
0x000032D8, 0x00003483, 0x00003E6E, 0x00003835, 
0x00002BB4, 0x00002DEF, 0x00002702, 0x00002159, 
0x000065B0, 0x000063EB, 0x00006906, 0x00006F5D, 
0x00007CDC, 0x00007A87, 0x0000706A, 0x00007631, 
0x00005768, 0x00005133, 0x00005BDE, 0x00005D85, 
0x00004E04, 0x0000485F, 0x000042B2, 0x000044E9, 
0x0000CB60, 0x0000CD3B, 0x0000C7D6, 0x0000C18D, 
0x0000D20C, 0x0000D457, 0x0000DEBA, 0x0000D8E1, 
0x0000F9B8, 0x0000FFE3, 0x0000F50E, 0x0000F355, 
0x0000E0D4, 0x0000E68F, 0x0000EC62, 0x0000EA39, 
0x0000AED0, 0x0000A88B, 0x0000A266, 0x0000A43D, 
0x0000B7BC, 0x0000B1E7, 0x0000BB0A, 0x0000BD51, 
0x00009C08, 0x00009A53, 0x000090BE, 0x000096E5, 
0x00008564, 0x0000833F, 0x000089D2, 0x00008F89, 
0x000196C0, 0x0001909B, 0x00019A76, 0x00019C2D, 
0x00018FAC, 0x000189F7, 0x0001831A, 0x00018541, 
0x0001A418, 0x0001A243, 0x0001A8AE, 0x0001AEF5, 
0x0001BD74, 0x0001BB2F, 0x0001B1C2, 0x0001B799, 
0x0001F370, 0x0001F52B, 0x0001FFC6, 0x0001F99D, 
0x0001EA1C, 0x0001EC47, 0x0001E6AA, 0x0001E0F1, 
0x0001C1A8, 0x0001C7F3, 0x0001CD1E, 0x0001CB45, 
0x0001D8C4, 0x0001DE9F, 0x0001D472, 0x0001D229, 
0x00015DA0, 0x00015BFB, 0x00015116, 0x0001574D, 
0x000144CC, 0x00014297, 0x0001487A, 0x00014E21, 
0x00016F78, 0x00016923, 0x000163CE, 0x00016595, 
0x00017614, 0x0001704F, 0x00017AA2, 0x00017CF9, 
0x00013810, 0x00013E4B, 0x000134A6, 0x000132FD, 
0x0001217C, 0x00012727, 0x00012DCA, 0x00012B91, 
0x00010AC8, 0x00010C93, 0x0001067E, 0x00010025, 
0x000113A4, 0x000115FF, 0x00011F12, 0x00011949, 
0x00032D80, 0x00032BDB, 0x00032136, 0x0003276D, 
0x000334EC, 0x000332B7, 0x0003385A, 0x00033E01, 
0x00031F58, 0x00031903, 0x000313EE, 0x000315B5, 
0x00030634, 0x0003006F, 0x00030A82, 0x00030CD9, 
0x00034830, 0x00034E6B, 0x00034486, 0x000342DD, 
0x0003515C, 0x00035707, 0x00035DEA, 0x00035BB1, 
0x00037AE8, 0x00037CB3, 0x0003765E, 0x00037005, 
0x00036384, 0x000365DF, 0x00036F32, 0x00036969, 
0x0003E6E0, 0x0003E0BB, 0x0003EA56, 0x0003EC0D, 
0x0003FF8C, 0x0003F9D7, 0x0003F33A, 0x0003F561, 
0x0003D438, 0x0003D263, 0x0003D88E, 0x0003DED5, 
0x0003CD54, 0x0003CB0F, 0x0003C1E2, 0x0003C7B9, 
0x00038350, 0x0003850B, 0x00038FE6, 0x000389BD, 
0x00039A3C, 0x00039C67, 0x0003968A, 0x000390D1, 
0x0003B188, 0x0003B7D3, 0x0003BD3E, 0x0003BB65, 
0x0003A8E4, 0x0003AEBF, 0x0003A452, 0x0003A209, 
0x0002BB40, 0x0002BD1B, 0x0002B7F6, 0x0002B1AD, 
0x0002A22C, 0x0002A477, 0x0002AE9A, 0x0002A8C1, 
0x00028998, 0x00028FC3, 0x0002852E, 0x00028375, 
0x000290F4, 0x000296AF, 0x00029C42, 0x00029A19, 
0x0002DEF0, 0x0002D8AB, 0x0002D246, 0x0002D41D, 
0x0002C79C, 0x0002C1C7, 0x0002CB2A, 0x0002CD71, 
0x0002EC28, 0x0002EA73, 0x0002E09E, 0x0002E6C5, 
0x0002F544, 0x0002F31F, 0x0002F9F2, 0x0002FFA9, 
0x00027020, 0x0002767B, 0x00027C96, 0x00027ACD, 
0x0002694C, 0x00026F17, 0x000265FA, 0x000263A1, 
0x000242F8, 0x000244A3, 0x00024E4E, 0x00024815, 
0x00025B94, 0x00025DCF, 0x00025722, 0x00025179, 
0x00021590, 0x000213CB, 0x00021926, 0x00021F7D, 
0x00020CFC, 0x00020AA7, 0x0002004A, 0x00020611, 
0x00022748, 0x00022113, 0x00022BFE, 0x00022DA5, 
0x00023E24, 0x0002387F, 0x00023292, 0x000234C9, 

 };

 
void delay_0_5__ms(uint32_t u32Cnt)
{

  uint32_t counter = u32Cnt*delay_cycle;
  while (counter-- > 0) {
    __nop();
  }
}

uint8_t mac_init(void)
{
	return 0;
}

void mac_set_power(uint8_t power)
{
	phy_write_reg(18,power);
}


/**
 * Swap bits between MSB and LSB of a given byte.
 *
 * @param byte The byte to tx_beacon processed.
 *
 * @return The byte with swapped bits.
 */
static uint8_t swap_bits(uint8_t byte)
{
    byte = (byte << 4) | (byte >> 4);
    byte = ((byte << 2) & 0xCC) | ((byte >> 2) & 0x33);
    byte = ((byte << 1) & 0xAA) | ((byte >> 1) & 0x55);

    return (byte);
}

/**
 * Swap bits for 3-byte CRC.
 *
 * @param crc The pointer to a 3-byte CRC that is to tx_beacon bit-swapped.
 */
static void swap_crc_bits(uint8_t *crc)
{
    uint8_t i;
    
    for (i = 0; i < 3; ++i) {
        *crc = swap_bits(*crc);
        crc++;
    }
}

static unsigned char reflected(unsigned char b)    
{      
	unsigned char c = 0;      
	unsigned char i;
	for(i=0; i<8; i++)        
	{          
		c <<= 1;          
		if (b&1) 
			c|=1;          
		b >>= 1;      
	}      
	return c;  
} 


static unsigned long crc24q_hash(unsigned char *dat, int len)
{
    int i;
		unsigned char d;
    unsigned long crc = 0x555555;

    for (i = 0; i < len; i++) {
			d = reflected(dat[i]);
			crc = (crc << 8) ^ crc24q[d ^ (unsigned char)(crc >> 16)];
    }

    crc = (crc & 0x00ffffff);

    return crc;
}


static void ble_calc_adv_crc_tab(const uint8_t *adv_pdu, uint8_t len, uint8_t *crc_c)
{

	unsigned long crc = 0;
    crc = crc24q_hash((uint8_t *)adv_pdu, len);

    crc_c[0] = HI(crc);
    crc_c[1] = MID(crc);
    crc_c[2] = LO(crc);
		swap_crc_bits(crc_c);
	
}

void mac_pack_packet(uint8_t *addr,uint8_t len) 
{
    uint8_t *crc_ptr;
	  BLE_PKT *pkt = (BLE_PKT *)var_tx_buf;
    /* Fill PDU */
		memcpy(pkt->adv_ind.mac_addr,addr,6);
	
			pkt->adv_ind.adv_flag[0] = 0x02;
		pkt->adv_ind.adv_flag[1] = 0x01;
		pkt->adv_ind.adv_flag[2] = 0x06;
		
	  pkt->adv_ind.len = 0x1A;
		pkt->adv_ind.type = 0xFF;
		
		pkt->adv_ind.company_id[0] = local_info.company_id[0];
		pkt->adv_ind.company_id[1] = local_info.company_id[1];
		pkt->adv_ind.beacon_type = 0x02;
		pkt->adv_ind.beacon_len = 0x15;
	
    pkt->adv_ind.header.bytes[0] = PDU_HDR_TYPE_ADV_NONCONN_IND | 0x40;
    pkt->adv_ind.header.bytes[1] = 36;
	

    /* Get the address for CRC */
    crc_ptr = pkt->adv_ind.mac_addr + pkt->adv_ind.header.bytes[1];

    /* Packet length for later use */
		pkt->len = PDU_HEADER_LEN +
            pkt->adv_ind.header.bytes[1] + CRC_LEN ;

    /* Calculate CRC */
						
		ble_calc_adv_crc_tab((uint8_t *)&(pkt->adv_ind),
            PDU_HEADER_LEN + pkt->adv_ind.header.bytes[1], crc_ptr);
}




/**
 * 函数：tpl_rx_mode
 * 功能：设置为接收模式
 * 参数：无
 * 返回：无
 */
static void tpl_rx_mode(uint8_t ch)
{
  phy_write_reg_bit(14,0,0);
  phy_write_reg(15,0x80|ch);
}


/**
 * 函数：tpl_tx_mode
 * 功能：设置为发送模式
 * 参数：无
 * 返回：无
 */
static void tpl_tx_mode(uint8_t ch)
{
  phy_write_reg(15,ch);
	phy_write_reg_bit(14,0,1);
}


/**
 * 功能：加载数据
 * 参数：无
 * 返回：无
 */
void mac_load_packet(void)
{
		BLE_PKT *pkt = (BLE_PKT *)var_tx_buf;
		phy_write_reg_bit(15,7,0);
		phy_write_reg_bit(14,0,0);
		phy_write_reg(104,0x80);//reset fifo wr ptr
		phy_write_reg(105,0x80);// reset fifo rd ptr
		phy_write_reg(95,0x80|pkt->len);//使能数据包长度//写长度
		phy_write_fifo(100,var_tx_buf,pkt->len);//写数据
}

/**
 * 功能：发送数据据
 * 参数：channel---信道
 * 返回：无
 */
void mac_send_packet(uint8_t channel)
{
	phy_write_reg(71,tpl_whiten[channel]);//白化
	phy_write_reg(105,0x80);// reset fifo rd ptr
	tpl_tx_mode(tpl_channels[channel]);
	delay_0_5__ms(1);
	phy_write_reg_bit(14,0,0);//使能发射
	delay_0_5__ms(1);
}


/**
 * 功能：接收监听
 * 参数：channel---信道
 * 返回：无
 */
void mac_listen_packet(uint8_t channel)
{
		phy_write_reg_bit(15,7,0);
		phy_write_reg_bit(14,0,0);
	  phy_write_reg(71,tpl_whiten[channel]);//白化
		phy_write_reg(95,0x80|0x29);//使能数据包长度
		phy_write_reg(104,0x80);//reset fifo wr ptr
		phy_write_reg(105,0x80);// reset fifo rd ptr
		phy_write_reg_bit(37, 1, 1);
    tpl_rx_mode(tpl_channels[channel]);
}

/**
 * 功能：读取接收到的数据
 * 参数：无
 * 返回：无
 */
uint8_t mac_read_packet(void) 
{
	uint8_t i;
	uint8_t crc_buf[3];
	uint8_t *crc_s;
	//uint8_t check=0;
	BLE_PKT *buf = (BLE_PKT *)var_rx_buf;

	i = phy_read_reg(104);
  /* We have received enough data. Stop Rx */
		

	// if(i<=0x2E){
  if(i==0x2A){
		phy_read_fifo(100,var_rx_buf, 41);
//		if(buf->adv_ind.company_id[0] == local_info.company_id[0] && buf->adv_ind.company_id[1] == local_info.company_id[1])
		{

//			phy_read_fifo(100,&buf->adv_ind.dat[0], i-15);
			crc_s = buf->adv_ind.mac_addr + buf->adv_ind.header.bytes[1];
			ble_calc_adv_crc_tab((uint8_t *)&(buf->adv_ind),PDU_HEADER_LEN + buf->adv_ind.header.bytes[1],crc_buf);
			buf->len = buf->adv_ind.header.bytes[1]-13;
		}
//		else{
//			return 0;
//		}

		if(memcmp(crc_buf,crc_s,3)==0)
		{
			return 1;
		}
	}
	
    return (0);
}


/**
 * 功能：接收处理
 * 参数：无
 * 返回：无
 */
uint8_t mac_packet_deal()
{
		BLE_PKT *rx_pkt;
		
		phy_write_reg_bit(37,1,1);

		if(mac_read_packet()){
//      GPIO_ToggleBits(TEST_GPIO_PORT, TEST_GPIO_PORT_PIN);
			rx_pkt = (BLE_PKT *)var_rx_buf;
			net_input(rx_pkt->adv_ind.mac_addr,(NET_PDU *)rx_pkt->adv_ind.dat,rx_pkt->len);
		}
		mac_listen_packet(local_info.channel);//进入接收模式
		return 0;
}


void mac_process(void)
{
		if(phy_read_reg(97)&0x40){
//			GPIO_ToggleBits(TEST_GPIO_PORT, TEST_GPIO_PORT_PIN);
			mac_packet_deal();
		}
}

