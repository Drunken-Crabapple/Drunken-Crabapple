#include "./BSP/BME280/bme280.h"
#include "./BSP/SPI/SPI.h"
#include "./BSP/BME280/bme_reg.h"

void bme280_cs_init(void)
{
    spi2_init();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    GPIO_InitTypeDef gpio_init_struct = {0};
    gpio_init_struct.Pin = GPIO_PIN_4;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOE,&gpio_init_struct);

    BME_CS(1);
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

    BME_CS(0);      //拉低片选 选中从机
    if(HAL_SPI_TransmitReceive(&g_spi2_handle,txbuffer,rxbuffer,totalsize,1000) != HAL_OK)
    {
        return 1;
    }
    BME_CS(1);
    if(prxdata)     //接收区不为空，把rxbuffer存储数据给prxdaata
    {
        for(uint16_t i = 0;i < datasize;i++)
        {
            prxdata[i] = rxbuffer[i + 1];       //第一位发送的是要读取数据的地址，所以传回来的rxbuffer第一个是废数据
        }
    }
    return 0;       
}

uint8_t BME280_ReadID(void)
{
    uint8_t temp = 0;
    SPI2_ReadWriteBytes(0xD0,0,&temp,1);
    return temp;
}

void bme_init(void)
{
    bme280_cs_init();
    SPI2_ReadWriteBytes(ctrl_hum,0x01,0,1);     //过采样x1
    SPI2_ReadWriteBytes(config,0x04,0,1);       //IIR FILTER:2  四线SPI
    SPI2_ReadWriteBytes(ctrl_meas,0x27,0,1);   //过采样x1  正常模式
}

/*
    补偿值读取
*/
void BME280_ReadTrim(BME280_CalibData *calib)
{
    uint8_t data[32] = {0};
    SPI2_ReadWriteBytes(0x88,0,&data[0],24);
    calib->dig_T1 = (data[1] << 8) | data[0];
    calib->dig_T2 = (data[3] << 8) | data[2];
    calib->dig_T3 = (data[5] << 8) | data[4];
    calib->dig_P1 = (data[7] << 8) | data[6];
    calib->dig_P2 = (data[9] << 8) | data[8];
    calib->dig_P3 = (data[11] << 8) | data[10];
    calib->dig_P4 = (data[13] << 8) | data[12];
    calib->dig_P5 = (data[15] << 8) | data[14];
    calib->dig_P6 = (data[17] << 8) | data[16];
    calib->dig_P7 = (data[19] << 8) | data[18];
    calib->dig_P8 = (data[21] << 8) | data[20];
    calib->dig_P9 = (data[23] << 8) | data[22];

    SPI2_ReadWriteBytes(0xA1,0,&data[24],1);
    calib->dig_H1 = data[24];

    SPI2_ReadWriteBytes(0xE1,0,&data[25],7);
    calib->dig_H2 = (data[26] << 8) | data[25];
    calib->dig_H3 = data[27];
    calib->dig_H4 = ((int16_t)((int8_t)data[28]) << 4) | (data[29] & 0x0F);
    calib->dig_H5 = ((int16_t)((int8_t)data[30]) << 4) | (data[29] >> 4);
    calib->dig_H6 = data[31];
}
/*
    原始数据读取
*/
void BME280_ReadRawData(BME280_RawData *raw)
{
    uint8_t data[8] = {0};
    SPI2_ReadWriteBytes(press_msb,0,data,8);

    raw->press_raw = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
    raw->temp_raw = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
    raw->hum_raw = (data[6] << 8) | data[7];
}

/*
    温度双精度补偿
    *t_fine是为了给后续两个补偿函数使用，固使用指针传出
    返回值T：单位-摄氏度
*/
double BME280_compensate_Temp_double(BME280_RawData *raw,BME280_CalibData *calib,int32_t *t_fine)
{
    double var1,var2,T;
    var1 = (((double)raw->temp_raw) / 16384.0 - ((double)calib->dig_T1) / 1024.0) * ((double)calib->dig_T2);
    var2 = ((((double)raw->temp_raw) / 131072.0 - ((double)calib->dig_T1) / 8192.0) * (((double)raw->temp_raw) / 131072.0 - 
            ((double) calib->dig_T1) / 8192.0)) * ((double)calib->dig_T3);
    *t_fine = (int32_t) (var1 + var2);
    T = (var1 + var2) / 5120.0;

    return T;
}

/*
    气压双精度补偿
    返回值P： 单位-Pa
*/
double BME280_compensate_Press_double(BME280_RawData *raw,BME280_CalibData *calib,int32_t t_fine)
{
    double var1, var2, p;

    var1 = ((double)t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double)calib->dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double)calib->dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((double)calib->dig_P4) * 65536.0);
    var1 = (((double)calib->dig_P3) * var1 * var1 / 524288.0 +
            ((double)calib->dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double)calib->dig_P1);
    if(var1 == 0.0)
    {
        return 0.0;
    }
    p = 1048576.0 - (double)raw->press_raw;
    p = (p - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double)calib->dig_P9) * p * p / 2147483648.0;
    var2 = p * ((double)calib->dig_P8) / 32768.0;
    p = p + (var1 + var2 + ((double)calib->dig_P7)) / 16.0;

    return p;   
}

/*
    湿度双精度补偿
    返回值var_H:单位-%RH
*/
double BME280_compensate_Hum_double(BME280_RawData *raw, BME280_CalibData *calib, int32_t t_fine)
{
    double var_H;
    var_H = ((double)t_fine) - 76800.0;
    var_H = (((double)raw->hum_raw) -
            (((double)calib->dig_H4) * 64.0 +
            ((double)calib->dig_H5) / 16384.0 * var_H)) *
            (((double)calib->dig_H2) / 65536.0 *
            (1.0 + ((double)calib->dig_H6) / 67108864.0 * var_H *
            (1.0 + ((double)calib->dig_H3) / 67108864.0 * var_H)));
    var_H = var_H * (1.0 - ((double)calib->dig_H1) * var_H / 524288.0);
    if(var_H > 100.0)
    {
        var_H = 100.0;
    }
    else if(var_H < 0.0)
    {
        var_H = 0.0;
    }

    return var_H;
}

/*
    最终初始化函数
    ID读取正确：返回0 并读取补偿值
    ID读取错误：返回1
*/
uint8_t BME280_Init(BME280_CalibData *calib)
{
    bme_init();
    if(BME280_ReadID() == 0x60)
    {
        BME280_ReadTrim(calib);
        return 0;
    }
    else
    {
        return 1;
    }
    
}

void BME280_GetData(BME280_RawData *raw, BME280_CalibData *calib,BME280_Data *data)
{
    int32_t t_fine = 0;

    BME280_ReadRawData(raw);
    data->temp = BME280_compensate_Temp_double(raw,calib,&t_fine);
    data->press = BME280_compensate_Press_double(raw,calib,t_fine);
    data->hum = BME280_compensate_Hum_double(raw,calib,t_fine);
}
