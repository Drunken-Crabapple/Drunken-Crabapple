#include "MPU6050.h"
#include "_I2C_.h"
#include "math.h"
#include "stdlib.h"
#include "stdio.h"
#include "stm32f10x.h"                  // Device header


/*
如果要移植的话不喜欢我的iic，可以换成你自己的只需把
I2C(MPU6050_I2C)->Write_Reg(reg,data)换成你自己的IIC写寄存器的函数即可然后删除掉"_I2C_.h"和"I2C_BUS MPU6050_I2C";
之后检查报错部分做对应处理,别忘记初始化
*/
I2C_BUS MPU6050_I2C;//创建了一个IIC对象
#define MPU6050_Delay_ms(ms)           Delay_ms(ms)
#define MPU6050Wirte_Reg(reg,data) I2C(MPU6050_I2C)->Write_Reg(reg,data)//可以替换为你的写寄存器函数
#define MPU6050Read_Reg(reg)    I2C(MPU6050_I2C)->Read_Reg(reg)//可以替换为你的读寄存器函数
const static float dt = 0.01f;// 采样时间间隔(s),注意和你的采样率密切相关，你的采样率是多少这里就改为对应的时间
//////////////////////////////////////以上为方便移植的接口///////////////////////////////////////////////////////





///////////////////////////下面是寄存器地址定义//////////////////////////////////
#define MPU6050_ADDRESS		      0x68//i2c address
#define	MPU6050_SMPLRT_DIV		  0x19
#define	MPU6050_CONFIG			  0x1A
#define	MPU6050_GYRO_CONFIG		  0x1B
#define	MPU6050_ACCEL_CONFIG	  0x1C
#define MPU6050_FIFO_EN           0x23

#define MPU6050_INTBP_CFG_REG	  0X37	//中断寄存器
#define MPU6050_INT_ENABLE        0x38

#define	MPU6050_ACCEL_XOUT_H	  0x3B
#define	MPU6050_ACCEL_XOUT_L	  0x3C
#define	MPU6050_ACCEL_YOUT_H	  0x3D
#define	MPU6050_ACCEL_YOUT_L	  0x3E
#define	MPU6050_ACCEL_ZOUT_H	  0x3F
#define	MPU6050_ACCEL_ZOUT_L	  0x40
#define	MPU6050_TEMP_OUT_H		  0x41
#define	MPU6050_TEMP_OUT_L		  0x42
#define	MPU6050_GYRO_XOUT_H		  0x43
#define	MPU6050_GYRO_XOUT_L		  0x44
#define	MPU6050_GYRO_YOUT_H		  0x45
#define	MPU6050_GYRO_YOUT_L		  0x46
#define	MPU6050_GYRO_ZOUT_H		  0x47
#define	MPU6050_GYRO_ZOUT_L		  0x48
#define MPU6050_SIGNAL_PATH_RESET 0x68
//校准地址不列出
#define MPU6050_USER_CTRL         0x6A
#define	MPU6050_PWR_MGMT_1		  0x6B
#define	MPU6050_WHO_AM_I		  0x75

//////////////////////////////////////////////////////////
static float angle_yaw = 0;
static float angle_roll = 0;
static float angle_pitch = 0;
typedef enum{//传感器的滤波带宽
    Band_256Hz = 0x00,
    Band_186Hz,
    Band_96Hz,
    Band_43Hz,
    Band_21Hz,
    Band_10Hz,
    Band_5Hz
}Filter_Typedef;

typedef enum{//传感器角速度测量范围
    gyro_250 = 0x00,
    gyro_500 = 0x08,
    gyro_1000 = 0x10,
    gyro_2000 = 0x18
}GYRO_CONFIG_Typedef;

typedef enum{//传感器加速度测量范围
    acc_2g = 0x00,
    acc_4g = 0x08,
    acc_8g = 0x10,
    acc_16g = 0x18
}ACCEL_CONFIG_Typedef;

