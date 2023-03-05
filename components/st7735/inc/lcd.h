/**
 ******************************************************************************
 * @file    adafruit_802_lcd.h
 * @author  MCD Application Team
 * @brief   This file contains the common defines and functions prototypes for
 *          the adafruit_802_lcd.c driver.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _LCD_H
#define _LCD_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "driver/gpio.h"
// #include "spi.h"
/* Includes ------------------------------------------------------------------*/
#define LCD_NUMBER 0x00
/*---------------------------液晶屏接线说明-------------------------------------*/
// 接线前请参考液晶屏说明书第10页引脚定义
#define LCD_SCL_PIN GPIO_NUM_14   // D5
#define LCD_SDA_PIN GPIO_NUM_13   // D7
#define LCD_RESET_PIN GPIO_NUM_12 // D6
#define LCD_DC_PIN GPIO_NUM_5     // D1
#define LCD_CS_PIN GPIO_NUM_15    // D8
#define LCD_BLK_PIN GPIO_NUM_4    // D2

#define GPIO_PIN_SET 1
#define GPIO_PIN_RESET 0

/*SPI_LCD_SCL:接模块CLK引脚,接裸屏Pin9_SCL*/
#define SPI_LCD_SCL(data) (data ? gpio_set_level(LCD_SCL_PIN, GPIO_PIN_SET) \
                                : gpio_set_level(LCD_SCL_PIN, GPIO_PIN_RESET))
/*SPI_LCD_SDA:接模块DIN/MOSI引脚，接裸屏Pin8_SDA*/
#define SPI_LCD_SDA(data) (data ? gpio_set_level(LCD_SDA_PIN, GPIO_PIN_SET) \
                                : gpio_set_level(LCD_SDA_PIN, GPIO_PIN_RESET))
/*接模块DCT引脚，接裸屏Pin6_RES*/
#define SPI_LCD_REST(data) (data ? gpio_set_level(LCD_RESET_PIN, GPIO_PIN_SET) \
                                 : gpio_set_level(LCD_RESET_PIN, GPIO_PIN_RESET))
/*接模块D/C引脚，接裸屏Pin7_A0*/
#define SPI_LCD_DC(data) (data ? gpio_set_level(LCD_DC_PIN, GPIO_PIN_SET) \
                               : gpio_set_level(LCD_DC_PIN, GPIO_PIN_RESET))
/*接模块CE引脚，接裸屏Pin12_CS*/
#define SPI_LCD_CS(data) (data ? gpio_set_level(LCD_CS_PIN, GPIO_PIN_SET) \
                               : gpio_set_level(LCD_CS_PIN, GPIO_PIN_RESET))
/*接模块BL引脚，背光可以采用IO控制或者PWM控制，也可以直接接到高电平常亮*/
#define SPI_LCD_BL(data) (data ? gpio_set_level(LCD_BLK_PIN, GPIO_PIN_SET) \
                               : gpio_set_level(LCD_BLK_PIN, GPIO_PIN_RESET))
/*---------------------------End of液晶屏接线---------------------------------*/
#define LCD_PIXEL_FORMAT_ARGB8888 0x00000000U /*!< ARGB8888 LTDC pixel format */
#define LCD_PIXEL_FORMAT_RGB888 0x00000001U   /*!< RGB888 LTDC pixel format   */
#define LCD_PIXEL_FORMAT_RGB565 0x00000002U   /*!< RGB565 LTDC pixel format   */
#define LCD_PIXEL_FORMAT_ARGB1555 0x00000003U /*!< ARGB1555 LTDC pixel format */
#define LCD_PIXEL_FORMAT_ARGB4444 0x00000004U /*!< ARGB4444 LTDC pixel format */
#define LCD_PIXEL_FORMAT_L8 0x00000005U       /*!< L8 LTDC pixel format       */
#define LCD_PIXEL_FORMAT_AL44 0x00000006U     /*!< AL44 LTDC pixel format     */
#define LCD_PIXEL_FORMAT_AL88 0x00000007U     /*!< AL88 LTDC pixel format     */
/**
 * @}
 */
/* Common Error codes */
#define BSP_ERROR_NONE 0
#define BSP_ERROR_NO_INIT -1
#define BSP_ERROR_WRONG_PARAM -2
#define BSP_ERROR_BUSY -3
#define BSP_ERROR_PERIPH_FAILURE -4
#define BSP_ERROR_COMPONENT_FAILURE -5
#define BSP_ERROR_UNKNOWN_FAILURE -6
#define BSP_ERROR_UNKNOWN_COMPONENT -7
#define BSP_ERROR_BUS_FAILURE -8
#define BSP_ERROR_CLOCK_FAILURE -9
#define BSP_ERROR_MSP_FAILURE -10
#define BSP_ERROR_FEATURE_NOT_SUPPORTED -11
/** @defgroup LCD_Exported_Types
 * @{
 */
