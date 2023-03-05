#include "lcd.h"
#include "st7735.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi.h"
#include "esp_attr.h"

/** @defgroup ADAFRUIT_802_LCD_Private_Variables LCD Private Variables
 * @{
 */
static ST7735_Object_t ST7735Obj;
static void *Lcd_CompObj = &ST7735Obj;
static LCD_Drv_t *Lcd = &ST7735_LCD_Driver.Lcd;
LCD_Ctx_t Lcd_Ctx[LCD_INSTANCES_NBR];

/**
 * @}
 */

/** @addtogroup ADAFRUIT_802_LCD_Exported_Variables
 * @{
 */
// const LCD_UTILS_Drv_t LCD_Driver =
// {
//   LCD_DrawBitmap,
//   LCD_FillRGBRect,
//   LCD_DrawHLine,
//   LCD_DrawVLine,
//   LCD_FillRect,
//   LCD_ReadPixel,
//   LCD_WritePixel,
//   LCD_GetXSize,
//   LCD_GetYSize,
//   NULL,
//   LCD_GetPixelFormat
// };
/**
 * @}
 */

/** @defgroup ADAFRUIT_802_LCD_Private_FunctionPrototypes LCD Private Function Prototypes
 * @{
 */
static int32_t ST7735_Probe(uint32_t Orientation);
// static int32_t LCD_IO_Init(void);
// static int32_t LCD_IO_DeInit(void);
static int32_t LCD_IO_WriteReg(uint8_t Reg, uint8_t *pData, uint32_t Length);
static int32_t LCD_IO_SendData(uint8_t *pData, uint32_t Length);
/**
 * @}
 */

/** @addtogroup ADAFRUIT_802_LCD_Exported_Functions
 * @{
 */

/**
 * @brief  Initializes the LCD with a given orientation.
 * @param  Instance    LCD Instance
 * @param  orientation Select display orientation:
 *         - LCD_ORIENTATION_PORTRAIT
 *         - LCD_ORIENTATION_LANDSCAPE
 *         - LCD_ORIENTATION_PORTRAIT_ROT180
 *         - LCD_ORIENTATION_LANDSCAPE_ROT180
 * @retval Error status
 */
int32_t LCD_Init(uint32_t Instance, uint32_t Orientation)
{
  int32_t ret;

  if ((Orientation > LCD_ORIENTATION_LANDSCAPE_ROT180) || (Instance >= LCD_INSTANCES_NBR))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (ST7735_Probe(Orientation) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_UNKNOWN_COMPONENT;
    }
    else if (LCD_GetXSize(Instance, &Lcd_Ctx[Instance].XSize) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else if (LCD_GetYSize(Instance, &Lcd_Ctx[Instance].YSize) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      /* Clear the LCD screen with white color */
      ret = LCD_FillRect(Instance, 0U, 0U, Lcd_Ctx[Instance].XSize, Lcd_Ctx[Instance].YSize, 0xFFFFFFFFU);
    }
  }
  return ret;
}

/**
 * @brief  DeInitializes the LCD.
 * @param  Instance    LCD Instance
 * @retval Error status
 */
int32_t LCD_DeInit(uint32_t Instance)
{
  int32_t ret;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    //    ret = LCD_IO_DeInit();
  }

  return ret;
}

/**
 * @brief  Gets the LCD Active LCD Pixel Format.
 * @param  Instance    LCD Instance
 * @param  PixelFormat Active LCD Pixel Format
 * @retval BSP status
 */
int32_t LCD_GetPixelFormat(uint32_t Instance, uint32_t *PixelFormat)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Only RGB565 format is supported */
    *PixelFormat = LCD_PIXEL_FORMAT_RGB565;
  }

  return ret;
}

/**
 * @brief  Gets the LCD X size.
 * @param  Instance LCD Instance
 * @param  XSize    LCD width
 * @retval Error status
 */
int32_t LCD_GetXSize(uint32_t Instance, uint32_t *XSize)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (Lcd->GetXSize(Lcd_CompObj, XSize) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}

