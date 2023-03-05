/**
 ******************************************************************************
 * @file    st7735.c
 * @author  MCD Application Team
 * @brief   This file includes the driver for ST7735 LCD mounted on the Adafruit
 *          1.8" TFT LCD shield (reference ID 802).
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
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "st7735.h"
#include "tft_front.h"
// #include "stm32h7xx_hal.h"
/** @addtogroup BSP
 * @{
 */

/** @addtogroup Components
 * @{
 */

/** @addtogroup ST7735
 * @brief      This file provides a set of functions needed to drive the
 *             ST7735 LCD.
 * @{
 */

/** @defgroup ST7735_Private_Types Private Types
 * @{
 */
typedef struct
{
  uint32_t Width;
  uint32_t Height;
  uint32_t Orientation;
} ST7735_Ctx_t;
/**
 * @}
 */

/** @defgroup ST7735_Private_Variables Private Variables
 * @{
 */
ST7735_LCD_Drv_t ST7735_LCD_Driver = {
    .Lcd = {
        ST7735_Init,
        ST7735_DeInit,
        ST7735_ReadID,
        ST7735_DisplayOn,
        ST7735_DisplayOff,
        ST7735_SetBrightness,
        ST7735_GetBrightness,
        ST7735_SetOrientation,
        ST7735_GetOrientation,
        ST7735_SetCursor,
        ST7735_DrawBitmap,
        ST7735_FillRGBRect,
        ST7735_DrawHLine,
        ST7735_DrawVLine,
        ST7735_FillRect,
        ST7735_GetPixel,
        ST7735_SetPixel,
        ST7735_DrawChar,
        ST7735_DrawString,
        ST7735_GetXSize,
        ST7735_GetYSize,
    },
};

/* The below table handle the different values to be set to Memory Data Access Control
   depending on the orientation and pbm image writing where the data order is inverted
*/
static uint8_t OrientationTab[4][2] =
    {
        {0x40U, 0xC0U}, /* Portrait orientation choice of LCD screen               */
        {0x80U, 0x00U}, /* Portrait rotated 180� orientation choice of LCD screen  */
        {0x20U, 0x60U}, /* Landscape orientation choice of LCD screen              */
        {0xE0U, 0xA0U}  /* Landscape rotated 180� orientation choice of LCD screen */
};

static ST7735_Ctx_t ST7735Ctx;
/**
 * @}
 */

/** @defgroup ST7735_Private_FunctionsPrototypes Private Functions Prototypes
 * @{
 */
static int32_t ST7735_SetDisplayWindow(ST7735_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height);
static int32_t ST7735_ReadRegWrap(void *Handle, uint8_t Reg, uint8_t *pData);
static int32_t ST7735_WriteRegWrap(void *Handle, uint8_t Reg, uint8_t *pData, uint32_t Length);
static int32_t ST7735_SendDataWrap(void *Handle, uint8_t *pData, uint32_t Length);
static int32_t ST7735_RecvDataWrap(void *Handle, uint8_t *pData, uint32_t Length);
static int32_t ST7735_IO_Delay(ST7735_Object_t *pObj, uint32_t Delay);
/**
 * @}
 */

/** @addtogroup ST7735_Exported_Functions
 * @{
 */
/**
 * @brief  Register component IO bus
 * @param  pObj Component object pointer
 * @param  pIO  Component IO structure pointer
 * @retval Component status
 */
int32_t ST7735_RegisterBusIO(ST7735_Object_t *pObj, ST7735_IO_t *pIO)
{
  int32_t ret = ST7735_OK;

  if (NULL == pObj)
  {
    ret = ST7735_ERROR;
  }
  else
  {
    pObj->IO.Init = pIO->Init;
    pObj->IO.DeInit = pIO->DeInit;
    pObj->IO.Address = pIO->Address;
    pObj->IO.WriteReg = pIO->WriteReg;
    pObj->IO.ReadReg = pIO->ReadReg;
    pObj->IO.SendData = pIO->SendData;
    pObj->IO.RecvData = pIO->RecvData;
    pObj->IO.GetTick = pIO->GetTick;

    pObj->Ctx.ReadReg = ST7735_ReadRegWrap;
    pObj->Ctx.WriteReg = ST7735_WriteRegWrap;
    pObj->Ctx.SendData = ST7735_SendDataWrap;
    pObj->Ctx.RecvData = ST7735_RecvDataWrap;
    pObj->Ctx.handle = pObj;

    if (pObj->IO.Init)
      ret = pObj->IO.Init();
  }

  return ret;
}

