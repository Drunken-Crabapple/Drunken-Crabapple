#include "./BSP/at24c02/c02.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/myiic/iic.h"


void at24c02_init(void)
{
    iic_init();
}

void at24c02_send_one_byte(uint8_t addr,uint8_t data)
{
    iic_start();
    iic_send_byte(0xA0);    //发送设备通讯地址，最后一位为零是写
    iic_wait_ack();
    iic_send_byte(addr);
    iic_wait_ack();
    iic_send_byte(data);
    iic_wait_ack();
    iic_stop();
    delay_ms(10);           //  等待EEPROM写入
}

uint8_t at24c02_receive_one_byte(uint8_t addr)
{
    uint8_t rec = 0;

    iic_start();
    iic_send_byte(0xA0);        //写
    iic_wait_ack();
    iic_send_byte(addr);
    iic_wait_ack();
    iic_start();                //restart
    iic_send_byte(0xA1);        //读
    iic_wait_ack();
    rec = iic_read_byte(1);     //只获取一个地址的数据，非应答即可
    iic_stop();                 //发送停止信号
    return rec;
}

