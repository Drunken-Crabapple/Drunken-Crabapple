#include "stm32f10x.h"                  // Device header
#include "NRF24L01_Define.h"

uint8_t NRF24L01_TxAddress[5] = {0x11,0x22,0x33,0x44,0x55};
#define NRF24L01_TX_PACKET_WIDTH		4
uint8_t NRF24L01_TxPacket[NRF24L01_TX_PACKET_WIDTH];

uint8_t NRF24L01_RxAddress[5] = {0x11,0x22,0x33,0x44,0x55};
#define NRF24L01_RX_PACKET_WIDTH		4 //接收数据四个字节
uint8_t NRF24L01_RxPacket[NRF24L01_RX_PACKET_WIDTH];

/*SPI通信  一主多从  默认CSN高电平 SCK低电平
SS/CSN->选中从机  CSN给低电平 表示选中从机，通信开始
				CSN给高电平 不选中从机 通信结束
SCK->主机控制时钟线 在主机驱动SCK下 主机移位寄存器依次左移，会导致左边高位数据移出，
	 写入从机MOSI引脚，通过MOSI移入从机寄存器的低位，
     此时从机高位会对应溢出一个数据，通过从机MISO写入主机寄存器的低位
   	 SCK来八个周期，主机发送一个完整字节，同时接收一个完整数据，从机接收一个完整字节（全双工）
	 
CSN产生下降沿表示通信开始: 开始第一次数据交换，主机传输给从机指令来确认后续是读操作还是写操作
     而从机把寄存器地址给主机 
 --如果主机给从机发送读指令，则下一次主机的数据随意，也就是主机用垃圾数据交换出从机的数据来读取
     主机读取从机的数据 后续可以一直进行此操作，直至CSN拉高，通信结束
 -- 如果主机给从机发送写指令，则下一次从机的数据随意，也就是从机用垃圾数据来交换主机的数据，主机
     往从机写数据 后续可以一直进行此操作，直至CSN拉高，通信结束
	 
读写取决于主机第一次发送的指令数据
写 把高三位给001  拿0x20 | 寄存器地址，这是发送的指令数据  然后可以开始发送数据
读 把高三位给000  拿0x00 | 寄存器地址
寄存器地址不超过0x1F 把高三位留给判断读写的指令

如果需要连续发或写多个数据（TX_ADDR例）
   --0x20 | TX_ADDR寄存器，是写，然后连续发送五个字节，从机收到后会自动把这五个字节写入EX_ADDR中

*/


//给PA0端口写高低电平
void NRF24L01_W_CE(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_0,(BitAction)BitValue);
}

void NRF24L01_W_CSN(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_1,(BitAction)BitValue);
}

void NRF24L01_W_MOSI(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_3,(BitAction)BitValue);
}

void NRF24L01_W_SCK(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_2,(BitAction)BitValue);
}

uint8_t  NRF24L01_R_MISO(void)
{
	return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4);
}

void NRF24L01_GPIO_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	NRF24L01_W_CE(0);  //CE默认低电平，退出收发模式
	NRF24L01_W_CSN(1); //CEN默认高电平 不选中从机
	NRF24L01_W_SCK(0); //空闲时SCK低电平 默认SPI模式0
	NRF24L01_W_MOSI(0); //1 0均可	
}

/*通信协议*/ //Byte最高位移出MOSI MISO的最低位也会移入Byte的最低位
//Byte的数据传给从机 从机的数据被交换回来存入Byte,所以后面return返回的是接收到的数据
//SPI移入移出一个bit,而想要实现一个字节的传输 只需要循环8次即可
uint8_t NRF24L01_SPI_SwapByte(uint8_t Byte)
{
	uint8_t i;
	
	for(i = 0; i < 8; i++)
	{
		//SPI移出数据 把参数Byte最高位移出到MOSI(SPI时高位先行)
		if(Byte & 0x80)
		{
			NRF24L01_W_MOSI(1);
		}
		else
		{
			NRF24L01_W_MOSI(0);
		}
		Byte <<= 1;  // Byte = Byte << 1
		
		//SCK置高电平
		NRF24L01_W_SCK(1);
		
		//SPI移入数据  把MISO的数据给Byte的最低位  前面一步Byte左移 最低位空出
		if(NRF24L01_R_MISO())
		{
			Byte |= 0x01;  //把Byte最低为给一 其余不变
		}
		else
		{
			Byte &= ~0x01; //把Byte最低位给零 其余不变  Byte左移 默认后一位补零 所以else不写也一样
		}
		
		//SPI置低电平
		NRF24L01_W_SCK(0);
	}
	
	return Byte;	
}