/**
 * @brief  Initialize the st7735 LCD Component.
 * @param  pObj Component object
 * @param  ColorCoding RGB mode
 * @param  Orientation Display orientation
 * @retval Component status
 */
int32_t ST7735_Init(ST7735_Object_t *pObj,
                    uint32_t ColorCoding,
                    uint32_t Orientation)
{
  uint8_t tmp;
  int32_t ret;

  if (NULL == pObj)
    return ST7735_ERROR;

  /* Out of sleep mode, 0 args, no delay */
  tmp = 0x00U;
  ret = st7735_write_reg(&pObj->Ctx, ST7735_SLEEP_OUT, &tmp, 1);
  if (pObj->IO.Delay)
    pObj->IO.Delay(120);
  /* Frame rate ctrl - normal mode, 3 args:Rate = fosc/(1x2+40) * (LINE+2C+2D)*/
  ret += st7735_write_reg(&pObj->Ctx, ST7735_FRAME_RATE_CTRL1, &tmp, 0);
  tmp = 0x01U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x2CU;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x2DU;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);

  /* Frame rate control - idle mode, 3 args:Rate = fosc/(1x2+40) * (LINE+2C+2D) */
  tmp = 0x01U;
  ret += st7735_write_reg(&pObj->Ctx, ST7735_FRAME_RATE_CTRL2, &tmp, 1);
  tmp = 0x2CU;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x2DU;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);

  /* Frame rate ctrl - partial mode, 6 args: Dot inversion mode, Line inversion mode */
  tmp = 0x01U;
  ret += st7735_write_reg(&pObj->Ctx, ST7735_FRAME_RATE_CTRL3, &tmp, 1);
  tmp = 0x2CU;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x2DU;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x01U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x2CU;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x2DU;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);

  /* Display inversion ctrl, 1 arg, no delay: No inversion */
  tmp = 0x07U;
  ret += st7735_write_reg(&pObj->Ctx, ST7735_FRAME_INVERSION_CTRL, &tmp, 1);

  /* Power control, 3 args, no delay: -4.6V , AUTO mode */
  tmp = 0xA2U;
  ret += st7735_write_reg(&pObj->Ctx, ST7735_PWR_CTRL1, &tmp, 1);
  tmp = 0x02U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x84U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);

  /* Power control, 1 arg, no delay: VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD */
  tmp = 0xC5U;
  ret += st7735_write_reg(&pObj->Ctx, ST7735_PWR_CTRL2, &tmp, 1);

  /* Power control, 2 args, no delay: Opamp current small, Boost frequency */
  tmp = 0x0AU;
  ret += st7735_write_reg(&pObj->Ctx, ST7735_PWR_CTRL3, &tmp, 1);
  tmp = 0x00U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);

  /* Power control, 2 args, no delay: BCLK/2, Opamp current small & Medium low */
  tmp = 0x8AU;
  ret += st7735_write_reg(&pObj->Ctx, ST7735_PWR_CTRL4, &tmp, 1);
  tmp = 0x2AU;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);

  /* Power control, 2 args, no delay */
  tmp = 0x8AU;
  ret += st7735_write_reg(&pObj->Ctx, ST7735_PWR_CTRL5, &tmp, 1);
  tmp = 0xEEU;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);

  /* Power control, 1 arg, no delay */
  tmp = 0x0EU;
  ret += st7735_write_reg(&pObj->Ctx, ST7735_VCOMH_VCOML_CTRL1, &tmp, 1);

  /* Don't invert display, no args, no delay */
  ret += st7735_write_reg(&pObj->Ctx, ST7735_DISPLAY_INVERSION_OFF, &tmp, 0);

  /* Set color mode, 1 arg, no delay */
  ret += st7735_write_reg(&pObj->Ctx, ST7735_COLOR_MODE, (uint8_t *)&ColorCoding, 1);

  /* Magical unicorn dust, 16 args, no delay */
  tmp = 0x0FU;
  ret += st7735_write_reg(&pObj->Ctx, ST7735_PV_GAMMA_CTRL, &tmp, 1);
  tmp = 0x1AU;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x0FU;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x18U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x2FU;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x28U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x20U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x22U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x1FU;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x1BU;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x23U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x37U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x00U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x07U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x02U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x10U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);

  /* Sparkles and rainbows, 16 args, no delay */
  tmp = 0x0FU;
  ret += st7735_write_reg(&pObj->Ctx, ST7735_NV_GAMMA_CTRL, &tmp, 1);
  tmp = 0x1BU;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x0FU;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x17U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x33U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x2CU;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x29U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x2EU;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x30U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x30U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x39U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x3FU;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x00U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x07U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x03U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x10U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);

  /* Normal display on, no args, no delay */
  // tmp  = 0x00U;
  // ret += st7735_write_reg(&pObj->Ctx, ST7735_NORMAL_DISPLAY_OFF, &tmp, 1);
  /*EnaBLe test command */
  tmp = 0x01U;
  ret += st7735_write_reg(&pObj->Ctx, ST7735_EXT_CTRL, &tmp, 1);
  /*DisaBLe ram power save mode*/
  tmp = 0x00U;
  ret += st7735_write_reg(&pObj->Ctx, 0xF6, &tmp, 1);
  /* Main screen turn on, no delay */
  ret += st7735_write_reg(&pObj->Ctx, ST7735_DISPLAY_ON, &tmp, 1);

  /* Set the display Orientation and the default display window */
  ret += ST7735_SetOrientation(pObj, Orientation);

  if (ret != ST7735_OK)
  {
    ret = ST7735_ERROR;
  }

  return ret;
}

