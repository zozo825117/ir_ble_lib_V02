#ifndef __NETSTACK_PHY_H__
#define __NETSTACK_PHY_H__

#include "comm.h"

uint8_t phy_init(void);
uint8_t phy_read_reg(uint8_t reg);
void phy_write_reg(uint8_t reg,uint8_t val);
void phy_write_reg_bit(uint8_t reg,uint8_t bits,uint8_t val);
void phy_read_fifo(uint8_t reg, uint8_t *buf, uint8_t len);
void phy_write_fifo(uint8_t reg, uint8_t *buf, uint8_t len);
void phy_set_channel(uint8_t channel);


#endif



