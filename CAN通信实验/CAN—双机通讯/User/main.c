/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2017-xx-xx
  * @brief   GPIO输出--使用固件库点亮LED灯
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 F429 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"  
#include "./can/bsp_can.h"
#include ".\key\bsp_key.h"

__IO uint32_t flag = 0;		 //用于标志是否接收到数据，在中断函数中赋值
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	/* 配置系统时钟为216 MHz */
    SystemClock_Config();
	/* 初始化LED */
	LED_GPIO_Config();	
	/* 初始化调试串口，一般为串口1 */
	DEBUG_USART_Config();	
    /*初始化can,在中断接收CAN数据包*/
    CAN_Config();

    printf("\r\n 欢迎使用野火  STM32 F429 开发板。\r\n");
    printf("\r\n 野火F429 CAN通讯实验例程\r\n");
	
	printf("\r\n 实验步骤：\r\n");

	printf("\r\n 1.使用导线连接好两个CAN讯设备\r\n");
	printf("\r\n 2.使用跳线帽连接好:5v --- C/4-5V \r\n");
	printf("\r\n 3.按下开发板的KEY1键，会使用CAN向外发送0-7的数据包，包的扩展ID为0x1314 \r\n");
	printf("\r\n 4.若开发板的CAN接收到扩展ID为0x1314的数据包，会把数据以打印到串口。 \r\n");
	printf("\r\n 5.本例中的can波特率为1MBps，为stm32的can最高速率。 \r\n");
	while(1)
	{
		/*按一次按键发送一次数据*/
		if(	Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON)
		{
			LED_BLUE;			
			/* 装载一帧数据 */
			CAN_SetMsg();
			/* 开始发送数据 */
			HAL_CAN_Transmit_IT(&Can_Handle);
			HAL_Delay(100);
			LED_RGBOFF;		

      printf("\r\n已使用CAN发送数据包！\r\n"); 			
			printf("\r\n发送的报文内容为：\r\n");
//			printf("\r\n 扩展ID号ExtId：0x%x \r\n",Can_Handle.ExtId);
			CAN_DEBUG_ARRAY(Can_Handle.pTxMsg->Data,8);     
		}
		if(flag==1)
		{				
    	printf("\r\nCAN接收到数据：\r\n");	
			CAN_DEBUG_ARRAY(Can_Handle.pRxMsg->Data,8); 			
			flag=0;
			HAL_Delay(100);
			LED_GREEN;
		}
	
	
	}
}

/**
  * @brief  系统时钟配置 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  无
  * @retval 无
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* 使能HSE，配置HSE为PLL的时钟源，配置PLL的各种分频因子M N P Q 
	 * PLLCLK = HSE/M*N/P = 25M / 25 *432 / 2 = 216M
	 */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
   while(1) {}
  }

  /* 激活 OverDrive 模式 */
  HAL_PWREx_EnableOverDrive();
 
  /* 选择PLLCLK作为SYSCLK，并配置 HCLK, PCLK1 and PCLK2 的时钟分频因子 
	 * SYSCLK = PLLCLK     = 180M
	 * HCLK   = SYSCLK / 1 = 180M
	 * PCLK2  = SYSCLK / 2 = 90M
	 * PCLK1  = SYSCLK / 4 = 45M
	 */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    while(1) {}
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
