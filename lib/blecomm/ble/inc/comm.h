#ifndef __COMM_H__
#define __COMM_H__

#include "blecomm.h"
#include "netstack_phy.h"
#include "netstack_mac.h"
#include "netstack_net.h"


/***************************************
* Definitions
***************************************/
/* cyBle_eventHandlerFlag defines */
#define CYBLE_CALLBACK                          (0x01u)
#define EVT_PAR_LEN                             (0x20)

/***************************************
* External function references
***************************************/
void CHIP_ID_Get(uint8_t *chipid);

/***************************************
* External data references
***************************************/
extern volatile uint8_t cyBle_eventHandlerFlag;
extern volatile uint8_t eventParam[EVT_PAR_LEN];
extern volatile uint32_t eventCode;
extern uint8_t db_mac[6];
extern uint8_t db_name[16];
extern uint8_t send_buf[16];
extern uint8_t sending;
extern volatile BLE_STATE_T bleState;
extern volatile uint16_t delay_cycle;
extern volatile uint16_t pairTimeOut;

#endif /*__COMM_H__*/