/**
 * @brief  De-Initialize the st7735 LCD Component.
 * @param  pObj Component object
 * @retval Component status
 */
int32_t ST7735_DeInit(ST7735_Object_t *pObj)
{
  (void)(pObj);

  return ST7735_OK;
}

/**
 * @brief  Get the st7735 ID.
 * @param  pObj Component object
 * @param  Id Component ID
 * @retval The component status
 */
int32_t ST7735_ReadID(ST7735_Object_t *pObj, uint32_t *Id)
{
  int32_t ret;
  uint8_t tmp;

  if (st7735_read_reg(&pObj->Ctx, ST7735_READ_ID1, &tmp) != ST7735_OK)
  {
    ret = ST7735_ERROR;
  }
  else if (st7735_recv_data(&pObj->Ctx, &tmp, 1) != ST7735_OK)
  {
    ret = ST7735_ERROR;
  }
  else
  {
    *Id = (uint32_t)tmp;
    ret = ST7735_OK;
  }

  return ret;
}

/**
 * @brief  Enables the Display.
 * @param  pObj Component object
 * @retval The component status
 */
int32_t ST7735_DisplayOn(ST7735_Object_t *pObj)
{
  int32_t ret;
  uint8_t tmp = 0;

  ret = st7735_write_reg(&pObj->Ctx, ST7735_NORMAL_DISPLAY_OFF, &tmp, 0);
  (void)ST7735_IO_Delay(pObj, 10);
  ret += st7735_write_reg(&pObj->Ctx, ST7735_DISPLAY_ON, &tmp, 0);
  (void)ST7735_IO_Delay(pObj, 10);
  ret += st7735_write_reg(&pObj->Ctx, ST7735_MADCTL, &tmp, 0);
  tmp = 0xC0U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  if (ret != ST7735_OK)
  {
    ret = ST7735_ERROR;
  }

  return ret;
}

/**
 * @brief  Disables the Display.
 * @param  pObj Component object
 * @retval The component status
 */
int32_t ST7735_DisplayOff(ST7735_Object_t *pObj)
{
  int32_t ret;
  uint8_t tmp = 0;

  ret = st7735_write_reg(&pObj->Ctx, ST7735_NORMAL_DISPLAY_OFF, &tmp, 0);
  (void)ST7735_IO_Delay(pObj, 10);
  ret += st7735_write_reg(&pObj->Ctx, ST7735_DISPLAY_OFF, &tmp, 0);
  (void)ST7735_IO_Delay(pObj, 10);
  ret += st7735_write_reg(&pObj->Ctx, ST7735_MADCTL, &tmp, 0);
  tmp = 0xC0U;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  if (ret != ST7735_OK)
  {
    ret = ST7735_ERROR;
  }

  return ret;
}