/*指令实现*/

//单字节写入
void NRF24L01_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_W_REGISTER | RegAddress);
	NRF24L01_SPI_SwapByte(Data);
	NRF24L01_W_CSN(1);
	
}

//单字节读出
uint8_t NRF24L01_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_R_REGISTER | RegAddress);
	Data = NRF24L01_SPI_SwapByte(0xFF);  //拿垃圾数据置换MISO里面有用的数据
	NRF24L01_W_CSN(1);
	return Data;
}

//多字节写入
void NRF24L01_WriteRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count)
{
	uint8_t i;
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_W_REGISTER | RegAddress);
	for(i = 0; i < Count; i++)
	{
		NRF24L01_SPI_SwapByte(DataArray[i]);
	}
	NRF24L01_W_CSN(1);
}

//多字节读出
void NRF24L01_ReadRegs(uint8_t RegAddress, uint8_t *DataArray,uint8_t Count)
{
	uint8_t i;
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_R_REGISTER | RegAddress);
	for(i = 0; i < Count; i ++)
	{
		DataArray[i] = NRF24L01_SPI_SwapByte(0xFF);
	}
	NRF24L01_W_CSN(1);
}

//TX_PAYLOAD
void NRF24L01_WriteTxPayload(uint8_t *DataArray,uint8_t Count)
{
	uint8_t i;
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_W_TX_PAYLOAD); //注意奥 FIFO不同于普通寄存器,操作FIFO靠专用的命令,不需要像普通寄存器一样把高三位置001
	//TX FIFO只写不读   RX FIFO只读不写
	for(i = 0; i < Count; i++)
	{
		NRF24L01_SPI_SwapByte(DataArray[i]);
	}
	NRF24L01_W_CSN(1);
	
}

//RX_PAYLOAD
void NRF24L01_ReadRxPayload(uint8_t *DataArray, uint8_t Count)
{
	uint8_t i;
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_R_RX_PAYLOAD);
	for(i = 0;i < Count;i++)
	{
		DataArray[i] = NRF24L01_SPI_SwapByte(0xFF); 
	}
	NRF24L01_W_CSN(1);
	
}

//清空发送FIFO
void NRF24L01_FlushTx(void)
{
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_FLUSH_TX);
	NRF24L01_W_CSN(1);
}

//清空接收FIFO
void NRF24L01_FlushRx(void)
{
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_FLUSH_RX);
	NRF24L01_W_CSN(1);
}

//快速读取寄存器状态  也就是拿垃圾数据换想要的数据
uint8_t NRF24L01_ReadStatus(void)
{
	uint8_t Status;
	NRF24L01_W_CSN(0);
	Status = NRF24L01_SPI_SwapByte(0xFF);
	NRF24L01_W_CSN(1);
	return Status;
}

/*模式配置 使用对应模式直接调用即可*/

//掉电模式
void NRF24L01_PowerDown(void)
{
	uint8_t Config;
	
	NRF24L01_W_CE(0);
	
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
	Config = Config & (~0x02); //把现在该寄存器状态读出来，在不改变其他位的前提下把第二位置零
	NRF24L01_WriteReg(NRF24L01_CONFIG,Config);
	
}

//待机模式1
void NRF24L01_StandbyI(void)
{
	uint8_t Config;
	NRF24L01_W_CE(0);
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
	Config = Config | 0x02; //把第二位置一
	NRF24L01_WriteReg(NRF24L01_CONFIG,Config);
	
}

