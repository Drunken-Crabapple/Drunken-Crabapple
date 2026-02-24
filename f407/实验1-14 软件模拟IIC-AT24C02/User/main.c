#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/at24c02/c02.h"
#include "./BSP/TIMER/gtim.h"

int main(void)
{
    extern TIM_HandleTypeDef g_timx_cnt_chy_handle;
    uint16_t curcnt = 0;
    uint16_t oldcnt = 0;

    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz  n m p q*/
    delay_init(168);                    /* 延时初始化 */
    at24c02_init();
    gtim_tim5_cnt_ch1_init(0);          //有效分频为0 + 1
    usart_init(115200);

   // at24c02_send_one_byte(5,0x51);
    
    while(1)
    {
        curcnt = __HAL_TIM_GET_COUNTER(&g_timx_cnt_chy_handle);
        if(oldcnt != curcnt)
        {
            //at24c02_send_one_byte(5,curcnt);
            printf("您存储的数据为：%d \r\n",at24c02_receive_one_byte(5));
            oldcnt = curcnt;    //更新
        }
    }
}

