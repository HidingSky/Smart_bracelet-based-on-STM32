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
	
	RTC_TimeTypeDef time;   //时间结构体参数
	RTC_DateTypeDef datebuff;   //日期结构体参数
	
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};
	
	__HAL_RCC_BKP_CLK_ENABLE();       //开启后备区域时钟
	__HAL_RCC_PWR_CLK_ENABLE();		  //开启电源时钟

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
	if(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR1)!= 0x5051)//这里加了个判断条件，是掉电保存的关键
	{
		sTime.Hours = 0x23;//如果从后备存储中读到的数不是0x5051则设置时间为如下数据
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
		
	
		
		
	__HAL_RTC_SECOND_ENABLE_IT(&hrtc,RTC_IT_SEC);	 //开启RTC时钟秒中断，就是时钟每过一秒，就停下来，然后单片机得以执行下面的语句，当然停下来的时间几乎为零，不会影响时钟的正常走时
	//datebuff = DateToUpdate;  //把日期数据拷贝到自己定义的data中，神坑！神坑！神坑！此句赋值并不好使，导致bug，换用下边的语句之后就好使了
	HAL_RTC_GetDate(&hrtc, &datebuff, RTC_FORMAT_BIN);//换用这个就好使了，把当前的年月日保存到datebuff变量中
		
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x5051);//向指定的后备区域寄存器写入数据
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, (uint16_t)datebuff.Year);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, (uint16_t)datebuff.Month);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, (uint16_t)datebuff.Date);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, (uint16_t)datebuff.WeekDay);//只用存年月日就行，时分秒会自己走，在这存起来之后，看下边的语句，每次上电会判断并读取
	
	}
	
	else//在读取到后备数据为0x5051的情况下，就不重新设置时间了，而是从每一秒都会保存的后备数据中读取并更新到当前日期中
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
		__HAL_RTC_SECOND_ENABLE_IT(&hrtc,RTC_IT_SEC);	 //开启RTC时钟秒中断
		
		HAL_RTC_GetDate(&hrtc, &datebuff, RTC_FORMAT_BIN);//再存，不然日期不更新，此处和下边的几句为神来之笔，如果没有，那么往
		//后备flash保存的日期会永远定格在写入的那一次，哪怕时钟的机制能在掉电的时候更新日期，重新上电之后也会被覆盖
		//虽然程序能用，但是还是有个疑问，为什么在掉电时跨天更新的日期不会被上述语句给覆盖掉？如果有某个强制更新的机制实现在上述的读取后备flash数据语句之后
		//那么为什么下边这几个语句能发挥作用？
		//我觉得可能是因为上边的那个秒中断发挥的作用
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x5051);//向指定的后备区域寄存器写入数据
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, (uint16_t)datebuff.Year);
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, (uint16_t)datebuff.Month);
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, (uint16_t)datebuff.Date);
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, (uint16_t)datebuff.WeekDay);//只用存年月日就行	
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
			RTC_TimeTypeDef GetTime;   //获取时间结构体
			RTC_DateTypeDef GetData;  //获取日期结构体
			HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
      /* Get the RTC current Date */
      HAL_RTC_GetDate(&hrtc, &GetData, RTC_FORMAT_BIN);//神级大坑，Gettime一定要在Getdate之前，否则rtc锁死
		
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
			sprintf(year,"%d",2000+GetData.Year);//加2000，因为cubemx里设置的时间不能设置千年和百年
			
			OLED_Show_String(0,0,year,16,1); //
			OLED_Show_String(48,0,mon,16,1);
			OLED_Show_String(72,0,day,16,1);
			
			OLED_Show_String(0,24,hour,16,1); //
			OLED_Show_String(15,24,":",16,1); 
			OLED_Show_String(24,24,min,16,1);
			OLED_Show_String(40,24,":",16,1); 
			OLED_Show_String(48,24,sec,16,1);
//			上边这些是实现时钟显示功能的
}

uint8_t RTC_fucking_change(uint8_t get)//使用hal_rtc_settime或者hal_rtc_setdate函数的时候，函数只认十六
//进制，比如设置分钟为16分时，必须是写入0x16，十进制的16直接写入会引起进位变成10，导致设置错误，不得不再此写个转换函数
//你可能会问，那十进制的15，也就是0xF，0xF也不等于0x15啊，为什么在十进制16以下就能够办事不用转换？
//因为0xF和0x15这些十进制16以下的数在这里直接等效了
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
