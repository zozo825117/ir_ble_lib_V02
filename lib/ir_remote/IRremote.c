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

#include "debug_uart.h"
#include "main.h"

// Provides ISR
// #include <avr/interrupt.h>

volatile irparams_t irparams;

// void _IRrecv(int recvpin);
void enableIRIn(void);
void mark(int usec);
/* Leave pin off for time (given in microseconds) */
void space(int usec);
void enableIROut(int khz);

// These versions of MATCH, MATCH_MARK, and MATCH_SPACE are only for debugging.
// To use them, set DEBUG in IRremoteInt.h
// Normally macros are used for efficiency

int MATCH(int measured, int desired) {
#ifdef DEBUG
  Debug_Print("Testing: %d <= %d <= %d \r\n",TICKS_LOW(desired), measured, TICKS_HIGH(desired));
#endif
  return measured >= TICKS_LOW(desired) && measured <= TICKS_HIGH(desired);
}

int MATCH_MARK(int measured_ticks, int desired_us) {
#ifdef DEBUG
  Debug_Print("Testing mark %d vs %d : %d <= %d <= %d \r\n",
    measured_ticks * USECPERTICK,
    desired_us,
    TICKS_LOW(desired_us + MARK_EXCESS),
    measured_ticks,
    TICKS_HIGH(desired_us + MARK_EXCESS));
#endif
  return measured_ticks >= TICKS_LOW(desired_us + MARK_EXCESS) && measured_ticks <= TICKS_HIGH(desired_us + MARK_EXCESS);
}

int MATCH_SPACE(int measured_ticks, int desired_us) {
#ifdef DEBUG
  Debug_Print("Testing space %d vs %d : %d <= %d <= %d \r\n",
    measured_ticks * USECPERTICK,
    desired_us,
    TICKS_LOW(desired_us - MARK_EXCESS),
    measured_ticks,
    TICKS_HIGH(desired_us - MARK_EXCESS));
#endif
  return measured_ticks >= TICKS_LOW(desired_us - MARK_EXCESS) && measured_ticks <= TICKS_HIGH(desired_us - MARK_EXCESS);
}


void sendNEC(unsigned long data, int nbits)
{
	int i;
  enableIROut(38);
  mark(NEC_HDR_MARK);
  space(NEC_HDR_SPACE);
  for (i = 0; i < nbits; i++) {
    if (data & TOPBIT) {
      mark(NEC_BIT_MARK);
      space(NEC_ONE_SPACE);
    } 
    else {
      mark(NEC_BIT_MARK);
      space(NEC_ZERO_SPACE);
    }
    data <<= 1;
  }
  mark(NEC_BIT_MARK);
  space(0);
}

void sendSony(unsigned long data, int nbits) {
	int i;
  enableIROut(40);
  mark(SONY_HDR_MARK);
  space(SONY_HDR_SPACE);
  data = data << (32 - nbits);
  for (i = 0; i < nbits; i++) {
    if (data & TOPBIT) {
      mark(SONY_ONE_MARK);
      space(SONY_HDR_SPACE);
    } 
    else {
      mark(SONY_ZERO_MARK);
      space(SONY_HDR_SPACE);
    }
    data <<= 1;
  }
}

void sendRaw(unsigned int buf[], int len, int hz)
{
	int i;
  enableIROut(hz);
  for (i = 0; i < len; i++) {
    if (i & 1) {
      space(buf[i]);
    } 
    else {
      mark(buf[i]);
    }
  }
  space(0); // Just to be sure
}

// Note: first bit must be a one (start bit)
void sendRC5(unsigned long data, int nbits)
{
	int i;
  enableIROut(36);
  data = data << (32 - nbits);
  mark(RC5_T1); // First start bit
  space(RC5_T1); // Second start bit
  mark(RC5_T1); // Second start bit
  for (i = 0; i < nbits; i++) {
    if (data & TOPBIT) {
      space(RC5_T1); // 1 is space, then mark
      mark(RC5_T1);
    } 
    else {
      mark(RC5_T1);
      space(RC5_T1);
    }
    data <<= 1;
  }
  space(0); // Turn off at end
}

