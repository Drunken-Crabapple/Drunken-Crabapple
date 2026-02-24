#include "./BSP/myiic/iic.h"
#include "SYSTEM/delay/delay.h"

void iic_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    __HAL_RCC_GPIOB_CLK_ENABLE();

    gpio_init_struct.Pin = GPIO_PIN_6 | GPIO_PIN_7;      //SCL PB6; SDA PB7
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;
    gpio_init_struct.Pull = GPIO_PULLUP;                //空闲拉高
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB,&gpio_init_struct);
}

static void iic_delay(void)
{
    delay_us(2);        //AT24C02有最小延时限制，无最大
}

/*
空闲时SCL SDA均为高电平，在SCL为高，SDA从高变低时产生起始信号
然后钳住SCL，准备发送或接收数据
写入数据到SDA，不管主机从机，都只能在SCL低时写，SCL高时读取
*/
void iic_start(void)
{
    IIC_SCL(1);
    IIC_SDA(1);
    iic_delay();
    IIC_SDA(0);
    iic_delay();
    IIC_SCL(0);     //必须拉低钳住，如果SCL为高，从机改变SDA拉低，会被视为另一个起始信号
    iic_delay();
}


/*
在SCL高电平时，SDA由低变高即为停止信号
*/
void iic_stop(void)
{
    IIC_SDA(0);     
    IIC_SCL(0);
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    IIC_SDA(1);
    iic_delay();
}

/*
主机发送数据后等待从机应答，需释放SDA交由从机，从机如果收到数据会把SDA拉低，此为应答
若未收到数据，则不管SDA,即SDA为高，此为不应答
*/
uint8_t iic_wait_ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;

    IIC_SDA(1);     //主机释放SDA
    iic_delay();    //主机等待从机应答
    IIC_SCL(1);     //主机释放SCL，读取SDA信号看从机是否应答
    iic_delay();
    
    while(IIC_READ_SDA)     //为0，应答  为1，不应答    在while里面等待，长时间都不应答，停止，返回标志位
    {
        waittime++;
        if(waittime > 250)
        {
            iic_stop();
            rack = 1;
            break;
        }
    }

    IIC_SCL(0);     //检查到应答，拉低SCL,准备下一次发送数据
    iic_delay();
    return rack;
}

/*
此时是从机发送数据，主机接收数据，但需要注意只有主机才能动SCL,所以主机拉低SDA返回应答后还要释放SCL告诉从机可以读取应答信号
然后还要拉低SCL结束第九个周期
*/
void iic_ack(void)
{
    IIC_SDA(0);
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    IIC_SCL(0);
    iic_delay();
    IIC_SDA(1);     //主机释放SDA
    iic_delay();
}

void iic_nack(void)
{
    IIC_SDA(1);
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    IIC_SCL(0);
    iic_delay();
}

void iic_send_byte(uint8_t data)
{
    uint8_t t;
    for(t = 0;t < 8;t++)
    {
        IIC_SDA((data & 0x80) >> 7);        //把数据最高位取出来，IIC发送数据从最高位开始
        iic_delay();                        
        IIC_SCL(1);                         //主机释放SCL，告诉从机可以读取数据了
        iic_delay();
        IIC_SCL(0);                         //从机读完，该周期结束
        data = data << 1;                   //data整体左移，把第二高位变为最高位，以便循环
    }
    
    IIC_SDA(1);                             //发送完成，主机释放SDA，以便从机应答
}

uint8_t iic_read_byte(uint8_t ack)
{
    uint8_t receive = 0;
    uint8_t i;

    for(i = 0;i < 8;i++)
    {
        receive = receive << 1;

        IIC_SCL(1);     //主机释放SCL,读取SDA数据(起始信号最后，包括发送数据的最后都钳住了SCL。就是为了下一次发送或接收数据)
        iic_delay();

        if(IIC_READ_SDA)
        {
            receive++;      //现在receive存放的是传回来的数据的最高位，需要对他进行左移一位
        }
        
        IIC_SCL(0);
        iic_delay();
        //receive = receive << 1;  不能在这里移位，否则最后一位始终是零
    }

    if(! ack)
    {
        iic_ack();
    }
    else
    {
        iic_nack();
    }
    
    return receive;
}
