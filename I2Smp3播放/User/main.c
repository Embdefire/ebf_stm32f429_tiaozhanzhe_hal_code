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
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx.h"
#include "./led/bsp_led.h" 
#include "./usart/bsp_debug_usart.h"
#include "./key/bsp_key.h" 
/* FatFs includes component */
//#include "ff.h"
//#include "ff_gen_drv.h"
//#include "sd_diskio.h"
#include "mp3Player.h"
#include "bsp_wm8978.h"
/**
  ******************************************************************************
  *                              �������
  ******************************************************************************
  */
#include "main.h"
#include "stm32f4xx.h"
#include "./led/bsp_led.h" 
#include "./usart/bsp_debug_usart.h"
#include "./key/bsp_key.h" 
/* FatFs includes component */
#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"
#include "mp3Player.h"
#include "bsp_wm8978.h"
/**
  ******************************************************************************
  *                              �������
  ******************************************************************************
  */
char SDPath[4]; /* SD�߼�������·�� */
FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FIL fnew;													/* �ļ����� */
FRESULT res_sd;                /* �ļ�������� */
UINT fnum;            			  /* �ļ��ɹ���д���� */
BYTE ReadBuffer[1024]={0};        /* �������� */
BYTE WriteBuffer[] =              /* д������*/
"��ӭʹ��Ұ��STM32 F429������ �����Ǹ������ӣ��½��ļ�ϵͳ�����ļ�\r\n";  

extern FATFS flash_fs;
extern Diskio_drvTypeDef  SD_Driver;
/**
	**************************************************************
	* Description : ��ʼ��WiFiģ��ʹ�����ţ�������WiFiģ��
	* Argument(s) : none.
	* Return(s)   : none.
	**************************************************************
	*/
//static void WIFI_PDN_INIT(void)
//{
//	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
//	GPIO_InitTypeDef GPIO_InitStruct;
//	/*ʹ������ʱ��*/	
//	__HAL_RCC_GPIOG_CLK_ENABLE();
//	/*ѡ��Ҫ���Ƶ�GPIO����*/															   
//	GPIO_InitStruct.Pin = GPIO_PIN_9;	
//	/*�������ŵ��������Ϊ�������*/
//	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;      
//	/*��������Ϊ����ģʽ*/
//	GPIO_InitStruct.Pull  = GPIO_PULLUP;
//	/*������������Ϊ���� */   
//	GPIO_InitStruct.Speed = GPIO_SPEED_FAST; 
//	/*���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO*/
//	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);	
//	/*����WiFiģ��*/
//	HAL_GPIO_WritePin(GPIOG,GPIO_PIN_9,GPIO_PIN_RESET);  
//}

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
  FRESULT res;                                          /* FatFs function common result code */
  
  /* ����ϵͳʱ��Ϊ168 MHz */
  SystemClock_Config();
//	/*����WiFiģ��*/
//	WIFI_PDN_INIT();

  /*��ʼ��USART1*/
  DEBUG_USART_Config();
 
  printf("Music Player\n");
  
  FATFS_LinkDriver(&SD_Driver, SDPath);
  
  //���ⲿSD�������ļ�ϵͳ���ļ�ϵͳ����ʱ���SD����ʼ��
	res = f_mount(&fs,"0:",1);
//  res = f_mount(&SDFatFs, (TCHAR const*)SDPath, 0) ;
  if(res!=FR_OK)
	{
		printf("����SD�������ļ�ϵͳʧ�ܡ�(%d)\r\n",res);
		printf("��������ԭ��SD����ʼ�����ɹ���\r\n");
		while(1);
  }
  else
	{
		printf("SD�����سɹ�\r\n");
	}
	/* ���WM8978оƬ���˺������Զ�����CPU��GPIO */
	if (wm8978_Init()==0)
	{
		printf("��ⲻ��WM8978оƬ!!!\n");
		while (1);	/* ͣ�� */
	}
	printf("��ʼ��WM8978�ɹ�\n");	

  mp3PlayerDemo("0:/mp3/�Ź���-����֮��.mp3");   
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
 void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1) {};
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    while(1) {};
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
