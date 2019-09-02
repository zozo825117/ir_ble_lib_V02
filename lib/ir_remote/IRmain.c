/*
 * IRremote
 * Version 0.11 August, 2009
 * Copyright 2009 Ken Shirriff
 * For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
 *
 * Modified by Paul Stoffregen <paul@pjrc.com> to support other boards and timers
 * Modified  by Mitra Ardron <mitra@mitra.biz> 
 * Added Sanyo and Mitsubishi controllers
 * Modified Sony to spot the repeat codes that some Sony's send
 *
 * Interrupt code based on NECIRrcv by Joe Knapp
 * http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
 * Also influenced by http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
 *
 * JVC and Panasonic protocol added by Kristian Lauszus (Thanks to zenwheel and other people at the original blog post)
 */

#include "IRremote.h"
#include "IRremoteInt.h"
#include "IRmain.h"
#include "string.h"

#include "debug_uart.h"


IRrecv irrecv;
IRsend irsend;

decode_results results;
IRdata_t IRdata;

// Storage for the recorded code
int codeType = -1; // The type of code
unsigned long codeValue; // The code value if not raw
unsigned int rawCodes[RAWBUF]; // The durations if raw
int codeLen; // The length of the code
int toggle = 0; // The RC5/6 toggle state

// Stores the code for later playback
// Most of this code is just logging
void storeCode(decode_results *results) {
  int count = results->rawlen;
	int i;
	codeType = results->decode_type;
  if (codeType == UNKNOWN) {
    Debug_Print("Received unknown code, saving as raw");
    codeLen = results->rawlen - 1;
    // To store raw codes:
    // Drop first value (gap)
    // Convert from ticks to microseconds
    // Tweak marks shorter, and spaces longer to cancel out IR receiver distortion
    for (i = 1; i <= codeLen; i++) {
      if (i % 2) {
        // Mark
        rawCodes[i - 1] = results->rawbuf[i]*USECPERTICK - MARK_EXCESS;
        Debug_Print(" m");
      } 
      else {
        // Space
        rawCodes[i - 1] = results->rawbuf[i]*USECPERTICK + MARK_EXCESS;
        Debug_Print(" s");
      }
      Debug_Print( " %d", rawCodes[i - 1]);
    }
    Debug_Print("\r\n");
  }
  else {
    if (codeType == NEC) {
      Debug_Print("Received NEC: ");
      if (results->value == REPEAT) {
        // Don't record a NEC repeat value as that's useless.
        Debug_Print("repeat; ignoring.\r\n");
        return;
      }
    } 
    else if (codeType == SONY) {
      Debug_Print("Received SONY: ");
    } 
    else if (codeType == RC5) {
      Debug_Print("Received RC5: ");
    } 
    else if (codeType == RC6) {
      Debug_Print("Received RC6: ");
    } 
    else {
      Debug_Print("Unexpected codeType %d ", codeType);
      // Debug_Print(codeType, DEC);
      // Debug_Print("");
    }
    Debug_Print("codeValue=%x codeLen=%d\r\n", results->value, results->bits);
    codeValue = results->value;
    codeLen = results->bits;
  }
}

void sendCode(int repeat) {
  if (codeType == NEC) {
    if (repeat) {
      irsend.sendNEC(REPEAT, codeLen);
      Debug_Print("Sent NEC repeat \r\n");
    } 
    else {
      irsend.sendNEC(codeValue, codeLen);
      // Debug_Print("Sent NEC ");
      // Debug_Print(codeValue, HEX);
      Debug_Print("Sent NEC %x\r\n", codeValue);
    }
  } 
  else if (codeType == SONY) {
    irsend.sendSony(codeValue, codeLen);
    // Debug_Print("Sent Sony ");
    // Debug_Print(codeValue, HEX);
    Debug_Print("Sent Sony %x\r\n", codeValue);
  } 
  else if (codeType == RC5 || codeType == RC6) {
    if (!repeat) {
      // Flip the toggle bit for a new button press
      toggle = 1 - toggle;
    }
    // Put the toggle bit into the code to send
    codeValue = codeValue & ~(1 << (codeLen - 1));
    codeValue = codeValue | (toggle << (codeLen - 1));
    if (codeType == RC5) {
      Debug_Print("Sent RC5 %x\r\n", codeValue);
      // Debug_Print(codeValue, HEX);
      irsend.sendRC5(codeValue, codeLen);
    } 
    else {
      irsend.sendRC6(codeValue, codeLen);
      Debug_Print("Sent RC6 %x\r\n", codeValue);
      // Debug_Print(codeValue, HEX);
    }
  } 
  else if (codeType == UNKNOWN /* i.e. raw */) {
    // Assume 38 KHz
    irsend.sendRaw(rawCodes, codeLen, 38);
    Debug_Print("Sent raw\r\n");
  }
}

uint8_t lastIrState;

void IRinit(void){
  GPIO_InitTypeDef GPIO_InitStruct;
    // CMU_APBPeriph1ClockCmd(CMU_APBPeriph1_PORT, ENABLE);
  GPIO_InitStruct.GPIO_Pin = IR_REC_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
  GPIO_Init(IR_REC_PORT, &GPIO_InitStruct);

  IRSetFunction(&irrecv, &irsend);

  irrecv._IRrecv(IR_REC_PIN);

  //test
  irrecv.enableIRIn(); // Re-enable receiver

}

void IRloop(IRdata_t *IRdata) {
  // If button pressed, send the code.
  uint8_t IrState;

    switch(IRdata->cmd){

      case IR_CMD_SEND:
          Debug_Print("Now, sending\r\n");
          IrState = IR_CMD_SEND;

          codeType = IRdata->codeType;
          memcpy((uint8_t*)codeValue, IRdata->codeValue, sizeof(codeValue));
          // codeValue = IRdata->codeValue;
          codeLen = IRdata->codeLen;
          // digitalWrite(STATUS_PIN, HIGH);
          sendCode(lastIrState == IrState);
          // digitalWrite(STATUS_PIN, LOW);
          delayMicroseconds(50); // Wait a bit between retransmissions
        break;

      case IR_CMD_REC:
          Debug_Print("IR_CMD_REC do null\r\n");
        break;

      case IR_CMD_REC_ENTER:
          Debug_Print("IR_CMD_REC_ENTER\r\n");
          irrecv.enableIRIn(); // Re-enable receiver
        break;

      case IR_CMD_REC_EXIT:
        break;

      default:
          IrState = IR_CMD_NULL;
        break;
    } 



  // if (buttonState) {
  //   Debug_Print("Now, sending\r\n");
  //   digitalWrite(STATUS_PIN, HIGH);
  //   sendCode(lastButtonState == buttonState);
  //   digitalWrite(STATUS_PIN, LOW);
  //   delay(50); // Wait a bit between retransmissions
  // } 
  if (irrecv.decode(&results)) {
    // digitalWrite(STATUS_PIN, HIGH);
    storeCode(&results);
    irrecv.resume(); // resume receiver
    // digitalWrite(STATUS_PIN, LOW);
  }
  lastIrState = IrState;
}
