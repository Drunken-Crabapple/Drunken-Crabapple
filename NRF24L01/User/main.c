#include "stm32f10x.h"                  // Device 
#include "Delay.h"
#include "LED.h"
#include "Key.h"
#include "OLED.h"
#include "NRF24L01.h"

uint8_t KeyNum;

int main(void)
{
	OLED_Init();
	Key_Init();
	NRF24L01_Init();
	
	while(1)
	{
		KeyNum = Key_GetNum();
		if(KeyNum == 1)
		{
			NRF24L01_TxPacket[0] ++;
			NRF24L01_TxPacket[1] += 2;
			NRF24L01_TxPacket[2] += 3;
			NRF24L01_TxPacket[3] += 4;
			
			NRF24L01_Send();
		}
		if(NRF24L01_Receive() == 1)
		{
			OLED_ShowHexNum(1,1,NRF24L01_RxPacket[0],2);
			OLED_ShowHexNum(1,4,NRF24L01_RxPacket[1],2);
			OLED_ShowHexNum(1,7,NRF24L01_RxPacket[2],2);
			OLED_ShowHexNum(1,10,NRF24L01_RxPacket[3],2);
			
		}
	}
}


	
	