/* RGB565 colors definitions */
#define LCD_COLOR_RGB565_BLUE 0x001FU
#define LCD_COLOR_RGB565_GREEN 0x07E0U
#define LCD_COLOR_RGB565_RED 0xF800U
#define LCD_COLOR_RGB565_CYAN 0x07FFU
#define LCD_COLOR_RGB565_MAGENTA 0xF81FU
#define LCD_COLOR_RGB565_YELLOW 0xFFE0U
#define LCD_COLOR_RGB565_LIGHTBLUE 0x841FU
#define LCD_COLOR_RGB565_LIGHTGREEN 0x87F0U
#define LCD_COLOR_RGB565_LIGHTRED 0xFC10U
#define LCD_COLOR_RGB565_LIGHTCYAN 0x87FFU
#define LCD_COLOR_RGB565_LIGHTMAGENTA 0xFC1FU
#define LCD_COLOR_RGB565_LIGHTYELLOW 0xFFF0U
#define LCD_COLOR_RGB565_DARKBLUE 0x0010U
#define LCD_COLOR_RGB565_DARKGREEN 0x0400U
#define LCD_COLOR_RGB565_DARKRED 0x8000U
#define LCD_COLOR_RGB565_DARKCYAN 0x0410U
#define LCD_COLOR_RGB565_DARKMAGENTA 0x8010U
#define LCD_COLOR_RGB565_DARKYELLOW 0x8400U
#define LCD_COLOR_RGB565_WHITE 0xFFFFU
#define LCD_COLOR_RGB565_LIGHTGRAY 0xD69AU
#define LCD_COLOR_RGB565_GRAY 0x8410U
#define LCD_COLOR_RGB565_DARKGRAY 0x4208U
#define LCD_COLOR_RGB565_BLACK 0x0000U
#define LCD_COLOR_RGB565_BROWN 0xA145U
#define LCD_COLOR_RGB565_ORANGE 0xFD20U
/* Definition of Official ST COLOR */
#define LCD_COLOR_RGB565_ST_BLUE_DARK 0x0001U
#define LCD_COLOR_RGB565_ST_BLUE 0x01EBU
#define LCD_COLOR_RGB565_ST_BLUE_LIGHT 0x06A7U
#define LCD_COLOR_RGB565_ST_GREEN_LIGHT 0x05ECU
#define LCD_COLOR_RGB565_ST_GREEN_DARK 0x001CU
#define LCD_COLOR_RGB565_ST_YELLOW 0x07F0U
#define LCD_COLOR_RGB565_ST_BROWN 0x02C8U
#define LCD_COLOR_RGB565_ST_PINK 0x0681U
#define LCD_COLOR_RGB565_ST_PURPLE 0x02CDU
#define LCD_COLOR_RGB565_ST_GRAY_DARK 0x0251U
#define LCD_COLOR_RGB565_ST_GRAY 0x04BAU
#define LCD_COLOR_RGB565_ST_GRAY_LIGHT 0x05E7U
  /** @defgroup LCD_Driver_structure  LCD Driver structure
   * @{
   */
  // typedef struct
  // {
  //   int32_t ( *DrawBitmap      ) (uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint8_t *);
  //   int32_t ( *FillRGBRect     ) (uint32_t, uint32_t, uint32_t, uint8_t*, uint32_t, uint32_t);
  //   int32_t ( *DrawHLine       ) (uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
  //   int32_t ( *DrawVLine       ) (uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
  //   int32_t ( *FillRect        ) (uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
  //   int32_t ( *GetPixel        ) (uint32_t, uint32_t, uint32_t, uint32_t*);
  //   int32_t ( *SetPixel        ) (uint32_t, uint32_t, uint32_t, uint32_t);
  //   int32_t ( *GetXSize        ) (uint32_t, uint32_t *);
  //   int32_t ( *GetYSize        ) (uint32_t, uint32_t *);
  //   int32_t ( *SetLayer        ) (uint32_t, uint32_t);
  //   int32_t ( *GetFormat       ) (uint32_t, uint32_t *);
  // } LCD_UTILS_Drv_t;

  typedef struct
  {
    /* Control functions */
    int32_t (*Init)(void *, uint32_t, uint32_t);
    int32_t (*DeInit)(void *);
    int32_t (*ReadID)(void *, uint32_t *);
    int32_t (*DisplayOn)(void *);
    int32_t (*DisplayOff)(void *);
    int32_t (*SetBrightness)(void *, uint32_t);
    int32_t (*GetBrightness)(void *, uint32_t *);
    int32_t (*SetOrientation)(void *, uint32_t);
    int32_t (*GetOrientation)(void *, uint32_t *);

    /* Drawing functions*/
    int32_t (*SetCursor)(void *, uint32_t, uint32_t);
    int32_t (*DrawBitmap)(void *, uint32_t, uint32_t, uint32_t, uint32_t, uint8_t *);
    int32_t (*FillRGBRect)(void *, uint32_t, uint32_t, uint8_t *, uint32_t, uint32_t);
    int32_t (*DrawHLine)(void *, uint32_t, uint32_t, uint32_t, uint32_t);
    int32_t (*DrawVLine)(void *, uint32_t, uint32_t, uint32_t, uint32_t);
    int32_t (*FillRect)(void *, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
    int32_t (*GetPixel)(void *, uint32_t, uint32_t, uint32_t *);
    int32_t (*SetPixel)(void *, uint32_t, uint32_t, uint32_t);
    int32_t (*DrawChar)(void *, uint32_t, uint32_t, uint8_t, uint8_t, uint32_t, uint32_t, uint8_t);
    int32_t (*DrawString)(void *, uint32_t, uint32_t, uint8_t *, uint8_t, uint32_t, uint32_t);
    int32_t (*GetXSize)(void *, uint32_t *);
    int32_t (*GetYSize)(void *, uint32_t *);
  } LCD_Drv_t;

/** @defgroup ADAFRUIT_802_LCD_Exported_Constants LCD Exported Constants
 * @{
 */
#define LCD_INSTANCES_NBR 1U

#define LCD_ORIENTATION_PORTRAIT 0x00U         /* Portrait orientation choice of LCD screen               */
#define LCD_ORIENTATION_PORTRAIT_ROT180 0x01U  /* Portrait rotated 180° orientation choice of LCD screen  */
#define LCD_ORIENTATION_LANDSCAPE 0x02U        /* Landscape orientation choice of LCD screen              */
#define LCD_ORIENTATION_LANDSCAPE_ROT180 0x03U /* Landscape rotated 180° orientation choice of LCD screen */

  /**
   * @}
   */

  /** @defgroup ADAFRUIT_802_LCD_Exported_Types LCD Exported Types
   * @{
   */
  typedef struct
  {
    uint32_t XSize;
    uint32_t YSize;
    uint32_t PixelFormat;
    uint32_t IsMspCallbacksValid;
  } LCD_Ctx_t;

  /** @addtogroup ADAFRUIT_802_LCD_Exported_Variables
   * @{
   */
  extern LCD_Ctx_t Lcd_Ctx[LCD_INSTANCES_NBR];

  /**
   * @}
   */

  /** @defgroup ADAFRUIT_802_LCD_Exported_Functions LCD Exported Functions
   * @{
   */
  int32_t LCD_Init(uint32_t Instance, uint32_t Orientation);
  // int32_t LCD_DeInit(uint32_t Instance);

  /* LCD generic APIs: Display control */
  int32_t LCD_DisplayOn(uint32_t Instance);
  int32_t LCD_DisplayOff(uint32_t Instance);
  int32_t LCD_SetBrightness(uint32_t Instance, uint32_t Brightness);
  int32_t LCD_GetBrightness(uint32_t Instance, uint32_t *Brightness);
  int32_t LCD_GetXSize(uint32_t Instance, uint32_t *XSize);
  int32_t LCD_GetYSize(uint32_t Instance, uint32_t *YSize);

  /* LCD generic APIs: Draw operations. This list of APIs is required for
     lcd gfx utilities */
  int32_t LCD_GetPixelFormat(uint32_t Instance, uint32_t *PixelFormat);
  int32_t LCD_DrawBitmap(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t height, uint8_t *pBmp);
  int32_t LCD_FillRGBRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint8_t *pData, uint32_t Width, uint32_t Height);
  int32_t LCD_DrawHLine(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color);
  int32_t LCD_DrawVLine(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color);
  int32_t LCD_FillRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t Color);
  int32_t LCD_ReadPixel(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t *Color);
  int32_t LCD_WritePixel(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Color);
  int32_t LCD_DrawChar(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint8_t num, uint8_t size, uint32_t fColor, uint32_t bColor, uint8_t mode);
  int32_t LCD_DrawString(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint8_t *string, uint8_t size, uint32_t fColor, uint32_t bColor);
  /**
   * @}
   */

#ifdef __cplusplus
}
#endif

#endif /*_LCD_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
