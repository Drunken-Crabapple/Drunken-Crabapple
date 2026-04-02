#include "./BSP/ICM/icm.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/ICM/icm_reg.h"
#include "./SYSTEM/delay/delay.h"
#include "main.h"

extern SPI_HandleTypeDef g_spi2_handle;


void icm_cs_init(void)
{
    spi2_init();

    __HAL_RCC_GPIOE_CLK_ENABLE();       //片选信号CS->PE7
    GPIO_InitTypeDef gpio_init_struct = {0};
    gpio_init_struct.Pin = GPIO_PIN_7;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOE,&gpio_init_struct);

    ICM_CS(1);
}

/*
返回值为0：正常
返回值为1：错误
*/
uint8_t SPI2_ReadWriteBytes(uint8_t addr,uint8_t ptxdata,uint8_t *prxdata,uint16_t datasize)
{
    uint16_t totalsize = datasize + 1;
    uint8_t txbuffer[totalsize];
    uint8_t rxbuffer[totalsize];

    if(prxdata)     //如果prxdata不为NULL，就说明这次是读数据
    {
        txbuffer[0] = addr | 0x80;  //对于读数据，需要知道读哪个地址的数据，这一位是发送地址给从机 高位为1是读命令
        for(uint16_t i = 1;i <= datasize;i++)
        {
            txbuffer[i] = 0xFF;
        }
    }
    else            //如果prxdata为NULL，说明这次是写数据，高位为0，其余七位地址位即为写数据
    {
        txbuffer[0] = addr & 0x7F;  //最高位清零
        txbuffer[1] = ptxdata;      //发送写地址后即为数据
    }

    ICM_CS(0);      //拉低片选 选中从机
    if(HAL_SPI_TransmitReceive(&g_spi2_handle,txbuffer,rxbuffer,totalsize,1000) != HAL_OK)
    {
        return 1;
    }
    ICM_CS(1);
    if(prxdata)     //接收区不为空，把rxbuffer存储数据给prxdaata
    {
        for(uint16_t i = 0;i < datasize;i++)
        {
            prxdata[i] = rxbuffer[i + 1];       //第一位发送的是要读取数据的地址，所以传回来的rxbuffer第一个是废数据
        }
    }
    return 0;       
}

/*
返回一，通信失败
返回0x47,即71为正常
*/
uint8_t ICM_ReadID(void)
{
    uint8_t temp;
    if(SPI2_ReadWriteBytes(WHO_AM_I,0,&temp,1))
    {
        return 1;
    }
    return temp;
}

void icm_init(void)
{
    icm_cs_init();
    SPI2_ReadWriteBytes(REG_BANK_SEL,0x00,0,1);         //确保BANK0
    SPI2_ReadWriteBytes(DEVICE_CONFIG,0x01,0,1);        //SPI模式0、3  软件复位
    delay_ms(1);                                        //等待1ms使软件复位生效
    SPI2_ReadWriteBytes(DRIVE_CONFIG,0x05,0,1);         //SPI速率<2ns
    SPI2_ReadWriteBytes(FIFO_CONFIG,0x00,0,1);          //FIFO旁路模式
    SPI2_ReadWriteBytes(PWR_MGMT0,0x0F,0,1);            //加速度计和陀螺仪开启，同时设置为低噪声模式
    delay_us(200);                                      //当从OFF模式切换至其他模式时，200us内不得对任何寄存器进行写入操作
    SPI2_ReadWriteBytes(GYRO_CONFIG0,0x06,0,1);         //设置陀螺仪量程为+-2000dps  ODR为1kHz
    SPI2_ReadWriteBytes(ACCEL_CONFIG0,0x06,0,1);        //设置加速度计量程为+-16g    ODR为1kHz



    
   
}


uint8_t	ACC_GYRO_Offset1 = 0;   //0：不自动校准；1：自动校准
void Do_ACC_GYRO_Offset(void)
{
	ACC_GYRO_Offset1=1;
}


void ACC_GYRO_Offset(void)
{   static int32_t ACC_X=0,ACC_Y=0,ACC_Z=0;
    static int32_t GYRO_X=0,GYRO_Y=0,GYRO_Z=0;
    static uint8_t count_acc_gyro=0;    
	if(ACC_GYRO_Offset1)
	{
        if(count_acc_gyro==0)
		{ 
			ACC_X = 0;GYRO_X = 0;
			ACC_Y = 0;GYRO_Y = 0;
			ACC_Z = 0;GYRO_Z = 0;
			count_acc_gyro = 1;
		}
		else
		{	count_acc_gyro++;
			ACC_X += acc.x;GYRO_X += gyro.x;
			ACC_Y += acc.y;GYRO_Y += gyro.y;
			ACC_Z += acc.z-2048;GYRO_Z += gyro.z;
		}
		if(count_acc_gyro==251)
		{	count_acc_gyro--;
			conf.offset_acc.x = ACC_X/count_acc_gyro;conf.offset_gyro.x = GYRO_X/count_acc_gyro;
			conf.offset_acc.y = ACC_Y/count_acc_gyro;conf.offset_gyro.y = GYRO_Y/count_acc_gyro;
			conf.offset_acc.z = ACC_Z/count_acc_gyro;conf.offset_gyro.z = GYRO_Z/count_acc_gyro;
			count_acc_gyro = 0;
            ACC_GYRO_Offset1 = 0;
		}	  
	}
}

void READ_ICM42688(void)
{
    static uint8_t tempbuffer[12];
    SPI2_ReadWriteBytes(ACCEL_DATA_X_H,0,tempbuffer,12);
    raw_acc.x =  (int16_t)(tempbuffer[0] << 8 | tempbuffer[1]);
    raw_acc.y =  (int16_t)(tempbuffer[2] << 8 | tempbuffer[3]);
    raw_acc.z =  (int16_t)(tempbuffer[4] << 8 | tempbuffer[5]);
    raw_gyro.x = (int16_t)(tempbuffer[6] << 8 | tempbuffer[7]);
    raw_gyro.y = (int16_t)(tempbuffer[8] << 8 | tempbuffer[9]);
    raw_gyro.z = (int16_t)(tempbuffer[10]<< 8 | tempbuffer[11]);
}

#define GYR_FILTER 0.22f
#define ACC_FILTER 0.22f
static float gyr_fx[4],gyr_fy[4],gyr_fz[4];
static float acc_fx[4],acc_fy[4],acc_fz[4]={2048,2048,2048,2048};
void sensor_data_prepare(void)
{
	gyr_fx[3]=raw_gyro.x; gyr_fy[3]=raw_gyro.y; gyr_fz[3]=raw_gyro.z;
	acc_fx[3]=raw_acc.x;  acc_fy[3]=raw_acc.y;  acc_fz[3]=raw_acc.z;
	for(uint8_t j=3; j>0; j--)
	{	gyr_fx[j-1] += GYR_FILTER * (gyr_fx[j] - gyr_fx[j-1]);
		gyr_fy[j-1] += GYR_FILTER * (gyr_fy[j] - gyr_fy[j-1]);
		gyr_fz[j-1] += GYR_FILTER * (gyr_fz[j] - gyr_fz[j-1]);
		acc_fx[j-1] += ACC_FILTER * (acc_fx[j] - acc_fx[j-1]);	
		acc_fy[j-1] += ACC_FILTER * (acc_fy[j] - acc_fy[j-1]);
		acc_fz[j-1] += ACC_FILTER * (acc_fz[j] - acc_fz[j-1]);
	}
	gyro.x = gyr_fx[0]; gyro.y = gyr_fy[0]; gyro.z = gyr_fz[0];
	acc.x = acc_fx[0];  acc.y = acc_fy[0];  acc.z = acc_fz[0];
	out_acc.x = acc.x-conf.offset_acc.x;
	out_acc.y = acc.y-conf.offset_acc.y;
	out_acc.z = acc.z-conf.offset_acc.z;
	out_gyro.x = gyro.x-conf.offset_gyro.x;
	out_gyro.y = gyro.y-conf.offset_gyro.y;
	out_gyro.z = gyro.z-conf.offset_gyro.z;
	ACC_GYRO_Offset();
	gyro_deg.x = out_gyro.x * 0.0610351f; gyro_deg.y = out_gyro.y * 0.0610351f; gyro_deg.z = out_gyro.z * 0.0610351f;
	gyro_rad.x = gyro_deg.x * 0.01745f;   gyro_rad.y = gyro_deg.y * 0.01745f;   gyro_rad.z = gyro_deg.z * 0.01745f;
}
