/**
  ******************************************************************************
  * File Name          : RTC.c
  * Description        : This file provides code for the configuration
  *                      of the RTC instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{
	
	RTC_TimeTypeDef time;   //ʱ��ṹ�����
	RTC_DateTypeDef datebuff;   //���ڽṹ�����
	
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};
	
	__HAL_RCC_BKP_CLK_ENABLE();       //����������ʱ��
	__HAL_RCC_PWR_CLK_ENABLE();		  //������Դʱ��

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
	if(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR1)!= 0x5051)//������˸��ж��������ǵ��籣��Ĺؼ�
	{
		sTime.Hours = 0x23;//����Ӻ󱸴洢�ж�����������0x5051������ʱ��Ϊ��������
		sTime.Minutes = 0x59;
		sTime.Seconds = 0x0;

		if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
		{
			Error_Handler();
		}
		DateToUpdate.WeekDay = RTC_WEEKDAY_THURSDAY;
		DateToUpdate.Month = RTC_MONTH_NOVEMBER;
		DateToUpdate.Date = 0x19;
		DateToUpdate.Year = 0x20;

		if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
		{
			Error_Handler();
		}
		
	
		
		
	__HAL_RTC_SECOND_ENABLE_IT(&hrtc,RTC_IT_SEC);	 //����RTCʱ�����жϣ�����ʱ��ÿ��һ�룬��ͣ������Ȼ��Ƭ������ִ���������䣬��Ȼͣ������ʱ�伸��Ϊ�㣬����Ӱ��ʱ�ӵ�������ʱ
	//datebuff = DateToUpdate;  //���������ݿ������Լ������data�У���ӣ���ӣ���ӣ��˾丳ֵ������ʹ������bug�������±ߵ����֮��ͺ�ʹ��
	HAL_RTC_GetDate(&hrtc, &datebuff, RTC_FORMAT_BIN);//��������ͺ�ʹ�ˣ��ѵ�ǰ�������ձ��浽datebuff������
		
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x5051);//��ָ���ĺ�����Ĵ���д������
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, (uint16_t)datebuff.Year);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, (uint16_t)datebuff.Month);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, (uint16_t)datebuff.Date);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, (uint16_t)datebuff.WeekDay);//ֻ�ô������վ��У�ʱ������Լ��ߣ����������֮�󣬿��±ߵ���䣬ÿ���ϵ���жϲ���ȡ
	
	}
	
	else//�ڶ�ȡ��������Ϊ0x5051������£��Ͳ���������ʱ���ˣ����Ǵ�ÿһ�붼�ᱣ��ĺ������ж�ȡ�����µ���ǰ������
	{
		datebuff.Year    = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
		datebuff.Month   = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3);
		datebuff.Date    = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4);
		datebuff.WeekDay = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR5);
		DateToUpdate = datebuff;
		if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
		{
			Error_Handler();
		}
		__HAL_RTC_SECOND_ENABLE_IT(&hrtc,RTC_IT_SEC);	 //����RTCʱ�����ж�
		
		HAL_RTC_GetDate(&hrtc, &datebuff, RTC_FORMAT_BIN);//�ٴ棬��Ȼ���ڲ����£��˴����±ߵļ���Ϊ����֮�ʣ����û�У���ô��
		//��flash��������ڻ���Զ������д�����һ�Σ�����ʱ�ӵĻ������ڵ����ʱ��������ڣ������ϵ�֮��Ҳ�ᱻ����
		//��Ȼ�������ã����ǻ����и����ʣ�Ϊʲô�ڵ���ʱ������µ����ڲ��ᱻ�����������ǵ��������ĳ��ǿ�Ƹ��µĻ���ʵ���������Ķ�ȡ��flash�������֮��
		//��ôΪʲô�±��⼸������ܷ������ã�
		//�Ҿ��ÿ�������Ϊ�ϱߵ��Ǹ����жϷ��ӵ�����
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x5051);//��ָ���ĺ�����Ĵ���д������
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, (uint16_t)datebuff.Year);
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, (uint16_t)datebuff.Month);
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, (uint16_t)datebuff.Date);
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, (uint16_t)datebuff.WeekDay);//ֻ�ô������վ���	
	}

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    HAL_PWR_EnableBkUpAccess();
    /* Enable BKP CLK enable for backup registers */
    __HAL_RCC_BKP_CLK_ENABLE();
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

void rtc_display()
{
			RTC_TimeTypeDef GetTime;   //��ȡʱ��ṹ��
			RTC_DateTypeDef GetData;  //��ȡ���ڽṹ��
			HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
      /* Get the RTC current Date */
      HAL_RTC_GetDate(&hrtc, &GetData, RTC_FORMAT_BIN);//�񼶴�ӣ�Gettimeһ��Ҫ��Getdate֮ǰ������rtc����
		
			char sec[]={"fuck"};
			char min[]={"fuck"};
			char hour[]={"fuck"};
			
			char day[]={"fuck"};
			char mon[]={"fuck"};
			char year[]={"fuck"};
			
			sprintf(sec,"%d",GetTime.Seconds);
			sprintf(min,"%d",GetTime.Minutes);
			sprintf(hour,"%d",GetTime.Hours);
			
			sprintf(day,"%d",GetData.Date);
			sprintf(mon,"%d",GetData.Month);
			sprintf(year,"%d",2000+GetData.Year);//��2000����Ϊcubemx�����õ�ʱ�䲻������ǧ��Ͱ���
			
			OLED_Show_String(0,0,year,16,1); //
			OLED_Show_String(48,0,mon,16,1);
			OLED_Show_String(72,0,day,16,1);
			
			OLED_Show_String(0,24,hour,16,1); //
			OLED_Show_String(15,24,":",16,1); 
			OLED_Show_String(24,24,min,16,1);
			OLED_Show_String(40,24,":",16,1); 
			OLED_Show_String(48,24,sec,16,1);
//			�ϱ���Щ��ʵ��ʱ����ʾ���ܵ�
}

uint8_t RTC_fucking_change(uint8_t get)//ʹ��hal_rtc_settime����hal_rtc_setdate������ʱ�򣬺���ֻ��ʮ��
//���ƣ��������÷���Ϊ16��ʱ��������д��0x16��ʮ���Ƶ�16ֱ��д��������λ���10���������ô��󣬲��ò��ٴ�д��ת������
//����ܻ��ʣ���ʮ���Ƶ�15��Ҳ����0xF��0xFҲ������0x15����Ϊʲô��ʮ����16���¾��ܹ����²���ת����
//��Ϊ0xF��0x15��Щʮ����16���µ���������ֱ�ӵ�Ч��
{
	uint8_t change=0x0;
	if(get<16){change=get;}
	if(get>=16&&get<26){change=get+6;}
	if(get>=26&&get<36){change=get+12;}
	if(get>=36&&get<46){change=get+18;}
	if(get>=46&&get<56){change=get+24;}
	if(get>=56&&get<66){change=get+30;}
	if(get>=66&&get<76){change=get+36;}
	if(get>=76&&get<86){change=get+42;}
	if(get>=86&&get<96){change=get+48;}
	if(get>=96){change=get+54;}
	return change;
}
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