// Caller needs to take care of flipping the toggle bit
void sendRC6(unsigned long data, int nbits)
{
	int i,t;
  enableIROut(36);
  data = data << (32 - nbits);
  mark(RC6_HDR_MARK);
  space(RC6_HDR_SPACE);
  mark(RC6_T1); // start bit
  space(RC6_T1);
  
  for (i = 0; i < nbits; i++) {
    if (i == 3) {
      // double-wide trailer bit
      t = 2 * RC6_T1;
    } 
    else {
      t = RC6_T1;
    }
    if (data & TOPBIT) {
      mark(t);
      space(t);
    } 
    else {
      space(t);
      mark(t);
    }

    data <<= 1;
  }
  space(0); // Turn off at end
}
void sendPanasonic(unsigned int address, unsigned long data) {
	int i;
    enableIROut(35);
    mark(PANASONIC_HDR_MARK);
    space(PANASONIC_HDR_SPACE);
    
    for(i=0;i<16;i++)
    {
        mark(PANASONIC_BIT_MARK);
        if (address & 0x8000) {
            space(PANASONIC_ONE_SPACE);
        } else {
            space(PANASONIC_ZERO_SPACE);
        }
        address <<= 1;        
    }    
    for (i=0; i < 32; i++) {
        mark(PANASONIC_BIT_MARK);
        if (data & TOPBIT) {
            space(PANASONIC_ONE_SPACE);
        } else {
            space(PANASONIC_ZERO_SPACE);
        }
        data <<= 1;
    }
    mark(PANASONIC_BIT_MARK);
    space(0);
}
void sendJVC(unsigned long data, int nbits, int repeat)
{
	int i;
    enableIROut(38);
    data = data << (32 - nbits);
    if (!repeat){
        mark(JVC_HDR_MARK);
        space(JVC_HDR_SPACE); 
    }
    for (i = 0; i < nbits; i++) {
        if (data & TOPBIT) {
            mark(JVC_BIT_MARK);
            space(JVC_ONE_SPACE); 
        } 
        else {
            mark(JVC_BIT_MARK);
            space(JVC_ZERO_SPACE); 
        }
        data <<= 1;
    }
    mark(JVC_BIT_MARK);
    space(0);
}

void mark(int usec) {
  // Sends an IR mark for the specified number of microseconds.
  // The mark output is modulated at the PWM frequency.
  TIMER_ENABLE_PWM; // Enable pin 3 PWM output
  delayMicroseconds(usec);
}

/* Leave pin off for time (given in microseconds) */
void space(int usec) {
  // Sends an IR space for the specified number of microseconds.
  // A space is no output, so the PWM output is disabled.
  TIMER_DISABLE_PWM; // Disable pin 3 PWM output
  delayMicroseconds(usec);
}

void enableIROut(int khz) {
  // Enables IR output.  The khz value controls the modulation frequency in kilohertz.
  // The IR output will be on pin 3 (OC2B).
  // This routine is designed for 36-40KHz; if you use it for other values, it's up to you
  // to make sure it gives reasonable results.  (Watch out for overflow / underflow / rounding.)
  // TIMER2 is used in phase-correct PWM mode, with OCR2A controlling the frequency and OCR2B
  // controlling the duty cycle.
  // There is no prescaling, so the output frequency is 16MHz / (2 * OCR2A)
  // To turn the output on and off, we leave the PWM running, but connect and disconnect the output pin.
  // A few hours staring at the ATmega documentation and this will all make sense.
  // See my Secrets of Arduino PWM at http://arcfn.com/2009/07/secrets-of-arduino-pwm.html for details.

  
  // Disable the Timer2 Interrupt (which is used for receiving IR)
  // TIMER_DISABLE_INTR; //Timer2 Overflow Interrupt

  
  pinMode(TIMER_PWM_PORT, TIMER_PWM_PIN, OUTPUT);
  digitalWrite(TIMER_PWM_PORT, TIMER_PWM_PIN, HIGH); // When not sending PWM, we want it low
  
  // COM2A = 00: disconnect OC2A
  // COM2B = 00: disconnect OC2B; to send signal set to 10: OC2B non-inverted
  // WGM2 = 101: phase-correct PWM with OCRA as top
  // CS2 = 000: no prescaling
  // The top value for the timer.  The modulation frequency will be SYSCLOCK / 2 / OCR2A.
  // TIMER_CONFIG_KHZ(khz);
}

void _IRrecv(int recvpin)
{
  irparams.recvpin = recvpin;
  irparams.blinkflag = 0;
  TIMER_CONFIG_NORMAL();
}

// initialization
void enableIRIn(void) {
  // cli();
  // setup pulse clock timer interrupt
  //Prescale /8 (16M/8 = 0.5 microseconds per tick)
  // Therefore, the timer interval can range from 0.5 to 128 microseconds
  // depending on the reset value (255 to 0)
  // TIMER_CONFIG_NORMAL();

  //Timer2 Overflow Interrupt Enable
  // TIMER_ENABLE_INTR;
  TIMER_ENABLE();

  // TIMER_RESET;

  // sei();  // enable interrupts

  // initialize state machine variables
  irparams.rcvstate = STATE_IDLE;
  irparams.rawlen = 0;

  // set pin modes
  pinMode(IR_REC_PORT, irparams.recvpin, INPUT);
}

void disableIRIn(){
  TIMER_DISABLE();
  irparams.rcvstate = STATE_DISABLE;
}

int isEnableIRIn() {
  return (irparams.rcvstate != STATE_DISABLE);
}