//接收模式
void NRF24L01_RxMode(void)
{
	uint8_t Config;
	
	NRF24L01_W_CE(0);
	
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
	Config = Config | 0x03; //把该寄存器一二位都置一
	NRF24L01_WriteReg(NRF24L01_CONFIG,Config);
	
	NRF24L01_W_CE(1);
}

//接收模式
void NRF24L01_TxMode(void)
{
	uint8_t Config;
	
	NRF24L01_W_CE(0);
	
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
	Config = Config | 0x02;
	Config = Config & (~0x01);
	NRF24L01_WriteReg(NRF24L01_CONFIG,Config);
	
	NRF24L01_W_CE(1);
}

//模块初始化
void NRF24L01_Init(void)
{
	NRF24L01_GPIO_Init();
	
	NRF24L01_WriteReg(NRF24L01_CONFIG,0x08);
	NRF24L01_WriteReg(NRF24L01_EN_AA,0x3F);
	NRF24L01_WriteReg(NRF24L01_EN_RXADDR,0x01); //开启接收通道0
	NRF24L01_WriteReg(NRF24L01_SETUP_AW,0x03);
	NRF24L01_WriteReg(NRF24L01_SETUP_RETR,0x03);
	NRF24L01_WriteReg(NRF24L01_RF_CH,0x02);
	NRF24L01_WriteReg(NRF24L01_RF_SETUP,0x0E);
	
	NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0,NRF24L01_RxAddress,5); //配置通道0的接收地址
	
	NRF24L01_WriteReg(NRF24L01_RX_PW_P0,NRF24L01_RX_PACKET_WIDTH); //配置通道0有效载荷的字节数
	
	NRF24L01_RxMode();  //开启接收模式
}

/*发送函数

1 -> 写入发送通道地址 发送数据 接收通道0的地址
2 -> 控制芯片进入发送模式
3 -> 读取状态寄存器 等待TX_DS事件 也就是等待发送成功

*/
void NRF24L01_Send(void)
{
	uint8_t Status;
	uint32_t Timeout;
	
	NRF24L01_WriteRegs(NRF24L01_TX_ADDR,NRF24L01_TxAddress,5);
	NRF24L01_WriteTxPayload(NRF24L01_TxPacket,NRF24L01_TX_PACKET_WIDTH); //发送字节数要和接收通道的RX_PW_P0保持一致
	NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0,NRF24L01_TxAddress,5);
	//在接收通道0中写入NRF24L01_TxAddress,  主机发送地址和从机接收地址需要完全一致 
	
	NRF24L01_TxMode();
	
	Timeout = 1000;
	
	while(1)
	{
		Status = NRF24L01_ReadStatus();  //状态寄存器位5为1  说明发送成功 位4为1说明发送失败
		
		Timeout--;
		//发送错误均不为一 超时退出
		if(Timeout == 0)
		{
			break;
		}
		
		if(Status & 0x20)
		{
			//发送成功
			break;
		}
		else if(Status & 0x10)
		{
			//发送失败
			break;
		}
		
	}
	NRF24L01_WriteReg(NRF24L01_STATUS,0x30); //清除位5位4
	
	NRF24L01_FlushTx(); //清除TX FIFO残留的数据
	
	NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0,NRF24L01_RxAddress,5);
	
	NRF24L01_RxMode();
}

//主循环循环调用  如果为1  则收到数据 直接从RXPacket取数据
uint8_t NRF24L01_Receive(void)
{
	uint8_t Status;
	
	Status = NRF24L01_ReadStatus();
	
	if(Status & 0x40)  //状态寄存器位6为1 表示收到数据了
	{
		NRF24L01_ReadRxPayload(NRF24L01_RxPacket,NRF24L01_RX_PACKET_WIDTH);
		
		NRF24L01_WriteReg(NRF24L01_STATUS,0x40);
		
		NRF24L01_FlushRx();
		return 1;
	}
	return 0;
}