/**
 * @brief  Set the display brightness.
 * @param  pObj Component object
 * @param  Brightness   display brightness to be set
 * @retval Component status
 */
int32_t ST7735_SetBrightness(ST7735_Object_t *pObj, uint32_t Brightness)
{
  (void)(pObj);
  (void)(Brightness);

  /* Feature not supported */
  return ST7735_ERROR;
}

/**
 * @brief  Get the display brightness.
 * @param  pObj Component object
 * @param  Brightness   display brightness to be returned
 * @retval Component status
 */
int32_t ST7735_GetBrightness(ST7735_Object_t *pObj, uint32_t *Brightness)
{
  (void)(pObj);
  (void)(Brightness);

  /* Feature not supported */
  return ST7735_ERROR;
}

/**
 * @brief  Set the Display Orientation.
 * @param  pObj Component object
 * @param  Orientation ST7735_ORIENTATION_PORTRAIT, ST7735_ORIENTATION_PORTRAIT_ROT180
 *                     ST7735_ORIENTATION_LANDSCAPE or ST7735_ORIENTATION_LANDSCAPE_ROT180
 * @retval The component status
 */
int32_t ST7735_SetOrientation(ST7735_Object_t *pObj, uint32_t Orientation)
{
  int32_t ret = 0;
  uint8_t tmp;

  if ((Orientation == ST7735_ORIENTATION_PORTRAIT) ||
      (Orientation == ST7735_ORIENTATION_PORTRAIT_ROT180))
  {
    ST7735Ctx.Width = ST7735_WIDTH;
    ST7735Ctx.Height = ST7735_HEIGHT;
  }
  else
  {
    ST7735Ctx.Width = ST7735_HEIGHT;
    ST7735Ctx.Height = ST7735_WIDTH;
  }

  ret = ST7735_SetDisplayWindow(pObj, 0U, 0U, ST7735Ctx.Width, ST7735Ctx.Height);

  tmp = OrientationTab[Orientation][1];
  ret += st7735_write_reg(&pObj->Ctx, ST7735_MADCTL, &tmp, 1);

  ST7735Ctx.Orientation = Orientation;

  if (ret != ST7735_OK)
  {
    ret = ST7735_ERROR;
  }

  return ret;
}

/**
 * @brief  Set the Display Orientation.
 * @param  pObj Component object
 * @param  Orientation ST7735_ORIENTATION_PORTRAIT, ST7735_ORIENTATION_LANDSCAPE
 *                      or ST7735_ORIENTATION_LANDSCAPE_ROT180
 * @retval The component status
 */
int32_t ST7735_GetOrientation(ST7735_Object_t *pObj, uint32_t *Orientation)
{

  *Orientation = ST7735Ctx.Orientation;

  return ST7735_OK;
}

/**
 * @brief  Set Cursor position.
 * @param  pObj Component object
 * @param  Xpos specifies the X position.
 * @param  Ypos specifies the Y position.
 * @retval The component status
 */
int32_t ST7735_SetCursor(ST7735_Object_t *pObj, uint32_t Start_Xpos, uint32_t Start_Ypos, uint32_t End_Xpos, uint32_t End_Ypos)
{
  int32_t ret;
  uint8_t tmp;
  /* Column addr set, 4 args, no delay: XSTART = Xpos, XEND = (Xpos + Width - 1) */
  ret = st7735_write_reg(&pObj->Ctx, ST7735_CASET, &tmp, 0);
  tmp = 0x02;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = Start_Xpos + 2;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x02;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = End_Xpos + 2;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);

  /* Row addr set, 4 args, no delay: YSTART = Ypos, YEND = (Ypos + Height - 1) */
  ret += st7735_write_reg(&pObj->Ctx, ST7735_RASET, &tmp, 0);
  tmp = 0x01;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = Start_Ypos + 1;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = 0x01;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  tmp = End_Ypos + 1;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  ret += st7735_write_reg(&pObj->Ctx, ST7735_WRITE_RAM, &tmp, 0);

  if (ret != ST7735_OK)
  {
    ret = ST7735_ERROR;
  }

  return ret;
}

/**
 * @brief  Displays an ASCII character.
 * @param  pObj Component object
 * @param  Xpos specifies the X position.
 * @param  Ypos specifies the Y position.
 * @param  num  Characters displayed.
 * @param  size Word size.
 * @param  fColor ForeColor.
 * @param  bColor Background Color.
 * @param  mode   display mode.
 * @retval The component status
 */
int32_t ST7735_DrawChar(ST7735_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint8_t num, uint8_t size, uint32_t fColor, uint32_t bColor, uint8_t mode)
{
  int32_t ret = ST7735_OK;
  uint8_t ctemp = 0;
  uint16_t Ystart = Ypos;
  /*得到字体一个字符对应点阵集所占的字节数*/
  uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
  /*得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）*/
  num = num - ' ';
  for (uint8_t i = 0; i < csize; i++)
  {
    switch (size)
    {
    case 12U:
      ctemp = asc2_1206[num][i]; // 调用1206字体
      break;
    case 16U:
      ctemp = asc2_1608[num][i]; // 调用1608字体
      break;
    case 24U:
      ctemp = asc2_2412[num][i]; // 调用2412字体
      break;
    case 32U:
      ctemp = asc2_3216[num][i]; // 调用3216字体
      break;
    default:
      ret = ST7735_ERROR; // 没有的字库
      break;
    }

    for (uint8_t j = 0; j < 8U; j++)
    {
      if (ctemp & 0x80)
      {
        ST7735_SetPixel(pObj, Xpos, Ypos, fColor); // 字体 画点
      }
      else if (mode == ST7735_NOTOVERLAY_DISPLAY)
      {
        ST7735_SetPixel(pObj, Xpos, Ypos, bColor); // 背景 画点
      }
      ctemp <<= 1U;
      Ypos++;
      if (Ypos >= ST7735Ctx.Height) // 超出屏幕高度(底)
      {
        ret = ST7735_ERROR;
        break;
      }
      if ((Ypos - Ystart) == size)
      {
        Ypos = Ystart;
        Xpos++;
        if (Xpos >= ST7735Ctx.Width) // 超出屏幕宽度(宽)
        {
          ret = ST7735_ERROR;
          break;
        }
        break;
      }
    }
  }
  return ret;
}

/**
 * @brief  Displays string.
 * @param  pObj Component object
 * @param  Xpos specifies the X position.
 * @param  Ypos specifies the Y position.
 * @param  string  an string.
 * @param  size Word size.
 * @param  fColor ForeColor.
 * @param  bColor Background Color.
 * @retval The component status
 */
int32_t ST7735_DrawString(ST7735_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint8_t *string, uint8_t size, uint32_t fColor, uint32_t bColor)
{
  int32_t ret = ST7735_OK;
  uint8_t Xstart = Xpos;
  /*没有到字符串尾部*/
  while (*string != 0)
  {
    /*如果这一行不够位置，就下一行 */
    if (Xpos > (ST7735Ctx.Width - size))
    {
      Xpos = Xstart;
      Ypos = Ypos + size;
    } /*如果到了屏幕底部，就返回，不再输出*/
    if (Ypos > (ST7735Ctx.Height - size))
    {
      ret = ST7735_ERROR;
      break;
    }
    /*判断文字是ASCII还是汉字*/
    if ((*string >= ' ') && (*string <= '~'))
    { /*非叠加模式显示*/
      ST7735_DrawChar(pObj, Xpos, Ypos, *string, size, fColor, bColor, ST7735_NOTOVERLAY_DISPLAY);
      string++;
      Xpos += size / 2U;
    }
    else // 汉字显示
    {
      // // 重要: 如果用的不是魔女开发板的字库, 就要修改或注释下面这一行, 这样就不影响ASCII英文字符的输出
      // drawGBK(x, y, (u8*)pFont, size, fColor, bColor);
      // pFont = pFont + 2;          // 下一个要显示的数据在内存中的位置
      // x = x+size;              // 下一个要显示的数据在屏幕上的X位置
    }
  }
  return ret;
}

/**
 * @brief  Displays a bitmap picture.
 * @param  pObj Component object
 * @param  Xpos Bmp X position in the LCD
 * @param  Ypos Bmp Y position in the LCD
 * @param  pBmp Bmp picture address.
 * @retval The component status
 */
