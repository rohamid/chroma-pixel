/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "fatfs.h"
#include "sdmmc.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <string.h>
#include "usbd_cdc_if.h"
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
SD_HandleTypeDef hsd1;
char txBuffer[300];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */
static void SDIO_SDCard_Test();
static FRESULT list_files(const char *path);

static void usb_cdc_printf(char *txtStr) {
	while(CDC_Transmit_HS((uint8_t*)txtStr, strlen(txtStr)) == USBD_BUSY) {

	}
}

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

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

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
  MX_USB_DEVICE_Init();
  MX_SDMMC2_SD_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */

  HAL_Delay(5000);
  SDIO_SDCard_Test();
//  if(list_files("") != FR_OK) {
//	  sprintf(txBuffer, "Error listing files\r\n");
//	  usb_cdc_printf(txBuffer);
//	  while(1);
//  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  usb_cdc_printf("Hello World!..\r\n");
	  HAL_Delay(1000);
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 10;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

static FRESULT list_files(const char *path) {
	FRESULT fRes;
	DIR dir;
	FILINFO fno;
	int nFile, nDir;

	fRes = f_opendir(&dir, path);	// Open the directory
	if(fRes == FR_OK) {
		nFile = nDir = 0;
		for(;;) {
			fRes = f_readdir(&dir, &fno);					// Read a directory item
			if(fRes != FR_OK || fno.fname[0] == 0) break;	// Error or end of dir
			if(fno.fattrib & AM_DIR) {
				sprintf(txBuffer, "    <DIR>    %s\r\n", fno.fname);
				usb_cdc_printf(txBuffer);
				nDir++;
			} else {
				sprintf(txBuffer, "%.1fMB %s   ", (float)fno.fsize / 1000000.0, fno.fname);
				usb_cdc_printf(txBuffer);

				sprintf(txBuffer, "Timestamp: %u-%02u-%02u, %02u:%02u  ",
						(fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31,
						fno.ftime >> 11, fno.ftime >> 5 & 63);
				usb_cdc_printf(txBuffer);

				sprintf(txBuffer, "Attributes: %c%c%c%c%c\r\n",
						(fno.fattrib & AM_DIR) ? 'D' : '-',
						(fno.fattrib & AM_RDO) ? 'R' : '-',
						(fno.fattrib & AM_HID) ? 'H' : '-',
						(fno.fattrib & AM_SYS) ? 'S' : '-',
						(fno.fattrib & AM_ARC) ? 'A' : '-');
				usb_cdc_printf(txBuffer);
				nFile++;
			}
		}
		f_closedir(&dir);
		sprintf(txBuffer, "%d dirs, %d files.\r\n", nDir, nFile);
		usb_cdc_printf(txBuffer);
	} else {
		sprintf(txBuffer, "Failed to open \"%s\". (%u)\r\n", path, fRes);
		usb_cdc_printf(txBuffer);
	}

	return fRes;
}

static void SDIO_SDCard_Test() {
	FATFS fatFS;
	FIL fil;
	FRESULT frStatus;
	FATFS *fsPtr;
	UINT RWC, WWC;	// Read/Write Word Counter
	DWORD freeCluster;
	uint32_t totalSize, freeSpace;
	char rwBuffer[200];
	do {
		// Mount the sd card
		frStatus = f_mount(&fatFS, SDPath, 1);
		if(frStatus != FR_OK) {
			sprintf(txBuffer, "Error! while mounting SD card, Error code: (%i)\r\n", frStatus);
			usb_cdc_printf(txBuffer);
			break;
		}
		sprintf(txBuffer, "SD card mounted successfully! \r\n\n");
		usb_cdc_printf(txBuffer);

		// Get & print the SD card size & free space
		f_getfree("", &freeCluster, &fsPtr);
		totalSize = (uint32_t)((fsPtr->n_fatent - 2) * fsPtr->csize * 0.5);
		freeSpace = (uint32_t)(freeCluster * fsPtr->csize * 0.5);
		sprintf(txBuffer, "Total SD card size: %lu Bytes\r\n", totalSize);
		usb_cdc_printf(txBuffer);
		sprintf(txBuffer, "Free SD card space: %lu Bytes\r\n\n", freeSpace);
		usb_cdc_printf(txBuffer);
	} while(0);
	list_files("");
	// Test complete! unmount SD the card
	frStatus = f_mount(NULL, "", 0);
	if(frStatus != FR_OK) {
		sprintf(txBuffer, "\r\nError! while un-mounting SD Card, Error Code: (%i)\r\n", frStatus);
		usb_cdc_printf(txBuffer);
	} else {
		sprintf(txBuffer, "\r\nSD Card un-mounted successfully! \r\n");
		usb_cdc_printf(txBuffer);
	}
}

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

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