// enable/disable blinking of pin 13 on IR processing
void blink13(int blinkflag)
{
  irparams.blinkflag = blinkflag;
//  if (blinkflag)
//    pinMode(BLINKLED_PORT, BLINKLED, OUTPUT);
}

// TIMER2 interrupt code to collect raw data.
// Widths of alternating SPACE, MARK are recorded in rawbuf.
// Recorded in ticks of 50 microseconds.
// rawlen counts the number of entries recorded so far.
// First entry is the SPACE between transmissions.
// As soon as a SPACE gets long, ready is set, state switches to IDLE, timing of SPACE continues.
// As soon as first MARK arrives, gap width is recorded, ready is cleared, and new logging starts
//ISR(TIMER_INTR_NAME)
void TIMER_INTR_NAME(void)
{
  // uint8_t irdata = (uint8_t)digitalRead(IR_REC_PORT, irparams.recvpin);
  uint8_t irdata;
	
  TIMER_RESET;

  if( ADVTIM_GetITStatus(ADVICE_TIMx, TIM_IT_Update) != RESET ) 
  { 
    // ulIntCntTmp = ADVTIM1->CNT; 
    ADVTIM_ClearITPendingBit(ADVICE_TIMx , TIM_FLAG_Update);
//    ADVTIM_Cmd(ADVICE_TIMx, DISABLE); 
    // ADVTIM_ITConfig(ADVICE_TIMx,TIM_IT_Update,DISABLE);
    TIMER_UPDATA_DISABLE_INTR;
    ADVICE_TIMx->CCER |= (TIM_ICPolarity_Falling<<4);
    irparams.rcvstate = STATE_STOP;
    // ucIntServedFlag = 1;  
   GPIO_ToggleBits(TEST_GPIO_PORT, TEST_GPIO_PORT_PIN);
    // cap_start = 0;
    // cap_cnt = 0;
  } 

	if (ADVTIM_GetITStatus(ADVICE_TIMx, TIM_IT_CC1) != RESET)
  {
			ADVTIM_ClearITPendingBit(ADVICE_TIMx, TIM_IT_CC1);
  }	
  else if (ADVTIM_GetITStatus(ADVICE_TIMx, TIM_IT_CC2) != RESET)
  {
    ADVTIM_ClearITPendingBit(ADVICE_TIMx, TIM_IT_CC2);
    // ucIntCC2Flag = 1;
    // ADVICE_TIMx->CCER ^= (TIM_ICPolarity_Falling<<4);
  //   TOG_TIMER_CAP;
  //   if(cap_start == 0){
  //     CaptureValue = 0;
  //     ulCaptureCC2Old = 0;
  //     TIMER_RESET;
  //     TIMER_UPDATA_ENABLE_INTR;
  //     // ADVICE_TIMx->CNT = 0;
  //     // ADVTIM_ClearITPendingBit(ADVICE_TIMx , TIM_FLAG_Update);
  //     // ADVTIM_ITConfig(ADVICE_TIMx,TIM_IT_Update,ENABLE);
  //     cap_start = 1;
  //   }else{
  //     CaptureValue = ADVTIM_GetCapture2(ADVICE_TIMx);
      
  // //    ulFrequencyCH2 = 24000000/(CaptureValue - ulCaptureCC2Old);
  //     ulFrequencyCH2 = (CaptureValue - ulCaptureCC2Old)*50;
  //     ulCaptureCC2Old = CaptureValue;   
      
  //     CaptureValue_Buf[cap_cnt] = CaptureValue;
  //     ulFrequencyCH2_Buf[cap_cnt] = ulFrequencyCH2;
      

      
  //     cap_cnt++;
  //     if(cap_cnt == sizeof(CaptureValue_Buf)/4){
  //       cap_cnt = 0;
  //     }   
  //   }

    // irparams.timer++; // One more 50us tick
		if(irparams.rcvstate != STATE_DISABLE){
			irdata = GET_TIMER_CAP_INPUT();
			irparams.timer = GET_TIMER_CAP();
			TOG_TIMER_CAP;
		}

		
    if (irparams.rawlen >= RAWBUF) {
      // Buffer overflow
      irparams.rcvstate = STATE_STOP;
    }
    switch(irparams.rcvstate) {
    case STATE_IDLE: // In the middle of a gap
      if (irdata == MARK) {
        if (irparams.timer < GAP_TICKS) {
          // Not big enough to be a gap.
          irparams.timer = 0;
        } 
        else {
          // gap just ended, record duration and start recording transmission
          irparams.rawlen = 0;
          irparams.rawbuf[irparams.rawlen++] = irparams.timer;
          irparams.timer = 0;
          irparams.rcvstate = STATE_MARK;
        }
      }else{
        TIMER_RESET;
        TIMER_UPDATA_ENABLE_INTR;
      }
      break;
    case STATE_MARK: // timing MARK
      if (irdata == SPACE) {   // MARK ended, record time
        irparams.rawbuf[irparams.rawlen++] = irparams.timer;
        irparams.timer = 0;
        irparams.rcvstate = STATE_SPACE;
      }
      break;
    case STATE_SPACE: // timing SPACE
      if (irdata == MARK) { // SPACE just ended, record it
        irparams.rawbuf[irparams.rawlen++] = irparams.timer;
        irparams.timer = 0;
        irparams.rcvstate = STATE_MARK;
      } 
      else { // SPACE
        if (irparams.timer > GAP_TICKS) {
          // big SPACE, indicates gap between codes
          // Mark current code as ready for processing
          // Switch to STOP
          // Don't reset timer; keep counting space width
          irparams.rcvstate = STATE_STOP;
        } 
      }
      break;
    case STATE_STOP: // waiting, measuring gap
      if (irdata == MARK) { // reset gap timer
        irparams.timer = 0;
        //test
          irparams.rcvstate = STATE_IDLE;
					irparams.rawlen = 0;
					ADVICE_TIMx->CCER |= (TIM_ICPolarity_Falling<<4);
      }
      break;
    }

    if (irparams.blinkflag) {
      if (irdata == MARK) {
        BLINKLED_ON();  // turn pin 13 LED on
      } 
      else {
        BLINKLED_OFF();  // turn pin 13 LED off
      }
    }
  }
	
	ADVICE_TIMx->SR = 0;
	
}