int32_t ST7735_DrawBitmap(ST7735_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t height, uint8_t *pBmp)
{
  int32_t ret = ST7735_OK;
  uint8_t pixel_val[2] = {0};

  if ((Xpos + width) > ST7735Ctx.Width)
  {
    ret = ST7735_ERROR;
  }
  else if ((Ypos + height) > ST7735Ctx.Height)
  {
    ret = ST7735_ERROR;
  }
  else
  { /* Set Cursor */
    if (ST7735_SetCursor(pObj, Xpos, Ypos, Xpos + width - 1U, Ypos + height - 1U) != ST7735_OK)
    {
      ret = ST7735_ERROR;
    }
    else /*填充颜色*/
    {
      for (uint16_t i = 0; i < width * height; i++)
      { /*STM32属于小端存储，数据高位在前*/
        pixel_val[0] = *(pBmp + i * 2 + 1);
        pixel_val[1] = *(pBmp + i * 2);
        if (st7735_send_data(&pObj->Ctx, pixel_val, sizeof(pixel_val)) != ST7735_OK)
        {
          ret = ST7735_ERROR;
          break;
        }
      }
    }
  }

  return ret;
}

/**
 * @brief  Draws a full RGB rectangle
 * @param  pObj Component object
 * @param  Xpos   specifies the X position.
 * @param  Ypos   specifies the Y position.
 * @param  pData  pointer to RGB data
 * @param  Width  specifies the rectangle width.
 * @param  Height Specifies the rectangle height
 * @retval The component status
 */
int32_t ST7735_FillRGBRect(ST7735_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint8_t *pData, uint32_t Width, uint32_t Height)
{
  int32_t ret = ST7735_OK;
  static uint8_t pdata[640];
  uint8_t *rgb_data = pData;
  uint32_t i, j;

  if (((Xpos + Width) > ST7735Ctx.Width) || ((Ypos + Height) > ST7735Ctx.Height))
  {
    ret = ST7735_ERROR;
  } /* Set Cursor */
  else
  {
    for (j = 0; j < Height; j++)
    {
      for (i = 0; i < Width; i++)
      {
        pdata[2U * i] = (uint8_t)(*(rgb_data + 1U));
        pdata[(2U * i) + 1U] = (uint8_t)(*rgb_data);
        rgb_data += 2U;
        if (ST7735_SetCursor(pObj, Xpos + i, Ypos + j, Xpos + i + 1U, Ypos + j + 1U) != ST7735_OK)
        {
          ret = ST7735_ERROR;
        }
        if (st7735_send_data(&pObj->Ctx, (uint8_t *)&pdata[0], 2U * Width) != ST7735_OK)
        {
          ret = ST7735_ERROR;
        }
      }
    }
  }

  return ret;
}

/**
 * @brief  Draw Horizontal line.
 * @param  pObj Component object
 * @param  Xpos   specifies the X position.
 * @param  Ypos   specifies the Y position.
 * @param  Length specifies the Line length.
 * @param  Color  Specifies the RGB color in RGB565 format
 * @retval The component status
 */
int32_t ST7735_DrawHLine(ST7735_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color)
{
  int32_t ret = ST7735_OK;
  uint32_t i;
  static uint8_t pdata[640];

  if ((Xpos + Length) > ST7735Ctx.Width)
  {
    ret = ST7735_ERROR;
  } /* Set Cursor */
  else if (ST7735_SetCursor(pObj, Xpos, Ypos, Xpos + Length, Ypos + 1) != ST7735_OK)
  {
    ret = ST7735_ERROR;
  }
  else
  {
    for (i = 0; i < Length; i++)
    {
      /* Exchange LSB and MSB to fit LCD specification */
      pdata[2U * i] = (uint8_t)(Color >> 8);
      pdata[(2U * i) + 1U] = (uint8_t)(Color);
    }
    if (st7735_send_data(&pObj->Ctx, (uint8_t *)&pdata[0], 2U * Length) != ST7735_OK)
    {
      ret = ST7735_ERROR;
    }
  }

  return ret;
}

/**
 * @brief  Draw vertical line.
 * @param  pObj Component object
 * @param  Color    Specifies the RGB color
 * @param  Xpos     specifies the X position.
 * @param  Ypos     specifies the Y position.
 * @param  Length   specifies the Line length.
 * @retval The component status
 */
