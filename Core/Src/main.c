/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "gpio.h"
#include "tim.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "u8g2_hal.h"  // 包含您的U8g2头文件
/* USER CODE END Includes */
#define TIMx TIM1  // 假设使用TIM1作为延时定时器
/* USER CODE BEGIN Includes */
 void Tims_delay_us(uint32_t us);
 // 假设使用STM32的定时器实现微秒延时
void Tims_delay_us(uint32_t us)
{
    uint32_t start = TIM1->CNT;  // 使用具体的定时器寄存器
    while((TIM1->CNT - start) < us);
}
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
// 显示模式枚举
typedef enum {
    MODE_DEMO = 0,
    MODE_STUDENT_INFO,
    MODE_SCROLL,
    MODE_ANIMATION,
    MODE_MAX
} DisplayMode_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
u8g2_t u8g2;
DisplayMode_t current_mode = MODE_DEMO;
uint32_t last_mode_change = 0;
uint32_t last_update = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
// 自定义显示函数
void ShowStudentInfo(u8g2_t *u8g2);
void ShowScrollText(u8g2_t *u8g2);
void ShowAnimation(u8g2_t *u8g2);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// 显示学号和姓名
void ShowStudentInfo(u8g2_t *u8g2)
{
    u8g2_ClearBuffer(u8g2);
    
    u8g2_SetFont(u8g2, u8g2_font_7x14_tf);
    u8g2_DrawStr(u8g2, 0, 15, "Student ID:");
    u8g2_DrawStr(u8g2, 0, 35, "632307030201"); // 替换为您的学号
    
    u8g2_SetFont(u8g2, u8g2_font_7x14_tf);
    u8g2_DrawStr(u8g2, 0, 55, "Name: wyx"); // 替换为您的姓名
    
    u8g2_SendBuffer(u8g2);
}

// 滚动显示
void ShowScrollText(u8g2_t *u8g2)
{
    static int16_t x_pos = 128;
    static const char *scroll_text = "Scrolling Text Demo - STM32 OLED Display ";
    
    u8g2_ClearBuffer(u8g2);
    
    u8g2_SetFont(u8g2, u8g2_font_6x10_tf);
    u8g2_DrawStr(u8g2, x_pos, 32, scroll_text);
    
    x_pos -= 2;
    if(x_pos < -300)
        x_pos = 128;
    
    u8g2_SendBuffer(u8g2);
}

// 简单动画
void ShowAnimation(u8g2_t *u8g2)
{
    static uint8_t ball_x = 20;
    static uint8_t ball_y = 20;
    static int8_t ball_dx = 2;
    static int8_t ball_dy = 2;
    
    u8g2_ClearBuffer(u8g2);
    
    // 绘制边界
    u8g2_DrawFrame(u8g2, 0, 0, 128, 64);
    
    // 绘制移动的小球
    u8g2_DrawDisc(u8g2, ball_x, ball_y, 5, U8G2_DRAW_ALL);
    
    // 更新位置
    ball_x += ball_dx;
    ball_y += ball_dy;
    
    // 边界检测
    if (ball_x <= 6 || ball_x >= 122) ball_dx = -ball_dx;
    if (ball_y <= 6 || ball_y >= 58) ball_dy = -ball_dy;
    
    u8g2_SetFont(u8g2, u8g2_font_6x10_tf);
    u8g2_DrawStr(u8g2, 40, 10, "Animation");
    
    u8g2_SendBuffer(u8g2);
}
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
  MX_I2C2_Init();  // 注意：这里使用I2C2，根据您的配置
  /* USER CODE BEGIN 2 */
  // 初始化U8g2
  u8g2Init(&u8g2);
  
  // 显示启动信息
  u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
  u8g2_DrawStr(&u8g2, 10, 20, "OLED Init OK!");
  u8g2_DrawStr(&u8g2, 10, 35, "STM32F103");
  u8g2_SendBuffer(&u8g2);
  HAL_Delay(2000);
  
  // 显示U8g2 Logo
  draw(&u8g2);
  HAL_Delay(2000);
  
  // 填充屏幕测试
  testDrawPixelToFillScreen(&u8g2);
  HAL_Delay(1000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    uint32_t current_time = HAL_GetTick();
    
    // 每5秒切换显示模式
    if(current_time - last_mode_change > 5000)
    {
        current_mode = (current_mode + 1) % MODE_MAX;
        last_mode_change = current_time;
    }
    
    // 根据模式更新显示
    switch(current_mode)
    {
    case MODE_DEMO:
        if(current_time - last_update > 2000)
        {
            draw(&u8g2);
            last_update = current_time;
        }
        break;
        
    case MODE_STUDENT_INFO:
        if(current_time - last_update > 2000)
        {
            ShowStudentInfo(&u8g2);
            last_update = current_time;
        }
        break;
        
    case MODE_SCROLL:
        if(current_time - last_update > 50)
        {
            ShowScrollText(&u8g2);
            last_update = current_time;
        }
        break;
        
    case MODE_ANIMATION:
        if(current_time - last_update > 100)
        {
            ShowAnimation(&u8g2);
            last_update = current_time;
        }
        break;
    }
    
    HAL_Delay(10);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
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
}

/* USER CODE BEGIN 4 */

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