typedef enum{
    FIFO_Disable,               //关闭FIFO
    Acc_OUT = 0x08,
    Gyro_zOUT = 0x10,
    Gyro_yOUT = 0x20,
    Gyro_xOUT = 0x40,
    Temp_OUT =0x80,
}FIFO_EN_Typedef;

typedef enum{
    interrupt_Disable,          //中断使能
    Data_Ready_EN = 0x01,       
    I2C_Master_EN = 0x08,       //IIC主机模式
    FIFO_overFolow_EN = 0x10,   //FIFO覆盖
    Motion_EN = 0x40,       
}INT_EN_Typedef;
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct MPU6050_InitTypeDef
{
    uint16_t SMPLRT_Rate;              //采样率Hz
    Filter_Typedef Filter;             //滤波器
    GYRO_CONFIG_Typedef gyro_range;    //陀螺仪测量范围
    ACCEL_CONFIG_Typedef acc_range;    //加速度计测量范围
    FIFO_EN_Typedef FIFO_EN;           //FIFO缓冲区使能
    INT_EN_Typedef INT;                //中断使能
}MPU6050_InitTypeDef;
///////////////////////////////////////////////////////////////////////////////////////////////////
static void MPU6050_Register_init(MPU6050_InitTypeDef* this){
    MPU6050Wirte_Reg(MPU6050_PWR_MGMT_1,0x80);//复位
    MPU6050_Delay_ms(100);
    MPU6050Wirte_Reg(MPU6050_PWR_MGMT_1,0x00);//唤醒
   	uint8_t SMPLRT_DIV;
	if(this->SMPLRT_Rate>=1000)this->SMPLRT_Rate=1000;
	else if(this->SMPLRT_Rate<4)this->SMPLRT_Rate=4;
	SMPLRT_DIV=1000.0f/this->SMPLRT_Rate-1;//由计算公式得
    MPU6050Wirte_Reg(MPU6050_SMPLRT_DIV,SMPLRT_DIV);

    MPU6050Wirte_Reg(MPU6050_INT_ENABLE,this->INT);
    MPU6050Wirte_Reg(MPU6050_CONFIG,this->Filter);
    MPU6050Wirte_Reg(MPU6050_GYRO_CONFIG,this->gyro_range);
    MPU6050Wirte_Reg(MPU6050_ACCEL_CONFIG,this->acc_range);
    MPU6050Wirte_Reg(MPU6050_FIFO_EN,this->FIFO_EN);
    uint8_t temp = 0x00;
    if(this->FIFO_EN!=0x00)//如果打开了FIFO
        temp = 0x40;
    if((this->INT & 0x01)==0)//如果打开了中断
        temp |= 0x08;
    MPU6050Wirte_Reg(MPU6050_USER_CTRL,temp);
    MPU6050Wirte_Reg(MPU6050_PWR_MGMT_1,0x01);//X轴为参考
}

// 传感器校准函数,减小零点漂移
static int16_t gyro_zero_z = 0;
static void MPU6050_SoftCalibrate_Z(uint16_t calibration_samples) {
    int32_t gz_sum = 0;
    int16_t GyroZ;

    for (uint16_t i = 0; i < calibration_samples; i++) {
        ///////////根据Z轴的变换规律进行修正/////////
        // 读取Z轴数据
        GyroZ = ((int16_t)(MPU6050Read_Reg(MPU6050_GYRO_ZOUT_H))<<8) | MPU6050Read_Reg(MPU6050_GYRO_ZOUT_L);
        // printf("%d,%d,%d\n",GyroZ,0,0);// 通过串口打印数据(用于检查驱动是否正常)
        gz_sum += GyroZ;
        MPU6050_Delay_ms(10);//要和dt同步
    }
    gyro_zero_z = gz_sum / calibration_samples;
    // printf("%d,%d,%d\n",gyro_zero_z,0,0);// 通过串口打印数据(用于检查驱动是否正常)
    // MPU6050_Delay_ms(1000);//(用于检查驱动是否正常)
}