/**
 * @brief  Gets the LCD Y size.
 * @param  Instance LCD Instance
 * @param  YSize    LCD hieght
 * @retval Error status
 */
int32_t LCD_GetYSize(uint32_t Instance, uint32_t *YSize)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (Lcd->GetYSize(Lcd_CompObj, YSize) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}

/**
 * @brief  Reads an LCD pixel.
 * @param  Instance    LCD Instance
 * @param  Xpos X position
 * @param  Ypos Y position
 * @param  Color RGB color
 * @retval Error status
 */
int32_t LCD_ReadPixel(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t *Color)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);
  UNUSED(Xpos);
  UNUSED(Ypos);
  UNUSED(Color);
  return BSP_ERROR_FEATURE_NOT_SUPPORTED;
}

/**
 * @brief  Draws a pixel on LCD.
 * @param  Instance    LCD Instance
 * @param  Xpos X position
 * @param  Ypos Y position
 * @param  Color RGB color
 * @retval Error status
 */
int32_t LCD_WritePixel(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Color)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (Lcd->SetPixel != NULL)
  {
    if (Lcd->SetPixel(Lcd_CompObj, Xpos, Ypos, Color) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
 * @brief  Draws an character on LCD.
 * @param  Instance    LCD Instance
 * @param  Xpos X position
 * @param  Ypos Y position
 * @param  num  character
 * @param  size Word size
 * @param  fColor ForeColor
 * @param  bColor Background Color
 * @param  mode   display mode
 * @retval Error status
 */
int32_t LCD_DrawChar(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint8_t num, uint8_t size, uint32_t fColor, uint32_t bColor, uint8_t mode)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (Lcd->DrawChar != NULL)
  {
    if (Lcd->DrawChar(Lcd_CompObj, Xpos, Ypos, num, size, fColor, bColor, mode) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
 * @brief  Draws characters on LCD.
 * @param  Instance    LCD Instance
 * @param  Xpos X position
 * @param  Ypos Y position
 * @param  string  string
 * @param  size   Word size
 * @param  fColor ForeColor
 * @param  bColor Background Color
 * @retval Error status
 */
int32_t LCD_DrawString(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint8_t *string, uint8_t size, uint32_t fColor, uint32_t bColor)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (Lcd->DrawChar != NULL)
  {
    if (Lcd->DrawString(Lcd_CompObj, Xpos, Ypos, string, size, fColor, bColor) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
 * @brief  Draws an horizontal line.
 * @param  Instance    LCD Instance
 * @param  Xpos X position
 * @param  Ypos Y position
 * @param  Length Line length
 * @param  Color RGB color
 * @retval Error status
 */
int32_t LCD_DrawHLine(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (Lcd->DrawHLine != NULL)
  {
    if (Lcd->DrawHLine(Lcd_CompObj, Xpos, Ypos, Length, Color) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
 * @brief  Draws a vertical line.
 * @param  Instance    LCD Instance
 * @param  Xpos X position
 * @param  Ypos Y position
 * @param  Length Line length
 * @param  Color RGB color
 * @retval Error status
 */
int32_t LCD_DrawVLine(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (Lcd->DrawVLine != NULL)
  {
    if (Lcd->DrawVLine(Lcd_CompObj, Xpos, Ypos, Length, Color) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
 * @brief  Draws a bitmap picture (16 bpp).
 * @param  Instance    LCD Instance
 * @param  Xpos Bmp X position in the LCD
 * @param  Ypos Bmp Y position in the LCD
 * @param  pBmp Pointer to Bmp picture address.
 * @retval Error status
 */
int32_t LCD_DrawBitmap(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t height, uint8_t *pBmp)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (Lcd->DrawBitmap != NULL)
  {
    if (Lcd->DrawBitmap(Lcd_CompObj, Xpos, Ypos, width, height, pBmp) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
 * @brief  Draws a full RGB rectangle
 * @param  Instance LCD Instance.
 * @param  Xpos   specifies the X position.
 * @param  Ypos   specifies the Y position.
 * @param  pData  pointer to RGB data
 * @param  Width  specifies the rectangle width.
 * @param  Height Specifies the rectangle height
 * @retval BSP status
 */
int32_t LCD_FillRGBRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint8_t *pData, uint32_t Width, uint32_t Height)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (Lcd->FillRGBRect != NULL)
  {
    /* Draw the horizontal line on LCD */
    if (Lcd->FillRGBRect(Lcd_CompObj, Xpos, Ypos, pData, Width, Height) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
 * @brief  Draws a full rectangle.
 * @param  Instance LCD instance
 * @param  Xpos X position
 * @param  Ypos Y position
 * @param  Width Rectangle width
 * @param  Height Rectangle height
 * @param  Color RGB color
 * @retval BSP status
 */
int32_t LCD_FillRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t Color)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (Lcd->FillRect != NULL)
  {
    if (Lcd->FillRect(Lcd_CompObj, Xpos, Ypos, Width, Height, Color) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
 * @brief  Enables the display.
 * @param  Instance    LCD Instance
 * @retval Error status
 */
int32_t LCD_DisplayOn(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (Lcd->DisplayOn != NULL)
  {
    if (Lcd->DisplayOn(Lcd_CompObj) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
 * @brief  Disables the display.
 * @param  Instance    LCD Instance
 * @retval Error status
 */
int32_t LCD_DisplayOff(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (Lcd->DisplayOff != NULL)
  {
    if (Lcd->DisplayOff(Lcd_CompObj) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
 * @brief  Set the brightness value
 * @param  Instance    LCD Instance
 * @param  Brightness [00: Min (black), 100 Max]
 * @retval Error status
 */
int32_t LCD_SetBrightness(uint32_t Instance, uint32_t Brightness)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (Lcd->SetBrightness != NULL)
  {
    if (Lcd->SetBrightness(Lcd_CompObj, Brightness) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
 * @brief  Get the brightness value
 * @param  Instance    LCD Instance
 * @param  Brightness [00: Min (black), 100 Max]
 * @retval Error status
 */
int32_t LCD_GetBrightness(uint32_t Instance, uint32_t *Brightness)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (Lcd->GetBrightness != NULL)
  {
    if (Lcd->GetBrightness(Lcd_CompObj, Brightness) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
 * @}
 */

/** @defgroup ADAFRUIT_802_LCD_Private_Functions LCD Private Functions
 * @{
 */

/**
 * @brief  Register Bus IOs if component ID is OK
 * @param  Orientation Display orientation
 * @retval Error status
 */
static int32_t ST7735_Probe(uint32_t Orientation)
{
  int32_t ret = BSP_ERROR_NONE;
  ST7735_IO_t IOCtx;

  /* Configure the audio driver */
  IOCtx.Address = 0;
  IOCtx.Init = NULL;   // MX_SPI1_Init
  IOCtx.DeInit = NULL; /*一次初始化后，不需要释放SPI接口*/
  IOCtx.GetTick = NULL;
  IOCtx.Delay = (ST7735_Delay_Func)vTaskDelay;
  IOCtx.WriteReg = LCD_IO_WriteReg;
  IOCtx.SendData = LCD_IO_SendData;

  if (ST7735_RegisterBusIO(&ST7735Obj, &IOCtx) != ST7735_OK)
  {
    ret = BSP_ERROR_BUS_FAILURE;
  }
  else
  {
    printf("lcd:%p.\n", Lcd);
    SPI_LCD_REST(GPIO_PIN_SET); /*初始化LCD之前先清除LCD复位信号*/
                                // HAL_Delay(100);
    vTaskDelay(100 / portTICK_RATE_MS);

    if (Lcd->Init && Lcd->Init(Lcd_CompObj, ST7735_FORMAT_DEFAULT, Orientation) != ST7735_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  return ret;
}

// typedef enum
// {
//   SPI_NULL = 0,
//   SPI_WRITE,
//   SPI_READ
// } spi_master_mode_t;
// /* SPI transmit data, format: 8bit command (read value: 3, write value: 4) + 8bit address(value: 0x0) + 64byte data
//  *  For convenience, every time we send 64bytes, SPI SLAVE will determine how much data to read based on the status value
//  */
// static esp_err_t IRAM_ATTR spi_master_transmit(spi_master_mode_t trans_mode, uint32_t *data)
// {
//   spi_trans_t trans;
//   uint16_t cmd;
//   // uint32_t addr = 0x0;

//   memset(&trans, 0x0, sizeof(trans));
//   trans.bits.val = 0; // clear all bit

//   if (trans_mode == SPI_WRITE)
//   {
//     cmd = SPI_MASTER_WRITE_DATA_TO_SLAVE_CMD;
//     trans.bits.mosi = 8 * 64; // One time transmit only support 64bytes
//     trans.mosi = data;
//   }
//   else if (trans_mode == SPI_READ)
//   {
//     cmd = SPI_MASTER_READ_DATA_FROM_SLAVE_CMD;
//     trans.bits.miso = 8 * 64;
//     trans.miso = data;
//   }

//   trans.bits.cmd = 8 * 1;
//   // trans.bits.addr = 8 * 1; // transmit data will use 8bit address
//   trans.cmd = &cmd;
//   // trans.addr = &addr;

//   return spi_trans(HSPI_HOST, &trans);
// }

// Write an 8-bit cmd
static esp_err_t spi_master_transmit(uint8_t data)
{
  /*In order to improve the transmission efficiency,
  it is recommended that the external incoming data is (uint32_t *) type data,
  do not use other type data.*/
  uint32_t buf = data; //  << 24
  spi_trans_t trans = {0};
  trans.mosi = &buf;
  trans.bits.mosi = 8;

  spi_trans(HSPI_HOST, &trans);
  return ESP_OK;
}

/**
 * @brief  Writes command to select the LCD register.
 * @param  Reg Address of the selected register.
 * @param  pData pointer to data to write to the register
 * @param  Length length of data to write to the register
 * @retval Error status
 */
static int32_t LCD_IO_WriteReg(uint8_t Reg, uint8_t *pData, uint32_t Length)
{
  int32_t ret = BSP_ERROR_NONE;

  // if ((NULL == pData) || !Length)
  //   return BSP_ERROR_PERIPH_FAILURE;

  /* Reset LCD control line CS */
  // SPI_LCD_CS(GPIO_PIN_RESET);
  /* Set LCD data/command line DC to Low */
  SPI_LCD_DC(GPIO_PIN_RESET);

  // while (Length--)
  {
    /* Send Command */
    ret = spi_master_transmit(Reg); //++
    // SPI_LCD_CS(GPIO_PIN_SET);
    /* Send Data */
    ret = LCD_IO_SendData(pData, Length);
  }

  // if (spi_master_transmit(*pData))
  // {
  //   ret = BSP_ERROR_PERIPH_FAILURE;
  // }
  // else
  // {
  //   /* Deselect : Chip Select high */
  //   // SPI_LCD_CS(GPIO_PIN_SET);
  //   /* Send Data */
  //   ret = LCD_IO_SendData(pData, Length);
  // }

  return ret;
}

/**
 * @brief  Send data to select the LCD GRAM.
 * @param  pData pointer to data to write to LCD GRAM.
 * @param  Length length of data to write to LCD GRAM
 * @retval Error status
 */
static int32_t LCD_IO_SendData(uint8_t *pData, uint32_t Length)
{
  int32_t ret = BSP_ERROR_NONE;

  if ((NULL == pData) || !Length)
    return ret; // BSP_ERROR_PERIPH_FAILURE

  /* Reset LCD control line CS */
  // SPI_LCD_CS(GPIO_PIN_RESET);
  /* Set LCD data/command line DC to High */
  SPI_LCD_DC(GPIO_PIN_SET);

  // for (uint32_t i = 0; i < Length; i++)
  while (Length--)
  {
    if (spi_master_transmit(*pData++)) //
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
      break;
    }
    // printf("data[%d]: %#x.\r\n", Length, *pData);
  }

  /* Deselect : Chip Select high */
  // SPI_LCD_CS(GPIO_PIN_SET);

  return ret;
}

/**
 * @}
 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
