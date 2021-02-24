/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "rtc.h"
#include "gpio.h"
#include "OLED.h"
#include "sys.h"
#include "ADXL.h"
#include "MAX30102.h"
#include "stdio.h"
#include "string.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RTC_Init();
  MX_I2C1_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
	OLED_Init();
	HAL_Delay(200);
	Init_ADXL345();
	
  /* USER CODE END 2 */
	Max30102_Init(&hi2c1);
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	int flag=0;//这三个变量是用来实现放手指时只测第一次心率的功能，放在这是因为要放在while函数外边，不受while的刷新影响
	int32_t getheart=0;
	int32_t realheart=0;
	
  while (1)
  {
    /* USER CODE END WHILE */
				



			if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12)==0)//点击Set键一次进入时钟设置界面
			{
			HAL_Delay(300);//延时真的很重要，没有延时的话，下边的语句瞬间执行完成，立马来到下一次判断
			RTC_TimeTypeDef GetTime;   //获取时间结构体
			RTC_DateTypeDef GetData;  //获取日期结构体
			HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
      HAL_RTC_GetDate(&hrtc, &GetData, RTC_FORMAT_BIN);//神级大坑，Gettime一定要在Getdate之前，否则rtc锁死
			uint8_t year=GetData.Year;
			uint8_t mon=GetData.Month;
			uint8_t date=GetData.Date;
			uint8_t hour=GetTime.Hours;
			uint8_t min=GetTime.Minutes;
			uint8_t sec=GetTime.Seconds;//获取当前年月日时分秒的数据,这些数据是准备用来实时可用按键修改并显示的
			int SetWhich=0;//用来判断当前设置的是年月日时分秒中的哪个值，通过控制此数值的变动来切换	
			
				while(1)
				{
					char tostring_sec[]={"fuck"};
					char tostring_min[]={"fuck"};
					char tostring_hour[]={"fuck"};
					char tostring_day[]={"fuck"};
					char tostring_mon[]={"fuck"};
					char tostring_year[]={"fuck"};//用来转为字符串并显示的
			
					sprintf(tostring_sec,"%d",sec);
					sprintf(tostring_min,"%d",min);
					sprintf(tostring_hour,"%d",hour);
			
					sprintf(tostring_day,"%d",date);
					sprintf(tostring_mon,"%d",mon);
					sprintf(tostring_year,"%d",2000+year);//加2000，因为cubemx里设置的时间不能设置千年和百年
			
					OLED_Show_String(0,0,tostring_year,16,1); 
					OLED_Show_String(48,0,tostring_mon,16,1);
					OLED_Show_String(72,0,tostring_day,16,1);
					OLED_Show_String(0,24,tostring_hour,16,1); 
					OLED_Show_String(15,24,":",16,1); 
					OLED_Show_String(24,24,tostring_min,16,1);
					OLED_Show_String(40,24,":",16,1); 
					OLED_Show_String(48,24,tostring_sec,16,1);
					
					
					
					if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13)==0)
					{
					break;
					}
					
					if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12)==0)
					{
						SetWhich+=1;//每按动一次set键，setwhich自加1，然后通过下边的switch判断语句循环在年月日时分秒几个变量中切换设置的是哪个
						HAL_Delay(200);//延时一下，不然数字加到起飞
					}
					
					switch(SetWhich)
					{
						case 0:
							if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)==0)
							{
								if(year==99){year=-1;}
								year+=1;
								HAL_Delay(200);
								uint8_t useful_year=RTC_fucking_change(year);
								GetData.Year=useful_year;
								uint8_t useful_date=RTC_fucking_change(date);
								GetData.Date=useful_date;//你可能会问，为什么要在设置年的地方还有顺便设置日期？
								//读取到的当前日期，包括年月日，会在下面一行的setdate函数中全部写入寄存器，
								//如果只修改年分，那么月和日会以默认读到的日期再保存一次，这一次保存如果没有经过RTC_fucking_change函数
								//就会默认变成16进制进位，导致设置错误，比如日期为16号，保存时变为0x10而不是0x16，写入的就是0x10导致16号变成10号
								//所以，哪怕是在这，只要涉及到任何的变动，都要再过一次RTC_fucking_change函数
								//下边也是一样的道理
								HAL_RTC_SetDate(&hrtc, &GetData, RTC_FORMAT_BCD);//真正地将修改后的值写入寄存器
							}
							if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15)==0)
							{
								if(year==0){year=100;}
								year-=1;
								HAL_Delay(200);
								uint8_t useful_year=RTC_fucking_change(year);
								GetData.Year=useful_year;
								uint8_t useful_date=RTC_fucking_change(date);
								GetData.Date=useful_date;
								HAL_RTC_SetDate(&hrtc, &GetData, RTC_FORMAT_BCD);//真正地将修改后的值写入寄存器
							}	
								
							
							OLED_Show_String(0,0,tostring_year,16,0);//将最后一个参数改为0，实现这一项的反色显示
							break;//设置年
							
						case 1:
							if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)==0)
							{
								if(mon==12){mon=0;}
								mon+=1;
								HAL_Delay(200);
								uint8_t useful_mon=RTC_fucking_change(mon);
								GetData.Month=useful_mon;
								uint8_t useful_year=RTC_fucking_change(year);
								GetData.Year=useful_year;
								uint8_t useful_date=RTC_fucking_change(date);
								GetData.Date=useful_date;
								HAL_RTC_SetDate(&hrtc, &GetData, RTC_FORMAT_BCD);//真正地将修改后的值写入寄存器
							}
							if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15)==0)
							{
								if(mon==1){mon=13;}
								mon-=1;
								HAL_Delay(200);
								uint8_t useful_mon=RTC_fucking_change(mon);
								GetData.Month=useful_mon;
								uint8_t useful_year=RTC_fucking_change(year);
								GetData.Year=useful_year;
								uint8_t useful_date=RTC_fucking_change(date);
								GetData.Date=useful_date;
								HAL_RTC_SetDate(&hrtc, &GetData, RTC_FORMAT_BCD);//真正地将修改后的值写入寄存器
							}
							
							
							OLED_Show_String(48,0,tostring_mon,16,0);//将最后一个参数改为0，实现这一项的反色显示
							break;//设置月
							
						case 2:
							if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)==0)
							{
								if(date==31){date=0;}
								date+=1;
								HAL_Delay(200);
								uint8_t useful_date=RTC_fucking_change(date);
								GetData.Date=useful_date;
								uint8_t useful_year=RTC_fucking_change(year);
								GetData.Year=useful_year;
								HAL_RTC_SetDate(&hrtc, &GetData, RTC_FORMAT_BCD);//真正地将修改后的值写入寄存器
							}
							if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15)==0)
							{
								if(date==1){date=32;}
								date-=1;
								HAL_Delay(200);
								uint8_t useful_date=RTC_fucking_change(date);
								GetData.Date=useful_date;
								uint8_t useful_year=RTC_fucking_change(year);
								GetData.Year=useful_year;
								HAL_RTC_SetDate(&hrtc, &GetData, RTC_FORMAT_BCD);//真正地将修改后的值写入寄存器
							}
							
							
							OLED_Show_String(72,0,tostring_day,16,0);//将最后一个参数改为0，实现这一项的反色显示
							break;//设置日
							
						case 3:
							if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)==0)
							{
								if(hour==23){hour=-1;}
								hour+=1;
								HAL_Delay(200);
								uint8_t useful_hour=RTC_fucking_change(hour);
								GetTime.Hours=useful_hour;
								uint8_t useful_min=RTC_fucking_change(min);
								GetTime.Minutes=useful_min;
								uint8_t useful_sec=RTC_fucking_change(sec);
								GetTime.Seconds=useful_sec;
								HAL_RTC_SetTime(&hrtc, &GetTime, RTC_FORMAT_BCD);//真正地将修改后的值写入寄存器
							
							}
							if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15)==0)
							{
								if(hour==0){hour=24;}
								hour-=1;
								HAL_Delay(200);
								uint8_t useful_hour=RTC_fucking_change(hour);
								GetTime.Hours=useful_hour;
								uint8_t useful_min=RTC_fucking_change(min);
								GetTime.Minutes=useful_min;
								uint8_t useful_sec=RTC_fucking_change(sec);
								GetTime.Seconds=useful_sec;
								HAL_RTC_SetTime(&hrtc, &GetTime, RTC_FORMAT_BCD);//真正地将修改后的值写入寄存器
							}
							
							
							OLED_Show_String(0,24,tostring_hour,16,0); //将最后一个参数改为0，实现这一项的反色显示
							break;//设置时
							
						case 4:
							if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)==0)
							{
								if(min==59){min=-1;}
								min+=1;
								HAL_Delay(200);
								uint8_t useful_min=RTC_fucking_change(min);
								GetTime.Minutes=useful_min;
								uint8_t useful_hour=RTC_fucking_change(hour);
								GetTime.Hours=useful_hour;
								uint8_t useful_sec=RTC_fucking_change(sec);
								GetTime.Seconds=useful_sec;
								HAL_RTC_SetTime(&hrtc, &GetTime, RTC_FORMAT_BCD);//真正地将修改后的值写入寄存器
							}
							if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15)==0)
							{
								if(min==0){min=60;}
								min-=1;
								HAL_Delay(200);
								uint8_t useful_min=RTC_fucking_change(min);
								GetTime.Minutes=useful_min;
								uint8_t useful_hour=RTC_fucking_change(hour);
								GetTime.Hours=useful_hour;
								uint8_t useful_sec=RTC_fucking_change(sec);
								GetTime.Seconds=useful_sec;
								HAL_RTC_SetTime(&hrtc, &GetTime, RTC_FORMAT_BCD);//真正地将修改后的值写入寄存器
							}
							
							
							OLED_Show_String(24,24,tostring_min,16,0);//将最后一个参数改为0，实现这一项的反色显示
							break;//设置分
							
						case 5:
							if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)==0)
							{
								if(sec==59){sec=-1;}
								sec+=1;
								HAL_Delay(200);
								uint8_t useful_sec=RTC_fucking_change(sec);
								GetTime.Seconds=useful_sec;
								uint8_t useful_hour=RTC_fucking_change(hour);
								GetTime.Hours=useful_hour;
								uint8_t useful_min=RTC_fucking_change(min);
								GetTime.Minutes=useful_min;
								HAL_RTC_SetTime(&hrtc, &GetTime, RTC_FORMAT_BCD);//真正地将修改后的值写入寄存器
							}
							if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15)==0)
							{
								if(sec==0){sec=60;}
								sec-=1;
								HAL_Delay(200);
								uint8_t useful_sec=RTC_fucking_change(sec);
								GetTime.Seconds=useful_sec;
								uint8_t useful_hour=RTC_fucking_change(hour);
								GetTime.Hours=useful_hour;
								uint8_t useful_min=RTC_fucking_change(min);
								GetTime.Minutes=useful_min;
								HAL_RTC_SetTime(&hrtc, &GetTime, RTC_FORMAT_BCD);//真正地将修改后的值写入寄存器
							}
							
							
							OLED_Show_String(48,24,tostring_sec,16,0);//将最后一个参数改为0，实现这一项的反色显示
							
							break;//设置秒
							
						case 6:
							SetWhich=0;//回到设置年
							HAL_Delay(200);
							break;
					}
					
					
					//写下边这几行是为了更新掉电保存的数据
					RTC_DateTypeDef datebuff;   //日期结构体参数
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
					HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, (uint16_t)datebuff.WeekDay);//只用存年月日星期就行	
					
					
					OLED_Refresh_GRAM();
					HAL_Delay(10);
					OLED_Clear();//刷新屏幕
					
					
					
					
				}
			
			}
			
			
			
			

			rtc_display();//时间处理并显示

			char tostring_Steps[32];
			sprintf(tostring_Steps, "FT: %d",adxl_display());
			OLED_Show_String(0,48,tostring_Steps,16,1);//计步处理并显示
			


			
			//关键变量flag，getheart和realheart定义在while函数外边，一开始都等于0
			Max30102_Task();
			getheart=Max30102_GetHeartRate();
			if(getheart>0&&getheart<200&&flag==0)//这么写是想在手指放上传感器并在离开之前只取第一次读到的心率，因为测越久越不准，小于200是因为人类是有极限的，超过200只能说明传感器没测准
			{
				realheart=getheart;
				flag=1;
			}
			if(getheart==0&&flag==1)//当手指离开心率传感器，getheart等于0，此时将真显示的值realheart置为0，并将flag置零，以开放下一次显示测得心率的权限
			{
			realheart=0;
			flag=0;
			}
					
			char UartBuffer1[32];
			sprintf(UartBuffer1, "HR:%d",realheart);
			OLED_Show_String(72,48,UartBuffer1,16,1);//心率处理并显示
			
			if(realheart>100)//心率过高引起报警
			{
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,SET);
			HAL_Delay(100);
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,RESET);
			}
					
			
		
			
			OLED_Refresh_GRAM();
			HAL_Delay(1);
			OLED_Clear();//刷新屏幕
		}
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* EXTI9_5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == INT_Pin) {
		Max30102_InterruptCallback();
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
