/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "crc.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "fsmc.h"
#include "app_touchgfx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <math.h>
#include "ili9486.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
volatile SystemMode systemMode = MODE_SETUP;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

// General variables
uint32_t i = 0; // Counter


// DAC
uint16_t V_DAC[N_DAC_voltage]; // Values of the generator

volatile float V_AC;
volatile float V_DC;
volatile float I_DC;
int Phase_shift[5]; // 5 channels for the generator
int V_DAC_phase[5];

void DAC8568_Write(uint16_t value, uint8_t channel)
{
    uint32_t frame;

    frame = ((uint32_t)0x3 << 24) |
			((uint32_t)(channel - 1) << 20) |
			((uint32_t)value << 4);


    uint8_t txData[4];

	txData[0] = (frame >> 24) & 0xFF;
	txData[1] = (frame >> 16) & 0xFF;
	txData[2] = (frame >> 8)  & 0xFF;
	txData[3] = frame & 0xFF;


    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, txData, 4, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}

void DAC8568_Init_SineTable(void)
{
    for (int i = 0; i < N_DAC_voltage; i++)
    {
        float x = sinf(2.0f * 3.14159265f * i / N_DAC_voltage);
        V_DAC[i] = (uint16_t)(32767.5f + 32767.5f * x / 10.0f); // 32767.5f - 0 V, 10.0f - maximum output voltage
    }
}


// ADC

int16_t ADC_data;
volatile float V_ADC[N_ADC_channels];

volatile float V_ADC_max = 3.0f;


// Display
uint8_t ShowImage = 0;


// TIM1
void usDelay(uint16_t useconds)
{
    __HAL_TIM_SET_COUNTER(&htim1, 0);

    while (__HAL_TIM_GET_COUNTER(&htim1) < useconds)
    {
    }
}

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void mainApp(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern void touchgfxSignalVSync(void);
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
  MX_SPI1_Init();
  MX_SPI3_Init();
  MX_TIM1_Init();
  MX_FSMC_Init();
  MX_CRC_Init();
  MX_TouchGFX_Init();
  /* USER CODE BEGIN 2 */

  // Starting the timer
  HAL_TIM_Base_Start(&htim1);


  // Initial states for SPI1 (DAC)

  // Grounding all the output channels
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  DAC8568_Write(32767.5f, 16); // Channel #16 leads to write to all outputs
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

  // Initialization of the voltage table
  DAC8568_Init_SineTable();


  // Initial states for SPI3 (ADC)
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET); // CB High
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET); // CS Low


  // Initial states for the display

  ili9486_Init();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	if (systemMode == MODE_SETUP) // Reading the ADC signals and updating the display
	{
		// ADC
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET); // CB Low
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET); // CB High
		usDelay(10);

		V_AC = 10.0f/V_ADC_max*V_ADC[1];

		V_DC = V_ADC[2];
		I_DC = V_ADC[3];

		Phase_shift[0] = (int)(180.0f/V_ADC_max/2*(V_ADC[4] - V_ADC_max));
		Phase_shift[1] = (int)(180.0f/V_ADC_max/2*(V_ADC[5] - V_ADC_max));
		Phase_shift[2] = (int)(180.0f/V_ADC_max/2*(V_ADC[6] - V_ADC_max));
		Phase_shift[3] = (int)(180.0f/V_ADC_max/2*(V_ADC[7] - V_ADC_max));
		Phase_shift[4] = (int)(180.0f/V_ADC_max/2*(V_ADC[0] - V_ADC_max));


		// Display
		touchgfxSignalVSync();
		MX_TouchGFX_Process();
	}
	else // Generating the voltage
	{

		if (ShowImage == 1)
		{
			touchgfxSignalVSync();
			MX_TouchGFX_Process();
		}

		// DAC
		V_DAC_phase[0] = (i + (Phase_shift[0] * N_DAC_voltage) / 360) % N_DAC_voltage;
		V_DAC_phase[1] = (i + (Phase_shift[1] * N_DAC_voltage) / 360) % N_DAC_voltage;
		V_DAC_phase[2] = (i + (Phase_shift[2] * N_DAC_voltage) / 360) % N_DAC_voltage;
		V_DAC_phase[3] = (i + (Phase_shift[3] * N_DAC_voltage) / 360) % N_DAC_voltage;
		V_DAC_phase[4] = (i + (Phase_shift[4] * N_DAC_voltage) / 360) % N_DAC_voltage;

		DAC8568_Write(V_AC*V_DAC[V_DAC_phase[0]], 2); // Channel B
		DAC8568_Write(V_AC*V_DAC[V_DAC_phase[1]], 4); // Channel D
		DAC8568_Write(V_AC*V_DAC[V_DAC_phase[2]], 6); // Channel F
		DAC8568_Write(V_AC*V_DAC[V_DAC_phase[3]], 8); // Channel H
		DAC8568_Write(V_AC*V_DAC[V_DAC_phase[4]], 7); // Channel G

		i++;
		if (i >= N_DAC_voltage)
		  i = 0;

	}

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

	// ADC interruption for BUST falling edge
    if (GPIO_Pin == ADC_BUSY_Pin)
    {
        // CS LOW
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);

        // Read 8 × 16 bits from DOUTA
        for (int i = 0; i < N_ADC_channels; i++)
        {
            HAL_SPI_Receive(&hspi3,
                            (uint8_t *)&ADC_data,
                            1,
                            HAL_MAX_DELAY);

            V_ADC[i] = (float)ADC_data * 5.0f / 32768.0f;
        }

        // CS HIGH
        usDelay(5);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
    }

    // Blue button
    if (GPIO_Pin == BUTTON_Pin)
    {
        if (systemMode == MODE_SETUP)
        {
            systemMode = MODE_RUN;
        }
        else
        {
            systemMode = MODE_SETUP;
        }
        ShowImage = 1;
    }
}


// Display TouchGFX function



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
#ifdef USE_FULL_ASSERT
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
