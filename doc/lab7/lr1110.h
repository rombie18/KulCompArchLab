/*
 * lr1110.h
 *
 *  Created on: 24 dec. 2021
 *      Author: robin
 */

#ifndef SRC_RADIO_LR1110_H_
#define SRC_RADIO_LR1110_H_


void lr1110llInit(void);
void lr1110llSetReset(int state);
void lr1110llSetNSS(int state);
int lr1110llGetBusy(void);
int lr1110llGetDIO1(void);
void lr1110llWaitOnBusy(void);
void lr1110Reset(void);
uint8_t lr1110Wakeup(void);

uint8_t lr1110Command(uint16_t cmd, uint8_t * arg, uint16_t arg_size, uint8_t * buffer, uint16_t size);

uint8_t lr1110WriteCommand(uint8_t cmd, uint8_t * buffer, uint16_t size);


#endif /* SRC_RADIO_LR1110_H_ */