int32_t ST7735_DrawVLine(ST7735_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color)
{
  int32_t ret = ST7735_OK;
  uint32_t counter;

  if ((Ypos + Length) > ST7735Ctx.Height)
  {
    ret = ST7735_ERROR;
  }
  else
  {
    for (counter = 0; counter < Length; counter++)
    {
      if (ST7735_SetPixel(pObj, Xpos, Ypos + counter, Color) != ST7735_OK)
      {
        ret = ST7735_ERROR;
        break;
      }
    }
  }

  return ret;
}

/**
 * @brief  Fill rectangle
 * @param  pObj Component object
 * @param  Xpos X position
 * @param  Ypos Y position
 * @param  Width Rectangle width
 * @param  Height Rectangle height
 * @param  Color Draw color
 * @retval Component status
 */
int32_t ST7735_FillRect(ST7735_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t Color)
{
  int32_t ret = ST7735_OK;
  uint32_t i, y_pos = Ypos;

  for (i = 0; i < Height; i++)
  {
    if (ST7735_DrawHLine(pObj, Xpos, y_pos, Width, Color) != ST7735_OK)
    {
      ret = ST7735_ERROR;
      break;
    }
    y_pos++;
  }

  return ret;
}

/**
 * @brief  Write pixel.
 * @param  pObj Component object
 * @param  Xpos specifies the X position.
 * @param  Ypos specifies the Y position.
 * @param  Color the RGB pixel color in RGB565 format
 * @retval The component status
 */
int32_t ST7735_SetPixel(ST7735_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Color)
{
  int32_t ret = ST7735_OK;
  uint16_t color;

  /* Exchange LSB and MSB to fit LCD specification */
  color = (uint16_t)((uint16_t)Color << 8);
  color |= (uint16_t)((uint16_t)(Color >> 8));

  if ((Xpos >= ST7735Ctx.Width) || (Ypos >= ST7735Ctx.Height))
  {
    ret = ST7735_ERROR;
  } /* Set Cursor */
  else if (ST7735_SetCursor(pObj, Xpos, Ypos, Xpos, Ypos) != ST7735_OK)
  {
    ret = ST7735_ERROR;
  }
  else
  {
    /* Write RAM data */
    if (st7735_send_data(&pObj->Ctx, (uint8_t *)&color, 2) != ST7735_OK)
    {
      ret = ST7735_ERROR;
    }
  }

  return ret;
}

/**
 * @brief  Read pixel.
 * @param  pObj Component object
 * @param  Xpos specifies the X position.
 * @param  Ypos specifies the Y position.
 * @param  Color the RGB pixel color in RGB565 format
 * @retval The component status
 */
int32_t ST7735_GetPixel(ST7735_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t *Color)
{
  int32_t ret = ST7735_OK;
  uint8_t pixel_lsb, pixel_msb;
  uint8_t tmp;

  /* Set Cursor */
  // ret = ST7735_SetCursor(pObj, Xpos, Ypos);

  /* Prepare to read LCD RAM */
  ret += st7735_read_reg(&pObj->Ctx, ST7735_READ_RAM, &tmp); /* RAM read data command */

  /* Dummy read */
  ret += st7735_recv_data(&pObj->Ctx, &tmp, 1);

  /* Read first part of the RGB888 data */
  ret += st7735_recv_data(&pObj->Ctx, &pixel_lsb, 1);
  /* Read first part of the RGB888 data */
  ret += st7735_recv_data(&pObj->Ctx, &pixel_msb, 1);

  *Color = ((uint32_t)(pixel_lsb)) + ((uint32_t)(pixel_msb) << 8);

  if (ret != ST7735_OK)
  {
    ret = ST7735_ERROR;
  }

  return ret;
}

/**
 * @brief  Get the LCD pixel Width.
 * @param  pObj Component object
 * @retval The Lcd Pixel Width
 */
int32_t ST7735_GetXSize(ST7735_Object_t *pObj, uint32_t *XSize)
{
  (void)pObj;

  *XSize = ST7735Ctx.Width;

  return ST7735_OK;
}

/**
 * @brief  Get the LCD pixel Height.
 * @param  pObj Component object
 * @retval The Lcd Pixel Height
 */