// 卡尔曼滤波器结构体
typedef struct {
    float q; // 过程噪声协方差
    float r; // 测量噪声协方差
    float x; // 状态估计值
    float p; // 估计误差协方差
    float k; // 卡尔曼增益
} MPU6050_KalmanFilter;
// 卡尔曼滤波更新函数
static float KalmanFilter_Update(MPU6050_KalmanFilter *kf, float measurement) {
    // 预测步骤
    kf->p = kf->p + kf->q;
    // 计算卡尔曼增益
    kf->k = kf->p / (kf->p + kf->r);
    // 更新估计值
    kf->x = kf->x + kf->k * (measurement - kf->x);
    // 更新估计误差协方差
    kf->p = (1 - kf->k) * kf->p;
    return kf->x;
}
// 快速平方根倒数算法
static inline float invSqrt(float x) {
    float halfx = 0.5f * x;
    float y = x;
    long i = *(long*)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float*)&i;
    y = y * (1.5f - (halfx * y * y));  // 一次牛顿迭代
    return y;
}
// //滑动平均滤波器:buffer:缓冲数组,input：信号输入
// static int16_t MPU6050_slidingWindowFilter(int16_t buffer[],uint16_t buffer_Length, int16_t input) {
// 	int16_t sum = 0;
//     static int16_t mindex = 0;
//     static int16_t count = 0;
// 	buffer[mindex] = input;
// 	mindex = (mindex + 1) % buffer_Length;
//     if (count < buffer_Length)
//     	count++;
//     for (int i = 0; i < count; i++)
//         sum += buffer[i];
//     return sum / count;
// }

///////////////////////////////////////////以下为用户可以访问的代码/////////////////////////////////////////////////////

/*例子:MPU6050_init(GPIOA,GPIO_Pin_1,GPIO_Pin_2);
GPIOx:选择你的GPIO
SCl:选择你的GPIO(choose your GPIO)
SDA:选择你的GPIO(choose your GPIO)
注意:SCL和SDA来自同一组GPIO口(notice:SCL and SDA come from a same GPIO port)*/
void MPU6050_init(GPIO_TypeDef* GPIOx,uint16_t SCl,uint16_t SDA){
    MPU6050_I2C = Create_SI2C(GPIOx,SCl,SDA,MPU6050_ADDRESS);//创建软件IIC

    MPU6050_InitTypeDef MPU6050_init_Struct;
    MPU6050_init_Struct.SMPLRT_Rate = 100;            //采样率Hz
    MPU6050_init_Struct.Filter = Band_5Hz;            //滤波器带宽
    MPU6050_init_Struct.gyro_range = gyro_250;       //陀螺仪测量范围
    MPU6050_init_Struct.acc_range = acc_2g;          //加速度计测量范围
    MPU6050_init_Struct.FIFO_EN = FIFO_Disable;       //FIFO
    MPU6050_init_Struct.INT = interrupt_Disable;      //中断配置
    MPU6050_Register_init(&MPU6050_init_Struct);      //初始化寄存器
    MPU6050_Delay_ms(250);//开机后等待
    MPU6050_SoftCalibrate_Z(200);//软件校准,减少yaw的零点漂移
}

void MPU6050_Get_Raw(MPU6050_raw* this){
    this->AccX = ((int16_t)(MPU6050Read_Reg(MPU6050_ACCEL_XOUT_H))<<8) | MPU6050Read_Reg(MPU6050_ACCEL_XOUT_L);
    this->AccY = ((int16_t)(MPU6050Read_Reg(MPU6050_ACCEL_YOUT_H))<<8) | MPU6050Read_Reg(MPU6050_ACCEL_YOUT_L);
    this->AccZ = ((int16_t)(MPU6050Read_Reg(MPU6050_ACCEL_ZOUT_H))<<8) | MPU6050Read_Reg(MPU6050_ACCEL_ZOUT_L);
    this->GyroX = ((int16_t)(MPU6050Read_Reg(MPU6050_GYRO_XOUT_H))<<8) | MPU6050Read_Reg(MPU6050_GYRO_XOUT_L);
    this->GyroY = ((int16_t)(MPU6050Read_Reg(MPU6050_GYRO_YOUT_H))<<8) | MPU6050Read_Reg(MPU6050_GYRO_YOUT_L);
    this->GyroZ = ((int16_t)(MPU6050Read_Reg(MPU6050_GYRO_ZOUT_H))<<8) | MPU6050Read_Reg(MPU6050_GYRO_ZOUT_L);
    this->Temp = ((uint16_t)(MPU6050Read_Reg(MPU6050_TEMP_OUT_H))<<8) | MPU6050Read_Reg(MPU6050_TEMP_OUT_L);
}

