#include "./BSP/WDG/wdg.h"


IWDG_HandleTypeDef g_iwdg_handle;

/* IWDG初始化
  取消PR/RLR寄存器写保护
  设置IWDG预分频系数和重装载值
  启动IWDG
  启动IWDG后，LSI会自动开启，F4为32k，但LSI精度较低(RC振荡)
*/
void iwdg_init(uint8_t pres,uint16_t rlr)
{
    g_iwdg_handle.Instance = IWDG;
    g_iwdg_handle.Init.Prescaler = pres;
    g_iwdg_handle.Init.Reload = rlr;
    
    HAL_IWDG_Init(&g_iwdg_handle);
}

/* 喂狗函数
  及时写入0xAAAA到IWDG)KR*/
void iwdg_feed(void)
{
    HAL_IWDG_Refresh(&g_iwdg_handle);
}
