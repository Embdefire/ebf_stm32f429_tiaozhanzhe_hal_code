/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   AP3216C ������ʵ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 F429 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

#include "main.h"
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h" 
#include "./delay/core_delay.h" 
#include "./i2c/bsp_i2c.h"
#include "./ap3216c/ap3216c.h"
#include "stm32f4xx_hal_gpio.h"

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
  /**/
  float ALS;
  uint16_t PS;
  uint16_t IR;
  uint8_t IntStatus;
  
  HAL_Init();        
  /* ����ϵͳʱ��Ϊ180 MHz */ 
  SystemClock_Config();
  /* ��ʼ���ں���ʱ */
  HAL_InitTick(5);
  /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
  DEBUG_USART_Config();
  
  printf("\r\n ��ӭʹ��Ұ�� STM32 F429 �����塣\r\n");	
  printf("\r\n ����һ������һ���մ������������� \r\n");
  
  printf(" оƬ��ʼ����.....\n");
  /* ��ʼ�� ���մ����� */
  ap3216c_init();

  while(1)    
  {
    IntStatus = ap3216c_get_IntStatus();    // �ȶ�״̬λ����ADC����λ�����״̬λ��Ĭ�����ã�
    ALS = ap3216c_read_ambient_light();
    PS = ap3216c_read_ps_data();
    IR = ap3216c_read_ir_data();

    printf("\n����ǿ���ǣ�%.2fLux\n����ǿ���ǣ�%d\n", ALS, IR);

    if (PS == 55555)    // IR ̫ǿ PS ������Ч
      printf("IR ̫ǿ PS ������Ч\n");
    else
    {
      printf("�ӽ������ǣ�%d\n", PS & 0x3FF);
    }
    
    if (AP_INT_Read() == 0)
      printf("���жϲ���\n");
    
    if ((PS >> 15) & 1)
      printf("����ӽ�\n");
    else
      printf("����Զ��\n");
    
    if (IntStatus & 0x1)
      printf("ALS �����ж�\n");
    
    if (IntStatus >> 1 & 0x1)
      printf("PS �����ж�\n");
    
    HAL_Delay(200);
  }
}



/**
  * @brief  ϵͳʱ������ 
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
  * @param  ��
  * @retval ��
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* ʹ��HSE������HSEΪPLL��ʱ��Դ������PLL�ĸ��ַ�Ƶ����M N P Q 
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

  /* ���� OverDrive ģʽ */
  HAL_PWREx_EnableOverDrive();
 
  /* ѡ��PLLCLK��ΪSYSCLK�������� HCLK, PCLK1 and PCLK2 ��ʱ�ӷ�Ƶ���� 
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
