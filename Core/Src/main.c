/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mcu.h"
#include "AVR_Programmer.h"

#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ssd1306_tests.h"

#include <stdbool.h>
#include <string.h>

#include "encBtn.h"

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
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi2_rx;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

void ssd1306_WriteMultiLineString(char* str, uint8_t x, uint8_t y) {
    uint8_t start_x = x;

    ssd1306_SetCursor(x, y);

    while (*str) {
        if (*str == '\n') {
            x = start_x;        // Возврат к началу строки
            y += 10;            // Смещение вниз на высоту шрифта + интервал
            ssd1306_SetCursor(x, y);
        }
        else {
            if (ssd1306_WriteChar(*str, Font_6x8, White) != *str) {
                return;
            }
            x += 6;
        }
        str++;
    }
}

void temp_callback(uint16_t maxVal, uint16_t curVal){
  static uint8_t oldProcent = 255;
  static uint8_t procent;
  static char buf[4];

  if (curVal > maxVal) curVal = maxVal; 
  procent =  (curVal * 100) / maxVal;

  if(procent % 10 == 0 && procent != oldProcent){
	DEBUG_PRINTF("PROGRESS: %d\n", procent);
    oldProcent = procent;
    
    if (procent == 100) {
        buf[0] = '1';
        buf[1] = '0';
        buf[2] = '0';
        buf[3] = '\0';
    } else if (procent == 0) {
        buf[0] = '0';
        buf[1] = '\0';
    } else {
        buf[0] = (procent / 10) + '0';
        buf[1] = '0';
        buf[2] = '\0';
    }

    ssd1306_DrawRectangle(13, 15, 114, 35, White);

	ssd1306_FillRectangle(13, 15, 13 + procent, 35, White);
	ssd1306_SetCursor(50, 40);

	ssd1306_WriteString(buf, Font_11x18, White);
	ssd1306_WriteString("%", Font_11x18, White);

    ssd1306_UpdateScreen();
  }
}
void temp_err_cb(const char* err_type, char* message){
  DEBUG_PRINTF("\n");
  DEBUG_PRINTF(err_type);
  DEBUG_PRINTF(message);
  DEBUG_PRINTF("\n");

  ssd1306_Fill(Black);
  ssd1306_FillRectangle(10, 0, 117, 10, White);
  ssd1306_SetCursor(35, 2);
  ssd1306_WriteString(err_type, Font_6x8, Black);

  ssd1306_WriteMultiLineString(message, 10, 20);
  ssd1306_UpdateScreen();


}
void temp_sucs_cb(){
	DEBUG_PRINTF("SUCCESS FINISHED!\n");
	ssd1306_Fill(Black);
	ssd1306_SetCursor(20, 25);
	ssd1306_WriteString("SACCESS!!", Font_11x18, White);
	ssd1306_UpdateScreen();
	HAL_Delay(500);
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

FATFS fileSystem;
FIL firmware;

button_t btnOK;
encoder_t encoder;

static const avp_init_t avrprog = {
	.prog_cb = temp_callback,
	.err_cb = &temp_err_cb,
	.sucs_cb = &temp_sucs_cb,
	.hspi = &hspi2,
	.CS_Pin = AVR_PROG_SPI_SS_Pin,
	.CS_Port = AVR_PROG_SPI_SS_GPIO_Port
};

avp_param_t param;
avp_spi_conf spiConf = {
	.sck_auto = false,
	.sck_div = SPI_BAUDRATEPRESCALER_64
};
avp_action avpAction;

typedef enum{
	MAIN_SCREEN,
	SET_MCU,
	SET_ACTION,
	SET_FILE,
	ACTION
}menu_t;

menu_t menuState = MAIN_SCREEN;
bool firstInState = 1;

char* const FILE_LIST[] = {
  "BL_1.BIN",
  "EMPTY.BIN",
  "LCD_EXAMPLE.BIN",
  "READ.BIN"
};

#define FILE_LIST_NUM (sizeof(FILE_LIST) / sizeof(FILE_LIST[0]))

void changeMenu(menu_t state){
	DEBUG_PRINTF("CHANGE MENU STATE: %d -> %d\n", (uint8_t)menuState, (uint8_t)state);
	menuState = state;
	firstInState = 1;
}

void menuTick(){
	static char mcuName[16];
	static bool updateScreen;
	static uint8_t cursor;
	static uint8_t topIndx;

	switch(menuState){
	case MAIN_SCREEN:
		if(firstInState){
			firstInState = 0;
			ssd1306_Fill(Black);
			ssd1306_SetCursor(30, 10);
			ssd1306_WriteString("lUprog", Font_11x18, White);
			ssd1306_FillRectangle(10, 35, 117, 60, White);
			ssd1306_SetCursor(35, 39);
			ssd1306_WriteString("START", Font_11x18, Black);
			ssd1306_UpdateScreen();
		}
		if(btnIsClick(&btnOK)) changeMenu(SET_ACTION);
		break;

	case SET_ACTION:
		if(firstInState){
			firstInState = 0;
			updateScreen = 1;
			cursor = 0;

			ssd1306_Fill(Black);
			ssd1306_FillRectangle(10, 0, 117, 10, White);
			ssd1306_SetCursor(35, 2);
			ssd1306_WriteString("SET ACTION", Font_6x8, Black);
			ssd1306_SetCursor(18, 18);
			ssd1306_WriteString("READ", Font_6x8, White);
			ssd1306_SetCursor(18, 30);
			ssd1306_WriteString("WRITE", Font_6x8, White);
			ssd1306_SetCursor(18, 42);
			ssd1306_WriteString("VERIFY", Font_6x8, White);
			ssd1306_UpdateScreen();
		}

		if(updateScreen){
		  updateScreen = 0;
		  ssd1306_FillRectangle(0, 12, 16, 127, Black);

		  switch(cursor){
			case 0:
			  ssd1306_SetCursor(10, 18);
			  avpAction = ACT_FL_READ;
			  break;
			case 1:
			  ssd1306_SetCursor(10, 30);
			  avpAction = ACT_FL_WRITE;
			  break;

			case 2:
			  ssd1306_SetCursor(10, 42);
			  avpAction = ACT_FL_VERIFY;
			  break;
		  }
		  ssd1306_WriteString(">", Font_6x8, White);
		  ssd1306_UpdateScreen();
		}

		if(btnIsClick(&btnOK)){
			changeMenu(SET_MCU);
			DEBUG_PRINTF("SET_ACTION: %d\n", (uint8_t) avpAction);
		}

		if(encIsCW(&encoder) && cursor < 2){
		  cursor++;
		  updateScreen = 1;
		}
		if(encIsCCW(&encoder) && cursor > 0){
		  cursor--;
		  updateScreen = 1;
		}
		break;

	case SET_MCU:
		if(firstInState){
			firstInState = 0;
			openChipList();
			updateScreen = 1;

			ssd1306_Fill(Black);
			ssd1306_FillRectangle(10, 0, 117, 10, White);
			ssd1306_SetCursor(35, 2);
			ssd1306_WriteString("SELECT MCU", Font_6x8, Black);

		}
		if(updateScreen){
			updateScreen = 0;
			param.mcu = getChip();
			memcpy(mcuName, param.mcu->chip_name, sizeof(param.mcu->chip_name));
			mcuName[15] = 0;

			ssd1306_FillRectangle(0, 25, 127, 43, Black);
			ssd1306_SetCursor(10, 25);
			ssd1306_WriteString(">", Font_11x18, White);
			ssd1306_SetCursor(25, 30);
			ssd1306_WriteString(mcuName, Font_6x8, White);
			ssd1306_UpdateScreen();
		}
		if(btnIsClick(&btnOK)){
			closeChipList();
			changeMenu(SET_FILE);
			DEBUG_PRINTF("SET MCU: ");
			DEBUG_PRINTF(mcuName);
			DEBUG_PRINTF("\n");
		}
		if(encIsCW(&encoder)) {
			incChip();
			updateScreen = 1;
		}
		if(encIsCCW(&encoder)) {
			decChip();
			updateScreen = 1;
		}

		break;

	case SET_FILE:

		if(firstInState){
			firstInState = 0;
			updateScreen = 1;
			topIndx = 0;
			cursor = 0;

			ssd1306_Fill(Black);
			ssd1306_FillRectangle(3, 0, 117, 10, White);
			ssd1306_SetCursor(40, 2);
			ssd1306_WriteString("SET FILE", Font_6x8, Black);
			ssd1306_UpdateScreen();
		}
		if(updateScreen){
		  updateScreen = 0;
		  ssd1306_FillRectangle(0, 12, 127, 63, Black);

		  for(uint8_t i = 0; i < 3; i++){
			  int fileIdx = topIndx + i;
			  if(fileIdx < FILE_LIST_NUM){
                  ssd1306_SetCursor(18, 18 + (i * 12));
                  ssd1306_WriteString(FILE_LIST[fileIdx], Font_6x8, White);
                  if(i == (cursor - topIndx)){
                	  ssd1306_SetCursor(10, 18 + (i * 12));
                	  ssd1306_WriteString(">", Font_6x8, White);
                  }
			  }
		  }
		  ssd1306_UpdateScreen();
		}
	    if(btnIsClick(&btnOK)){
	    	param.path = FILE_LIST[cursor];
	    	DEBUG_PRINTF("SET FILE: ");
	    	DEBUG_PRINTF(FILE_LIST[cursor]);
	    	DEBUG_PRINTF("\n");
			changeMenu(ACTION);
		}

	    if(encIsCW(&encoder) && cursor < FILE_LIST_NUM - 1){
	      cursor++;
	      updateScreen = 1;
	      if(cursor - topIndx >= 3) topIndx++;
	    }
	    if(encIsCCW(&encoder) && cursor > 0){
	      cursor--;
	      updateScreen = 1;
	      if(cursor < topIndx) topIndx--;
	    }
		break;

	case ACTION:
		ssd1306_Fill(Black);
		ssd1306_SetCursor(40, 23);
		ssd1306_WriteString("GO!!!", Font_11x18, White);
		ssd1306_UpdateScreen();
		HAL_Delay(500);

		ssd1306_Fill(Black);
		ssd1306_UpdateScreen();

		AVP_Execute(avpAction, &param);

		while(!btnIsClick(&btnOK)) btnTick(&btnOK);
		changeMenu(MAIN_SCREEN);

		break;
	}
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
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_FATFS_Init();
  MX_I2C1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  FATFS fs;
  FRESULT fres;

  DEBUG_INIT(&huart1);
  DEBUG_PRINTF(" \n");

  init_btn(&btnOK,BTN_OK_GPIO_Port, BTN_OK_Pin, true, 1);
  init_enc(&encoder, &htim3);

  ssd1306_Init();
  ssd1306_SetContrast(255);
  ssd1306_Fill(Black);
  ssd1306_UpdateScreen();


  if(!AVP_Init(&avrprog)){
    DEBUG_PRINTF("AVR Programmer not init!");
    while(1);
  }

  fres = f_mount(&fs, "", 1);
  if(fres != FR_OK){
	 DEBUG_PRINTF("SD Card mount error!");
  ssd1306_SetCursor(0, 26);
  ssd1306_WriteString("SD_MOUNT_ER", Font_11x18, White);
  ssd1306_UpdateScreen();

	 while(1);
  }
  DEBUG_PRINTF("SD Card mounted\n");

  AVP_Set_SPI(&spiConf);


//  AVP_Execute(ACT_FL_WRITE, &param); // Пишем
//
//  AVP_Execute(ACT_FL_VERIFY, &param); // Верификация
//
//  param.path = "READ.BIN";
//  AVP_Execute(ACT_FL_READ, &param); // Читение

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		btnTick(&btnOK);
		encTick(&encoder);
		menuTick();
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
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 39999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 10;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : SD_CS_Pin */
  GPIO_InitStruct.Pin = SD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SD_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : AVR_PROG_SPI_SS_Pin */
  GPIO_InitStruct.Pin = AVR_PROG_SPI_SS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(AVR_PROG_SPI_SS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BTN_OK_Pin */
  GPIO_InitStruct.Pin = BTN_OK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BTN_OK_GPIO_Port, &GPIO_InitStruct);

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
	while (1) {
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
