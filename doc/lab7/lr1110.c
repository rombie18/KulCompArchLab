/*
 * lr1110.c
 *
 *  Created on: 24 dec. 2021
 *      Author: robin
 */

#include <string.h>

#include "main.h"
#include "lr1110.h"

extern SPI_HandleTypeDef hspi1;
/**
 * Init the low level layer of the radio
 */
void lr1110llInit(void){
    lr1110llSetNSS(1);
    lr1110Reset();
    HAL_Delay(250);
}

/**
 * Controls the reset pin of the lr1110
 * @param state
 */
void lr1110llSetReset(int state){
    HAL_GPIO_WritePin(LR_RESET_GPIO_Port, LR_RESET_Pin, state);
}

/**
 * Controls the NSS pin of the lr1110
 * @param state
 */
void lr1110llSetNSS(int state){
    HAL_GPIO_WritePin(LR_NSS_GPIO_Port, LR_NSS_Pin, state);
}

/**
 * Reads the Busy pin of the lr1110
 * @return
 */
int lr1110llGetBusy(void){
   return HAL_GPIO_ReadPin(LR_BUSY_GPIO_Port, LR_BUSY_Pin);
}

/**
 * Reads the Busy pin of the lr1110
 * @return
 */
int lr1110llGetDIO1(void){
   return HAL_GPIO_ReadPin(LR_IRQ_GPIO_Port, LR_IRQ_Pin);
}

/**
 * Blocking wait for the busy pin
 */
void lr1110llWaitOnBusy(void){
    while(lr1110llGetBusy() == 1);
}

/**
 * Generates a reset pulse for the lr1110
 */
void lr1110Reset(void){
    HAL_Delay(2);
    lr1110llSetReset(0);
    HAL_Delay(2);
    lr1110llSetReset(1);
    HAL_Delay(2);
}


/**
 * Wakes up the lr1110
 * @return status
 */
uint8_t lr1110Wakeup(void){
    return 0;
}

/**
 * Read command from the lr1110
 * @param cmd command to be executed
 * @param buffer buffer to store the return values
 * @param size size of buffer
 * @return status
 */
uint8_t lr1110Command(uint16_t cmd, uint8_t * arg, uint16_t arg_size, uint8_t * buffer, uint16_t size){

    lr1110llWaitOnBusy();
    lr1110llSetNSS(0);
    uint8_t txBuffer[2];
    txBuffer[0] = cmd >> 8;
    txBuffer[1] = cmd >> 0;
    HAL_SPI_Transmit(&hspi1, txBuffer, sizeof(txBuffer), 1000);
    if(arg != NULL){
        HAL_SPI_Transmit(&hspi1, arg, arg_size, 1000);

    }
    lr1110llSetNSS(1);

    if(buffer != NULL){
        lr1110llWaitOnBusy();
        lr1110llSetNSS(0);
        HAL_SPI_TransmitReceive(&hspi1,  buffer, buffer, size, 1000);
        lr1110llSetNSS(1);
        return buffer[0];
    }else{
    	return 0;
    }

}

/**
 * Write Command to the lr1110
 * @param cmd command to be executed
 * @param buffer buffer to parameters
 * @param size size of buffer
 * @return status
 */
uint8_t lr1110WriteCommand(uint8_t cmd, uint8_t * buffer, uint16_t size){
    lr1110llWaitOnBusy();
    lr1110llSetNSS(0);


    const uint8_t preambleSize = 1;
    uint8_t txBuffer[size + preambleSize];
    memset(txBuffer, 0, sizeof(txBuffer));
    txBuffer[0] = cmd;
    uint8_t rxBuffer[sizeof(txBuffer)];
    memcpy(txBuffer + preambleSize, buffer, size);

    HAL_SPI_TransmitReceive(&hspi1, txBuffer, rxBuffer, size + preambleSize, 1000);

    lr1110llSetNSS(1);

    return buffer[0];
}