float MPU6050_GetTemp(){//返回温度
	uint8_t temp = (((uint16_t)MPU6050Read_Reg(MPU6050_TEMP_OUT_H)) << 8) | MPU6050Read_Reg(MPU6050_TEMP_OUT_L);
	float temperature = (float)temp/340 + 36.53;
	return temperature;
}

float ACC_abs = 0;
void MPU6050_Get_Angle(MPU6050* this){
    int16_t temp=0;
	float Ax,Ay,Az;
	float Gx,Gy,Gz;
    //const static float dt=0.01f;//积分时间10ms

    // 初始化卡尔曼滤波器
    static MPU6050_KalmanFilter kf_roll, kf_pitch;
    static int once_flag = 1;
    if (once_flag) {//只执行一次,修改卡尔曼滤波器:如果需要高速运动加大q，噪声大的时候增大r
        //roll的初始化
        kf_roll.q = 0.001f;
        kf_roll.r = 0.1f;
        kf_roll.x = 0;
        kf_roll.p = 1;
        //Pitch的初始化
        kf_pitch.q = 0.001f;
        kf_pitch.r = 0.1f;
        kf_pitch.x = 0;
        kf_pitch.p = 1;
        once_flag = 0;
    }
    
    /////////////////////////////Ax = temp * 加速度计量程/32768;//////////////////////////////////
	temp = ((uint16_t)MPU6050Read_Reg(MPU6050_ACCEL_XOUT_H) << 8)+MPU6050Read_Reg(MPU6050_ACCEL_XOUT_L);
	Ax = temp * 2.0f/32768;

	temp = ((uint16_t)MPU6050Read_Reg(MPU6050_ACCEL_YOUT_H) << 8)+MPU6050Read_Reg(MPU6050_ACCEL_YOUT_L);
	Ay = temp * 2.0f/32768;

	temp = ((uint16_t)MPU6050Read_Reg(MPU6050_ACCEL_ZOUT_H) << 8)+MPU6050Read_Reg(MPU6050_ACCEL_ZOUT_L);
	Az = temp * 2.0f/32768;

	temp = ((uint16_t)MPU6050Read_Reg(MPU6050_GYRO_XOUT_H) << 8)+MPU6050Read_Reg(MPU6050_GYRO_XOUT_L);
	Gx = temp *dt*0.0174532f;

	temp = ((uint16_t)MPU6050Read_Reg(MPU6050_GYRO_YOUT_H) << 8)+MPU6050Read_Reg(MPU6050_GYRO_YOUT_L);
	Gy = temp *dt*0.0174532f;

	temp = ((uint16_t)MPU6050Read_Reg(MPU6050_GYRO_ZOUT_H) << 8) + MPU6050Read_Reg(MPU6050_GYRO_ZOUT_L) - gyro_zero_z;//软件校准减掉漂移值
	Gz = temp *dt*0.0174532f;

    // 计算加速度的绝对值
    float absAcc = sqrt(Ax * Ax + Ay * Ay + Az * Az);
    //ACC_abs = absAcc;
    // 动态调整权重
    float weight;
    if (absAcc > 1.2) {
        // 快速运动或剧烈振动状态，减小加速度计权重
        weight = 0.8f;
    } else {
        // 正常运动状态，强烈信任加速度计
        weight = 1.0f;
    }

    static float Gyroscope_roll=0.0f;
	static float Gyroscope_pitch=0.0f;
	Gyroscope_roll+=Gy;
	Gyroscope_pitch+=Gx;
	float raw_roll=weight * atan2(Ay,Az)/3.1415926f * 180.f + (1-weight) * Gyroscope_roll;
	float raw_pitch=-(weight * atan2(Ax,Az)/3.1415926f * 180.f + (1-weight) * Gyroscope_pitch);
    // 应用卡尔曼滤波
    this->roll = KalmanFilter_Update(&kf_roll, raw_roll);
    this->pitch = KalmanFilter_Update(&kf_pitch, raw_pitch);
    //减小零飘(如果零票严重，可以通过加减一个小数来解决,这是二次手动校准,值得注意的是，每次上电的时候gyro_zero_z的值都会不同
    //，并且VCC也会变换所以这个不是很好的解决办法，不想漂就加磁力计)
    if(fabsf(Gz)>=0.01)
    	this->yaw += Gz/2;
    // printf("%.3f,%.3f,%.3f,%d\n", this->roll, this->pitch, this->yaw,temp); // 通过串口打印数据(用于检查驱动是否正常)
}

