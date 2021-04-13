/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "usart.h"

#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "kb.h"

#include "sdk_uart.h"

#include "pca9538.h"

#include "oled.h"

#include "fonts.h"

int numbers[9] = {
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9
};
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KB_Test(void);
void OLED_KB(uint8_t OLED_Keys[]);
void oled_Reset(void);
int main(void) {
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART6_UART_Init();
  oled_Init();
  /* USER CODE BEGIN 2 */


  while (1) {
    KB_Test();
    HAL_Delay(500);

  }
}

void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {
    0
  };
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {
    0
  };

  /** Configure the main internal regulator output voltage 
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig( & RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
    RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig( & RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void KB_Test(void) {
  UART_Transmit((uint8_t * )
    "KB test start\n");
  uint8_t R = 0, C = 0, L = 0, Row[4] = {
    0xF7,
    0x7B,
    0x3D,
    0x1E
  }, Key, OldKey, OLED_Keys[12] = {
    0x30,
    0x30,
    0x30,
    0x30,
    0x30,
    0x30,
    0x30,
    0x30,
    0x30,
    0x30,
    0x30,
    0x30
  };

  int a = 0;
  int b = 0;
  int actions = 0;
  int result = 0;
  char operation = "unset";
  int settings = 0;
  oled_Reset();
  oled_WriteString("Available mods:", Font_7x10, White);
  oled_UpdateScreen();
  HAL_Delay(1000);
  oled_Reset();
  oled_WriteString("Sum - left col", Font_7x10, White);
  oled_UpdateScreen();
  HAL_Delay(1000);
  oled_Reset();
  oled_WriteString("Sub - middle col", Font_7x10, White);
  oled_UpdateScreen();
  HAL_Delay(1000);
  oled_Reset();
  oled_WriteString("Mult - right col", Font_7x10, White);
  oled_UpdateScreen();
  HAL_Delay(1000);
  oled_Reset();
  oled_WriteString("Choose mode->", Font_7x10, White);
  oled_UpdateScreen();
  HAL_Delay(1000);
  while (settings == 0) {
    for (int i = 0; i < 4; i++) {
      OldKey = Key;
      Key = Check_Row(Row[i]);

      if (Key == 0x01 && Key != OldKey) {
        operation = "mult";
        settings = 3;
        break;
      } else if (Key == 0x02 && Key != OldKey) {
        operation = "sub";
        settings = 2;
        break;

      } else if (Key == 0x04 && Key != OldKey) {
        operation = "sum";
        settings = 1;
        break;

      }
    }
  }

  oled_Reset();
  if (settings == 1) {
    oled_WriteString("Sum was set", Font_7x10, White);
  }

  if (settings == 2) {
    oled_WriteString("Sub was set", Font_7x10, White);
  }

  if (settings == 3) {
    oled_WriteString("Mult was set", Font_7x10, White);
  }

  oled_UpdateScreen();
  HAL_Delay(1000);
  oled_Reset();
  oled_WriteString("Input two numbers", Font_7x10, White);
  oled_UpdateScreen();
  HAL_Delay(1000);

  while (1) {
    for (int i = 0; i < 4; i++) {
      OldKey = Key;
      Key = Check_Row(Row[i]);
      if (Key == 0x01 && Key != OldKey) {
        UART_Transmit((uint8_t * )
          "Right pressed\n");
        R = 1;
        oled_Reset();
        if (i == 1) {
          oled_WriteString("3", Font_7x10, White);
          if (actions == 0) {
            a = 3;
          } else {
            b = 3;
          }
        }
        if (i == 2) {
          oled_WriteString("6", Font_7x10, White);
          if (actions == 0) {
            a = 6;
          } else {
            b = 6;
          }
        }
        if (i == 3) {
          oled_WriteString("9", Font_7x10, White);
          if (actions == 0) {
            a = 9;
          } else {
            b = 9;
          }
        }
        actions++;
        oled_UpdateScreen();
        //OLED_KB(OLED_Keys);
      }
      if (Key == 0x02 && Key != OldKey) {
        UART_Transmit((uint8_t * )
          "Center pressed\n");
        C = 1;
        oled_Reset();
        if (i == 1) {
          oled_WriteString("2", Font_7x10, White);
          if (actions == 0) {
            a = 2;
          } else {
            b = 2;
          }
        }
        if (i == 2) {
          oled_WriteString("5", Font_7x10, White);
          if (actions == 0) {
            a = 5;
          } else {
            b = 5;
          }
        }
        if (i == 3) {
          oled_WriteString("8", Font_7x10, White);
          if (actions == 0) {
            a = 8;
          } else {
            b = 8;
          }
        }
        actions++;
        oled_UpdateScreen();
      }

      if (Key == 0x04 && Key != OldKey) {
        UART_Transmit((uint8_t * )
          "Left pressed\n");
        L = 1;
        oled_Reset();
        if (i == 1) {
          oled_WriteString("1", Font_7x10, White);
          if (actions == 0) {
            a = 1;
          } else {
            b = 1;
          }

        }
        if (i == 2) {
          oled_WriteString("4", Font_7x10, White);
          if (actions == 0) {
            a = 4;
          } else {
            b = 4;
          }
        }
        if (i == 3) {
          oled_WriteString("7", Font_7x10, White);
          if (actions == 0) {
            a = 7;
          } else {
            b = 7;
          }
        }
        actions++;
        oled_UpdateScreen();
      }

      if (actions > 1) {

        if (settings == 1) {
          result = a + b;
        }
        if (settings == 2) {
          result = a - b;
        }
        if (settings == 3) {
          result = a * b;
        }


        decodeNumber(result);
        actions = 0;
      }
      if (i == 3) i = 0;
    }

    HAL_Delay(25);
  }
  UART_Transmit((uint8_t * )
    "KB test complete\n");
}

void decodeNumber(int num) {
  oled_Reset();
  if (num < 0) {
    oled_WriteString("-", Font_7x10, White);
    oled_UpdateScreen();
  }
  int score = abs(num);
  int nDigits = floor(log10(abs(score))) + 1;
  int digits = 0;

  int div;
  for (div = 1; div <= score; div *= 10)
  ;

  do {
    div /= 10;
    num = score / div;
    if (num == 0) {
      oled_WriteString("0", Font_7x10, White);
      oled_UpdateScreen();
    }
    if (num == 1) {
      oled_WriteString("1", Font_7x10, White);
      oled_UpdateScreen();
    }
    if (num == 2) {
      oled_WriteString("2", Font_7x10, White);
      oled_UpdateScreen();
    }
    if (num == 3) {
      oled_WriteString("3", Font_7x10, White);
      oled_UpdateScreen();
    }
    if (num == 4) {
      oled_WriteString("4", Font_7x10, White);
      oled_UpdateScreen();
    }
    if (num == 5) {
      oled_WriteString("5", Font_7x10, White);
      oled_UpdateScreen();
    }
    if (num == 6) {
      oled_WriteString("6", Font_7x10, White);
      oled_UpdateScreen();
    }
    if (num == 7) {
      oled_WriteString("7", Font_7x10, White);
      oled_UpdateScreen();
    }
    if (num == 8) {
      oled_WriteString("8", Font_7x10, White);
      oled_UpdateScreen();
    }
    if (num == 9) {
      oled_WriteString("9", Font_7x10, White);
      oled_UpdateScreen();
    }
    digits++;
    score %= div;

  } while (score);

  if (digits != nDigits) {
    oled_WriteString("0", Font_7x10, White);
    oled_UpdateScreen();
  }

}
void OLED_KB(uint8_t OLED_Keys[12]) {
  for (int i = 3; i >= 0; i--) {
    oled_SetCursor(56, 5 + (4 - i) * 10);
    for (int j = 0; j < 3; j++) {
      oled_WriteChar(OLED_Keys[j + 3 * i], Font_7x10, White);
    }
  }
  oled_UpdateScreen();
}
void oled_Reset(void) {
  oled_Fill(Black);
  oled_SetCursor(0, 0);
  oled_UpdateScreen();
}
void Error_Handler(void) {

}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t * file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
