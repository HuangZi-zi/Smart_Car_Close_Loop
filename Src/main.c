/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
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
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "motor.h"
#include "key.h"
#include "beeper.h"
#include "IR_Tracing.h"
#include "servo.h"
#include "UltrasonicWave.h"
#include "stdio.h"
#include "WIFI_Command.h"

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
extern float UltrasonicWave_Distance;
extern int pan_angle;
extern int pitch_angle;
extern uint8_t USART2_RX_BUFF; 																	//接收缓存
extern uint8_t USART2_RX[USART2_MAX_RECV_LEN];									//接收储存
extern uint16_t USART2_RX_STA;																//接收数据计数
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
  MX_DMA_Init();
  MX_TIM4_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_TIM5_Init();
  MX_TIM8_Init();
  /* USER CODE BEGIN 2 */
	//开启中断
	HAL_UART_Receive_IT(&huart2,(uint8_t *)&USART2_RX_BUFF, 1);//开启接收中断
	HAL_TIM_Base_Start_IT(&htim8);//开启测速中断
	
	//开启输入捕获
	HAL_TIM_IC_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIM_IC_Start(&htim5,TIM_CHANNEL_2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	brake(50);
	Motor_PWM_Start();
	keyscan();
	Servo_PWM_Start();
	SetJointAngle(Servo_Ultrasonic,93);
	SetJointAngle(Servo_Pan,pan_angle);
	SetJointAngle(Servo_Pitch,pitch_angle);
	printf("Hello\n");
  while (1)
  {
		UltrasonicWave_StartMeasure();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		//printf("%f\n",UltrasonicWave_Distance);
		//HAL_Delay(500);

		command_run(TracingRun(),40);
		//receive_command();
		//command_run(receive_command(),70);
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
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
}

/* USER CODE BEGIN 4 */

//GPIO中断服务程序，用于处理超声波避障
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	//传感器接收到回声后Ecoh输出高电平，高电平的持续时间就是从发出到接收的总时间
  if(GPIO_Pin==Echo_Pin)
	{
			__HAL_TIM_SET_COUNTER(&htim2,0);
			HAL_TIM_Base_Start(&htim2);//开启时钟                         
			while(HAL_GPIO_ReadPin(Echo_GPIO_Port,Echo_Pin));//等待低电平
			HAL_TIM_Base_Stop(&htim2);//关闭时钟 
		//每一次计数为1us
			UltrasonicWave_Distance=(float)__HAL_TIM_GET_COUNTER(&htim2)*17/1000.0;//计算距离，单位为cm
	} 
	//printf("distance: %.2f/n",UltrasonicWave_Distance);
	if (UltrasonicWave_Distance<=8.0)
	{
	Error_Handler();
	}
}

//串口2中断服务程序，用于处理串口2接收
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart==&huart2)//接收到数据
	{	  
		USART2_RX[USART2_RX_STA++]=USART2_RX_BUFF;		//记录接收到的值
//		if(USART2_RX[USART2_RX_STA-1]== 0x0A)		//接收完毕
//		{
//			USART2_RX_STA=0;
//		}			 
		//printf("%s!\n",USART2_RX);
		HAL_UART_Receive_IT(&huart2, (uint8_t *)&USART2_RX_BUFF, 1);   //再开启接收中断
	}
}

//定时计数器8溢出中断服务程序
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	int count_left = 0;
	int count_right = 0;	

    if(htim==&htim8)
    {
        count_left = __HAL_TIM_GET_COUNTER(&htim1)*4; 
			  count_right = __HAL_TIM_GET_COUNTER(&htim5)*4; 
				__HAL_TIM_SET_COUNTER(&htim8, 0);
				__HAL_TIM_SET_COUNTER(&htim1, 0);
				__HAL_TIM_SET_COUNTER(&htim5, 0);
			printf("left: %d, right: %d\n",count_left,count_right);
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
	while(1)
	{
		BEEPER_SET;
		brake(500);
		BEEPER_RESET;
		brake(500);
	}
	
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
