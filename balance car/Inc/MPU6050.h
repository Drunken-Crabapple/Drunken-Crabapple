#ifndef MPU6050_H
#define MPU6050_H
#include "stm32f10x.h"
/*
bilibili 小努班 UID:437280309
@time时间: 2025.8.13
@version版本:V1_5
@Encoding :UTF-8
@attention:
1.注意杜邦线别用劣质的
2.如果打开INT要确保连接一定要好，否则ICM20602_Get_Angle_Plus会突然失控(这也是使用其的弊端)
3.注意如果ADD接了1，iic地址是0x69,接0为0x68

@update:
1.修复ICM20602_Get_Angle_Plus中角度突变的问题
2.更新MPU6050_SoftCalibrate_Z函数确保校准正确
3.强效减小ICM20602_Get_Angle_Plus中零点漂移

if you can't display Chinese correctly,please check your encoding mode(please set encoding mode to UTF-8)
*/

//陀螺仪原生数据结构体
typedef struct MPU6050_raw
{
    int16_t AccX;
    int16_t AccY;
    int16_t AccZ;
    int16_t GyroX;
    int16_t GyroY;
    int16_t GyroZ;
    uint16_t Temp;
}MPU6050_raw;
//陀螺仪角度结构体
typedef struct MPU6050
{
    float yaw;
    float roll;
    float pitch;
}MPU6050;

/////////////////////////常用函数//////////////////////////
void MPU6050_init(GPIO_TypeDef* GPIOx,uint16_t SCl,uint16_t SDA);   //初始化姿态传感器
void MPU6050_Get_Angle_Plus(MPU6050* this) ;                        //得到角度(Madgwick+自适应+无万向锁)
void MPU6050_Get_Angle(MPU6050* this);                              //得到角度(正常方法+卡尔曼滤波+互补滤波)
float MPU6050_GetTemp(void);                                        //得到温度
void MPU6050_Set_Angle0(MPU6050* this);//把角度设置为0(让上电的时候角度都从0开始,前提是让传感器采集数据稳定后)

/////////////////////////扩展函数//////////////////////////
uint8_t MPU6050_ID(void);                                           //读取ID
void MPU6050_Get_Raw(MPU6050_raw* this);                            //得到原生数据

#endif