//四元素法+动态互补滤波
void MPU6050_Get_Angle_Plus(MPU6050* this) {
    ////////////////////////四元素优化//////////////////////////
    static uint16_t times = 0;
    static float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
    
    // 控制器参数 - 优化响应速度
    static float Kp;       // 比例增益(加快响应)
    static float Ki;    // 积分增益
    static float integralX = 0.0f, integralY = 0.0f, integralZ = 0.0f;
    
    // 预计算常量，减少每次循环的计算量
    const float gyroScale = 0.0174533f / 131.0f;  // °/s to rad/s (±250°/s量程)
    const float accelScale = 1.0f / 16384.0f;     // 加速度计量程±2g

    // static int16_t gz_raw_slider_filter[3];//滑动窗口滤波器

    // 读取传感器原始数据(使用寄存器批量读取优化)
    int16_t ax_raw = ((int16_t)(MPU6050Read_Reg(MPU6050_ACCEL_XOUT_H))<<8) | MPU6050Read_Reg(MPU6050_ACCEL_XOUT_L);
    int16_t ay_raw = ((int16_t)(MPU6050Read_Reg(MPU6050_ACCEL_YOUT_H))<<8) | MPU6050Read_Reg(MPU6050_ACCEL_YOUT_L);
    int16_t az_raw = ((int16_t)(MPU6050Read_Reg(MPU6050_ACCEL_ZOUT_H))<<8) | MPU6050Read_Reg(MPU6050_ACCEL_ZOUT_L);
    int16_t gx_raw = ((int16_t)(MPU6050Read_Reg(MPU6050_GYRO_XOUT_H))<<8) | MPU6050Read_Reg(MPU6050_GYRO_XOUT_L);
    int16_t gy_raw = ((int16_t)(MPU6050Read_Reg(MPU6050_GYRO_YOUT_H))<<8) | MPU6050Read_Reg(MPU6050_GYRO_YOUT_L);
    int16_t gz_raw = ((int16_t)(MPU6050Read_Reg(MPU6050_GYRO_ZOUT_H))<<8) | MPU6050Read_Reg(MPU6050_GYRO_ZOUT_L);
    uint32_t status = abs(gx_raw+gy_raw);
    // printf("%d\n",status);// 通过串口打印数据(用于检查驱动是否正常)
    const uint16_t threshold = 400;
    if(status>threshold-200 && status < threshold +200){
        gz_raw = gz_raw - gyro_zero_z - gyro_zero_z/2;//通用性有待考证
        // gz_raw = MPU6050_slidingWindowFilter(gz_raw_slider_filter,3,gz_raw) ;//使用滑动窗口滤波滤除噪声
    }else{
        gz_raw = gz_raw - gyro_zero_z;
    }
    
    // 转换为物理单位(使用预计算常量优化)
    float ax = (float)ax_raw * accelScale;
    float ay = (float)ay_raw * accelScale;
    float az = (float)az_raw * accelScale;
    float gx = (float)gx_raw * gyroScale;  // °/s to rad/s
    float gy = (float)gy_raw * gyroScale;
    float gz = (float)gz_raw * gyroScale;;
    
    // 计算加速度幅值(用于自适应增益控制)
    float accMag = ax * ax + ay * ay + az * az;
    
    if(times<400)//开机的时候快速达到目标值
    {
        times ++;
        Kp = 10.f;     // 略微减小比例增益
        Ki = 0.004f;   // 略微减小积分增益
    }else{
        // 自适应参数调整 - 根据运动状态动态调整增益
        if (accMag > 1.44f || accMag < 0.64f) {  // 对应1.2g和0.8g
            // 剧烈运动或可能的自由落体状态
            Kp = 1.6f;     // 略微减小比例增益
            Ki = 0.001f;   // 略微减小积分增益
        } else {
            // 正常运动状态
            Kp = 2.8f;
            Ki = 0.0015f;
        }
    }

    // 加速度计数据有效时进行姿态校正
    if (accMag > 0.01f) {  // 避免除以零
        // 归一化加速度计数据(使用快速invSqrt)
        float recipNorm = invSqrt(accMag);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;
        
        // 估计重力方向
        float vx = 2.0f * (q1*q3 - q0*q2);
        float vy = 2.0f * (q0*q1 + q2*q3);
        float vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;
        
        // 计算重力方向与加速度计测量方向之间的误差
        float ex = (ay*vz - az*vy);
        float ey = (az*vx - ax*vz);
        float ez = (ax*vy - ay*vx);
        
        // 累积误差积分项
        if (Ki > 0.0f) {
            integralX += ex * dt;
            integralY += ey * dt;
            integralZ += ez * dt;
            
            // 应用积分项到陀螺仪数据
            gx += Ki * integralX;
            gy += Ki * integralY;
            gz += Ki * integralZ;
        }
        
        // 应用比例项到陀螺仪数据
        gx += Kp * ex;
        gy += Kp * ey;
        gz += Kp * ez;
    }
    
    // 四元数导数计算
    float qDot0 = 0.5f * (-q1*gx - q2*gy - q3*gz);
    float qDot1 = 0.5f * (q0*gx + q2*gz - q3*gy);
    float qDot2 = 0.5f * (q0*gy - q1*gz + q3*gx);
    float qDot3 = 0.5f * (q0*gz + q1*gy - q2*gx);
    
    // 积分更新四元数
    q0 += qDot0 * dt;
    q1 += qDot1 * dt;
    q2 += qDot2 * dt;
    q3 += qDot3 * dt;
    
    // 归一化四元数(使用快速invSqrt)
    float norm = q0*q0 + q1*q1 + q2*q2 + q3*q3;
    if (norm > 0.0f) {
        float recipNorm = invSqrt(norm);
        q0 *= recipNorm;
        q1 *= recipNorm;
        q2 *= recipNorm;
        q3 *= recipNorm;
    }
    
    // 从四元数计算欧拉角(单位:度)
    this->roll = atan2f(2.0f * (q0*q1 + q2*q3), 1.0f - 2.0f * (q1*q1 + q2*q2)) * 57.29578f;
    this->pitch = asinf(2.0f * (q0*q2 - q3*q1)) * 57.29578f;
    this->yaw = atan2f(2.0f * (q0*q3 + q1*q2), 1.0f - 2.0f * (q2*q2 + q3*q3)) * 57.29578f;
    this->roll = this->roll - angle_roll;
    this->pitch = this->pitch - angle_pitch;
    this->yaw = this->yaw - angle_yaw;
    // printf("%.3f,%.3f,%.3f,%d\n", this->roll, this->pitch, this->yaw,gz_raw); // 通过串口打印数据(用于检查驱动是否正常)
}

void MPU6050_Set_Angle0(MPU6050* this){
    //记录下当前角度
    for(uint32_t i = 0;i<1000000;i++);
    angle_yaw = this->yaw;
    angle_roll = this->roll;
    angle_pitch = this->pitch;
}

uint8_t MPU6050_ID(){
    return MPU6050Read_Reg(MPU6050_WHO_AM_I);
}