int32_t ST7735_GetYSize(ST7735_Object_t *pObj, uint32_t *YSize)
{
  (void)pObj;

  *YSize = ST7735Ctx.Height;

  return ST7735_OK;
}

/**
 * @}
 */

/** @defgroup ST7735_Private_Functions  Private Functions
 * @{
 */
/**
 * @brief  Sets a display window
 * @param  Xpos   specifies the X bottom left position.
 * @param  Ypos   specifies the Y bottom left position.
 * @param  Height display window height.
 * @param  Width  display window width.
 * @retval Component status
 */
static int32_t ST7735_SetDisplayWindow(ST7735_Object_t *pObj,
                                       uint32_t Xpos,
                                       uint32_t Ypos,
                                       uint32_t Width,
                                       uint32_t Height)
{
  int32_t ret;
  uint8_t tmp;

  /* Column addr set, 4 args, no delay: XSTART = Xpos, XEND = (Xpos + Width - 1) */
  ret = st7735_write_reg(&pObj->Ctx, ST7735_CASET, (uint8_t *)&Xpos, 1);
  tmp = (uint8_t)Xpos + 2;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  ret += st7735_send_data(&pObj->Ctx, (uint8_t *)&Xpos, 1);
  tmp = 0x80 + 2;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);

  /* Row addr set, 4 args, no delay: YSTART = Ypos, YEND = (Ypos + Height - 1) */
  ret += st7735_write_reg(&pObj->Ctx, ST7735_RASET, (uint8_t *)&Ypos, 1);
  tmp = (uint8_t)Ypos + 3;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);
  ret += st7735_send_data(&pObj->Ctx, (uint8_t *)&Ypos, 1);
  tmp = 0x80 + 3;
  ret += st7735_send_data(&pObj->Ctx, &tmp, 1);

  if (ret != ST7735_OK)
  {
    ret = ST7735_ERROR;
  }

  return ret;
}

/**
 * @brief  Wrap component ReadReg to Bus Read function
 * @param  Handle  Component object handle
 * @param  Reg  The target register address to write
 * @param  pData  The target register value to be written
 * @retval Component error status
 */
static int32_t ST7735_ReadRegWrap(void *Handle, uint8_t Reg, uint8_t *pData)
{
  ST7735_Object_t *pObj = (ST7735_Object_t *)Handle;

  return pObj->IO.ReadReg(Reg, pData);
}

/**
 * @brief  Wrap component WriteReg to Bus Write function
 * @param  handle  Component object handle
 * @param  Reg  The target register address to write
 * @param  pData  The target register value to be written
 * @param  Length  buffer size to be written
 * @retval Component error status
 */
static int32_t ST7735_WriteRegWrap(void *Handle, uint8_t Reg, uint8_t *pData, uint32_t Length)
{
  ST7735_Object_t *pObj = (ST7735_Object_t *)Handle;

  return pObj->IO.WriteReg(Reg, pData, Length);
}

/**
 * @brief  Wrap component SendData to Bus Write function
 * @param  handle  Component object handle
 * @param  pData  The target register value to be written
 * @retval Component error status
 */
static int32_t ST7735_SendDataWrap(void *Handle, uint8_t *pData, uint32_t Length)
{
  ST7735_Object_t *pObj = (ST7735_Object_t *)Handle;

  return pObj->IO.SendData(pData, Length);
}

/**
 * @brief  Wrap component SendData to Bus Write function
 * @param  handle  Component object handle
 * @param  pData  The target register value to be written
 * @retval Component error status
 */
static int32_t ST7735_RecvDataWrap(void *Handle, uint8_t *pData, uint32_t Length)
{
  ST7735_Object_t *pObj = (ST7735_Object_t *)Handle;

  return pObj->IO.RecvData(pData, Length);
}

/**
 * @brief  ST7735 delay
 * @param  Delay  Delay in ms
 * @retval Component error status
 */
static int32_t ST7735_IO_Delay(ST7735_Object_t *pObj, uint32_t Delay)
{
  // uint32_t tickstart;
  // tickstart = pObj->IO.GetTick();
  // while ((pObj->IO.GetTick() - tickstart) < Delay)
  // {
  // }
  if (pObj->IO.Delay)
    pObj->IO.Delay(Delay);
  else
    return ST7735_ERROR;

  return ST7735_OK;
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
