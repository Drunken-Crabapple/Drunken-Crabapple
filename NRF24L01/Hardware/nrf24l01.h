#ifndef __NRF24L01__H
#define __NEF24L01__H

#include "NRF24L01_Define.h"

extern uint8_t NRF24L01_TxPacket[];
extern uint8_t NRF24L01_RxPacket[];


uint8_t NRF24L01_SPI_SwapByte(uint8_t Byte);
void NRF24L01_GPIO_Init(void);
void NRF24L01_W_CSN(uint8_t BitValue);
void NRF24L01_WriteReg(uint8_t RegAddress, uint8_t Data);
uint8_t NRF24L01_ReadReg(uint8_t RegAddress);
void NRF24L01_WriteRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count);
void NRF24L01_ReadRegs(uint8_t RegAddress, uint8_t *DataArray,uint8_t Count);

uint8_t NRF24L01_Receive(void);
void NRF24L01_Send(void);
void NRF24L01_Init(void);




#endif
