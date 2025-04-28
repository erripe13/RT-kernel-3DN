/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "shell.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define STACK_SIZE 256
#define TASK1_PRIORITY 1
#define TASK2_PRIORITY 1
#define SHELL_TASK_PRIORITY 1
#define QUEUE_LENGTH 10
#define QUEUE_WAIT_TIME pdMS_TO_TICKS(100)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
QueueHandle_t xQueue;
SemaphoreHandle_t xMutex;
TaskHandle_t blinkTaskHandle;
TickType_t ledPeriod = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
void BlinkTask(void *pvParameters);
void TaskShell(void *pvParameters);
int fonction(int argc, char **argv);
int led(int argc, char **argv);
int spam(int argc, char **argv);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int __io_putchar(int ch) {
	HAL_UART_Transmit(&huart1, (uint8_t*) &ch, 1, HAL_MAX_DELAY);
	return ch;
}

char uart_read() {
	char c;
	HAL_UART_Receive(&huart1, (uint8_t*)&c, 1, HAL_MAX_DELAY);
	vTaskDelay(pdMS_TO_TICKS(1));
	return c & 0x7F;
}


int fonction(int argc, char **argv) {
    printf("Appel de fonction avec %d argument(s) :\r\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("arg[%d] = %s\r\n", i, argv[i]);
    }
    return 0;
}

int led(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage : led <periode_ms>\r\n");
        return -1;
    }

    int periode = atoi(argv[1]);
    if (periode < 0) periode = 0;

    ledPeriod = (TickType_t) periode;
    printf("Période de clignotement mise à %lu ms\r\n", ledPeriod);
    return 0;
}

int spam(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage : spam <message> <nb>\r\n");
        return -1;
    }

    int count = atoi(argv[2]);
    for (int i = 0; i < count; i++) {
        printf("[%d] %s\r\n", i, argv[1]);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    return 0;
}

void BlinkTask(void *pvParameters) {
	while (1) {
		if (ledPeriod > 0) {
			HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_1);
			vTaskDelay(pdMS_TO_TICKS(ledPeriod));
		} else {
			vTaskDelay(pdMS_TO_TICKS(100));
		}
	}
}

void TaskShell(void *pvParameters) {
	shell_init();
	shell_add('f', fonction, "Fonction test shell");
	shell_add('l', led, "LED clignotement (ex: led 500)");
	shell_add('s', spam, "Affiche un message N fois");
	shell_run();  // Bloquant, mais compatible via uart_read() patché
}
/* USER CODE END 0 */

int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  HAL_Init();
  SystemClock_Config();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  MX_GPIO_Init();
  MX_USART1_UART_Init();

  /* USER CODE BEGIN 2 */
  xMutex = xSemaphoreCreateMutex();
  configASSERT(xMutex != NULL);

  xQueue = xQueueCreate(QUEUE_LENGTH, sizeof(TickType_t));
  configASSERT(xQueue != NULL);

  BaseType_t ret;
  ret = xTaskCreate(BlinkTask, "BlinkTask", STACK_SIZE, NULL, 2, &blinkTaskHandle);
  configASSERT(ret == pdPASS);

  ret = xTaskCreate(TaskShell, "Shell", STACK_SIZE, NULL, SHELL_TASK_PRIORITY, NULL);
  configASSERT(ret == pdPASS);

  vTaskStartScheduler();
  /* USER CODE END 2 */

  MX_FREERTOS_Init();
  osKernelStart();

  /* USER CODE BEGIN WHILE */
  while (1) {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK) {
    Error_Handler();
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM1) {
		HAL_IncTick();
	}
}

void Error_Handler(void)
{
  __disable_irq();
  while (1) {
  }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif
