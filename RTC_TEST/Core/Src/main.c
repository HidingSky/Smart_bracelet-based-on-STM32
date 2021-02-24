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
	
	int flag=0;//����������������ʵ�ַ���ָʱֻ���һ�����ʵĹ��ܣ�����������ΪҪ����while������ߣ�����while��ˢ��Ӱ��
	int32_t getheart=0;
	int32_t realheart=0;
	
  while (1)
  {
    /* USER CODE END WHILE */
				



			if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12)==0)//���Set��һ�ν���ʱ�����ý���
			{
			HAL_Delay(300);//��ʱ��ĺ���Ҫ��û����ʱ�Ļ����±ߵ����˲��ִ����ɣ�����������һ���ж�
			RTC_TimeTypeDef GetTime;   //��ȡʱ��ṹ��
			RTC_DateTypeDef GetData;  //��ȡ���ڽṹ��
			HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
      HAL_RTC_GetDate(&hrtc, &GetData, RTC_FORMAT_BIN);//�񼶴�ӣ�Gettimeһ��Ҫ��Getdate֮ǰ������rtc����
			uint8_t year=GetData.Year;
			uint8_t mon=GetData.Month;
			uint8_t date=GetData.Date;
			uint8_t hour=GetTime.Hours;
			uint8_t min=GetTime.Minutes;
			uint8_t sec=GetTime.Seconds;//��ȡ��ǰ������ʱ���������,��Щ������׼������ʵʱ���ð����޸Ĳ���ʾ��
			int SetWhich=0;//�����жϵ�ǰ���õ���������ʱ�����е��ĸ�ֵ��ͨ�����ƴ���ֵ�ı䶯���л�	
			
				while(1)
				{
					char tostring_sec[]={"fuck"};
					char tostring_min[]={"fuck"};
					char tostring_hour[]={"fuck"};
					char tostring_day[]={"fuck"};
					char tostring_mon[]={"fuck"};
					char tostring_year[]={"fuck"};//����תΪ�ַ�������ʾ��
			
					sprintf(tostring_sec,"%d",sec);
					sprintf(tostring_min,"%d",min);
					sprintf(tostring_hour,"%d",hour);
			
					sprintf(tostring_day,"%d",date);
					sprintf(tostring_mon,"%d",mon);
					sprintf(tostring_year,"%d",2000+year);//��2000����Ϊcubemx�����õ�ʱ�䲻������ǧ��Ͱ���
			
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
						SetWhich+=1;//ÿ����һ��set����setwhich�Լ�1��Ȼ��ͨ���±ߵ�switch�ж����ѭ����������ʱ���뼸���������л����õ����ĸ�
						HAL_Delay(200);//��ʱһ�£���Ȼ���ּӵ����
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
								GetData.Date=useful_date;//����ܻ��ʣ�ΪʲôҪ��������ĵط�����˳���������ڣ�
								//��ȡ���ĵ�ǰ���ڣ����������գ���������һ�е�setdate������ȫ��д��Ĵ�����
								//���ֻ�޸���֣���ô�º��ջ���Ĭ�϶����������ٱ���һ�Σ���һ�α������û�о���RTC_fucking_change����
								//�ͻ�Ĭ�ϱ��16���ƽ�λ���������ô��󣬱�������Ϊ16�ţ�����ʱ��Ϊ0x10������0x16��д��ľ���0x10����16�ű��10��
								//���ԣ����������⣬ֻҪ�漰���κεı䶯����Ҫ�ٹ�һ��RTC_fucking_change����
								//�±�Ҳ��һ���ĵ���
								HAL_RTC_SetDate(&hrtc, &GetData, RTC_FORMAT_BCD);//�����ؽ��޸ĺ��ֵд��Ĵ���
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
								HAL_RTC_SetDate(&hrtc, &GetData, RTC_FORMAT_BCD);//�����ؽ��޸ĺ��ֵд��Ĵ���
							}	
								
							
							OLED_Show_String(0,0,tostring_year,16,0);//�����һ��������Ϊ0��ʵ����һ��ķ�ɫ��ʾ
							break;//������
							
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
								HAL_RTC_SetDate(&hrtc, &GetData, RTC_FORMAT_BCD);//�����ؽ��޸ĺ��ֵд��Ĵ���
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
								HAL_RTC_SetDate(&hrtc, &GetData, RTC_FORMAT_BCD);//�����ؽ��޸ĺ��ֵд��Ĵ���
							}
							
							
							OLED_Show_String(48,0,tostring_mon,16,0);//�����һ��������Ϊ0��ʵ����һ��ķ�ɫ��ʾ
							break;//������
							
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
								HAL_RTC_SetDate(&hrtc, &GetData, RTC_FORMAT_BCD);//�����ؽ��޸ĺ��ֵд��Ĵ���
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
								HAL_RTC_SetDate(&hrtc, &GetData, RTC_FORMAT_BCD);//�����ؽ��޸ĺ��ֵд��Ĵ���
							}
							
							
							OLED_Show_String(72,0,tostring_day,16,0);//�����һ��������Ϊ0��ʵ����һ��ķ�ɫ��ʾ
							break;//������
							
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
								HAL_RTC_SetTime(&hrtc, &GetTime, RTC_FORMAT_BCD);//�����ؽ��޸ĺ��ֵд��Ĵ���
							
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
								HAL_RTC_SetTime(&hrtc, &GetTime, RTC_FORMAT_BCD);//�����ؽ��޸ĺ��ֵд��Ĵ���
							}
							
							
							OLED_Show_String(0,24,tostring_hour,16,0); //�����һ��������Ϊ0��ʵ����һ��ķ�ɫ��ʾ
							break;//����ʱ
							
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
								HAL_RTC_SetTime(&hrtc, &GetTime, RTC_FORMAT_BCD);//�����ؽ��޸ĺ��ֵд��Ĵ���
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
								HAL_RTC_SetTime(&hrtc, &GetTime, RTC_FORMAT_BCD);//�����ؽ��޸ĺ��ֵд��Ĵ���
							}
							
							
							OLED_Show_String(24,24,tostring_min,16,0);//�����һ��������Ϊ0��ʵ����һ��ķ�ɫ��ʾ
							break;//���÷�
							
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
								HAL_RTC_SetTime(&hrtc, &GetTime, RTC_FORMAT_BCD);//�����ؽ��޸ĺ��ֵд��Ĵ���
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
								HAL_RTC_SetTime(&hrtc, &GetTime, RTC_FORMAT_BCD);//�����ؽ��޸ĺ��ֵд��Ĵ���
							}
							
							
							OLED_Show_String(48,24,tostring_sec,16,0);//�����һ��������Ϊ0��ʵ����һ��ķ�ɫ��ʾ
							
							break;//������
							
						case 6:
							SetWhich=0;//�ص�������
							HAL_Delay(200);
							break;
					}
					
					
					//д�±��⼸����Ϊ�˸��µ��籣�������
					RTC_DateTypeDef datebuff;   //���ڽṹ�����
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
					HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, (uint16_t)datebuff.WeekDay);//ֻ�ô����������ھ���	
					
					
					OLED_Refresh_GRAM();
					HAL_Delay(10);
					OLED_Clear();//ˢ����Ļ
					
					
					
					
				}
			
			}
			
			
			
			

			rtc_display();//ʱ�䴦����ʾ

			char tostring_Steps[32];
			sprintf(tostring_Steps, "FT: %d",adxl_display());
			OLED_Show_String(0,48,tostring_Steps,16,1);//�Ʋ�������ʾ
			


			
			//�ؼ�����flag��getheart��realheart������while������ߣ�һ��ʼ������0
			Max30102_Task();
			getheart=Max30102_GetHeartRate();
			if(getheart>0&&getheart<200&&flag==0)//��ôд��������ָ���ϴ����������뿪֮ǰֻȡ��һ�ζ��������ʣ���Ϊ��Խ��Խ��׼��С��200����Ϊ�������м��޵ģ�����200ֻ��˵��������û��׼
			{
				realheart=getheart;
				flag=1;
			}
			if(getheart==0&&flag==1)//����ָ�뿪���ʴ�������getheart����0����ʱ������ʾ��ֵrealheart��Ϊ0������flag���㣬�Կ�����һ����ʾ������ʵ�Ȩ��
			{
			realheart=0;
			flag=0;
			}
					
			char UartBuffer1[32];
			sprintf(UartBuffer1, "HR:%d",realheart);
			OLED_Show_String(72,48,UartBuffer1,16,1);//���ʴ�����ʾ
			
			if(realheart>100)//���ʹ������𱨾�
			{
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,SET);
			HAL_Delay(100);
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,RESET);
			}
					
			
		
			
			OLED_Refresh_GRAM();
			HAL_Delay(1);
			OLED_Clear();//ˢ����Ļ
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