void resume() {
  irparams.rcvstate = STATE_IDLE;
  irparams.rawlen = 0;
}


// NECs have a repeat only 4 items long
int decodeNEC(decode_results *results) {
  long data = 0;
  int offset = 0; // Skip first space
	int i;
  // Initial mark
  if (!MATCH_MARK(results->rawbuf[offset], NEC_HDR_MARK)) {
    return ERR;
  }
  offset++;
  // Check for repeat
  if (irparams.rawlen == 4 &&
    MATCH_SPACE(results->rawbuf[offset], NEC_RPT_SPACE) &&
    MATCH_MARK(results->rawbuf[offset+1], NEC_BIT_MARK)) {
    results->bits = 0;
    results->value = REPEAT;
    results->decode_type = NEC;
    return DECODED;
  }
  if (irparams.rawlen < 2 * NEC_BITS + 2) {
    return ERR;
  }
  // Initial space  
  if (!MATCH_SPACE(results->rawbuf[offset], NEC_HDR_SPACE)) {
    return ERR;
  }
  offset++;
  for (i = 0; i < NEC_BITS; i++) {
    if (!MATCH_MARK(results->rawbuf[offset], NEC_BIT_MARK)) {
      return ERR;
    }
    offset++;
    if (MATCH_SPACE(results->rawbuf[offset], NEC_ONE_SPACE)) {
      data = (data << 1) | 1;
    } 
    else if (MATCH_SPACE(results->rawbuf[offset], NEC_ZERO_SPACE)) {
      data <<= 1;
    } 
    else {
      return ERR;
    }
    offset++;
  }
  // Success
  results->bits = NEC_BITS;
  results->value = data;
  results->decode_type = NEC;
  return DECODED;
}

int decodeSony(decode_results *results) {
  long data = 0;
	int offset = 0; // Dont skip first space, check its size
  if (irparams.rawlen < 2 * SONY_BITS + 2) {
    return ERR;
  }
  

  // Some Sony's deliver repeats fast after first
  // unfortunately can't spot difference from of repeat from two fast clicks
  if (results->rawbuf[offset] < SONY_DOUBLE_SPACE_USECS) {
    // Serial.print("IR Gap found: ");
    results->bits = 0;
    results->value = REPEAT;
    results->decode_type = SANYO;
    return DECODED;
  }
  offset++;

  // Initial mark
  if (!MATCH_MARK(results->rawbuf[offset], SONY_HDR_MARK)) {
    return ERR;
  }
  offset++;

  while (offset + 1 < irparams.rawlen) {
    if (!MATCH_SPACE(results->rawbuf[offset], SONY_HDR_SPACE)) {
      break;
    }
    offset++;
    if (MATCH_MARK(results->rawbuf[offset], SONY_ONE_MARK)) {
      data = (data << 1) | 1;
    } 
    else if (MATCH_MARK(results->rawbuf[offset], SONY_ZERO_MARK)) {
      data <<= 1;
    } 
    else {
      return ERR;
    }
    offset++;
  }

  // Success
  results->bits = (offset - 1) / 2;
  if (results->bits < 12) {
    results->bits = 0;
    return ERR;
  }
  results->value = data;
  results->decode_type = SONY;
  return DECODED;
}

