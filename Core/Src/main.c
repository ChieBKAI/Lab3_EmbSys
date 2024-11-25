/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
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
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "software_timer.h"
#include "led_7seg.h"
#include "button.h"
#include "lcd.h"
#include "picture.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define GREEN_RED 0
#define YELLOW_RED 1
#define RED_GREEN 2
#define RED_YELLOW 3
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void system_init();
void test_button();
void test_lcd();
void Traffic_Light();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int RED_TIME = 260;
int YELLOW_TIME = 60;
int GREEN_TIME = 200;
int mode = 0;
int temp_red_time = 0;
int temp_yellow_time = 0;
int temp_green_time = 0;
int normal_count = 0;
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
  MX_TIM2_Init();
  MX_SPI1_Init();
  MX_FSMC_Init();
  /* USER CODE BEGIN 2 */
  system_init();
  lcd_Clear(WHITE);
  test_lcd();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	while(!flag_timer2);
	flag_timer2 = 0;
	button_Scan();
	if (button_count[0] == 1) {
	  mode = (mode > 2) ? 0 : (mode + 1);
	  if (mode == 0) {
		  normal_count = 0;
	  }
	  lcd_Clear(WHITE);
	}
	Traffic_Light();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void system_init() {
  HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 0);
  HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin, 0);
  HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, 0);

	timer_init();
	led7_init();
	button_init();
	lcd_init();
	setTimer2(50);

	temp_green_time = GREEN_TIME;
	temp_red_time = RED_TIME;
	temp_yellow_time = YELLOW_TIME;
}
void test_button() {
  for (int i = 0; i < 16; i++) {
    if (button_count[i] > 0) {
      lcd_ShowIntNum(140, 105, i, 2, BRED, WHITE, 32);
    }
  }
}
void test_lcd() {
  // min(0, 0) max(240, 320)
  lcd_Fill(0, 0, 240, 20, BLUE); //
  lcd_StrCenter(0, 2, "Hello World", RED, BLUE, 16, 1);
  lcd_ShowStr(20, 30, "Test LCD Screen", WHITE, RED, 24, 0);
  lcd_DrawCircle(60, 120, GREEN, 40, 1); //lcd_DrawCircle(x_center, y_center, color, radius, fill)
  lcd_DrawCircle(160, 120, BRED, 40, 0);
  lcd_ShowPicture(80, 200, 90, 120, gImage_pic);
}
void draw_light(int state) {
  switch (state) {
    case 99:
      lcd_DrawCircle(60, 120, BRED, 30, 1);
      lcd_DrawCircle(60, 190, BRED, 30, 1);
      lcd_DrawCircle(60, 260, BRED, 30, 1);
      lcd_DrawCircle(160, 120, BRED, 30, 1);
      lcd_DrawCircle(160, 190, BRED, 30, 1);
      lcd_DrawCircle(160, 260, BRED, 30, 1);
      break;
    case GREEN_RED:
      lcd_DrawCircle(60, 120, GREEN, 30, 1);
      lcd_DrawCircle(60, 190, BRED, 30, 0);
      lcd_DrawCircle(60, 260, BRED, 30, 0);
      lcd_DrawCircle(160, 120, BRED, 30, 0);
      lcd_DrawCircle(160, 190, BRED, 30, 0);
      lcd_DrawCircle(160, 260, RED, 30, 1);
      break;
    case YELLOW_RED:
      lcd_DrawCircle(60, 120, BRED, 30, 0);
      lcd_DrawCircle(60, 190, YELLOW, 30, 1);
      lcd_DrawCircle(60, 260, BRED, 30, 0);
      lcd_DrawCircle(160, 120, BRED, 30, 0);
      lcd_DrawCircle(160, 190, BRED, 30, 0);
      lcd_DrawCircle(160, 260, RED, 30, 1);
      break;
    case RED_GREEN:
      lcd_DrawCircle(60, 120, BRED, 30, 0);
      lcd_DrawCircle(60, 190, BRED, 30, 0);
      lcd_DrawCircle(60, 260, RED, 30, 1);
      lcd_DrawCircle(160, 120, GREEN, 30, 1);
      lcd_DrawCircle(160, 190, BRED, 30, 0);
      lcd_DrawCircle(160, 260, BRED, 30, 0);
      break;
    case RED_YELLOW:
      lcd_DrawCircle(60, 120, BRED, 30, 0);
      lcd_DrawCircle(60, 190, BRED, 30, 0);
      lcd_DrawCircle(60, 260, RED, 30, 1);
      lcd_DrawCircle(160, 120, BRED, 30, 0);
      lcd_DrawCircle(160, 190, YELLOW, 30, 1);
      lcd_DrawCircle(160, 260, BRED, 30, 0);
      break;
  }
}


void normal_mode_traffic_light() {
  static int state = GREEN_RED;
  lcd_ShowStr(20, 30, "NORMAL MODE", WHITE, RED, 24, 0);
  if (normal_count == 0) {
	lcd_Clear(WHITE);
    draw_light(state);
    switch (state) {
      case GREEN_RED:
    	  normal_count = GREEN_TIME;
        state = YELLOW_RED;
        break;
      case YELLOW_RED:
    	  normal_count = YELLOW_TIME;
        state = RED_GREEN;
        break;
      case RED_GREEN:
    	  normal_count = GREEN_TIME;
        state = RED_YELLOW;
        break;
      case RED_YELLOW:
    	  normal_count = YELLOW_TIME;
        state = GREEN_RED;
        break;
    }
  }
  normal_count--;
}

void blink_light(int light_type) {
  static int count = 0;
  static int toggle = 0;
  if (count % 10 == 0) {
	toggle = 1 - toggle;
	draw_light(99);
	if (toggle == 1) {
		switch (light_type) {
			  case 1:
				lcd_DrawCircle(60, 260, RED, 30, 1);
				lcd_DrawCircle(160, 260, RED, 30, 1);
				break;
		      case 2:
		        lcd_DrawCircle(60, 120, GREEN, 30, 1);
		        lcd_DrawCircle(160, 120, GREEN, 30, 1);
		        break;
		      case 3:
		        lcd_DrawCircle(60, 190, YELLOW, 30, 1);
		        lcd_DrawCircle(160, 190, YELLOW, 30, 1);
		        break;
		    }
	} else {
		switch (light_type) {
			  case 1:
				lcd_DrawCircle(60, 260, WHITE, 30, 1);
				lcd_DrawCircle(160, 260, WHITE, 30, 1);
				break;
		      case 2:
		        lcd_DrawCircle(60, 120, WHITE, 30, 1);
		        lcd_DrawCircle(160, 120, WHITE, 30, 1);
		        break;
		      case 3:
		        lcd_DrawCircle(60, 190, WHITE, 30, 1);
		        lcd_DrawCircle(160, 190, WHITE, 30, 1);
		        break;
		    }
	}

  }
  count++;
}
void modify_mode_traffic_light() {
  switch (mode) {
    case 1:
      if (button_count[1] == 1) {
        temp_red_time+=20;
      } else if (button_count[2] == 1) {
        RED_TIME = temp_red_time;
      } else if (button_count[0] == 1) {
        temp_red_time = RED_TIME;
        temp_green_time = GREEN_TIME;
        temp_yellow_time = YELLOW_TIME;
      }
      break;
    case 2:
      if (button_count[1] == 1) {
        temp_green_time+=20;
      } else if (button_count[2] == 1) {
        GREEN_TIME = temp_green_time;
      } else if (button_count[0] == 1) {
    	  temp_red_time = RED_TIME;
    	          temp_green_time = GREEN_TIME;
    	          temp_yellow_time = YELLOW_TIME;
      }
      break;
    case 3:
      if (button_count[1] == 1) {
        temp_yellow_time+=20;
      } else if (button_count[2] == 1) {
        YELLOW_TIME = temp_yellow_time;
      } else if (button_count[0] == 1) {
    	  temp_red_time = RED_TIME;
    	          temp_green_time = GREEN_TIME;
    	          temp_yellow_time = YELLOW_TIME;
      }
      break;
  }
  lcd_ShowStr(20, 10, "MODIFICATION MODE", WHITE, RED, 16, 0);
  lcd_ShowStr(20, 30, "RED TIME:", WHITE, RED, 16, 0);
  lcd_ShowIntNum(140, 30, temp_red_time/20, 2, BRED, WHITE, 16);
  lcd_ShowStr(20, 50, "YELLOW TIME:", WHITE, RED, 16, 0);
  lcd_ShowIntNum(140, 50, temp_yellow_time/20, 2, BRED, WHITE, 16);
  lcd_ShowStr(20, 70, "GREEN TIME:", WHITE, RED, 16, 0);
  lcd_ShowIntNum(140, 70, temp_green_time/20, 2, BRED, WHITE, 16);
  blink_light(mode);
}

void Traffic_Light() {
  if (mode == 0) {
    normal_mode_traffic_light();
  } else {
    modify_mode_traffic_light();
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
  __disable_irq();
  while (1)
  {
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
