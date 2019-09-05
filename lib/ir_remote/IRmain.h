/*
 * IRremote
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.htm http://arcfn.com
 * Edited by Mitra to add new controller SANYO
 *
 * Interrupt code based on NECIRrcv by Joe Knapp
 * http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
 * Also influenced by http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
 *
 * JVC and Panasonic protocol added by Kristian Lauszus (Thanks to zenwheel and other people at the original blog post)
 */

#ifndef IRmain_h
#define IRmain_h


/*控制字*/
#define IR_CMD_NULL         0x00
#define IR_CMD_SEND         0x01
#define IR_CMD_REC          0x02
#define IR_CMD_REC_ENTER    0x03
#define IR_CMD_REC_EXIT     0x04
//test
#define IR_CMD_REC_REPLAY   0x05

typedef struct{
  uint8_t len;
  uint8_t cmd;
  uint8_t codeType;
  uint8_t codeLen;
  uint8_t codeValue[6];
  uint8_t reserved[5];
  uint8_t chkSum;
}IRdata_t;


extern IRdata_t IRdata;

void IRinit(void);
void IRloop(IRdata_t *IRdata);
#endif