// I think this is a Sanyo decoder - serial = SA 8650B
// Looks like Sony except for timings, 48 chars of data and time/space different
int decodeSanyo(decode_results *results) {
  long data = 0;
	int offset = 0; // Skip first space
  if (irparams.rawlen < 2 * SANYO_BITS + 2) {
    return ERR;
  }
  
  // Initial space  
  /* Put this back in for debugging - note can't use #DEBUG as if Debug on we don't see the repeat cos of the delay
  Serial.print("IR Gap: ");
  Debug_Print( results->rawbuf[offset]);
  Debug_Print( "test against:");
  Debug_Print(results->rawbuf[offset]);
  */
  if (results->rawbuf[offset] < SANYO_DOUBLE_SPACE_USECS) {
    // Serial.print("IR Gap found: ");
    results->bits = 0;
    results->value = REPEAT;
    results->decode_type = SANYO;
    return DECODED;
  }
  offset++;

  // Initial mark
  if (!MATCH_MARK(results->rawbuf[offset], SANYO_HDR_MARK)) {
    return ERR;
  }
  offset++;

  // Skip Second Mark
  if (!MATCH_MARK(results->rawbuf[offset], SANYO_HDR_MARK)) {
    return ERR;
  }
  offset++;

  while (offset + 1 < irparams.rawlen) {
    if (!MATCH_SPACE(results->rawbuf[offset], SANYO_HDR_SPACE)) {
      break;
    }
    offset++;
    if (MATCH_MARK(results->rawbuf[offset], SANYO_ONE_MARK)) {
      data = (data << 1) | 1;
    } 
    else if (MATCH_MARK(results->rawbuf[offset], SANYO_ZERO_MARK)) {
      data <<= 1;
    } 
    else {
      return ERR;
    }
    offset++;
  }

  // Success
  results->bits = (offset - 1) / 2;
  if (results->bits < 12) {
    results->bits = 0;
    return ERR;
  }
  results->value = data;
  results->decode_type = SANYO;
  return DECODED;
}

// Looks like Sony except for timings, 48 chars of data and time/space different
int decodeMitsubishi(decode_results *results) {
  // Serial.print("?!? decoding Mitsubishi:");Serial.print(irparams.rawlen); Serial.print(" want "); Serial.println( 2 * MITSUBISHI_BITS + 2);
  long data = 0;
	int offset = 0; // Skip first space
  if (irparams.rawlen < 2 * MITSUBISHI_BITS + 2) {
    return ERR;
  }
  
  // Initial space  
  /* Put this back in for debugging - note can't use #DEBUG as if Debug on we don't see the repeat cos of the delay
  Serial.print("IR Gap: ");
  Serial.println( results->rawbuf[offset]);
  Serial.println( "test against:");
  Serial.println(results->rawbuf[offset]);
  */
  /* Not seeing double keys from Mitsubishi
  if (results->rawbuf[offset] < MITSUBISHI_DOUBLE_SPACE_USECS) {
    // Serial.print("IR Gap found: ");
    results->bits = 0;
    results->value = REPEAT;
    results->decode_type = MITSUBISHI;
    return DECODED;
  }
  */
  offset++;

  // Typical
  // 14200 7 41 7 42 7 42 7 17 7 17 7 18 7 41 7 18 7 17 7 17 7 18 7 41 8 17 7 17 7 18 7 17 7 

  // Initial Space
  if (!MATCH_MARK(results->rawbuf[offset], MITSUBISHI_HDR_SPACE)) {
    return ERR;
  }
  offset++;
  while (offset + 1 < irparams.rawlen) {
    if (MATCH_MARK(results->rawbuf[offset], MITSUBISHI_ONE_MARK)) {
      data = (data << 1) | 1;
    } 
    else if (MATCH_MARK(results->rawbuf[offset], MITSUBISHI_ZERO_MARK)) {
      data <<= 1;
    } 
    else {
      // Serial.println("A"); Serial.println(offset); Serial.println(results->rawbuf[offset]);
      return ERR;
    }
    offset++;
    if (!MATCH_SPACE(results->rawbuf[offset], MITSUBISHI_HDR_SPACE)) {
      // Serial.println("B"); Serial.println(offset); Serial.println(results->rawbuf[offset]);
      break;
    }
    offset++;
  }

  // Success
  results->bits = (offset - 1) / 2;
  if (results->bits < MITSUBISHI_BITS) {
    results->bits = 0;
    return ERR;
  }
  results->value = data;
  results->decode_type = MITSUBISHI;
  return DECODED;
}


