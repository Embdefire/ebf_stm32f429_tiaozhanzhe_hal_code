/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2017-xx-xx
  * @brief   GPIO���--ʹ�ù̼������LED��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F429 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "stm32f4xx.h"
#include "./led/bsp_led.h" 
#include "./usart/bsp_debug_usart.h"
#include "./key/bsp_key.h" 
#include "./pvd/bsp_pvd.h"

/*
*�� ����ʵ�������
* 1.ʹ���ⲿ�ɵ���Դ�����ڳ�5V��������ӵ�ʵ��������� 5V<--->GND��������ӹ��磻
*2.��λʵ��壬��ѹ����ʱ�����ϵ�LED�ʵ�ӦΪ��ɫ
*3.���µ��ڿɵ���Դ�ĵ�ѹ����Լ������4V��ʱ��LED�ʵƻ�תΪ��ɫ��
*�������п���PVD��ص�ѹԼΪ2.8V,��5V��Դ����4V��ʱ������STM32��VDD��Դ(3.3V��Դ)�����2.8V������PVD�¼������ж��п�������ƣ�
*/

/*�� ������ע�����
*ʹ�ÿɵ���Դ��ʵ��幩�磬������Դ�߶��ε�(������������USB��)��
*����ֱ�ӽ����빩��û�е�·���������ڵ�ԴʱС�Ĳ�Ҫʹ�����ѹԶ����5V,��ѹ̫�߻��ջ�ʵ��壡��
*���ں��İ�͵װ�ʹ�õĽ�ѹоƬ��һ�������װ��5V�����Сһ��ֵʱ���İ���װ��3V3ѹ���ǲ�һ���ģ������ڲ�����ѹʱӦ���������İ�ĵ�ѹ
*/
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{ 
	/* ����ϵͳʱ��Ϊ216 MHz */
    SystemClock_Config();
	/* ��ʼ��LED */
	LED_GPIO_Config();	
	//���̵ƣ���ʾ��������
	LED_GREEN; 
	
	//����PVD������ѹ����ʱ��������жϷ������������
	PVD_Config();
	
	while(1)
	{				
		/*�������еĳ���*/
	}

}
/**
  * @brief  PWR PVD interrupt callback
  * @param  None 
  * @retval None
  */
void HAL_PWR_PVDCallback(void)
{
  /* ����ƣ�ʵ��Ӧ����Ӧ�������״̬���� */
  LED_RED;
}
/**
  * @brief  ϵͳʱ������ 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 12000000
  *            PLL_M                          = 25
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 4
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
  HAL_StatusTypeDef ret = HAL_OK;
  
   /* ʹ��HSE������HSEΪPLL��ʱ��Դ������PLL�ĸ��ַ�Ƶ����M N P Q 
	  * PLLCLK = HSE/M*N/P = 12M / 25 *360 / 2 = 180M
	  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  if( ret != HAL_OK)
  {
    while(1) {}
  }

  /* ���� OverDrive ģʽ�Դﵽ180MƵ�� */
  ret =HAL_PWREx_EnableOverDrive();
   if( ret != HAL_OK)
  {
    while(1) {}
  }
 
  /* ѡ��PLLCLK��ΪSYSCLK�������� HCLK, PCLK1 and PCLK2 ��ʱ�ӷ�Ƶ���� 
	 * SYSCLK = PLLCLK     = 216M
	 * HCLK   = SYSCLK / 1 = 216M
	 * PCLK2  = SYSCLK / 2 = 108M
	 * PCLK1  = SYSCLK / 4 = 54M
	 */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  
  ret =HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
  
   if( ret != HAL_OK)
  {
    while(1) {}
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
