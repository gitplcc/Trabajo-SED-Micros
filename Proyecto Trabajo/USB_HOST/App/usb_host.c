/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file            : usb_host.c
  * @version         : v1.0_Cube
  * @brief           : This file implements the USB Host
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

#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_msc.h"
#include "ff.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USB Host core handle declaration */
USBH_HandleTypeDef hUsbHostFS;
ApplicationTypeDef Appli_state = APPLICATION_IDLE;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */
#define MAX_MUSIC 30
FATFS *fs;
FRESULT result;
const uint8_t oktext[] = "USB mounted !";
const uint8_t etext[] = "USB not mounted !";
static uint8_t isMounted = 0;
static uint8_t isIndexed = 0;
DIR dir;
FILINFO finfo;
char* mp3Files[MAX_MUSIC];

/* USER CODE END 0 */

/*
 * user callback declaration
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

int scanMp3Music(void)
{
	if (f_opendir(&dir, "/") == FR_OK) //abrimos el directorio raiz
	{
		int numf = 0; //para contar numero de archibos mp3
		while(f_readdir(&dir, &finfo) == FR_OK && finfo.fname[0] != 0 && numf < MAX_MUSIC) //mientras queden archivos por leer
		{
			if(!(finfo.fattrib & AM_DIR) && strstr(finfo.fname, ".mp3") != NULL) //si el archivo es un directorio y el nombre del archivo tiene una subcadena .mp3
			{
				mp3Files[numf] = finfo.fname;	//guardamos el nombre del archivo en un vector de chars, en el elemento de numero actual encontrado
				numf++;	//aumentamos el numero de archivos mp3 encontrados
				printf("Se han encontrado &d canciones.\n", numf);
			}
		}
		f_close(&dir);
		printf("Se ha podido abrir el directorio.\n");
		return 1;
	}
	else
	{
		printf("Error al abrir el directorio.\n");
		return 0;
	}
}

/* USER CODE END 1 */

/**
  * Init USB host library, add supported class and start the library
  * @retval None
  */
void MX_USB_HOST_Init(void)
{
  /* USER CODE BEGIN USB_HOST_Init_PreTreatment */

  /* USER CODE END USB_HOST_Init_PreTreatment */

  /* Init host Library, add supported class and start the library. */
  if (USBH_Init(&hUsbHostFS, USBH_UserProcess, HOST_FS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_RegisterClass(&hUsbHostFS, USBH_MSC_CLASS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_Start(&hUsbHostFS) != USBH_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_HOST_Init_PostTreatment */

  /* USER CODE END USB_HOST_Init_PostTreatment */
}

/*
 * Background task
 */
void MX_USB_HOST_Process(void)
{
  /* USB Host Background task */
  USBH_Process(&hUsbHostFS);
}
/*
 * user callback definition
 */
static void USBH_UserProcess  (USBH_HandleTypeDef *phost, uint8_t id)
{
  /* USER CODE BEGIN CALL_BACK_1 */
  switch(id)
  {
  case HOST_USER_SELECT_CONFIGURATION:
  break;

  case HOST_USER_DISCONNECTION:
  Appli_state = APPLICATION_DISCONNECT;
  break;

  case HOST_USER_CLASS_ACTIVE:
  Appli_state = APPLICATION_READY;
  break;

  case HOST_USER_CONNECTION:
  Appli_state = APPLICATION_START;
  break;

  default:
  break;
  }

  /* TODO : Aqui va el codigo de buscar ficheros en el pen */

  //primero montamos el sistema de ficheros
  if (isMounted != 1) //solo se va a montar el sistema de ficheros una vez ya que no lo vamos a modificar
  {
	  result = f_mount(fs, "", 1); //puntero al objeto filesystem, direccion (default = raiz), 0 para no montar ahora (cuando se haga el primer acceso), 1 para forzarlo y ver si esta listo para trabajar.
	  if (result != FR_OK)
	    {
	  	  printf("Coudnt mount the file sistem for the USB.\n");
	  	  //HAL_UART_Transmit(&huart3, etext, sizeof(text), 100); //para deteccion de errores
	    }
	    else
	    {
	  	  printf("USB file system has been mounted.\n");
	  	  isMounted = 1;
	  	  //HAL_UART_Transmit(&huart3, oktext, sizeof(oktext), 100); //para deteccion de errores
	    }
  }
  if (isMounted == 1 && isIndexed == 0)
  {
	  if(scanMp3Music() == 1) //funcion buscar musica consigue encontrar las canciones
	  {
		  isIndexed = 1;
	  }

  }

  /* USER CODE END CALL_BACK_1 */
}

/**
  * @}
  */

/**
  * @}
  */