// Gets one undecoded level at a time from the raw buffer.
// The RC5/6 decoding is easier if the data is broken into time intervals.
// E.g. if the buffer has MARK for 2 time intervals and SPACE for 1,
// successive calls to getRClevel will return MARK, MARK, SPACE.
// offset and used are updated to keep track of the current position.
// t1 is the time interval for a single bit in microseconds.
// Returns -1 for error (measured time interval is not a multiple of t1).
int getRClevel(decode_results *results, int *offset, int *used, int t1) {

  int width = results->rawbuf[*offset];
  int val = ((*offset) % 2) ? MARK : SPACE;
  int correction = (val == MARK) ? MARK_EXCESS : - MARK_EXCESS;

  int avail;
	if (*offset >= results->rawlen) {
	// After end of recorded buffer, assume SPACE.
	return SPACE;
}
  if (MATCH(width, t1 + correction)) {
    avail = 1;
  } 
  else if (MATCH(width, 2*t1 + correction)) {
    avail = 2;
  } 
  else if (MATCH(width, 3*t1 + correction)) {
    avail = 3;
  } 
  else {
    return -1;
  }

  (*used)++;
  if (*used >= avail) {
    *used = 0;
    (*offset)++;
  }
#ifdef DEBUG
  if (val == MARK) {
    Debug_Print("MARK");
  } 
  else {
    Debug_Print("SPACE");
  }
#endif
  return val;   
}

int decodeRC5(decode_results *results) {
  int offset = 1; // Skip gap space
  long data = 0;
  int used = 0;
	int nbits;
	if (irparams.rawlen < MIN_RC5_SAMPLES + 2) {
	return ERR;
}
  // Get start bits
  if (getRClevel(results, &offset, &used, RC5_T1) != MARK) return ERR;
  if (getRClevel(results, &offset, &used, RC5_T1) != SPACE) return ERR;
  if (getRClevel(results, &offset, &used, RC5_T1) != MARK) return ERR;
  
  for (nbits = 0; offset < irparams.rawlen; nbits++) {
    int levelA = getRClevel(results, &offset, &used, RC5_T1); 
    int levelB = getRClevel(results, &offset, &used, RC5_T1);
    if (levelA == SPACE && levelB == MARK) {
      // 1 bit
      data = (data << 1) | 1;
    } 
    else if (levelA == MARK && levelB == SPACE) {
      // zero bit
      data <<= 1;
    } 
    else {
      return ERR;
    } 
  }

  // Success
  results->bits = nbits;
  results->value = data;
  results->decode_type = RC5;
  return DECODED;
}

int decodeRC6(decode_results *results) {
	long data = 0;
  int used = 0;
	int nbits;
	int offset = 1; // Skip first space
  if (results->rawlen < MIN_RC6_SAMPLES) {
    return ERR;
  }
  
  // Initial mark
  if (!MATCH_MARK(results->rawbuf[offset], RC6_HDR_MARK)) {
    return ERR;
  }
  offset++;
  if (!MATCH_SPACE(results->rawbuf[offset], RC6_HDR_SPACE)) {
    return ERR;
  }
  offset++;

  // Get start bit (1)
  if (getRClevel(results, &offset, &used, RC6_T1) != MARK) return ERR;
  if (getRClevel(results, &offset, &used, RC6_T1) != SPACE) return ERR;
  
  for (nbits = 0; offset < results->rawlen; nbits++) {
    int levelA, levelB; // Next two levels
    levelA = getRClevel(results, &offset, &used, RC6_T1); 
    if (nbits == 3) {
      // T bit is double wide; make sure second half matches
      if (levelA != getRClevel(results, &offset, &used, RC6_T1)) return ERR;
    } 
    levelB = getRClevel(results, &offset, &used, RC6_T1);
    if (nbits == 3) {
      // T bit is double wide; make sure second half matches
      if (levelB != getRClevel(results, &offset, &used, RC6_T1)) return ERR;
    } 
    if (levelA == MARK && levelB == SPACE) { // reversed compared to RC5
      // 1 bit
      data = (data << 1) | 1;
    } 
    else if (levelA == SPACE && levelB == MARK) {
      // zero bit
      data <<= 1;
    } 
    else {
      return ERR; // Error
    } 
  }
  // Success
  results->bits = nbits;
  results->value = data;
  results->decode_type = RC6;
  return DECODED;
}
int decodePanasonic(decode_results *results) {
    unsigned long long data = 0;
    int offset = 1;
		int i;
    
    if (!MATCH_MARK(results->rawbuf[offset], PANASONIC_HDR_MARK)) {
        return ERR;
    }
    offset++;
    if (!MATCH_MARK(results->rawbuf[offset], PANASONIC_HDR_SPACE)) {
        return ERR;
    }
    offset++;
    
    // decode address
    for (i = 0; i < PANASONIC_BITS; i++) {
        if (!MATCH_MARK(results->rawbuf[offset++], PANASONIC_BIT_MARK)) {
            return ERR;
        }
        if (MATCH_SPACE(results->rawbuf[offset],PANASONIC_ONE_SPACE)) {
            data = (data << 1) | 1;
        } else if (MATCH_SPACE(results->rawbuf[offset],PANASONIC_ZERO_SPACE)) {
            data <<= 1;
        } else {
            return ERR;
        }
        offset++;
    }
    results->value = (unsigned long)data;
    results->panasonicAddress = (unsigned int)(data >> 32);
    results->decode_type = PANASONIC;
    results->bits = PANASONIC_BITS;
    return DECODED;
}

int decodeJVC(decode_results *results) {
    long data = 0;
    int offset = 1; // Skip first space
		int i;
    // Check for repeat
    if (irparams.rawlen - 1 == 33 &&
        MATCH_MARK(results->rawbuf[offset], JVC_BIT_MARK) &&
        MATCH_MARK(results->rawbuf[irparams.rawlen-1], JVC_BIT_MARK)) {
        results->bits = 0;
        results->value = REPEAT;
        results->decode_type = JVC;
        return DECODED;
    } 
    // Initial mark
    if (!MATCH_MARK(results->rawbuf[offset], JVC_HDR_MARK)) {
        return ERR;
    }
    offset++; 
    if (irparams.rawlen < 2 * JVC_BITS + 1 ) {
        return ERR;
    }
    // Initial space 
    if (!MATCH_SPACE(results->rawbuf[offset], JVC_HDR_SPACE)) {
        return ERR;
    }
    offset++;
    for (i = 0; i < JVC_BITS; i++) {
        if (!MATCH_MARK(results->rawbuf[offset], JVC_BIT_MARK)) {
            return ERR;
        }
        offset++;
        if (MATCH_SPACE(results->rawbuf[offset], JVC_ONE_SPACE)) {
            data = (data << 1) | 1;
        } 
        else if (MATCH_SPACE(results->rawbuf[offset], JVC_ZERO_SPACE)) {
            data <<= 1;
        } 
        else {
            return ERR;
        }
        offset++;
    }
    //Stop bit
    if (!MATCH_MARK(results->rawbuf[offset], JVC_BIT_MARK)){
        return ERR;
    }
    // Success
    results->bits = JVC_BITS;
    results->value = data;
    results->decode_type = JVC;
    return DECODED;
}

// Decodes the received IR message
// Returns 0 if no data ready, 1 if data ready.
// Results of decoding are stored in results
int decode(decode_results *results) {

  if(!isEnableIRIn())
  {
    return ERR;
  }

  results->rawbuf = irparams.rawbuf;
  results->rawlen = irparams.rawlen;
  if (irparams.rcvstate != STATE_STOP) {
    return ERR;
  }
#ifdef DEBUG
  Debug_Print("Attempting NEC decode");
#endif
  if (decodeNEC(results)) {
    return DECODED;
  }
#ifdef DEBUG
  Debug_Print("Attempting Sony decode");
#endif
  if (decodeSony(results)) {
    return DECODED;
  }
#ifdef DEBUG
  Debug_Print("Attempting Sanyo decode");
#endif
  if (decodeSanyo(results)) {
    return DECODED;
  }
#ifdef DEBUG
  Debug_Print("Attempting Mitsubishi decode");
#endif
  if (decodeMitsubishi(results)) {
    return DECODED;
  }
#ifdef DEBUG
  Debug_Print("Attempting RC5 decode");
#endif  
  if (decodeRC5(results)) {
    return DECODED;
  }
#ifdef DEBUG
  Debug_Print("Attempting RC6 decode");
#endif 
  if (decodeRC6(results)) {
    return DECODED;
  }
#ifdef DEBUG
    Debug_Print("Attempting Panasonic decode");
#endif 
    if (decodePanasonic(results)) {
        return DECODED;
    }
#ifdef DEBUG
    Debug_Print("Attempting JVC decode");
#endif 
    if (decodeJVC(results)) {
        return DECODED;
    }
  // decodeHash returns a hash on any input.
  // Thus, it needs to be last in the list.
  // If you add any decodes, add them before this.
  // if (decodeHash(results)) {
  //   return DECODED;
  // }
  // Throw away and start over
  resume();
  return ERR;
}

/* -----------------------------------------------------------------------
 * hashdecode - decode an arbitrary IR code.
 * Instead of decoding using a standard encoding scheme
 * (e.g. Sony, NEC, RC5), the code is hashed to a 32-bit value.
 *
 * The algorithm: look at the sequence of MARK signals, and see if each one
 * is shorter (0), the same length (1), or longer (2) than the previous.
 * Do the same with the SPACE signals.  Hszh the resulting sequence of 0's,
 * 1's, and 2's to a 32-bit value.  This will give a unique value for each
 * different code (probably), for most code systems.
 *
 * http://arcfn.com/2010/01/using-arbitrary-remotes-with-arduino.html
 */

// Compare two tick values, returning 0 if newval is shorter,
// 1 if newval is equal, and 2 if newval is longer
// Use a tolerance of 20%
// int compare(unsigned int oldval, unsigned int newval) {
//   if (newval < oldval * .8) {
//     return 0;
//   } 
//   else if (oldval < newval * .8) {
//     return 2;
//   } 
//   else {
//     return 1;
//   }
// }

// Use FNV hash algorithm: http://isthe.com/chongo/tech/comp/fnv/#FNV-param
// #define FNV_PRIME_32 16777619
// #define FNV_BASIS_32 2166136261

/* Converts the raw code values into a 32-bit hash code.
 * Hopefully this code is unique for each button.
 * This isn't a "real" decoding, just an arbitrary value.
 */
// int decodeHash(decode_results *results) {
//   long hash = FNV_BASIS_32;
// 	int i;
//   // Require at least 6 samples to prevent triggering on noise
//   if (results->rawlen < 6) {
//     return ERR;
//   }
//   for (i = 1; i+2 < results->rawlen; i++) {
//     int value =  compare(results->rawbuf[i], results->rawbuf[i+2]);
//     // Add value into the hash
//     hash = (hash * FNV_PRIME_32) ^ value;
//   }
//   results->value = hash;
//   results->bits = 32;
//   results->decode_type = UNKNOWN;
//   return DECODED;
// }

/* Sharp and DISH support by Todd Treece ( http://unionbridge.org/design/ircommand )

The Dish send function needs to be repeated 4 times, and the Sharp function
has the necessary repeat built in because of the need to invert the signal.

Sharp protocol documentation:
http://www.sbprojects.com/knowledge/ir/sharp.htm

Here are the LIRC files that I found that seem to match the remote codes
from the oscilloscope:

Sharp LCD TV:
http://lirc.sourceforge.net/remotes/sharp/GA538WJSA

DISH NETWORK (echostar 301):
http://lirc.sourceforge.net/remotes/echostar/301_501_3100_5100_58xx_59xx

For the DISH codes, only send the last for characters of the hex.
i.e. use 0x1C10 instead of 0x0000000000001C10 which is listed in the
linked LIRC file.
*/

void sendSharp(unsigned long data, int nbits) {
  unsigned long invertdata = data ^ SHARP_TOGGLE_MASK;
	int i;
  enableIROut(38);
  for (i = 0; i < nbits; i++) {
    if (data & 0x4000) {
      mark(SHARP_BIT_MARK);
      space(SHARP_ONE_SPACE);
    }
    else {
      mark(SHARP_BIT_MARK);
      space(SHARP_ZERO_SPACE);
    }
    data <<= 1;
  }
  
  mark(SHARP_BIT_MARK);
  space(SHARP_ZERO_SPACE);
  delayMicroseconds(46);
  for (i = 0; i < nbits; i++) {
    if (invertdata & 0x4000) {
      mark(SHARP_BIT_MARK);
      space(SHARP_ONE_SPACE);
    }
    else {
      mark(SHARP_BIT_MARK);
      space(SHARP_ZERO_SPACE);
    }
    invertdata <<= 1;
  }
  mark(SHARP_BIT_MARK);
  space(SHARP_ZERO_SPACE);
  delayMicroseconds(46);
}

void sendDISH(unsigned long data, int nbits)
{
	int i;
  enableIROut(56);
  mark(DISH_HDR_MARK);
  space(DISH_HDR_SPACE);
  for (i = 0; i < nbits; i++) {
    if (data & DISH_TOP_BIT) {
      mark(DISH_BIT_MARK);
      space(DISH_ONE_SPACE);
    }
    else {
      mark(DISH_BIT_MARK);
      space(DISH_ZERO_SPACE);
    }
    data <<= 1;
  }
}

void IRSetFunction(IRrecv *irrecv, IRsend *irsend){

  irrecv->_IRrecv = _IRrecv;
  irrecv->blink13 = blink13;
  irrecv->decode = decode;
  irrecv->enableIRIn = enableIRIn;
  irrecv->resume = resume;
  irrecv->getRClevel = getRClevel;
  irrecv->decodeNEC = decodeNEC;
  irrecv->decodeSony = decodeSony;
  irrecv->decodeSanyo = decodeSanyo;
  irrecv->decodeMitsubishi = decodeMitsubishi;
  irrecv->decodeRC5 = decodeRC5;
  irrecv->decodeRC6 = decodeRC6;
  irrecv->decodePanasonic = decodePanasonic;
  irrecv->decodeJVC = decodeJVC;

  irsend->sendNEC = sendNEC;
  irsend->sendSony = sendSony; 
  irsend->sendRaw = sendRaw; 
  irsend->sendRC5 = sendRC5; 
  irsend->sendRC6 = sendRC6; 
  irsend->sendDISH = sendDISH; 
  irsend->sendSharp = sendSharp; 
  irsend->sendPanasonic = sendPanasonic; 
  irsend->sendJVC = sendJVC; 
  irsend->enableIROut = enableIROut; 
  irsend->mark = mark; 
  irsend->space = space; 
  
}
