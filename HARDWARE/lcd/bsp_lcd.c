/**
  ******************************************************************************
  * @file    bsp_sdram.c
  * @author  fire
  * @version V1.1
  * @date    2015-xx-xx
  * @brief   LCDӦ�ú����ӿڣ�֧��RGB888/565����������ʾ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F429 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  

/* Includes ------------------------------------------------------------------*/
#include "./lcd/bsp_lcd.h"
#include "pbdata.h"
#include "hzk.h"
#include "./font/fonts.h"


/** @addtogroup Utilities
  * @{
  */ 

/** @addtogroup STM32F4_DISCOVERY
  * @{
  */ 

/** @addtogroup STM32F429I_DISCOVERY
  * @{
  */
    
/** @defgroup STM32F429I_DISCOVERY_LCD 
  * @brief This file includes the LCD driver for (ILI9341) 
  * @{
  */ 

/** @defgroup STM32F429I_DISCOVERY_LCD_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM32F429I_DISCOVERY_LCD_Private_Defines
  * @{
  */
Typedf_Disp_Colour Colour; 
#define POLY_Y(Z)          ((int32_t)((Points + Z)->X))
#define POLY_X(Z)          ((int32_t)((Points + Z)->Y))   
/**
  * @}
  */ 

/** @defgroup STM32F429I_DISCOVERY_LCD_Private_Macros
  * @{
  */
#define ABS(X)  ((X) > 0 ? (X) : -(X))    
/**
  * @}
  */ 
	

#if  LCD_RGB_888
/****************RGB888����*****************************RGB888����*****************************RGB888����**********************************RGB888����**********************************RGB888����*********************************/
	
  
/** @defgroup STM32F429I_DISCOVERY_LCD_Private_Variables
  * @{
  */ 
static sFONT *LCD_Currentfonts;
/* Global variables to set the written text color */
static uint32_t CurrentTextColor   = 0x000000;
static uint32_t CurrentBackColor   = 0xFFFFFF;
/* Default LCD configuration with LCD Layer 1 */
static uint32_t CurrentFrameBuffer = LCD_FRAME_BUFFER;
static uint32_t CurrentLayer = LCD_BACKGROUND_LAYER;
//static uint32_t g_LcdDirection;
/**
  * @}
  */ 

/** @defgroup STM32F429I_DISCOVERY_LCD_Private_FunctionPrototypes
  * @{
  */ 


static void LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount, uint16_t Closed);
static void LCD_GPIO_Config(void);

/**
  * @}
  */ 

 

/**
  * @brief  Initializes the LCD.
  * @param  None
  * @retval None
  */
#define HBP  46
#define VBP   23

#define HSW   1
#define VSW   1

#define HFP  22
#define VFP   22
#define ZOOMMAXBUFF 16384
uint8_t zoomBuff[ZOOMMAXBUFF] = {0};	//�������ŵĻ��棬���֧�ֵ�128*128

unsigned int findHzIndex_16(const unsigned char *hz)                                                                       
{
    uint16_t j;
  //�FNT_GB16 *ptGb16 = (FNT_GB16 *)GBHZ_16;
    for(j=0;j<sizeof(GB_16)/sizeof(GB_16[0]);j++)
	{
        //if ((*hz == ptGb16->Index[0]) && (*(hz+1) == ptGb16->Index[1])) FNT_GB16//&&(*(hz+1) == GB_21[j].Index[1])
		if((GB_16[j].Index[0]==*hz)&&(*(hz+1)==GB_16[j].Index[1]))
        {
            return j;
        }
    }
    return 0;
}



void LCD_Init(void)
{ 
  LTDC_InitTypeDef       LTDC_InitStruct;
  
  /* ʹ��LTDC����ʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_LTDC, ENABLE);
  
  /* ʹ��DMA2Dʱ�� */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2D, ENABLE);
	
	/* ��ʼ��LCD�Ŀ������� */
  LCD_GPIO_Config();   
	
	/* ��ʼ��SDRAM���Ա�ʹ��SDRAM���Դ� */
  SDRAM_Init();
	
	/* ���� PLLSAI ��Ƶ�������������Ϊ����ͬ��ʱ��CLK*/
  /* PLLSAI_VCO ����ʱ�� = HSE_VALUE/PLL_M = 1 Mhz */
  /* PLLSAI_VCO ���ʱ�� = PLLSAI_VCO���� * PLLSAI_N = 416 Mhz */
  /* PLLLCDCLK = PLLSAI_VCO ���/PLLSAI_R = 420/6  Mhz */
  /* LTDC ʱ��Ƶ�� = PLLLCDCLK / DIV = 420/6/8 = 8.75 Mhz */
	/* LTDCʱ��̫�߻ᵼ����������ˢ���ٶ�Ҫ�󲻸ߣ�����ʱ��Ƶ�ʿɼ��ٻ�������*/
	/* ���º������������ֱ�Ϊ��PLLSAIN,PLLSAIQ,PLLSAIR������PLLSAIQ��LTDC�޹�*/
  RCC_PLLSAIConfig(300,7, 6);
	/*���º����Ĳ���ΪDIVֵ*/
  RCC_LTDCCLKDivConfig(RCC_PLLSAIDivR_Div4);
  
  /* ʹ�� PLLSAI ʱ�� */
  RCC_PLLSAICmd(ENABLE);
  /* �ȴ� PLLSAI ��ʼ����� */
  while(RCC_GetFlagStatus(RCC_FLAG_PLLSAIRDY) == RESET)
  {
  }
  
  /* LTDC����*********************************************************/  
  /*�źż�������*/
  /* ��ͬ���źż��� */
  LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AL;     
  /* ��ֱͬ���źż��� */  
  LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;     
  /* ����ʹ���źż��� */
  LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AL;     
  /* ����ͬ��ʱ�Ӽ��� */ 
  LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IPC;
  
  /* ����LCD������ɫ */                   
  LTDC_InitStruct.LTDC_BackgroundRedValue = 0;            
  LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;          
  LTDC_InitStruct.LTDC_BackgroundBlueValue = 0;    
 
  /* ʱ��������� */  
 /* ������ͬ���źſ���(HSW-1) */
 LTDC_InitStruct.LTDC_HorizontalSync =HSW-1;
 /* ���ô�ֱͬ���źſ���(VSW-1) */
 LTDC_InitStruct.LTDC_VerticalSync = VSW-1;
 /* ����(HSW+HBP-1) */
 LTDC_InitStruct.LTDC_AccumulatedHBP =HSW+HBP-1;
 /* ����(VSW+VBP-1) */
 LTDC_InitStruct.LTDC_AccumulatedVBP = VSW+VBP-1;
 /* ����(HSW+HBP+��Ч���ؿ���-1) */
 LTDC_InitStruct.LTDC_AccumulatedActiveW = HSW+HBP+LCD_PIXEL_WIDTH-1;
 /* ����(VSW+VBP+��Ч���ظ߶�-1) */
 LTDC_InitStruct.LTDC_AccumulatedActiveH = VSW+VBP+LCD_PIXEL_HEIGHT-1;
 /* �����ܿ���(HSW+HBP+��Ч���ؿ���+HFP-1) */
 LTDC_InitStruct.LTDC_TotalWidth =HSW+ HBP+LCD_PIXEL_WIDTH  + HFP-1; 
 /* �����ܸ߶�(VSW+VBP+��Ч���ظ߶�+VFP-1) */
 LTDC_InitStruct.LTDC_TotalHeigh =VSW+ VBP+LCD_PIXEL_HEIGHT  + VFP-1;

  LTDC_Init(&LTDC_InitStruct);
  
  LTDC_Cmd(ENABLE);
}  


/**
  * @brief ��ʼ��LTD�� �� ����
  *           - �����Դ�ռ�
  *           - ���÷ֱ���
  * @param  None
  * @retval None
  */
void LCD_LayerInit(void)
{
  LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct; 
  
  /* �㴰������ */
  /* ���ñ���Ĵ��ڱ߽磬ע����Щ�����ǰ���HBP HSW VBP VSW�� */    
	//һ�еĵ�һ����ʼ���أ��ó�ԱֵӦ��Ϊ (LTDC_InitStruct.LTDC_AccumulatedHBP+1)��ֵ
	LTDC_Layer_InitStruct.LTDC_HorizontalStart = HBP + HSW;
	//һ�е����һ�����أ��ó�ԱֵӦ��Ϊ (LTDC_InitStruct.LTDC_AccumulatedActiveW)��ֵ
	LTDC_Layer_InitStruct.LTDC_HorizontalStop = HSW+HBP+LCD_PIXEL_WIDTH-1;
	//һ�еĵ�һ����ʼ���أ��ó�ԱֵӦ��Ϊ (LTDC_InitStruct.LTDC_AccumulatedVBP+1)��ֵ
	LTDC_Layer_InitStruct.LTDC_VerticalStart =  VBP + VSW;
	//һ�е����һ�����أ��ó�ԱֵӦ��Ϊ (LTDC_InitStruct.LTDC_AccumulatedActiveH)��ֵ
	LTDC_Layer_InitStruct.LTDC_VerticalStop = VSW+VBP+LCD_PIXEL_HEIGHT-1;
	
  /* ���ظ�ʽ����*/
  LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB888;
  /* �㶨Alphaֵ���ã�0-255 */
  LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255; 
  /* Ĭ�ϱ�����ɫ������ɫ�ڶ���Ĳ㴰������ڲ��ֹʱʹ�á� */          
  LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0xFF;        
  LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0xFF;       
  LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0xFF;         
  LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0xFF;
  /* ���û������ CA��ʾʹ�ú㶨Alphaֵ��PAxCA��ʾʹ������Alpha x �㶨Alphaֵ */       
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;    
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_PAxCA;
  
  /* �ó�ԱӦд��(һ����������ռ�õ��ֽ���+3)
  Line Lenth = ����Ч���ظ��� x ÿ�����ص��ֽ��� + 3 
  ����Ч���ظ��� = LCD_PIXEL_WIDTH 
  ÿ�����ص��ֽ��� = 2��RGB565/RGB1555��/ 3 (RGB888)/ 4��ARGB8888�� 
  */
  LTDC_Layer_InitStruct.LTDC_CFBLineLength = ((LCD_PIXEL_WIDTH * 3) + 3);
  /* ��ĳ�е���ʼλ�õ���һ����ʼλ�ô�����������
  Pitch = ����Ч���ظ��� x ÿ�����ص��ֽ��� */ 
  LTDC_Layer_InitStruct.LTDC_CFBPitch = (LCD_PIXEL_WIDTH * 3);
  
  /* ������Ч������ */  
  LTDC_Layer_InitStruct.LTDC_CFBLineNumber = LCD_PIXEL_HEIGHT;
  
  /* ���ñ�����Դ��׵�ַ */    
  LTDC_Layer_InitStruct.LTDC_CFBStartAdress = LCD_FRAME_BUFFER;
  
  /* ����������ó�ʼ���� 1 ��*/
//  LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);

  /*���õ� 2 �㣬��û����дĳ����Ա��ֵ����ó�Աʹ�ø���1��һ�������� */
  /* ���ñ�����Դ��׵�ַ�����������������ڵ�1��ĺ���*/     
  LTDC_Layer_InitStruct.LTDC_CFBStartAdress = LCD_FRAME_BUFFER + BUFFER_OFFSET;

	/* ���û�����ӣ�ʹ������Alpha������ */       
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_PAxCA;    
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_PAxCA;

  /* ��ʼ����2�� */
  LTDC_LayerInit(LTDC_Layer2, &LTDC_Layer_InitStruct);
 
 /* ������������ */  
 LTDC_ReloadConfig(LTDC_IMReload);

 /*ʹ��ǰ���������� */
//  LTDC_LayerCmd(LTDC_Layer1, ENABLE); 
  LTDC_LayerCmd(LTDC_Layer2, ENABLE);
 
  /* ������������ */  
  LTDC_ReloadConfig(LTDC_IMReload);

  /* �趨����(Ӣ��) */    
  LCD_SetFont(&LCD_DEFAULT_FONT); 
}



/**
  * @brief  Sets the LCD Layer.
  * @param  Layerx: specifies the Layer foreground or background.
  * @retval None
  */
void LCD_SetLayer(uint32_t Layerx)
{
  if (Layerx == LCD_BACKGROUND_LAYER)
  {
    CurrentFrameBuffer = LCD_FRAME_BUFFER; 
    CurrentLayer = LCD_BACKGROUND_LAYER;
  }
  else
  {
    CurrentFrameBuffer = LCD_FRAME_BUFFER + BUFFER_OFFSET;
    CurrentLayer = LCD_FOREGROUND_LAYER;
  }
}  

/**
  * @brief  Sets the LCD Text and Background colors.
  * @param  TextColor: specifies the Text Color.
  * @param  BackColor: specifies the Background Color.
  * @retval None
  */
void LCD_SetColors(uint32_t TextColor, uint32_t BackColor) 
{
  CurrentTextColor = TextColor; 
  CurrentBackColor = BackColor;
}

/**
  * @brief  Gets the LCD Text and Background colors.
  * @param  TextColor: pointer to the variable that will contain the Text 
            Color.
  * @param  BackColor: pointer to the variable that will contain the Background 
            Color.
  * @retval None
  */
void LCD_GetColors(uint32_t *TextColor, uint32_t *BackColor)
{
  *TextColor = CurrentTextColor;
  *BackColor = CurrentBackColor;
}

/**
  * @brief  Sets the Text color.
  * @param  Color: specifies the Text color code RGB(5-6-5).
  * @retval None
  */
void LCD_SetTextColor(uint32_t Color)
{
  CurrentTextColor = Color;
}

/**
  * @brief  Sets the Background color.
  * @param  Color: specifies the Background color code RGB(5-6-5).
  * @retval None
  */
void LCD_SetBackColor(uint32_t Color)
{
  CurrentBackColor = Color;
}

/**
  * @brief  Sets the Text Font.
  * @param  fonts: specifies the font to be used.
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts;
}

/**
  * @brief  Configure the transparency.
  * @param  transparency: specifies the transparency, 
  *         This parameter must range from 0x00 to 0xFF.
  * @retval None
  */
void LCD_SetTransparency(uint8_t transparency)
{
  if (CurrentLayer == LCD_BACKGROUND_LAYER)
  {
    LTDC_LayerAlpha(LTDC_Layer1, transparency);
  }
  else
  {     
    LTDC_LayerAlpha(LTDC_Layer2, transparency);
  }
  LTDC_ReloadConfig(LTDC_IMReload);
}

/**
  * @brief  Gets the Text Font.
  * @param  None.
  * @retval the used font.
  */
sFONT *LCD_GetFont(void)
{
  return LCD_Currentfonts;
}

/**
  * @brief  Clears the selected line.
  * @param  Line: the Line to be cleared.
  *   This parameter can be one of the following values:
  *     @arg LCD_LINE_x: where x can be: 0..13 if LCD_Currentfonts is Font16x24
  *                                      0..26 if LCD_Currentfonts is Font12x12 or Font8x12
  *                                      0..39 if LCD_Currentfonts is Font8x8
  * @retval None
  */
void LCD_ClearLine(uint16_t Line)
{
  uint16_t refcolumn = 0;
  /* Send the string character by character on lCD */
  while ((refcolumn < LCD_PIXEL_WIDTH) && (((refcolumn + LCD_Currentfonts->Width)& 0xFFFF) >= LCD_Currentfonts->Width))
  {
    /* Display one character on LCD */
    LCD_DisplayChar(Line, refcolumn, ' ');
    /* Decrement the column position by 16 */
    refcolumn += LCD_Currentfonts->Width;
  }
}

/**
  * @brief  Clears the hole LCD.
  * @param  Color: the color of the background.
  * @retval None
  */
void LCD_Clear(uint32_t Color)
{

  DMA2D_InitTypeDef      DMA2D_InitStruct;
  
  uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0;
  
 
  Red_Value = (0xFF0000 & Color) >> 16;
  Green_Value = (0x00FF00 & Color) >> 8;
  Blue_Value = 0x0000FF & Color;
  
  /* Configure DMA2D */    
  DMA2D_DeInit();  
  DMA2D_InitStruct.DMA2D_Mode = DMA2D_R2M;       
  DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB888;      
  DMA2D_InitStruct.DMA2D_OutputRed =Red_Value; //Red_Value;     
  DMA2D_InitStruct.DMA2D_OutputGreen =Green_Value;// Green_Value;      
  DMA2D_InitStruct.DMA2D_OutputBlue =Blue_Value;// Blue_Value;     
             
  DMA2D_InitStruct.DMA2D_OutputAlpha = 0x0F;                  
  DMA2D_InitStruct.DMA2D_OutputMemoryAdd = CurrentFrameBuffer;                  
  
  {                                                      
    DMA2D_InitStruct.DMA2D_OutputOffset = 0;                
    DMA2D_InitStruct.DMA2D_NumberOfLine = LCD_PIXEL_HEIGHT;            
    DMA2D_InitStruct.DMA2D_PixelPerLine = LCD_PIXEL_WIDTH; 
  }
  
  DMA2D_Init(&DMA2D_InitStruct);  
  /* Start Transfer */ 
  DMA2D_StartTransfer();  
  /* Wait for CTC Flag activation */
  while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET)
  {
  }  

}

/**
  * @brief  Sets the cursor position.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position. 
  * @retval Display Address
  */
uint32_t LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{  
  return CurrentFrameBuffer + 3*(Xpos + (LCD_PIXEL_WIDTH*Ypos));
}

/**
  * @brief  Config and Sets the color Keying.
  * @param  RGBValue: Specifies the Color reference. 
  * @retval None
  */
void LCD_SetColorKeying(uint32_t RGBValue)
{  
  LTDC_ColorKeying_InitTypeDef   LTDC_colorkeying_InitStruct;
  
  /* configure the color Keying */
  LTDC_colorkeying_InitStruct.LTDC_ColorKeyBlue = 0x0000FF & RGBValue;
  LTDC_colorkeying_InitStruct.LTDC_ColorKeyGreen = (0x00FF00 & RGBValue);
  LTDC_colorkeying_InitStruct.LTDC_ColorKeyRed = (0xFF0000 & RGBValue) ;  

  if (CurrentLayer == LCD_BACKGROUND_LAYER)
  {   
    /* Enable the color Keying for Layer1 */
    LTDC_ColorKeyingConfig(LTDC_Layer1, &LTDC_colorkeying_InitStruct, ENABLE);
    LTDC_ReloadConfig(LTDC_IMReload);
  }
  else
  {
    /* Enable the color Keying for Layer2 */
    LTDC_ColorKeyingConfig(LTDC_Layer2, &LTDC_colorkeying_InitStruct, ENABLE);
    LTDC_ReloadConfig(LTDC_IMReload);
  }
}

/**
  * @brief  Disable the color Keying.
  * @param  RGBValue: Specifies the Color reference. 
  * @retval None
  */
void LCD_ReSetColorKeying(void)
{
  LTDC_ColorKeying_InitTypeDef   LTDC_colorkeying_InitStruct;
  
  if (CurrentLayer == LCD_BACKGROUND_LAYER)
  {   
    /* Disable the color Keying for Layer1 */
    LTDC_ColorKeyingConfig(LTDC_Layer1, &LTDC_colorkeying_InitStruct, DISABLE);
    LTDC_ReloadConfig(LTDC_IMReload);
  }
  else
  {
    /* Disable the color Keying for Layer2 */
    LTDC_ColorKeyingConfig(LTDC_Layer2, &LTDC_colorkeying_InitStruct, DISABLE);
    LTDC_ReloadConfig(LTDC_IMReload);
  }
} 
/*
*********************************************************************************************************
*	�� �� ��: LCD429_PutPixel
*	����˵��: ��1������
*	��    ��:
*			_usX,_usY : ��������
*			_usColor  : ������ɫ ( RGB = 565 ��ʽ)
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD429_PutPixel(uint16_t _usX, uint16_t _usY, uint32_t _usColor)
{
//	uint16_t *p;
	uint32_t index = 0;
	
	//p = (uint16_t *)LCD_FRAME_BUFFER;
//	g_LcdDirection=2;
		
	if (g_LcdDirection == 0)		/* ���� */
	{
		//index = (uint32_t)3*_usY * LCD_PIXEL_WIDTH + _usX;
//		index = (uint32_t)_usY*3 +( LCD_PIXEL_WIDTH *_usX)*3;
		index = (uint32_t)_usY*LCD_PIXEL_WIDTH*3 +( _usX)*3;
	}
	else if (g_LcdDirection == 1)	/* ����180��*/
	{
		index = (uint32_t)((LCD_PIXEL_HEIGHT - _usY - 1) * LCD_PIXEL_WIDTH + (LCD_PIXEL_WIDTH - _usX - 1))*3;
	}
	else if (g_LcdDirection == 2)	/* ���� */
	{
		index = (uint32_t)((LCD_PIXEL_HEIGHT- _usX - 1) * LCD_PIXEL_WIDTH  + _usY)*3;
	}
	else if (g_LcdDirection == 3)	/* ����180�� */
	{
		index = (uint32_t)_usX * LCD_PIXEL_HEIGHT + LCD_PIXEL_HEIGHT - _usY - 1;
	}	
	
	if (index < LCD_PIXEL_HEIGHT * LCD_PIXEL_WIDTH*3)
	{
		*(__IO uint16_t*)(CurrentFrameBuffer+index) = (0x00FFFF & _usColor); 
		*(__IO uint8_t*)(CurrentFrameBuffer+index+2)=(0xFF0000 & _usColor) >> 16;
	}
}
/**
  * @brief  ��ʾһ���ַ���Һ����.
  * @param  Xpos: �ַ�Ҫ��ʾ����Һ���е�ַ.
  * @param  Ypos: �ַ�Ҫ��ʾ����Һ���е�ַ
  * @param  c: ָ�룬ָ��Ҫ��ʾ�ַ�����ģ���ݵĵ�ַ
  * @retval None
  */
void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint16_t *c)
{
  uint32_t index = 0, counter = 0;//, xpos =0;
//  uint32_t  Xaddress = 0;
	u32 x,y;
  
	/*xpos��ʾ��ǰ�е��Դ�ƫ��λ��*/
  //xpos = Xpos*LCD_PIXEL_WIDTH*3;
	/*Xaddress��ʾҺ�����ص�����Һ��������λ��*/
  y= Ypos;
  
	/*index�����м���*/
  for(index = 0; index < LCD_Currentfonts->Height; index++)
  {    
	
	x=Xpos;/*counter�����������ص��λ�ü���*/
    for(counter = 0; counter < LCD_Currentfonts->Width; counter++)
    { 
			/*������ģ�����ж�����ɫ���ػ�����ɫ����*/			
      if((((c[index] & ((0x80 << ((LCD_Currentfonts->Width / 12 ) * 8 ) ) >> counter)) == 0x00) &&(LCD_Currentfonts->Width <= 12))||
        (((c[index] & (0x1 << counter)) == 0x00)&&(LCD_Currentfonts->Width > 12 )))
      {
				/*��ʾ����ɫ*/
//        *(__IO uint16_t*)(CurrentFrameBuffer + (3*Xaddress) + xpos) = (0x00FFFF & CurrentBackColor);        //GB
//        *(__IO uint8_t*)(CurrentFrameBuffer + (3*Xaddress) + xpos+2) = (0xFF0000 & CurrentBackColor) >> 16; //R
		 LCD429_PutPixel(x, y, Colour.black);
      }
      else
      {
				/*��ʾ������ɫ*/
//        *(__IO uint16_t*)(CurrentFrameBuffer + (3*Xaddress) + xpos) = (0x00FFFF & CurrentTextColor);        //GB
//        *(__IO uint8_t*)(CurrentFrameBuffer + (3*Xaddress) + xpos+2) = (0xFF0000 & CurrentTextColor) >> 16; //R
		  LCD429_PutPixel(x, y, Colour.Fword);
      }
			/*ָ��ǰ�е���һ����*/	
      //Xaddress++;	
	  x++;
    }
	y++;
			/*��ʾ��һ��*/
		  /*ָ���ַ���ʾ������һ�еĵ�һ�����ص�*/
     // Xaddress += (LCD_PIXEL_WIDTH - LCD_Currentfonts->Width); 
  }
}

/**
  * @brief  ��ʾһ���ַ�(Ӣ��).
  * @param  Line: Ҫ��ʾ���б��LINE(0) - LINE(N)
  * @param  Column: �ַ����ڵ�Һ���е�ַ
  * @param  Ascii: ASCII�ַ����� 0x20 and 0x7E.
  * @retval None
  */
void LCD_DisplayChar(uint16_t Line, uint16_t Column, uint8_t Ascii)
{
  Ascii -= 32;

  LCD_DrawChar(Line, Column, &LCD_Currentfonts->table[Ascii * LCD_Currentfonts->Height]);
}



/**
  * @brief  ��ʾһ���ַ�(Ӣ��)��������Һ�����ȣ����Զ����С�
  * @param  Line: Ҫ��ʾ���б��LINE(0) - LINE(N)
  * @param  *ptr: Ҫ��ʾ���ַ���ָ��
  * @retval None
  */
void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr)
{  
  uint16_t refcolumn = 0;
  /* �ж���ʾλ�ò��ܳ���Һ���ı߽� */
  while ((refcolumn < LCD_PIXEL_WIDTH) && ((*ptr != 0) & (((refcolumn + LCD_Currentfonts->Width) & 0xFFFF) >= LCD_Currentfonts->Width)))
  {
    /* ʹ��LCD��ʾһ���ַ� */
    LCD_DisplayChar(refcolumn, Line, *ptr);
    /* ���������ַƫ����ʾ��λ�� */
    refcolumn += LCD_Currentfonts->Width;
    /* ָ���ַ����е���һ���ַ� */
    ptr++;
  }
}




/**
 * @brief  ����ʾ������ʾһ�������ַ�
 * @param  usX �����ض�ɨ�跽�����ַ�����ʼX����
 * @param  usY �����ض�ɨ�跽�����ַ�����ʼY����
 * @param  usChar ��Ҫ��ʾ�������ַ��������룩
 * @retval ��
 */ 
void LCD_DispChar_CH ( uint16_t usX, uint16_t usY, uint16_t usChar)
{
	uint8_t ucPage, ucColumn;
	uint8_t ucBuffer [ 24*24/8 ];	
	u32 x,y;
  uint32_t usTemp; 	
	
	
//	uint32_t  xpos =0;
//  uint32_t  Xaddress = 0;
  
	/*xpos��ʾ��ǰ�е��Դ�ƫ��λ��*/
 // xpos = usX*LCD_PIXEL_WIDTH*3;
	
	/*Xaddress��ʾ���ص�*/
  //Xaddress += usY;
	   
  macGetGBKCode ( ucBuffer, usChar );	//ȡ��ģ����
	
	/*ucPage��ʾ��ǰ����*/
	y=usY;
	for ( ucPage = 0; ucPage < macHEIGHT_CH_CHAR; ucPage ++ )
	{
    /* ȡ��3���ֽڵ����ݣ���lcd�ϼ���һ�����ֵ�һ�� */
		x=usX;
		usTemp = ucBuffer [ ucPage * 3 ];
		usTemp = ( usTemp << 8 );
		usTemp |= ucBuffer [ ucPage * 3 + 1 ];
		usTemp = ( usTemp << 8 );
		usTemp |= ucBuffer [ ucPage * 3 + 2];
	
		
		for ( ucColumn = 0; ucColumn < macWIDTH_CH_CHAR; ucColumn ++ ) 
		{			
			if ( usTemp & ( 0x01 << 23 ) )  //��λ��ǰ 				
			{
				//����ɫ
//			  *(__IO uint16_t*)(CurrentFrameBuffer + (3*Xaddress) + xpos) = (0x00FFFF & CurrentTextColor);        //GB
//        *(__IO uint8_t*)(CurrentFrameBuffer + (3*Xaddress) + xpos+2) = (0xFF0000 & CurrentTextColor) >> 16; //R
				LCD429_PutPixel(x,y,CurrentTextColor);

			}				
			else	
			{
				//����ɫ
//				*(__IO uint16_t*)(CurrentFrameBuffer + (3*Xaddress) + xpos) = (0x00FFFF & CurrentBackColor);        //GB
//        *(__IO uint8_t*)(CurrentFrameBuffer + (3*Xaddress) + xpos+2) = (0xFF0000 & CurrentBackColor) >> 16; //R
				LCD429_PutPixel(x,y,CurrentBackColor);

			}	
			/*ָ��ǰ�е���һ����*/	
			//Xaddress++;	
			x++;			
			usTemp <<= 1;
			
		}
		/*��ʾ��һ��*/
		/*ָ���ַ���ʾ������һ�еĵ�һ�����ص�*/
		//Xaddress += (LCD_PIXEL_WIDTH - macWIDTH_CH_CHAR);
		y++;
		
	}
}

void PutChar( uint16_t Xpos, uint16_t Ypos, uint8_t ASCI, uint32_t charColor, uint32_t bkColor )
{
	uint32_t i, j;
    uint8_t buffer[16], tmp_char;
    GetASCIICode(buffer,ASCI);
    for( i=0; i<16; i++ )
    {
        tmp_char = buffer[i];
        for( j=0; j<8; j++ )
        {
            if( (tmp_char >> 7-j) & 0x01 == 0x01 )
            {
               LCD429_PutPixel(Xpos+j, Ypos+i, charColor);
            }
            else
            {
 //               LCD_SetPoint( Xpos + j, Ypos + i, bkColor );
                LCD429_PutPixel(Xpos+j, Ypos+i, bkColor);
            }
        }
    }
}
void lcd_image(uint8_t *pt)
{
    unsigned long i;
//	uint32_t color;
	unsigned long pDst = (unsigned long )CurrentFrameBuffer;
  
    for( i = 0; (272 * 480) > i; i++)
    {
		//color=(uint32_t)*pt<<16|(uint32_t)*(pt+1)<<8|*(pt+2);
        
    	//*pDst = color; 
		*(__IO uint8_t*)(pDst++)=*pt++;
        *(__IO uint8_t*)(pDst++)=*pt++;
        *(__IO uint8_t*)(pDst++)=*pt++;
        //*(__IO uint8_t*)(pDst+2)= (0xFF0000 & color) >> 16; //R

//        *(__IO uint16_t*)(CurrentFrameBuffer) = (0x00FFFF & color);        //GB
//        *(__IO uint8_t*)(pDst+2)= (0xFF0000 & color) >> 16; //R
//        *(__IO uint8_t*)(pDst)+3;
        //*(__IO uint32_t*)(pDst)++;
    }
}
void LCD_ShowFontCN_16(uint16_t x,uint16_t y,uint16_t fno,uint8_t mode)
{                                  
	uint8_t temp,t,t1;
	uint16_t x0=x;
	temp=0;  
	if(!mode) //������ʾ
	{        
		for(t=0;t<16*2;t++)
		{   
			temp=GB_16[fno].Msk[t];                //gb16Dot[fno][t];    //����16*08??   

			for(t1=0;t1<8;t1++)
			{
				if(temp&0x80)
					LCD429_PutPixel(x,y,Colour.Fword);
				else 
					LCD429_PutPixel(x,y,Colour.black);
				   
				temp<<=1; 
				x++;
				if(x-x0==16)
				{
					x=x0;
					y++;                        
					break;
				}                        
			} 
		}
	}
	else
	{
		for(t=0;t<16*2;t++)
		{   
			temp=GB_16[fno].Msk[t];                //gb16Dot[fno][t];    //����16*08����                                 
			for(t1=0;t1<8;t1++)
			{
				if(temp&0x80)
					LCD429_PutPixel(x,y,Colour.Fword);                                     
				else 
					LCD429_PutPixel(x,y,Colour.black);
				     
				temp<<=1; 
				x++;
				if(x-x0==16)
				{
					x=x0;
					y++;                        
					break;
				}                        
			} 
		}                
	}
	
}
//findHzIndex_16(pcStr);
uint16_t WriteOneHzChar_16(uint16_t fno,uint16_t x0,uint16_t y0,uint8_t mode)
{
	LCD_ShowFontCN_16(x0,y0,fno,mode);
	return (16);                                          /* ����16λ�п�               */
}
void WriteString_16(uint16_t x0, uint16_t y0, const uint8_t *pcStr, uint8_t mode)
{
	uint16_t usIndex=0;
	uint16_t usWidth = 0; 
	while(1)
	{
		if(*pcStr == 0) 
		{
			break;                                     /*�ַ�������         */
		}      
		x0 = x0 + (usWidth);                           /* �����ַ������ɽ���         */
		if(*pcStr > 0x80)                              /* �ж�Ϊ����                  */
		{
			if((x0 + 16) > LCD_PIXEL_WIDTH)                      /* ���ʣ��ռ��Ƿ����        */
			{
				x0 = 0;
				y0 = y0 + 16;                          /* �ı���ʾ����                */
				if(y0 > LCD_PIXEL_HEIGHT)                         /* �����곬��                  */
				{
					y0 = 0;
				}
			}
			usIndex = findHzIndex_16(pcStr);
			usWidth = WriteOneHzChar_16(usIndex, x0, y0, mode);
			/* ????                     */
			pcStr += 2;
		}
		else 
		{                                               /* �ж�Ϊ�Ǻ���                */
			if (*pcStr == '\r')                         /* ����                        */
			{ 
				y0 = y0 + 16;                           /* �ı���ʾ����                */
				if(y0 > LCD_PIXEL_HEIGHT)                          /* �����곬��                   */
				{
					y0 = 0;
				}
				pcStr++;
				usWidth = 0;
				continue;
			} 
			else if (*pcStr == '\n')                    /* ���䵽���                  */
			{
				x0 = 0;
				pcStr++;
				usWidth = 0;
				continue;
			} 
			else 
			{
				if((x0 + 8) > LCD_PIXEL_WIDTH)                     /* ���ʣ��ռ��Ƿ����       */
				{
					x0 = 0;
					y0 = y0 + 8;                        /* �ı���ʾ����                  */
					if(y0 > LCD_PIXEL_HEIGHT)                       /* �����곬��                   */
					{ 
						y0 = 0;
					}
				}
				//usWidth = WriteOneASCII(pcStr, x0, y0, mode);
				
				PutChar( x0, y0, *pcStr, Colour.Fword, Colour.black );   
				/* ASCII���21H��ֵ��Ӧ��λ��3��*/
				pcStr += 1;
				usWidth=10;
			}
		}
	}                                                                                                            
}

void LCD_ShowFontCN_40_55(uint16_t x,uint16_t y,uint16_t xsize,uint16_t ysize,uint8_t * image)
{                                  
	uint8_t temp,t1;
	uint16_t x0=x,t;
	temp=0;  	       
	for(t=0;t<xsize*ysize/8;t++)
	{   
		//temp=GB_16[fno].Msk[t];                //gb16Dot[fno][t];    //����16*08??   
		temp=*(image+t);
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
				LCD429_PutPixel(x,y,Colour.Fword); 
			else 
				LCD429_PutPixel(x,y,Colour.black); 
			   
			temp<<=1; 
			x++;
			if(x-x0==xsize)
			{
				x=x0;
				y++;                        
				break;
			}                        
		} 
	}			
}
void Disp_Usbflag(uint8_t flag)
{
    u32 colour;
    colour=Colour.Fword;
	Colour.black=LCD_COLOR_TEST_BUTON;
	if(flag==1)
		Colour.Fword=LCD_COLOR_GREEN;
		
	else if(flag==2)
		Colour.Fword=LCD_COLOR_RED;
	else
		Colour.Fword=LCD_COLOR_RED;
//	LCD_ShowFontCN_40_55(uint16_t x,uint16_t y,uint16_t xsize,uint16_t ysize,uint8_t * image);
	LCD_ShowFontCN_40_55(BUTTOM_X_VALUE+4*BUTTOM_MID_VALUE,BUTTOM_Y_VALUE,48,24,(uint8_t*)nBitmapDot);
    Colour.Fword=colour;
	//WriteString_16(BUTTOM_X_VALUE+4*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, " ��ʾ",  0);
	

}
void WriteString_Big(uint16_t x0, uint16_t y0, const uint8_t *pcStr,uint8_t fonts)
{
//	uint16_t usIndex=0;
	uint16_t i,usWidth = 0; 
    uint16_t wide,high;
     uint8_t *pt;
	while(1)
	{
		if(*pcStr == 0) 
		{
			break;                                     /*�ַ�������         */
		}      		

		if(*pcStr>='0'&&*pcStr<='9')
		{
			i=*pcStr-'0';
		
		}
		else if(*pcStr=='.')
			i=10;
		else if(*pcStr=='-')
			i=11;
		else
			i=12;
		if(fonts)
        {
            wide=16;
            high=24;
            pt=(uint8_t*)Out_1224;
            
        
        }else
        {
            wide=32;
            high=55;
            pt=(uint8_t*)_num;
        
        
        }
		LCD_ShowFontCN_40_55(x0+usWidth,y0,wide,high,pt+i*wide/8*high);		
		/* ASCII���21H��ֵ��Ӧ��λ��3��*/
		pcStr += 1;
		usWidth+=wide;
		
		
	}                                                                                                            
}
void Disp_R_V(void)
{
    //LCD_DisplayChar(0,TESTVALUE_Y+32, 'R');//( TESTVALUE_X-32, TESTVALUE_Y, "R",0 );
	//LCD_ShowFontCN_40_55(0,92,40,55,(uint8_t*)Out_Assic+5*40*55/8);//R
//	
	
	LCD_ShowFontCN_40_55(60+40*7,92,40,55,(uint8_t*)Out_Assic+20*40*55/8);//��
	//LCD_ShowFontCN_40_55(0,92+55,40,55,(uint8_t*)Out_Assic+27*40*55/8);//V
//	
	LCD_ShowFontCN_40_55(60+40*7,92+55,40,55,(uint8_t*)Out_Assic+27*40*55/8);//V
	
//	if(Test_Unit.Res_dot)
//		LCD_ShowFontCN_40_55(60+40*6,92,40,55, (uint8_t*)Out_Assic+22*40*55/8);//�ո�
//	else
//		LCD_ShowFontCN_40_55(60+40*6,92,40,55, (uint8_t*)Out_Assic+17*40*55/8);//m

}
void Disp_R_Uint(void)
{
    switch(Test_Value.uint)
    {
        case 0:
            LCD_ShowFontCN_40_55(60+40*6,92,40,55, (uint8_t*)Out_Assic+17*40*55/8);//m
            break;
        case 1:
            LCD_ShowFontCN_40_55(60+40*6,92,40,55, (uint8_t*)Out_Assic+22*40*55/8);//m
            break;
        case 2:
            LCD_ShowFontCN_40_55(60+40*6,92,40,55, (uint8_t*)Out_Assic+14*40*55/8);//m
            break;
        default:
            break;
    
    
    }


}

/**
 * @brief  ����ʾ������ʾ��Ӣ���ַ���,����Һ������ʱ���Զ����С�
					 ��Ӣ�Ļ���ʾʱ�����Ӣ����������ΪFont16x24��ʽ
 * @param  Line ����(Ҳ������Ϊy����)
 * @param  Column ���У�Ҳ������Ϊx���꣩
 * @param  pStr ��Ҫ��ʾ���ַ������׵�ַ
 * @retval ��
 */
void LCD_DispString_EN_CH( uint16_t Line, uint16_t Column, const uint8_t * pStr )
{
	uint16_t usCh;
	
	
	while( * pStr != '\0' )
	{
		if ( * pStr <= 126 )	           	//Ӣ���ַ�
		{
	
			/*�Զ�����*/
			if ( ( Column + LCD_Currentfonts->Width ) > LCD_PIXEL_WIDTH )
			{
				Column = 0;
				Line += LCD_Currentfonts->Height;
			}
			
			if ( ( Line + LCD_Currentfonts->Height ) > LCD_PIXEL_HEIGHT )
			{
				Column = 0;
				Line = 0;
			}			
					
			LCD_DisplayChar(Column,Line,*pStr);
			
			Column += LCD_Currentfonts->Width;
		
		  pStr ++;

		}
		
		else	                            //�����ַ�
		{
			if ( ( Column + macWIDTH_CH_CHAR ) > LCD_PIXEL_HEIGHT  )
			{
				Column = 0;
				Line += macHEIGHT_CH_CHAR;
			}
			
			if ( ( Line + macHEIGHT_CH_CHAR ) > LCD_PIXEL_WIDTH)
			{
				Column = 0;
				Line = 0;
			}	
			
			/*һ���������ֽ�*/
			usCh = * ( uint16_t * ) pStr;			
			usCh = ( usCh << 8 ) + ( usCh >> 8 );		

			LCD_DispChar_CH (Column,Line, usCh);
			
			Column += macWIDTH_CH_CHAR;
			
			pStr += 2;           //һ�����������ֽ� 
		
    }
		
  }
	
	
} 

/**
  * @brief  ��ʾһ���ַ���������Һ�����ȣ����Զ����С�
						��Ӣ����ʱ�����Ӣ����������ΪFont16x24��ʽ
  * @param  Line: Ҫ��ʾ���б��LINE(0) - LINE(N)
  * @param  *ptr: Ҫ��ʾ���ַ���ָ��
  * @retval None
  */
void LCD_DisplayStringLine_EN_CH(uint16_t Line, uint8_t *ptr)
{  
  uint16_t refcolumn = 0;
  /* �ж���ʾλ�ò��ܳ���Һ���ı߽� */
  while ((refcolumn < LCD_PIXEL_WIDTH) && ((*ptr != 0) & (((refcolumn + LCD_Currentfonts->Width) & 0xFFFF) >= LCD_Currentfonts->Width)))
  {
    /* ʹ��LCD��ʾһ���ַ� */
		if ( * ptr <= 126 )	           	//Ӣ���ַ�
		{
					
			LCD_DisplayChar(refcolumn, Line, *ptr);
    /* ��������ƫ����ʾ��λ�� */
			refcolumn += LCD_Currentfonts->Width;
    /* ָ���ַ����е���һ���ַ� */
			ptr++;
		}
		
		else	                            //�����ַ�
		{	
			uint16_t usCh;
			
			/*һ���������ֽ�*/
			usCh = * ( uint16_t * ) ptr;	
			/*��������˳��*/
			usCh = ( usCh << 8 ) + ( usCh >> 8 );		
			
			/*��ʾ����*/
			LCD_DispChar_CH ( refcolumn,Line , usCh );
			/*��ʾλ��ƫ��*/
			refcolumn += macWIDTH_CH_CHAR;
      /* ָ���ַ����е���һ���ַ� */
			ptr += 2; 		
    }		

		

  }
}

/**
 * @brief  ������ģ�����ź����ģ��1�����ص���8������λ����ʾ
										0x01��ʾ�ʼ���0x00��ʾ�հ���
 * @param  in_width ��ԭʼ�ַ�����
 * @param  in_heig ��ԭʼ�ַ��߶�
 * @param  out_width �����ź���ַ�����
 * @param  out_heig�����ź���ַ��߶�
 * @param  in_ptr ���ֿ�����ָ��	ע�⣺1pixel 1bit
 * @param  out_ptr �����ź���ַ����ָ�� ע��: 1pixel 8bit
 *		out_ptrʵ����û������������ĳ���ֱ�������ȫ��ָ��zoomBuff��
 * @param  en_cn ��0ΪӢ�ģ�1Ϊ����
 * @retval ��
 */
void LCD_zoomChar(uint16_t in_width,	//ԭʼ�ַ�����
									uint16_t in_heig,		//ԭʼ�ַ��߶�
									uint16_t out_width,	//���ź���ַ�����
									uint16_t out_heig,	//���ź���ַ��߶�
									uint8_t *in_ptr,	//�ֿ�����ָ��	ע�⣺1pixel 1bit
									uint8_t *out_ptr, //���ź���ַ����ָ�� ע��: 1pixel 8bit
									uint8_t en_cn)		//0ΪӢ�ģ�1Ϊ����	
{
	uint8_t *pts,*ots;
	//����Դ��ģ��Ŀ����ģ��С���趨����������ӣ�����16��Ϊ�˰Ѹ�������ת�ɶ�������
	unsigned int xrIntFloat_16=(in_width<<16)/out_width+1; 
  unsigned int yrIntFloat_16=(in_heig<<16)/out_heig+1;
	
	unsigned int srcy_16=0;
	unsigned int y,x;
	uint8_t *pSrcLine;
	uint8_t tempBuff[1024] = {0};
	u32			uChar;
	u16			charBit = in_width / 8;
	u16			Bitdiff = 32 - in_width;
	
	//�������Ƿ�Ϸ�
	if(in_width >= 32) return;												//�ֿⲻ��������32����
	if(in_width * in_heig == 0) return;	
	if(in_width * in_heig >= 1024 ) return; 					//����������� 32*32
	
	if(out_width * out_heig == 0) return;	
	if(out_width * out_heig >= ZOOMMAXBUFF ) return; //����������� 128*128
	pts = (uint8_t*)&tempBuff;
	
	//Ϊ�������㣬�ֿ��������1 pixel 1bit ӳ�䵽1pixel 8bit
	//0x01��ʾ�ʼ���0x00��ʾ�հ���
	if(en_cn == 0x00)//Ӣ��
	{
		//������16 * 24�ֿ���Ϊ���ԣ�������С���ֿ����и������д�������ӳ��Ϳ���
		//Ӣ�ĺ������ֿ����±߽粻�ԣ����ڴ˴ε�������Ҫע��tempBuff��ֹ���
			pts+=in_width*4;
			for(y=0;y<in_heig;y++)	
			{
				uChar = *(u32 *)(in_ptr + y * charBit) >> Bitdiff;
				for(x=0;x<in_width;x++)
					{
						*pts++ = (uChar >> x) & 0x01;
					}
			}		
	}
	else //����
	{
			for(y=0;y<in_heig;y++)	
			{
				/*Դ��ģ����*/
				uChar = in_ptr [ y * 3 ];
				uChar = ( uChar << 8 );
				uChar |= in_ptr [ y * 3 + 1 ];
				uChar = ( uChar << 8 );
				uChar |= in_ptr [ y * 3 + 2];
				/*ӳ��*/
				for(x=0;x<in_width;x++)
					{
						if(((uChar << x) & 0x800000) == 0x800000)
							*pts++ = 0x01;
						else
							*pts++ = 0x00;
					}
			}		
	}

	//zoom����
	pts = (uint8_t*)&tempBuff;	//ӳ����Դ����ָ��
	ots = (uint8_t*)&zoomBuff;	//������ݵ�ָ��
	for (y=0;y<out_heig;y++)	/*�б���*/
    {
				unsigned int srcx_16=0;
        pSrcLine=pts+in_width*(srcy_16>>16);				
        for (x=0;x<out_width;x++) /*�������ر���*/
        {
            ots[x]=pSrcLine[srcx_16>>16]; //��Դ��ģ���ݸ��Ƶ�Ŀ��ָ����
            srcx_16+=xrIntFloat_16;			//������ƫ��Դ���ص�
        }
        srcy_16+=yrIntFloat_16;				  //������ƫ��Դ���ص�
        ots+=out_width;						
    }
	/*���������ź����ģ����ֱ�Ӵ洢��ȫ��ָ��zoomBuff����*/
	out_ptr = (uint8_t*)&zoomBuff;	//out_ptrû����ȷ�������������ֱ�Ӹĳ���ȫ�ֱ���ָ�룡
	
	/*ʵ�������ʹ��out_ptr����Ҫ������һ�䣡����
		ֻ����Ϊout_ptrû��ʹ�ã��ᵼ��warning��ǿ��֢*/
	out_ptr++; 
}			

/**
 * @brief  �������ź����ģ��ʾ�ַ�
 * @param  Xpos ���ַ���ʾλ��x
 * @param  Ypos ���ַ���ʾλ��y
 * @param  Font_width ���ַ�����
 * @param  Font_Heig���ַ��߶�
 * @param  c ��Ҫ��ʾ����ģ����
 * @param  DrawModel ���Ƿ�ɫ��ʾ 
 * @retval ��
 */
void LCD_DrawChar_Ex(uint16_t Xpos, //�ַ���ʾλ��x
												uint16_t Ypos, //�ַ���ʾλ��y
												uint16_t Font_width, //�ַ�����
												uint16_t Font_Heig,  //�ַ��߶� 
												uint8_t *c,						//��ģ����
												uint16_t DrawModel)		//�Ƿ�ɫ��ʾ
{
  uint32_t index = 0, counter = 0;//, xpos =0;
//  uint32_t  Xaddress = 0;
	u32 x,y;
  
  //xpos = Xpos*LCD_PIXEL_WIDTH*3;
  //Xaddress += Ypos;
  y=Ypos;
  for(index = 0; index < Font_Heig; index++)
  {
    x=Xpos;
    for(counter = 0; counter < Font_width; counter++)
    {
      if(*c++ == DrawModel)	//������ģ����ɫ���þ�����ʾ������ɫ
      {
//        *(__IO uint16_t*)(CurrentFrameBuffer + (3*Xaddress) + xpos) = (0x00FFFF & CurrentBackColor);        //GB
//        *(__IO uint8_t*)(CurrentFrameBuffer + (3*Xaddress) + xpos+2) = (0xFF0000 & CurrentBackColor) >> 16; //R
		  LCD429_PutPixel(x, y, CurrentBackColor);
      }
      else
      {
//        *(__IO uint16_t*)(CurrentFrameBuffer + (3*Xaddress) + xpos) = (0x00FFFF & CurrentTextColor);        //GB
//        *(__IO uint8_t*)(CurrentFrameBuffer + (3*Xaddress) + xpos+2) = (0xFF0000 & CurrentTextColor) >> 16; //R
		  LCD429_PutPixel(x, y, CurrentTextColor);
      }
      //Xaddress++;
	  x++;
    }
	y++;
      //Xaddress += (LCD_PIXEL_WIDTH - Font_width);
  }
}

/**
 * @brief  �������ź����ģ��ʾ�ַ���
 * @param  Xpos ���ַ���ʾλ��x
 * @param  Ypos ���ַ���ʾλ��y
 * @param  Font_width ���ַ����ȣ�Ӣ���ַ��ڴ˻�����/2��ע��Ϊż��
 * @param  Font_Heig���ַ��߶ȣ�ע��Ϊż��
 * @param  c ��Ҫ��ʾ���ַ���
 * @param  DrawModel ���Ƿ�ɫ��ʾ 
 * @retval ��
 */
void LCD_DisplayStringLineEx(uint16_t x, 		//�ַ���ʾλ��x
														 uint16_t y, 				//�ַ���ʾλ��y
														 uint16_t Font_width,	//Ҫ��ʾ��������ȣ�Ӣ���ַ��ڴ˻�����/2��ע��Ϊż��
														 uint16_t Font_Heig,	//Ҫ��ʾ������߶ȣ�ע��Ϊż��
														 uint8_t *ptr,					//��ʾ���ַ�����
														 uint16_t DrawModel)  //�Ƿ�ɫ��ʾ
{
	uint16_t refcolumn = y; //x����
	uint16_t Charwidth;
	uint8_t *psr;
	uint8_t Ascii;	//Ӣ��
	uint16_t usCh;  //����
	uint8_t ucBuffer [ 24*24/8 ];	
	
	while ((refcolumn < LCD_PIXEL_WIDTH) && ((*ptr != 0) & (((refcolumn + LCD_Currentfonts->Width) & 0xFFFF) >= LCD_Currentfonts->Width)))
	{
		if(*ptr > 0x80) //���������
		{
			Charwidth = Font_width;
			usCh = * ( uint16_t * ) ptr;				
			usCh = ( usCh << 8 ) + ( usCh >> 8 );
			macGetGBKCode ( ucBuffer, usCh );	//ȡ��ģ����
			//������ģ����
			LCD_zoomChar(24,24,Charwidth,Font_Heig,(uint8_t *)&ucBuffer,psr,1); 
			//��ʾ�����ַ�
			LCD_DrawChar_Ex(refcolumn,x,Charwidth,Font_Heig,(uint8_t*)&zoomBuff,DrawModel);
			refcolumn+=Charwidth;
			ptr+=2;
		}
		else
		{
				Charwidth = Font_width / 2;
				Ascii = *ptr - 32;
				//������ģ����
				LCD_zoomChar(16,24,Charwidth,Font_Heig,(uint8_t *)&LCD_Currentfonts->table[Ascii * LCD_Currentfonts->Height],psr,0);
			  //��ʾ�����ַ�
				LCD_DrawChar_Ex(refcolumn,x,Charwidth,Font_Heig,(uint8_t*)&zoomBuff,DrawModel);
				refcolumn+=Charwidth;
				ptr++;
		}
	}
}

/**
  * @brief  Sets a display window
  * @param  Xpos: specifies the X bottom left position from 0 to 240.
  * @param  Ypos: specifies the Y bottom left position from 0 to 320.
  * @param  Height: display window height, can be a value from 0 to 320.
  * @param  Width: display window width, can be a value from 0 to 240.
  * @retval None
  */
void LCD_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width)
{

  if (CurrentLayer == LCD_BACKGROUND_LAYER)
  { 
    /* reconfigure the layer1 position */
    LTDC_LayerPosition(LTDC_Layer1, Xpos, Ypos);
    LTDC_ReloadConfig(LTDC_IMReload);
    
    /* reconfigure the layer1 size */
    LTDC_LayerSize(LTDC_Layer1, Width, Height);
    LTDC_ReloadConfig(LTDC_IMReload);
 }
 else
 {   
    /* reconfigure the layer2 position */
    LTDC_LayerPosition(LTDC_Layer2, Xpos, Ypos);
    LTDC_ReloadConfig(LTDC_IMReload); 
   
   /* reconfigure the layer2 size */
    LTDC_LayerSize(LTDC_Layer2, Width, Height);
    LTDC_ReloadConfig(LTDC_IMReload);
  }
}

/**
  * @brief  Disables LCD Window mode.
  * @param  None
  * @retval None
  */
void LCD_WindowModeDisable(void)
{
  LCD_SetDisplayWindow(0, 0, LCD_PIXEL_HEIGHT, LCD_PIXEL_WIDTH); 
}

/**
  * @brief  Displays a line.
  * @param Xpos: specifies the X position, can be a value from 0 to 240.
  * @param Ypos: specifies the Y position, can be a value from 0 to 320.
  * @param Length: line length.-
  * @param Direction: line direction.
  *   This parameter can be one of the following values: LCD_DIR_HORIZONTAL or LCD_DIR_VERTICAL.
  * @retval None
  */
void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction)
{
  DMA2D_InitTypeDef      DMA2D_InitStruct;

  uint32_t  Xaddress = 0;
  uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0;
    
    if (g_LcdDirection == 0)		/* ���� */
	{
		//index = (uint32_t)3*_usY * LCD_PIXEL_WIDTH + _usX;
//		index = (uint32_t)_usY*3 +( LCD_PIXEL_WIDTH *_usX)*3;
		Xaddress = (uint32_t)Ypos*LCD_PIXEL_WIDTH*3 +( Xpos)*3;
	}
	else if (g_LcdDirection == 1)	/* ����180��*/
	{
//        Xpos=479-Xpos;
//        Ypos=271-Ypos;
        Xaddress = CurrentFrameBuffer + LCD_PIXEL_WIDTH*LCD_PIXEL_HEIGHT*3-3*(LCD_PIXEL_WIDTH*Ypos + Xpos);//-Xpos*3;
		//Xaddress = (uint32_t)((LCD_PIXEL_HEIGHT - Xpos - 1) * LCD_PIXEL_WIDTH + (LCD_PIXEL_WIDTH - Ypos - 1))*3;
	}
	else if (g_LcdDirection == 2)	/* ���� */
	{
		Xaddress = (uint32_t)((LCD_PIXEL_HEIGHT- Xpos - 1) * LCD_PIXEL_WIDTH  + Ypos)*3;
	}
	else if (g_LcdDirection == 3)	/* ����180�� */
	{
		Xaddress = CurrentFrameBuffer + 3*(LCD_PIXEL_WIDTH*Ypos + Xpos);
	}
  
  //Xaddress = CurrentFrameBuffer + 3*(LCD_PIXEL_WIDTH*Ypos - Xpos);
 
  Red_Value = (0xFF0000 & Colour.Fword) >>16;
  Blue_Value = 0x0000FF & Colour.Fword;
  Green_Value = (0x00FF00 & Colour.Fword)>>8 ;

  /* Configure DMA2D */    
  DMA2D_DeInit();  
  DMA2D_InitStruct.DMA2D_Mode = DMA2D_R2M;       
  DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB888;      
  DMA2D_InitStruct.DMA2D_OutputGreen = Green_Value;      
  DMA2D_InitStruct.DMA2D_OutputBlue = Blue_Value;     
  DMA2D_InitStruct.DMA2D_OutputRed = Red_Value;                
  DMA2D_InitStruct.DMA2D_OutputAlpha = 0x0F;                  
  DMA2D_InitStruct.DMA2D_OutputMemoryAdd = Xaddress;                  
  
  if(Direction == LCD_DIR_HORIZONTAL)
  {                                                      
    DMA2D_InitStruct.DMA2D_OutputOffset = 0;                
    DMA2D_InitStruct.DMA2D_NumberOfLine = 1;            
    DMA2D_InitStruct.DMA2D_PixelPerLine = Length; 
  }
  else
  {                                                            
    DMA2D_InitStruct.DMA2D_OutputOffset = LCD_PIXEL_WIDTH - 1;                
    DMA2D_InitStruct.DMA2D_NumberOfLine = Length;            
    DMA2D_InitStruct.DMA2D_PixelPerLine = 1;  
  }
  
  DMA2D_Init(&DMA2D_InitStruct);  
  /* Start Transfer */ 
  DMA2D_StartTransfer();  
  /* Wait for CTC Flag activation */
  while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET)
  {
  }
  
}

/**
  * @brief  Displays a rectangle.
  * @param  Xpos: specifies the X position, can be a value from 0 to 240.
  * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
  * @param  Height: display rectangle height, can be a value from 0 to 320.
  * @param  Width: display rectangle width, can be a value from 0 to 240.
  * @retval None
  */
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  /* draw horizontal lines */
	LCD_DrawFullRect(Xpos,Ypos,1,Height);
	LCD_DrawFullRect(Xpos,Ypos,1,Width);
	LCD_DrawFullRect(Xpos,Height,1,Width);
	LCD_DrawFullRect(Width,Ypos,Height,1);
//  LCD_DrawLine(Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);
//  LCD_DrawLine(Xpos, (Ypos+ Height), Width, LCD_DIR_HORIZONTAL);
//  
//  /* draw vertical lines */
//  LCD_DrawLine(Xpos, Ypos, Height, LCD_DIR_VERTICAL);
//  LCD_DrawLine((Xpos + Width), Ypos, Height, LCD_DIR_VERTICAL);
}

/**
  * @brief  Draw a circle.
  * @param  Xpos: specifies the X position, can be a value from 0 to 240.
  * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
  * @param  Radius: radius of the circle.
  * @retval None
  */
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  
    int x = -Radius, y = 0, err = 2-2*Radius, e2;
    do {
      

        *(__IO uint16_t*) (CurrentFrameBuffer + (3*((Xpos-x) + LCD_PIXEL_WIDTH*(Ypos+y))))= (0x00FFFF & CurrentTextColor);       //GB
        *(__IO uint8_t*)(CurrentFrameBuffer + (3*((Xpos-x) + LCD_PIXEL_WIDTH*(Ypos+y))+2))= (0xFF0000 & CurrentTextColor) >> 16; //R
      
        *(__IO uint16_t*) (CurrentFrameBuffer + (3*((Xpos+x) + LCD_PIXEL_WIDTH*(Ypos+y)))) = (0x00FFFF & CurrentTextColor);      //GB
        *(__IO uint8_t*)(CurrentFrameBuffer + (3*((Xpos+x) + LCD_PIXEL_WIDTH*(Ypos+y))+2))= (0xFF0000 & CurrentTextColor) >> 16; //R
      
        *(__IO uint16_t*) (CurrentFrameBuffer + (3*((Xpos+x) + LCD_PIXEL_WIDTH*(Ypos-y)))) = (0x00FFFF & CurrentTextColor);      //GB
        *(__IO uint8_t*)(CurrentFrameBuffer + (3*((Xpos+x) + LCD_PIXEL_WIDTH*(Ypos-y))+2))= (0xFF0000 & CurrentTextColor) >> 16; //R
      
        *(__IO uint16_t*) (CurrentFrameBuffer + (3*((Xpos-x) + LCD_PIXEL_WIDTH*(Ypos-y)))) = (0x00FFFF & CurrentTextColor);      //GB
        *(__IO uint8_t*)(CurrentFrameBuffer + (3*((Xpos-x) + LCD_PIXEL_WIDTH*(Ypos-y))+2))= (0xFF0000 & CurrentTextColor) >> 16; //R


        e2 = err;
        if (e2 <= y) {
            err += ++y*2+1;
            if (-x == y && e2 <= x) e2 = 0;
        }
        if (e2 > x) err += ++x*2+1;
    }
    while (x <= 0);
    
}

/**
  * @brief  Draw a full ellipse.
  * @param  Xpos: specifies the X position, can be a value from 0 to 240.
  * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
  * @param  Radius: minor radius of ellipse.
  * @param  Radius2: major radius of ellipse.  
  * @retval None
  */
void LCD_DrawFullEllipse(int Xpos, int Ypos, int Radius, int Radius2)
{
  int x = -Radius, y = 0, err = 2-2*Radius, e2;
  float K = 0, rad1 = 0, rad2 = 0;
  
  rad1 = Radius;
  rad2 = Radius2;
  
  if (Radius > Radius2)
  { 
    do 
    {
      K = (float)(rad1/rad2);
      LCD_DrawLine((Xpos+x), (Ypos-(uint16_t)(y/K)), (2*(uint16_t)(y/K) + 1), LCD_DIR_VERTICAL);
      LCD_DrawLine((Xpos-x), (Ypos-(uint16_t)(y/K)), (2*(uint16_t)(y/K) + 1), LCD_DIR_VERTICAL);
      
      e2 = err;
      if (e2 <= y) 
      {
        err += ++y*2+1;
        if (-x == y && e2 <= x) e2 = 0;
      }
      if (e2 > x) err += ++x*2+1;
      
    }
    while (x <= 0);
  }
  else
  {
    y = -Radius2; 
    x = 0;
    do 
    { 
      K = (float)(rad2/rad1);       
      LCD_DrawLine((Xpos-(uint16_t)(x/K)), (Ypos+y), (2*(uint16_t)(x/K) + 1), LCD_DIR_HORIZONTAL);
      LCD_DrawLine((Xpos-(uint16_t)(x/K)), (Ypos-y), (2*(uint16_t)(x/K) + 1), LCD_DIR_HORIZONTAL);
      
      e2 = err;
      if (e2 <= x) 
      {
        err += ++x*2+1;
        if (-y == x && e2 <= y) e2 = 0;
      }
      if (e2 > y) err += ++y*2+1;
    }
    while (y <= 0);
  }
}

/**
  * @brief  Displays an Ellipse.
  * @param  Xpos: specifies the X position, can be a value from 0 to 240.
  * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
  * @param  Radius: specifies Radius.
  * @param  Radius2: specifies Radius2.
  * @retval None
  */
void LCD_DrawEllipse(int Xpos, int Ypos, int Radius, int Radius2)
{
  int x = -Radius, y = 0, err = 2-2*Radius, e2;
  float K = 0, rad1 = 0, rad2 = 0;
   
  rad1 = Radius;
  rad2 = Radius2;
  
  if (Radius > Radius2)
  { 
    do {
      K = (float)(rad1/rad2);
   
      *(__IO uint16_t*)(CurrentFrameBuffer + (3*((Xpos-x) + LCD_PIXEL_WIDTH*(Ypos+(uint16_t)(y/K))))) = (0x00FFFF & CurrentTextColor);        //GB
      *(__IO uint8_t*)(CurrentFrameBuffer + (3*((Xpos-x) + LCD_PIXEL_WIDTH*(Ypos+(uint16_t)(y/K)))+2)) = (0xFF0000 & CurrentTextColor) >> 16; //R
     
      *(__IO uint16_t*)(CurrentFrameBuffer + (3*((Xpos+x) + LCD_PIXEL_WIDTH*(Ypos+(uint16_t)(y/K))))) = (0x00FFFF & CurrentTextColor);        //GB
      *(__IO uint8_t*)(CurrentFrameBuffer + (3*((Xpos+x) + LCD_PIXEL_WIDTH*(Ypos+(uint16_t)(y/K)))+2)) = (0xFF0000 & CurrentTextColor) >> 16; //R

      *(__IO uint16_t*)(CurrentFrameBuffer + (3*((Xpos+x) + LCD_PIXEL_WIDTH*(Ypos-(uint16_t)(y/K))))) = (0x00FFFF & CurrentTextColor);        //GB
      *(__IO uint8_t*)(CurrentFrameBuffer + (3*((Xpos+x) + LCD_PIXEL_WIDTH*(Ypos-(uint16_t)(y/K)))+2)) = (0xFF0000 & CurrentTextColor) >> 16; //R

      *(__IO uint16_t*)(CurrentFrameBuffer + (3*((Xpos-x) + LCD_PIXEL_WIDTH*(Ypos-(uint16_t)(y/K))))) = (0x00FFFF & CurrentTextColor);        //GB
      *(__IO uint8_t*)(CurrentFrameBuffer + (3*((Xpos-x) + LCD_PIXEL_WIDTH*(Ypos-(uint16_t)(y/K)))+2)) = (0xFF0000 & CurrentTextColor) >> 16; //R

      
      e2 = err;
      if (e2 <= y) {
        err += ++y*2+1;
        if (-x == y && e2 <= x) e2 = 0;
      }
      if (e2 > x) err += ++x*2+1;
    }
    while (x <= 0);
  }
  else
  {
    y = -Radius2; 
    x = 0;
    do { 
      K = (float)(rad2/rad1);

      *(__IO uint16_t*)(CurrentFrameBuffer + (3*((Xpos-(uint16_t)(x/K)) + LCD_PIXEL_WIDTH*(Ypos+y))))= (0x00FFFF & CurrentTextColor);        //GB
      *(__IO uint8_t*)(CurrentFrameBuffer + (3*((Xpos-(uint16_t)(x/K)) + LCD_PIXEL_WIDTH*(Ypos+y))+2)) = (0xFF0000 & CurrentTextColor) >> 16; //R

      *(__IO uint16_t*)(CurrentFrameBuffer + (3*((Xpos+(uint16_t)(x/K)) + LCD_PIXEL_WIDTH*(Ypos+y))))= (0x00FFFF & CurrentTextColor);        //GB
      *(__IO uint8_t*)(CurrentFrameBuffer + (3*((Xpos+(uint16_t)(x/K)) + LCD_PIXEL_WIDTH*(Ypos+y))+2)) = (0xFF0000 & CurrentTextColor) >> 16; //R

      *(__IO uint16_t*)(CurrentFrameBuffer + (3*((Xpos+(uint16_t)(x/K)) + LCD_PIXEL_WIDTH*(Ypos-y))))= (0x00FFFF & CurrentTextColor);        //GB
      *(__IO uint8_t*)(CurrentFrameBuffer + (3*((Xpos+(uint16_t)(x/K)) + LCD_PIXEL_WIDTH*(Ypos-y))+2)) = (0xFF0000 & CurrentTextColor) >> 16; //R

      *(__IO uint16_t*)(CurrentFrameBuffer + (3*((Xpos-(uint16_t)(x/K)) + LCD_PIXEL_WIDTH*(Ypos-y))))= (0x00FFFF & CurrentTextColor);        //GB
      *(__IO uint8_t*)(CurrentFrameBuffer + (3*((Xpos-(uint16_t)(x/K)) + LCD_PIXEL_WIDTH*(Ypos-y))+2)) = (0xFF0000 & CurrentTextColor) >> 16; //R

      
      e2 = err;
      if (e2 <= x) {
        err += ++x*3+1;
        if (-y == x && e2 <= y) e2 = 0;
      }
      if (e2 > y) err += ++y*3+1;     
    }
    while (y <= 0);
  }
}

/**
  * @brief  Displays a mono-color picture.
  * @param  Pict: pointer to the picture array.
  * @retval None
  */
void LCD_DrawMonoPict(const uint32_t *Pict)
{
  uint32_t index = 0, counter = 0;
  
   
  for(index = 0; index < 2400; index++)
  {
    for(counter = 0; counter < 32; counter++)
    {
      if((Pict[index] & (1 << counter)) == 0x00)
      {
       
        *(__IO uint16_t*)(CurrentFrameBuffer) = (0x00FFFF & CurrentBackColor);        //GB
        *(__IO uint8_t*)(CurrentFrameBuffer+2)= (0xFF0000 & CurrentBackColor) >> 16; //R

      }
      else
      {
        
        *(__IO uint16_t*)(CurrentFrameBuffer) = (0x00FFFF & CurrentTextColor);        //GB
        *(__IO uint8_t*)(CurrentFrameBuffer+2)= (0xFF0000 & CurrentTextColor) >> 16; //R
      }
    }
  }
}

/**
  * @brief  Displays a bitmap picture loaded in the internal Flash.
  * @param  BmpAddress: Bmp picture address in the internal Flash.
  * @retval None
  */
void LCD_WriteBMP(uint32_t BmpAddress)
{
  uint32_t index = 0, size = 0, width = 0, height = 0, bit_pixel = 0;
  uint32_t Address;
  uint32_t currentline = 0, linenumber = 0;
 
  Address = CurrentFrameBuffer;

  /* Read bitmap size */
  size = *(__IO uint16_t *) (BmpAddress + 2);
  size |= (*(__IO uint16_t *) (BmpAddress + 4)) << 16;
  
  /* Get bitmap data address offset */
  index = *(__IO uint16_t *) (BmpAddress + 10);
  index |= (*(__IO uint16_t *) (BmpAddress + 12)) << 16;

  /* Read bitmap width */
  width = *(uint16_t *) (BmpAddress + 18);
  width |= (*(uint16_t *) (BmpAddress + 20)) << 16;

  /* Read bitmap height */
  height = *(uint16_t *) (BmpAddress + 22);
  height |= (*(uint16_t *) (BmpAddress + 24)) << 16;

  /* Read bit/pixel */
  bit_pixel = *(uint16_t *) (BmpAddress + 28);  
 
  if (CurrentLayer == LCD_BACKGROUND_LAYER)
  {
    /* reconfigure layer size in accordance with the picture */
    LTDC_LayerSize(LTDC_Layer1, width, height);
    LTDC_ReloadConfig(LTDC_VBReload);

    /* Reconfigure the Layer pixel format in accordance with the picture */    
    if ((bit_pixel/8) == 4)
    {
      LTDC_LayerPixelFormat(LTDC_Layer1, LTDC_Pixelformat_ARGB8888);
      LTDC_ReloadConfig(LTDC_VBReload);
    }
    else if ((bit_pixel/8) == 2)
    {
      LTDC_LayerPixelFormat(LTDC_Layer1, LTDC_Pixelformat_RGB565);
      LTDC_ReloadConfig(LTDC_VBReload);
    }
    else 
    {
      LTDC_LayerPixelFormat(LTDC_Layer1, LTDC_Pixelformat_RGB888);
      LTDC_ReloadConfig(LTDC_VBReload);
    } 
  }
  else
  {
    /* reconfigure layer size in accordance with the picture */
    LTDC_LayerSize(LTDC_Layer2, width, height);
    LTDC_ReloadConfig(LTDC_VBReload); 
    
    /* Reconfigure the Layer pixel format in accordance with the picture */
    if ((bit_pixel/8) == 4)
    {
      LTDC_LayerPixelFormat(LTDC_Layer2, LTDC_Pixelformat_ARGB8888);
      LTDC_ReloadConfig(LTDC_VBReload);
    }
    else if ((bit_pixel/8) == 2)
    {
      LTDC_LayerPixelFormat(LTDC_Layer2, LTDC_Pixelformat_RGB565);
      LTDC_ReloadConfig(LTDC_VBReload);
    }
    else
    {
      LTDC_LayerPixelFormat(LTDC_Layer2, LTDC_Pixelformat_RGB888);
      LTDC_ReloadConfig(LTDC_VBReload);  
    }
  }
 
  /* compute the real size of the picture (without the header)) */  
  size = (size - index); 

  /* bypass the bitmap header */
  BmpAddress += index;

  /* start copie image from the bottom */
  Address += width*(height-1)*(bit_pixel/8);
  
  for(index = 0; index < size; index++)
  {
    *(__IO uint8_t*) (Address) = *(__IO uint8_t *)BmpAddress;
    
    /*jump on next byte */   
    BmpAddress++;
    Address++;
    currentline++;
    
    if((currentline/(bit_pixel/8)) == width)
    {
      if(linenumber < height)
      {
        linenumber++;
        Address -=(2*width*(bit_pixel/8));
        currentline = 0;
      }
    }
  }
}

/**
  * @brief  Displays a full rectangle.
  * @param  Xpos: specifies the X position, can be a value from 0 to 240.
  * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
  * @param  Height: rectangle height.
  * @param  Width: rectangle width.
  * @retval None
  */
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  DMA2D_InitTypeDef      DMA2D_InitStruct;
  
  uint32_t  Xaddress = 0; 
  uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0;
	uint16_t  OutputOffset = 0;
	uint16_t  NumberOfLine = 0;
	uint16_t  PixelPerLine = 0;	
// g_LcdDirection=2;
  Red_Value = (0xFF0000 & Colour.black)>>16 ;
  Blue_Value = 0x0000FF & Colour.black;
  Green_Value = (0x00FF00 & Colour.black)>>8;
  if (g_LcdDirection == 0)		/* ���� */
  {
	Xaddress = CurrentFrameBuffer + 3*(LCD_PIXEL_WIDTH*Ypos + Xpos);
	 OutputOffset = LCD_PIXEL_WIDTH - Width;
		NumberOfLine = Height;
		PixelPerLine = Width;
  }
  else if(g_LcdDirection == 1)	
  {
  Xaddress = CurrentFrameBuffer + 3 * (LCD_PIXEL_WIDTH * (LCD_PIXEL_HEIGHT - Height - Ypos) + LCD_PIXEL_WIDTH - Xpos - Width);	
		OutputOffset = LCD_PIXEL_WIDTH - Width;
		NumberOfLine = Height;
		PixelPerLine = Width;
  }
  else if(g_LcdDirection == 2)
  {
	  Xaddress = CurrentFrameBuffer + 3 * (LCD_PIXEL_WIDTH * (LCD_PIXEL_HEIGHT- Xpos -  Width) + Ypos);	
		OutputOffset = LCD_PIXEL_WIDTH  - Height;
		NumberOfLine = Width;
		PixelPerLine = Height;
  
  }else
  {
  Xaddress = CurrentFrameBuffer + 3 * (LCD_PIXEL_HEIGHT * Xpos + LCD_PIXEL_HEIGHT - Height - Ypos);	
		OutputOffset = LCD_PIXEL_HEIGHT - Height;
		NumberOfLine = Width;
		PixelPerLine = Height;
  
  }
  
  /* configure DMA2D */
  DMA2D_DeInit();
  DMA2D_InitStruct.DMA2D_Mode = DMA2D_R2M;       
  DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB888;      
  DMA2D_InitStruct.DMA2D_OutputGreen = Green_Value;      
  DMA2D_InitStruct.DMA2D_OutputBlue = Blue_Value;     
  DMA2D_InitStruct.DMA2D_OutputRed = Red_Value;                
  DMA2D_InitStruct.DMA2D_OutputAlpha = 0xfF;                  
  DMA2D_InitStruct.DMA2D_OutputMemoryAdd = Xaddress;                
  DMA2D_InitStruct.DMA2D_OutputOffset = OutputOffset;                
  DMA2D_InitStruct.DMA2D_NumberOfLine = NumberOfLine;            
  DMA2D_InitStruct.DMA2D_PixelPerLine = PixelPerLine;
  DMA2D_Init(&DMA2D_InitStruct); 
  
  /* Start Transfer */ 
  DMA2D_StartTransfer();
  
  /* Wait for CTC Flag activation */
  while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET)
  {
  } 

  LCD_SetTextColor(CurrentTextColor);
}

/**
  * @brief  Displays a full circle.
  * @param  Xpos: specifies the X position, can be a value from 0 to 240.
  * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
  * @param  Radius
  * @retval None
  */
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t  D;    /* Decision Variable */ 
  uint32_t  CurX;/* Current X Value */
  uint32_t  CurY;/* Current Y Value */ 
  
  D = 3 - (Radius << 1);
  
  CurX = 0;
  CurY = Radius;
  
  while (CurX <= CurY)
  {
    if(CurY > 0) 
    {
      LCD_DrawLine(Xpos - CurX, Ypos - CurY, 2*CurY, LCD_DIR_VERTICAL);
      LCD_DrawLine(Xpos + CurX, Ypos - CurY, 2*CurY, LCD_DIR_VERTICAL);
    }
    
    if(CurX > 0) 
    {
      LCD_DrawLine(Xpos - CurY, Ypos - CurX, 2*CurX, LCD_DIR_VERTICAL);
      LCD_DrawLine(Xpos + CurY, Ypos - CurX, 2*CurX, LCD_DIR_VERTICAL);
    }
    if (D < 0)
    { 
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
  
  LCD_DrawCircle(Xpos, Ypos, Radius);  
}

/**
  * @brief  Displays an uni-line (between two points).
  * @param  x1: specifies the point 1 x position.
  * @param  y1: specifies the point 1 y position.
  * @param  x2: specifies the point 2 x position.
  * @param  y2: specifies the point 2 y position.
  * @retval None
  */
void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
  curpixel = 0;
  
  deltax = ABS(x2 - x1);        /* The difference between the x's */
  deltay = ABS(y2 - y1);        /* The difference between the y's */
  x = x1;                       /* Start x off at the first pixel */
  y = y1;                       /* Start y off at the first pixel */
  
  if (x2 >= x1)                 /* The x-values are increasing */
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* The x-values are decreasing */
  {
    xinc1 = -1;
    xinc2 = -1;
  }
  
  if (y2 >= y1)                 /* The y-values are increasing */
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* The y-values are decreasing */
  {
    yinc1 = -1;
    yinc2 = -1;
  }
  
  if (deltax >= deltay)         /* There is at least one x-value for every y-value */
  {
    xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
    yinc2 = 0;                  /* Don't change the y for every iteration */
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;         /* There are more x-values than y-values */
  }
  else                          /* There is at least one y-value for every x-value */
  {
    xinc2 = 0;                  /* Don't change the x for every iteration */
    yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;         /* There are more y-values than x-values */
  }
  
  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    PutPixel(x, y);             /* Draw the current pixel */
    num += numadd;              /* Increase the numerator by the top of the fraction */
    if (num >= den)             /* Check if numerator >= denominator */
    {
      num -= den;               /* Calculate the new numerator value */
      x += xinc1;               /* Change the x as appropriate */
      y += yinc1;               /* Change the y as appropriate */
    }
    x += xinc2;                 /* Change the x as appropriate */
    y += yinc2;                 /* Change the y as appropriate */
  }
}

/**
  * @brief  Displays an triangle.
  * @param  Points: pointer to the points array.
  * @retval None
  */
void LCD_Triangle(pPoint Points, uint16_t PointCount)
{
  int16_t X = 0, Y = 0;
  pPoint First = Points;

  if(PointCount != 3)
  {
    return;
  }

  while(--PointCount)
  {
    X = Points->X;
    Y = Points->Y;
    Points++;
    LCD_DrawUniLine(X, Y, Points->X, Points->Y);
  }
  LCD_DrawUniLine(First->X, First->Y, Points->X, Points->Y);
}

/**
  * @brief  Fill an triangle (between 3 points).
  * @param  x1..3: x position of triangle point 1..3.
  * @param  y1..3: y position of triangle point 1..3.
  * @retval None
  */
void LCD_FillTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3)
{ 
  
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
  curpixel = 0;
  
  deltax = ABS(x2 - x1);        /* The difference between the x's */
  deltay = ABS(y2 - y1);        /* The difference between the y's */
  x = x1;                       /* Start x off at the first pixel */
  y = y1;                       /* Start y off at the first pixel */
  
  if (x2 >= x1)                 /* The x-values are increasing */
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* The x-values are decreasing */
  {
    xinc1 = -1;
    xinc2 = -1;
  }
  
  if (y2 >= y1)                 /* The y-values are increasing */
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* The y-values are decreasing */
  {
    yinc1 = -1;
    yinc2 = -1;
  }
  
  if (deltax >= deltay)         /* There is at least one x-value for every y-value */
  {
    xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
    yinc2 = 0;                  /* Don't change the y for every iteration */
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;         /* There are more x-values than y-values */
  }
  else                          /* There is at least one y-value for every x-value */
  {
    xinc2 = 0;                  /* Don't change the x for every iteration */
    yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;         /* There are more y-values than x-values */
  }
  
  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    LCD_DrawUniLine(x, y, x3, y3);
    
    num += numadd;              /* Increase the numerator by the top of the fraction */
    if (num >= den)             /* Check if numerator >= denominator */
    {
      num -= den;               /* Calculate the new numerator value */
      x += xinc1;               /* Change the x as appropriate */
      y += yinc1;               /* Change the y as appropriate */
    }
    x += xinc2;                 /* Change the x as appropriate */
    y += yinc2;                 /* Change the y as appropriate */
  }  
  
  
}
/**
  * @brief  Displays an poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_PolyLine(pPoint Points, uint16_t PointCount)
{
  int16_t X = 0, Y = 0;

  if(PointCount < 2)
  {
    return;
  }

  while(--PointCount)
  {
    X = Points->X;
    Y = Points->Y;
    Points++;
    LCD_DrawUniLine(X, Y, Points->X, Points->Y);
  }
}

/**
  * @brief  Displays an relative poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @param  Closed: specifies if the draw is closed or not.
  *           1: closed, 0 : not closed.
  * @retval None
  */
static void LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount, uint16_t Closed)
{
  int16_t X = 0, Y = 0;
  pPoint First = Points;

  if(PointCount < 2)
  {
    return;
  }  
  X = Points->X;
  Y = Points->Y;
  while(--PointCount)
  {
    Points++;
    LCD_DrawUniLine(X, Y, X + Points->X, Y + Points->Y);
    X = X + Points->X;
    Y = Y + Points->Y;
  }
  if(Closed)
  {
    LCD_DrawUniLine(First->X, First->Y, X, Y);
  }  
}

/**
  * @brief  Displays a closed poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_ClosedPolyLine(pPoint Points, uint16_t PointCount)
{
  LCD_PolyLine(Points, PointCount);
  LCD_DrawUniLine(Points->X, Points->Y, (Points+PointCount-1)->X, (Points+PointCount-1)->Y);
}

/**
  * @brief  Displays a relative poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_PolyLineRelative(pPoint Points, uint16_t PointCount)
{
  LCD_PolyLineRelativeClosed(Points, PointCount, 0);
}

/**
  * @brief  Displays a closed relative poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_ClosedPolyLineRelative(pPoint Points, uint16_t PointCount)
{
  LCD_PolyLineRelativeClosed(Points, PointCount, 1);
}

/**
  * @brief  Displays a  full poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_FillPolyLine(pPoint Points, uint16_t PointCount)
{
 
  int16_t X = 0, Y = 0, X2 = 0, Y2 = 0, X_center = 0, Y_center = 0, X_first = 0, Y_first = 0, pixelX = 0, pixelY = 0, counter = 0;
  uint16_t  IMAGE_LEFT = 0, IMAGE_RIGHT = 0, IMAGE_TOP = 0, IMAGE_BOTTOM = 0;  

  IMAGE_LEFT = IMAGE_RIGHT = Points->X;
  IMAGE_TOP= IMAGE_BOTTOM = Points->Y;

  for(counter = 1; counter < PointCount; counter++)
  {
    pixelX = POLY_X(counter);
    if(pixelX < IMAGE_LEFT)
    {
      IMAGE_LEFT = pixelX;
    }
    if(pixelX > IMAGE_RIGHT)
    {
      IMAGE_RIGHT = pixelX;
    }
    
    pixelY = POLY_Y(counter);
    if(pixelY < IMAGE_TOP)
    { 
      IMAGE_TOP = pixelY;
    }
    if(pixelY > IMAGE_BOTTOM)
    {
      IMAGE_BOTTOM = pixelY;
    }
  }  
  
  if(PointCount < 2)
  {
    return;
  }
  
  X_center = (IMAGE_LEFT + IMAGE_RIGHT)/2;
  Y_center = (IMAGE_BOTTOM + IMAGE_TOP)/2;
 
  X_first = Points->X;
  Y_first = Points->Y;
  
  while(--PointCount)
  {
    X = Points->X;
    Y = Points->Y;
    Points++;
    X2 = Points->X;
    Y2 = Points->Y;    
  
    LCD_FillTriangle(X, X2, X_center, Y, Y2, Y_center);
    LCD_FillTriangle(X, X_center, X2, Y, Y_center, Y2);
    LCD_FillTriangle(X_center, X2, X, Y_center, Y2, Y);   
  }
  
  LCD_FillTriangle(X_first, X2, X_center, Y_first, Y2, Y_center);
  LCD_FillTriangle(X_first, X_center, X2, Y_first, Y_center, Y2);
  LCD_FillTriangle(X_center, X2, X_first, Y_center, Y2, Y_first); 
}




/**
  * @brief  Sets or reset LCD control lines.
  * @param  GPIOx: where x can be B or D to select the GPIO peripheral.
  * @param  CtrlPins: the Control line.
  *   This parameter can be:
  *     @arg LCD_NCS_PIN: Chip Select pin
  *     @arg LCD_NWR_PIN: Read/Write Selection pin
  *     @arg LCD_RS_PIN: Register/RAM Selection pin
  * @param  BitVal: specifies the value to be written to the selected bit.
  *   This parameter can be:
  *     @arg Bit_RESET: to clear the port pin
  *     @arg Bit_SET: to set the port pin
  * @retval None
  */
void LCD_CtrlLinesWrite(GPIO_TypeDef* GPIOx, uint16_t CtrlPins, BitAction BitVal)
{
  /* Set or Reset the control line */
  GPIO_WriteBit(GPIOx, (uint16_t)CtrlPins, (BitAction)BitVal);
}



static void LCD_GPIO_Config(void)
{ 
	GPIO_InitTypeDef GPIO_InitStruct;
  
  /* ʹ��LCDʹ�õ�������ʱ�� */
                          //��ɫ������
  RCC_AHB1PeriphClockCmd(LTDC_R0_GPIO_CLK | LTDC_R1_GPIO_CLK | LTDC_R2_GPIO_CLK| 
                         LTDC_R3_GPIO_CLK | LTDC_R4_GPIO_CLK | LTDC_R5_GPIO_CLK|
                         LTDC_R6_GPIO_CLK | LTDC_R7_GPIO_CLK |
                          //��ɫ������
                          LTDC_G0_GPIO_CLK|LTDC_G1_GPIO_CLK|LTDC_G2_GPIO_CLK|
                          LTDC_G3_GPIO_CLK|LTDC_G4_GPIO_CLK|LTDC_G5_GPIO_CLK|
                          LTDC_G6_GPIO_CLK|LTDC_G7_GPIO_CLK|
                          //��ɫ������
                          LTDC_B0_GPIO_CLK|LTDC_B1_GPIO_CLK|LTDC_B2_GPIO_CLK|
                          LTDC_B3_GPIO_CLK|LTDC_B4_GPIO_CLK|LTDC_B5_GPIO_CLK|
                          LTDC_B6_GPIO_CLK|LTDC_B7_GPIO_CLK|
                          //�����ź���
                          LTDC_CLK_GPIO_CLK | LTDC_HSYNC_GPIO_CLK |LTDC_VSYNC_GPIO_CLK|
                          LTDC_DE_GPIO_CLK  | LTDC_BL_GPIO_CLK    |LTDC_DISP_GPIO_CLK ,ENABLE);

/* GPIO���� */

 /* ��ɫ������ */
  GPIO_InitStruct.GPIO_Pin = LTDC_R0_GPIO_PIN;                             
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  
  GPIO_Init(LTDC_R0_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_R0_GPIO_PORT, LTDC_R0_PINSOURCE, LTDC_R0_AF);

  GPIO_InitStruct.GPIO_Pin = LTDC_R1_GPIO_PIN;
  GPIO_Init(LTDC_R1_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_R1_GPIO_PORT, LTDC_R1_PINSOURCE, LTDC_R1_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_R2_GPIO_PIN;
  GPIO_Init(LTDC_R2_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_R2_GPIO_PORT, LTDC_R2_PINSOURCE, LTDC_R2_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_R3_GPIO_PIN;
  GPIO_Init(LTDC_R3_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_R3_GPIO_PORT, LTDC_R3_PINSOURCE, LTDC_R3_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_R4_GPIO_PIN;
  GPIO_Init(LTDC_R4_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_R4_GPIO_PORT, LTDC_R4_PINSOURCE, LTDC_R4_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_R5_GPIO_PIN;
  GPIO_Init(LTDC_R5_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_R5_GPIO_PORT, LTDC_R5_PINSOURCE, LTDC_R5_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_R6_GPIO_PIN;
  GPIO_Init(LTDC_R6_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_R6_GPIO_PORT, LTDC_R6_PINSOURCE, LTDC_R6_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_R7_GPIO_PIN;
  GPIO_Init(LTDC_R7_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_R7_GPIO_PORT, LTDC_R7_PINSOURCE, LTDC_R7_AF);
  
  //��ɫ������
  GPIO_InitStruct.GPIO_Pin = LTDC_G0_GPIO_PIN;
  GPIO_Init(LTDC_G0_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_G0_GPIO_PORT, LTDC_G0_PINSOURCE, LTDC_G0_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_G1_GPIO_PIN;
  GPIO_Init(LTDC_G1_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_G1_GPIO_PORT, LTDC_G1_PINSOURCE, LTDC_G1_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_G2_GPIO_PIN;
  GPIO_Init(LTDC_G2_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_G2_GPIO_PORT, LTDC_G2_PINSOURCE, LTDC_G2_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_G3_GPIO_PIN;
  GPIO_Init(LTDC_G3_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_G3_GPIO_PORT, LTDC_G3_PINSOURCE, LTDC_G3_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_G4_GPIO_PIN;
  GPIO_Init(LTDC_G4_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_G4_GPIO_PORT, LTDC_G4_PINSOURCE, LTDC_G4_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_G5_GPIO_PIN;
  GPIO_Init(LTDC_G5_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_G5_GPIO_PORT, LTDC_G5_PINSOURCE, LTDC_G5_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_G6_GPIO_PIN;
  GPIO_Init(LTDC_G6_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_G6_GPIO_PORT, LTDC_G6_PINSOURCE, LTDC_G6_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_G7_GPIO_PIN;
  GPIO_Init(LTDC_G7_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_G7_GPIO_PORT, LTDC_G7_PINSOURCE, LTDC_G7_AF);
  
  //��ɫ������
  GPIO_InitStruct.GPIO_Pin = LTDC_B0_GPIO_PIN;
  GPIO_Init(LTDC_B0_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_B0_GPIO_PORT, LTDC_B0_PINSOURCE, LTDC_B0_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_B1_GPIO_PIN;
  GPIO_Init(LTDC_B1_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_B1_GPIO_PORT, LTDC_B1_PINSOURCE, LTDC_B1_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_B2_GPIO_PIN;
  GPIO_Init(LTDC_B2_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_B2_GPIO_PORT, LTDC_B2_PINSOURCE, LTDC_B2_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_B3_GPIO_PIN;
  GPIO_Init(LTDC_B3_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_B3_GPIO_PORT, LTDC_B3_PINSOURCE, LTDC_B3_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_B4_GPIO_PIN;
  GPIO_Init(LTDC_B4_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_B4_GPIO_PORT, LTDC_B4_PINSOURCE, LTDC_B4_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_B5_GPIO_PIN;
  GPIO_Init(LTDC_B5_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_B5_GPIO_PORT, LTDC_B5_PINSOURCE, LTDC_B5_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_B6_GPIO_PIN;
  GPIO_Init(LTDC_B6_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_B6_GPIO_PORT, LTDC_B6_PINSOURCE, LTDC_B6_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_B7_GPIO_PIN;
  GPIO_Init(LTDC_B7_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_B7_GPIO_PORT, LTDC_B7_PINSOURCE, LTDC_B7_AF);
  
  //�����ź���
  GPIO_InitStruct.GPIO_Pin = LTDC_CLK_GPIO_PIN;
  GPIO_Init(LTDC_CLK_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_CLK_GPIO_PORT, LTDC_CLK_PINSOURCE, LTDC_CLK_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_HSYNC_GPIO_PIN;
  GPIO_Init(LTDC_HSYNC_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_HSYNC_GPIO_PORT, LTDC_HSYNC_PINSOURCE, LTDC_HSYNC_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_VSYNC_GPIO_PIN;
  GPIO_Init(LTDC_VSYNC_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_VSYNC_GPIO_PORT, LTDC_VSYNC_PINSOURCE, LTDC_VSYNC_AF);
  
  GPIO_InitStruct.GPIO_Pin = LTDC_DE_GPIO_PIN;
  GPIO_Init(LTDC_DE_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(LTDC_DE_GPIO_PORT, LTDC_DE_PINSOURCE, LTDC_DE_AF);
  
  //BL DISP
  GPIO_InitStruct.GPIO_Pin = LTDC_DISP_GPIO_PIN;                             
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  
  GPIO_Init(LTDC_DISP_GPIO_PORT, &GPIO_InitStruct);
  
  
  GPIO_InitStruct.GPIO_Pin = LTDC_BL_GPIO_PIN; 
  GPIO_Init(LTDC_BL_GPIO_PORT, &GPIO_InitStruct);
  
  //����ʹ��lcd
  GPIO_SetBits(LTDC_DISP_GPIO_PORT,LTDC_DISP_GPIO_PIN);
  GPIO_SetBits(LTDC_BL_GPIO_PORT,LTDC_BL_GPIO_PIN);


}

/**
  * @brief  Displays a pixel.
  * @param  x: pixel x.
  * @param  y: pixel y.  
  * @retval None
  */
 void PutPixel(int16_t x, int16_t y) 
{ 
  if(x < 0 || x > LCD_PIXEL_HEIGHT || y < 0 || y > LCD_PIXEL_WIDTH)
  {
    return;  
  }
#if 0
 LCD_DrawLine(x, y, 1, LCD_DIR_HORIZONTAL);
#else /*����ֱ�����Ч�ʹ��Ƹߵ��*/
 {
	  /*RGB888*/
//	  uint32_t  Xaddress =0;
//    Xaddress =  CurrentFrameBuffer + 3*(LCD_PIXEL_WIDTH*y + x);
//	  *(__IO uint16_t*) Xaddress= (0x00FFFF & CurrentTextColor);        //GB
//  	*(__IO uint8_t*)( Xaddress+2)= (0xFF0000 & CurrentTextColor) >> 16; //R
	 LCD429_PutPixel(x, y, CurrentTextColor);
  }
#endif
}




//#endif
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

/**
  * @}
  */ 
	
#else	
/****************RG565����***************************RGB565����**********************************RGB565����*****************************RGB565����*****************************RGB565����*****************************/


/**
 ******************************************************************************
 * @file    stm32f429i_discovery_lcd.c
 * @author  MCD Application Team
 * @version V1.0.1
 * @date    28-October-2013
 * @brief   This file includes the LCD driver for ILI9341 Liquid Crystal
 *          Display Modules of STM32F429I-DISCO kit (MB1075).
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */
/**
 * @}
 */

/** @defgroup STM32F429I_DISCOVERY_LCD_Private_Variables
 * @{
 */
static sFONT *LCD_Currentfonts;
/* Global variables to set the written text color */
static uint16_t CurrentTextColor   = 0x0000;
static uint16_t CurrentBackColor   = 0xFFFF;
/* Default LCD configuration with LCD Layer 1 */
static uint32_t CurrentFrameBuffer = LCD_FRAME_BUFFER;
static uint32_t CurrentLayer = LCD_BACKGROUND_LAYER;
/**
 * @}
 */
static void LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount, uint16_t Closed);
static void LCD_GPIO_Config(void);

/**
 * @}
 */



/**
 * @brief  Initializes the LCD.
 * @param  None
 * @retval None
 */

/*����Һ�������ֲ�Ĳ�������*/
#define HBP  46		//HSYNC�����Ч����
#define VBP  23		//VSYNC�����Ч����

#define HSW   1		//HSYNC����
#define VSW   1		//VSYNC����

#define HFP  20		//HSYNCǰ����Ч����
#define VFP   22		//VSYNCǰ����Ч����

#define ZOOMMAXBUFF 16384
uint8_t zoomBuff[ZOOMMAXBUFF] = {0};	//�������ŵĻ��棬���֧�ֵ�128*128

void LCD_Init(void)
{
 LTDC_InitTypeDef       LTDC_InitStruct;

 /* Enable the LTDC Clock */
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_LTDC, ENABLE);

 /* Enable the DMA2D Clock */
 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2D, ENABLE);

 /* Configure the LCD Control pins */
 LCD_GPIO_Config();

 /* Configure the FMC Parallel interface : SDRAM is used as Frame Buffer for LCD */
 SDRAM_Init();

 /* LTDC Configuration *********************************************************/
 /* Polarity configuration */
 /* Initialize the horizontal synchronization polarity as active low */
 LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AL;
 /* Initialize the vertical synchronization polarity as active low */
 LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;
 /* Initialize the data enable polarity as active low */
 LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AL;
 /* Initialize the pixel clock polarity as input pixel clock */
 LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IPC;

 /* Configure R,G,B component values for LCD background color */
 LTDC_InitStruct.LTDC_BackgroundRedValue = 0;
 LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;
 LTDC_InitStruct.LTDC_BackgroundBlueValue = 0;

	/* ���� PLLSAI ��Ƶ�������������Ϊ����ͬ��ʱ��CLK*/
  /* PLLSAI_VCO ����ʱ�� = HSE_VALUE/PLL_M = 1 Mhz */
  /* PLLSAI_VCO ���ʱ�� = PLLSAI_VCO���� * PLLSAI_N = 384 Mhz */
  /* PLLLCDCLK = PLLSAI_VCO ���/PLLSAI_R = 384/6  Mhz */
  /* LTDC ʱ��Ƶ�� = PLLLCDCLK / DIV = 384/4/4 = 24 Mhz */
	/* LTDCʱ��̫�߻ᵼ����������ˢ���ٶ�Ҫ�󲻸ߣ�����ʱ��Ƶ�ʿɼ��ٻ�������*/
	/* ���º������������ֱ�Ϊ��PLLSAIN,PLLSAIQ,PLLSAIR������PLLSAIQ��LTDC�޹�*/
 RCC_PLLSAIConfig(384, 7, 4);
 	/*���º����Ĳ���ΪDIVֵ*/
 RCC_LTDCCLKDivConfig(RCC_PLLSAIDivR_Div4);

 /* Enable PLLSAI Clock */
 RCC_PLLSAICmd(ENABLE);
 /* Wait for PLLSAI activation */
 while(RCC_GetFlagStatus(RCC_FLAG_PLLSAIRDY) == RESET)
 {
 }

  /* ʱ��������� */  
 /* ������ͬ���źſ���(HSW-1) */
 LTDC_InitStruct.LTDC_HorizontalSync =HSW-1;
 /* ���ô�ֱͬ���źſ���(VSW-1) */
 LTDC_InitStruct.LTDC_VerticalSync = VSW-1;
 /* ����(HSW+HBP-1) */
 LTDC_InitStruct.LTDC_AccumulatedHBP =HSW+HBP-1;
 /* ����(VSW+VBP-1) */
 LTDC_InitStruct.LTDC_AccumulatedVBP = VSW+VBP-1;
 /* ����(HSW+HBP+��Ч���ؿ���-1) */
 LTDC_InitStruct.LTDC_AccumulatedActiveW = HSW+HBP+LCD_PIXEL_WIDTH-1;
 /* ����(VSW+VBP+��Ч���ظ߶�-1) */
 LTDC_InitStruct.LTDC_AccumulatedActiveH = VSW+VBP+LCD_PIXEL_HEIGHT-1;
 /* �����ܿ���(HSW+HBP+��Ч���ؿ���+HFP-1) */
 LTDC_InitStruct.LTDC_TotalWidth =HSW+ HBP+LCD_PIXEL_WIDTH  + HFP-1; 
 /* �����ܸ߶�(VSW+VBP+��Ч���ظ߶�+VFP-1) */
 LTDC_InitStruct.LTDC_TotalHeigh =VSW+ VBP+LCD_PIXEL_HEIGHT  + VFP-1;

 LTDC_Init(&LTDC_InitStruct);
}
/**
 * @brief  Initializes the LCD Layers.
 * @param  None
 * @retval None
 */
void LCD_LayerInit(void)
{
 LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct;

  /* �㴰������ */
  /* ���ñ���Ĵ��ڱ߽磬ע����Щ�����ǰ���HBP HSW VBP VSW�� */    
	//һ�еĵ�һ����ʼ���أ��ó�ԱֵӦ��Ϊ (LTDC_InitStruct.LTDC_AccumulatedHBP+1)��ֵ
	LTDC_Layer_InitStruct.LTDC_HorizontalStart = HBP + HSW;
	//һ�е����һ�����أ��ó�ԱֵӦ��Ϊ (LTDC_InitStruct.LTDC_AccumulatedActiveW)��ֵ
	LTDC_Layer_InitStruct.LTDC_HorizontalStop = HSW+HBP+LCD_PIXEL_WIDTH-1;
	//һ�еĵ�һ����ʼ���أ��ó�ԱֵӦ��Ϊ (LTDC_InitStruct.LTDC_AccumulatedVBP+1)��ֵ
	LTDC_Layer_InitStruct.LTDC_VerticalStart =  VBP + VSW;
	//һ�е����һ�����أ��ó�ԱֵӦ��Ϊ (LTDC_InitStruct.LTDC_AccumulatedActiveH)��ֵ
	LTDC_Layer_InitStruct.LTDC_VerticalStop = VSW+VBP+LCD_PIXEL_HEIGHT-1;

 /* Pixel Format configuration*/
 LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_ARGB1555;
 /* Alpha constant (255 totally opaque) */
 LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255;
 /* Default Color configuration (configure A,R,G,B component values) */
 LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;
 LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0;
 LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;
 LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;
 /* Configure blending factors */
 LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;
 LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;

 /* the length of one line of pixels in bytes + 3 then :
 Line Lenth = Active high width x number of bytes per pixel + 3
 Active high width         = LCD_PIXEL_WIDTH
 number of bytes per pixel = 2    (pixel_format : RGB565)
 */
 LTDC_Layer_InitStruct.LTDC_CFBLineLength = ((LCD_PIXEL_WIDTH * 2) + 3);
 /* the pitch is the increment from the start of one line of pixels to the
 start of the next line in bytes, then :
 Pitch = Active high width x number of bytes per pixel */
 LTDC_Layer_InitStruct.LTDC_CFBPitch = (LCD_PIXEL_WIDTH * 2);

 /* Configure the number of lines */
 LTDC_Layer_InitStruct.LTDC_CFBLineNumber = LCD_PIXEL_HEIGHT;

 /* Start Address configuration : the LCD Frame buffer is defined on SDRAM */
 LTDC_Layer_InitStruct.LTDC_CFBStartAdress = LCD_FRAME_BUFFER;

 /* Initialize LTDC layer 1 */
 LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);

  /* Configure Layer2 */
 /* Pixel Format configuration*/
 LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_ARGB1555;
 
  /* Start Address configuration : the LCD Frame buffer is defined on SDRAM w/ Offset */
 LTDC_Layer_InitStruct.LTDC_CFBStartAdress = LCD_FRAME_BUFFER + BUFFER_OFFSET;

  /* Configure blending factors */
 LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_PAxCA;
 LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_PAxCA;

  /* Initialize LTDC layer 2 */
 LTDC_LayerInit(LTDC_Layer2, &LTDC_Layer_InitStruct);

  /* LTDC configuration reload */
 LTDC_ReloadConfig(LTDC_IMReload);

  /* Enable foreground & background Layers */
 LTDC_LayerCmd(LTDC_Layer1, ENABLE);
 LTDC_LayerCmd(LTDC_Layer2, ENABLE);

  /* LTDC configuration reload */
 LTDC_ReloadConfig(LTDC_IMReload);

  /* Set default font */
 LCD_SetFont(&LCD_DEFAULT_FONT);

  /* dithering activation */
 LTDC_DitherCmd(ENABLE);
}



/**
 * @brief  Sets the LCD Layer.
 * @param  Layerx: specifies the Layer foreground or background.
 * @retval None
 */
void LCD_SetLayer(uint32_t Layerx)
{
 if (Layerx == LCD_BACKGROUND_LAYER)
 {
   CurrentFrameBuffer = LCD_FRAME_BUFFER;
   CurrentLayer = LCD_BACKGROUND_LAYER;
 }
 else
 {
   CurrentFrameBuffer = LCD_FRAME_BUFFER + BUFFER_OFFSET;
   CurrentLayer = LCD_FOREGROUND_LAYER;
 }
}

/**
 * @brief  Sets the LCD Text and Background colors.
 * @param  TextColor: specifies the Text Color.
 * @param  BackColor: specifies the Background Color.
 * @retval None
 */
void LCD_SetColors(uint16_t TextColor, uint16_t BackColor)
{
 CurrentTextColor = TextColor;
 CurrentBackColor = BackColor;
}

/**
 * @brief  Gets the LCD Text and Background colors.
 * @param  TextColor: pointer to the variable that will contain the Text
           Color.
 * @param  BackColor: pointer to the variable that will contain the Background
           Color.
 * @retval None
 */
void LCD_GetColors(uint16_t *TextColor, uint16_t *BackColor)
{
 *TextColor = CurrentTextColor;
 *BackColor = CurrentBackColor;
}

/**
 * @brief  Sets the Text color.
 * @param  Color: specifies the Text color code RGB(5-6-5).
 * @retval None
 */
void LCD_SetTextColor(uint16_t Color)
{
 CurrentTextColor = Color;
}

/**
 * @brief  Sets the Background color.
 * @param  Color: specifies the Background color code RGB(5-6-5).
 * @retval None
 */
void LCD_SetBackColor(uint16_t Color)
{
 CurrentBackColor = Color;
}

/**
 * @brief  Sets the Text Font.
 * @param  fonts: specifies the font to be used.
 * @retval None
 */
void LCD_SetFont(sFONT *fonts)
{
 LCD_Currentfonts = fonts;
}

/**
 * @brief  Configure the transparency.
 * @param  transparency: specifies the transparency,
 *         This parameter must range from 0x00 to 0xFF.
 *         0x00 -> Transparent   0xFF -> Opaque
 * @retval None
 */
void LCD_SetTransparency(uint8_t transparency)
{
 if (CurrentLayer == LCD_BACKGROUND_LAYER)
 {
   LTDC_LayerAlpha(LTDC_Layer1, transparency);
 }
 else
 {
   LTDC_LayerAlpha(LTDC_Layer2, transparency);
 }
 LTDC_ReloadConfig(LTDC_IMReload);
}

/**
 * @brief  Gets the Text Font.
 * @param  None.
 * @retval the used font.
 */
sFONT *LCD_GetFont(void)
{
 return LCD_Currentfonts;
}

/**
 * @brief  Clears the selected line.
 * @param  Line: the Line to be cleared.
 *   This parameter can be one of the following values:
 *     @arg LCD_LINE_x: where x can be: 0..13 if LCD_Currentfonts is Font16x24
 *                                      0..26 if LCD_Currentfonts is Font12x12 or Font8x12
 *                                      0..39 if LCD_Currentfonts is Font8x8
 * @retval None
 */
void LCD_ClearLine(uint16_t Line)
{
 uint16_t refcolumn = 0;
 /* Send the string character by character on lCD */
 while ((refcolumn < LCD_PIXEL_WIDTH) && (((refcolumn + LCD_Currentfonts->Width)& 0xFFFF) >= LCD_Currentfonts->Width))
 {
   /* Display one character on LCD */
   LCD_DisplayChar(Line, refcolumn, ' ');
   /* Decrement the column position by 16 */
   refcolumn += LCD_Currentfonts->Width;
 }
}

/**
 * @brief  Clears the hole LCD.
 * @param  Color: the color of the background.
 * @retval None
 */
void LCD_Clear(uint16_t Color)
{
 DMA2D_InitTypeDef      DMA2D_InitStruct;

 uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0;

 Red_Value = (0xF800 & Color) >> 11;
 Blue_Value = 0x001F & Color;
 Green_Value = (0x07E0 & Color) >> 5;


 /* configure DMA2D */
 DMA2D_DeInit();
 DMA2D_InitStruct.DMA2D_Mode = DMA2D_R2M;
 DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB565;
 DMA2D_InitStruct.DMA2D_OutputGreen = Green_Value;
 DMA2D_InitStruct.DMA2D_OutputBlue = Blue_Value;
 DMA2D_InitStruct.DMA2D_OutputRed = Red_Value;
 DMA2D_InitStruct.DMA2D_OutputAlpha = (Color&0x8000) ? 0xFF:0x00;		//����͸����
 DMA2D_InitStruct.DMA2D_OutputMemoryAdd = CurrentFrameBuffer;
 DMA2D_InitStruct.DMA2D_OutputOffset = 0;
 DMA2D_InitStruct.DMA2D_NumberOfLine = LCD_PIXEL_HEIGHT;
 DMA2D_InitStruct.DMA2D_PixelPerLine = LCD_PIXEL_WIDTH;
 DMA2D_Init(&DMA2D_InitStruct);

 /* Start Transfer */
 DMA2D_StartTransfer();

 /* Wait for CTC Flag activation */
 while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET)
 {
 }

 LCD_SetTextColor(CurrentTextColor);
}

/**
 * @brief  Sets the cursor position.
 * @param  Xpos: specifies the X position.
 * @param  Ypos: specifies the Y position.
 * @retval Display Address
 */
uint32_t LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
 return CurrentFrameBuffer + 2*(Xpos + (LCD_PIXEL_WIDTH*Ypos));
}

/**
 * @brief  Config and Sets the color Keying.
 * @param  RGBValue: Specifies the Color reference.
 * @retval None
 */
void LCD_SetColorKeying(uint32_t RGBValue)
{
 LTDC_ColorKeying_InitTypeDef   LTDC_colorkeying_InitStruct;

 /* configure the color Keying */
 LTDC_colorkeying_InitStruct.LTDC_ColorKeyBlue = 0x0000FF & RGBValue;
 LTDC_colorkeying_InitStruct.LTDC_ColorKeyGreen = (0x00FF00 & RGBValue) >> 8;
 LTDC_colorkeying_InitStruct.LTDC_ColorKeyRed = (0xFF0000 & RGBValue) >> 16;

 if (CurrentLayer == LCD_BACKGROUND_LAYER)
 {
   /* Enable the color Keying for Layer1 */
   LTDC_ColorKeyingConfig(LTDC_Layer1, &LTDC_colorkeying_InitStruct, ENABLE);
   LTDC_ReloadConfig(LTDC_IMReload);
 }
 else
 {
   /* Enable the color Keying for Layer2 */
   LTDC_ColorKeyingConfig(LTDC_Layer2, &LTDC_colorkeying_InitStruct, ENABLE);
   LTDC_ReloadConfig(LTDC_IMReload);
 }
}

/**
 * @brief  Disable the color Keying.
 * @param  RGBValue: Specifies the Color reference.
 * @retval None
 */
void LCD_ReSetColorKeying(void)
{
 LTDC_ColorKeying_InitTypeDef   LTDC_colorkeying_InitStruct;

 if (CurrentLayer == LCD_BACKGROUND_LAYER)
 {
   /* Disable the color Keying for Layer1 */
   LTDC_ColorKeyingConfig(LTDC_Layer1, &LTDC_colorkeying_InitStruct, DISABLE);
   LTDC_ReloadConfig(LTDC_IMReload);
 }
 else
 {
   /* Disable the color Keying for Layer2 */
   LTDC_ColorKeyingConfig(LTDC_Layer2, &LTDC_colorkeying_InitStruct, DISABLE);
   LTDC_ReloadConfig(LTDC_IMReload);
 }
}

/**
 * @brief  Draws a character on LCD.
 * @param  Xpos: the Line where to display the character shape.
 * @param  Ypos: start column address.
 * @param  c: pointer to the character data.
 * @retval None
 */
void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint16_t *c)
{
 uint32_t index = 0, counter = 0, xpos =0;
 uint32_t  Xaddress = 0;

 xpos = Xpos*LCD_PIXEL_WIDTH*2;
 Xaddress += Ypos;

 for(index = 0; index < LCD_Currentfonts->Height; index++)
 {

   for(counter = 0; counter < LCD_Currentfonts->Width; counter++)
   {

     if((((c[index] & ((0x80 << ((LCD_Currentfonts->Width / 12 ) * 8 ) ) >> counter)) == 0x00) &&(LCD_Currentfonts->Width <= 12))||
       (((c[index] & (0x1 << counter)) == 0x00)&&(LCD_Currentfonts->Width > 12 )))
     {
         /* Write data value to all SDRAM memory */
        *(__IO uint16_t*) (CurrentFrameBuffer + (2*Xaddress) + xpos) = CurrentBackColor;
     }
     else
     {
         /* Write data value to all SDRAM memory */
        *(__IO uint16_t*) (CurrentFrameBuffer + (2*Xaddress) + xpos) = CurrentTextColor;
     }
     Xaddress++;
   }
     Xaddress += (LCD_PIXEL_WIDTH - LCD_Currentfonts->Width);
 }
}

/**
 * @brief  Displays one character (16dots width, 24dots height).
 * @param  Line: the Line where to display the character shape .
 *   This parameter can be one of the following values:
 *     @arg Linex: where x can be 0..29
 * @param  Column: start column address.
 * @param  Ascii: character ascii code, must be between 0x20 and 0x7E.
 * @retval None
 */
void LCD_DisplayChar(uint16_t Line, uint16_t Column, uint8_t Ascii)
{
 Ascii -= 32;

 LCD_DrawChar(Line, Column, &LCD_Currentfonts->table[Ascii * LCD_Currentfonts->Height]);
}

/**
 * @brief  Displays a maximum of 20 char on the LCD.
 * @param  Line: the Line where to display the character shape .
 *   This parameter can be one of the following values:
 *     @arg Linex: where x can be 0..9
 * @param  *ptr: pointer to string to display on LCD.
 * @retval None
 */
void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr)
{
 uint16_t refcolumn = 0;
 /* Send the string character by character on lCD */
 while ((refcolumn < LCD_PIXEL_WIDTH) && ((*ptr != 0) & (((refcolumn + LCD_Currentfonts->Width) & 0xFFFF) >= LCD_Currentfonts->Width)))
 {
   /* Display one character on LCD */
   LCD_DisplayChar(Line, refcolumn, *ptr);
   /* Decrement the column position by width */
   refcolumn += LCD_Currentfonts->Width;
   /* Point on the next character */
   ptr++;
 }
}




/**
 * @brief  ����ʾ������ʾһ�������ַ�
 * @param  usX �����ض�ɨ�跽�����ַ�����ʼX����
 * @param  usY �����ض�ɨ�跽�����ַ�����ʼY����
 * @param  usChar ��Ҫ��ʾ�������ַ��������룩
 * @retval ��
 */ 
void LCD_DispChar_CH ( uint16_t usX, uint16_t usY, uint16_t usChar)
{
	uint8_t ucPage, ucColumn;
	uint8_t ucBuffer [ 24*24/8 ];	

  uint32_t usTemp; 	
	
	
	uint32_t  xpos =0;
  uint32_t  Xaddress = 0;
  
	/*xpos��ʾ��ǰ�е��Դ�ƫ��λ��*/
  xpos = usX*LCD_PIXEL_WIDTH*2;
	
	/*Xaddress��ʾ���ص�*/
  Xaddress += usY;
	   
  macGetGBKCode ( ucBuffer, usChar );	//ȡ��ģ����
	
	/*ucPage��ʾ��ǰ����*/
	for ( ucPage = 0; ucPage < macHEIGHT_CH_CHAR; ucPage ++ )
	{
    /* ȡ��3���ֽڵ����ݣ���lcd�ϼ���һ�����ֵ�һ�� */
		usTemp = ucBuffer [ ucPage * 3 ];
		usTemp = ( usTemp << 8 );
		usTemp |= ucBuffer [ ucPage * 3 + 1 ];
		usTemp = ( usTemp << 8 );
		usTemp |= ucBuffer [ ucPage * 3 + 2];
	
		
		for ( ucColumn = 0; ucColumn < macWIDTH_CH_CHAR; ucColumn ++ ) 
		{			
			if ( usTemp & ( 0x01 << 23 ) )  //��λ��ǰ 				
			{
				//����ɫ
        /* Write data value to all SDRAM memory */
        *(__IO uint16_t*) (CurrentFrameBuffer + (2*Xaddress) + xpos) = CurrentTextColor;

			}				
			else	
			{
				//����ɫ
         /* Write data value to all SDRAM memory */
        *(__IO uint16_t*) (CurrentFrameBuffer + (2*Xaddress) + xpos) = CurrentBackColor;
			}	
			/*ָ��ǰ�е���һ����*/	
			Xaddress++;			
			usTemp <<= 1;
			
		}
		/*��ʾ��һ��*/
		/*ָ���ַ���ʾ������һ�еĵ�һ�����ص�*/
		Xaddress += (LCD_PIXEL_WIDTH - macWIDTH_CH_CHAR);
		
	}
}


/**
 * @brief  ����ʾ������ʾ��Ӣ���ַ���,����Һ������ʱ���Զ����С�
					 ��Ӣ�Ļ���ʾʱ�����Ӣ����������ΪFont16x24��ʽ
 * @param  Line ����(Ҳ������Ϊy����)
 * @param  Column ���У�Ҳ������Ϊx���꣩
 * @param  pStr ��Ҫ��ʾ���ַ������׵�ַ
 * @retval ��
 */
void LCD_DispString_EN_CH( uint16_t Line, uint16_t Column, const uint8_t * pStr )
{
	uint16_t usCh;
	
	
	while( * pStr != '\0' )
	{
		if ( * pStr <= 126 )	           	//Ӣ���ַ�
		{
	
			/*�Զ�����*/
			if ( ( Column + LCD_Currentfonts->Width ) > LCD_PIXEL_WIDTH )
			{
				Column = 0;
				Line += LCD_Currentfonts->Height;
			}
			
			if ( ( Line + LCD_Currentfonts->Height ) > LCD_PIXEL_HEIGHT )
			{
				Column = 0;
				Line = 0;
			}			
					
			LCD_DisplayChar(Line,Column,*pStr);
			
			Column += LCD_Currentfonts->Width;
		
		  pStr ++;

		}
		
		else	                            //�����ַ�
		{
			if ( ( Column + macWIDTH_CH_CHAR ) > LCD_PIXEL_WIDTH )
			{
				Column = 0;
				Line += macHEIGHT_CH_CHAR;
			}
			
			if ( ( Line + macHEIGHT_CH_CHAR ) > LCD_PIXEL_HEIGHT )
			{
				Column = 0;
				Line = 0;
			}	
			
			/*һ���������ֽ�*/
			usCh = * ( uint16_t * ) pStr;				
			usCh = ( usCh << 8 ) + ( usCh >> 8 );		

			LCD_DispChar_CH (Line,Column, usCh);
			
			Column += macWIDTH_CH_CHAR;
			
			pStr += 2;           //һ�����������ֽ� 
		
    }
		
  }
	
	
} 

/**
  * @brief  ��ʾһ���ַ���������Һ�����ȣ����Զ����С�
						��Ӣ����ʱ�����Ӣ����������ΪFont16x24��ʽ
  * @param  Line: Ҫ��ʾ���б��LINE(0) - LINE(N)
  * @param  *ptr: Ҫ��ʾ���ַ���ָ��
  * @retval None
  */
void LCD_DisplayStringLine_EN_CH(uint16_t Line, uint8_t *ptr)
{  
  uint16_t refcolumn = 0;
  /* Send the string character by character on lCD */
  while ((refcolumn < LCD_PIXEL_WIDTH) && ((*ptr != 0) & (((refcolumn + LCD_Currentfonts->Width) & 0xFFFF) >= LCD_Currentfonts->Width)))
  {
    /* Display one character on LCD */
		if ( * ptr <= 126 )	           	//Ӣ���ַ�
		{
					
			LCD_DisplayChar(Line, refcolumn, *ptr);
			/* Decrement the column position by width */
			refcolumn += LCD_Currentfonts->Width;
			/* Point on the next character */
			ptr++;
		}
		
		else	                            //�����ַ�
		{	
			uint16_t usCh;
			
			/*һ���������ֽ�*/
			usCh = * ( uint16_t * ) ptr;				
			usCh = ( usCh << 8 ) + ( usCh >> 8 );		
			
			LCD_DispChar_CH ( Line, refcolumn, usCh );
			refcolumn += macWIDTH_CH_CHAR;

			ptr += 2; 		
    }	
  }
}

/**
 * @brief  ������ģ�����ź����ģ��1�����ص���8������λ����ʾ
										0x01��ʾ�ʼ���0x00��ʾ�հ���
 * @param  in_width ��ԭʼ�ַ�����
 * @param  in_heig ��ԭʼ�ַ��߶�
 * @param  out_width �����ź���ַ�����
 * @param  out_heig�����ź���ַ��߶�
 * @param  in_ptr ���ֿ�����ָ��	ע�⣺1pixel 1bit
 * @param  out_ptr �����ź���ַ����ָ�� ע��: 1pixel 8bit
 *		out_ptrʵ����û������������ĳ���ֱ�������ȫ��ָ��zoomBuff��
 * @param  en_cn ��0ΪӢ�ģ�1Ϊ����
 * @retval ��
 */
void LCD_zoomChar(uint16_t in_width,	//ԭʼ�ַ�����
									uint16_t in_heig,		//ԭʼ�ַ��߶�
									uint16_t out_width,	//���ź���ַ�����
									uint16_t out_heig,	//���ź���ַ��߶�
									uint8_t *in_ptr,	//�ֿ�����ָ��	ע�⣺1pixel 1bit
									uint8_t *out_ptr, //���ź���ַ����ָ�� ע��: 1pixel 8bit
									uint8_t en_cn)		//0ΪӢ�ģ�1Ϊ����	
{
	uint8_t *pts,*ots;
	//����Դ��ģ��Ŀ����ģ��С���趨����������ӣ�����16��Ϊ�˰Ѹ�������ת�ɶ�������
	unsigned int xrIntFloat_16=(in_width<<16)/out_width+1; 
  unsigned int yrIntFloat_16=(in_heig<<16)/out_heig+1;
	
	unsigned int srcy_16=0;
	unsigned int y,x;
	uint8_t *pSrcLine;
	uint8_t tempBuff[1024] = {0};
	u32			uChar;
	u16			charBit = in_width / 8;
	u16			Bitdiff = 32 - in_width;
	
	//�������Ƿ�Ϸ�
	if(in_width >= 32) return;												//�ֿⲻ��������32����
	if(in_width * in_heig == 0) return;	
	if(in_width * in_heig >= 1024 ) return; 					//����������� 32*32
	
	if(out_width * out_heig == 0) return;	
	if(out_width * out_heig >= ZOOMMAXBUFF ) return; //����������� 128*128
	pts = (uint8_t*)&tempBuff;
	
	//Ϊ�������㣬�ֿ��������1 pixel 1bit ӳ�䵽1pixel 8bit
	//0x01��ʾ�ʼ���0x00��ʾ�հ���
	if(en_cn == 0x00)//Ӣ��
	{
		//������16 * 24�ֿ���Ϊ���ԣ�������С���ֿ����и������д�������ӳ��Ϳ���
		//Ӣ�ĺ������ֿ����±߽粻�ԣ����ڴ˴ε�������Ҫע��tempBuff��ֹ���
			pts+=in_width*4;
			for(y=0;y<in_heig;y++)	
			{
				uChar = *(u32 *)(in_ptr + y * charBit) >> Bitdiff;
				for(x=0;x<in_width;x++)
					{
						*pts++ = (uChar >> x) & 0x01;
					}
			}		
	}
	else //����
	{
			for(y=0;y<in_heig;y++)	
			{
				/*Դ��ģ����*/
				uChar = in_ptr [ y * 3 ];
				uChar = ( uChar << 8 );
				uChar |= in_ptr [ y * 3 + 1 ];
				uChar = ( uChar << 8 );
				uChar |= in_ptr [ y * 3 + 2];
				/*ӳ��*/
				for(x=0;x<in_width;x++)
					{
						if(((uChar << x) & 0x800000) == 0x800000)
							*pts++ = 0x01;
						else
							*pts++ = 0x00;
					}
			}		
	}

	//zoom����
	pts = (uint8_t*)&tempBuff;	//ӳ����Դ����ָ��
	ots = (uint8_t*)&zoomBuff;	//������ݵ�ָ��
	for (y=0;y<out_heig;y++)	/*�б���*/
    {
				unsigned int srcx_16=0;
        pSrcLine=pts+in_width*(srcy_16>>16);				
        for (x=0;x<out_width;x++) /*�������ر���*/
        {
            ots[x]=pSrcLine[srcx_16>>16]; //��Դ��ģ���ݸ��Ƶ�Ŀ��ָ����
            srcx_16+=xrIntFloat_16;			//������ƫ��Դ���ص�
        }
        srcy_16+=yrIntFloat_16;				  //������ƫ��Դ���ص�
        ots+=out_width;						
    }
	/*���������ź����ģ����ֱ�Ӵ洢��ȫ��ָ��zoomBuff����*/
	out_ptr = (uint8_t*)&zoomBuff;	//out_ptrû����ȷ�������������ֱ�Ӹĳ���ȫ�ֱ���ָ�룡
	
	/*ʵ�������ʹ��out_ptr����Ҫ������һ�䣡����
		ֻ����Ϊout_ptrû��ʹ�ã��ᵼ��warning��ǿ��֢*/
	out_ptr++; 
}			

/**
 * @brief  �������ź����ģ��ʾ�ַ�
 * @param  Xpos ���ַ���ʾλ��x
 * @param  Ypos ���ַ���ʾλ��y
 * @param  Font_width ���ַ�����
 * @param  Font_Heig���ַ��߶�
 * @param  c ��Ҫ��ʾ����ģ����
 * @param  DrawModel ���Ƿ�ɫ��ʾ 
 * @retval ��
 */
void LCD_DrawChar_Ex(uint16_t Xpos, //�ַ���ʾλ��x
												uint16_t Ypos, //�ַ���ʾλ��y
												uint16_t Font_width, //�ַ�����
												uint16_t Font_Heig,  //�ַ��߶� 
												uint8_t *c,						//��ģ����
												uint16_t DrawModel)		//�Ƿ�ɫ��ʾ
{
  uint32_t index = 0, counter = 0, xpos =0;
  uint32_t  Xaddress = 0;
  
  xpos = Xpos*LCD_PIXEL_WIDTH*2;
  Xaddress += Ypos;
  
  for(index = 0; index < Font_Heig; index++)
  {
    
    for(counter = 0; counter < Font_width; counter++)
    {
      if(*c++ == DrawModel)	//������ģ����ɫ���þ�����ʾ������ɫ
      {
				*(__IO uint16_t*) (CurrentFrameBuffer + (2*Xaddress) + xpos) = CurrentBackColor;

      }
      else
      {
				*(__IO uint16_t*) (CurrentFrameBuffer + (2*Xaddress) + xpos) = CurrentTextColor;

      }
      Xaddress++;
    }
      Xaddress += (LCD_PIXEL_WIDTH - Font_width);
  }
}

/**
 * @brief  �������ź����ģ��ʾ�ַ���
 * @param  Xpos ���ַ���ʾλ��x
 * @param  Ypos ���ַ���ʾλ��y
 * @param  Font_width ���ַ����ȣ�Ӣ���ַ��ڴ˻�����/2��ע��Ϊż��
 * @param  Font_Heig���ַ��߶ȣ�ע��Ϊż��
 * @param  c ��Ҫ��ʾ���ַ���
 * @param  DrawModel ���Ƿ�ɫ��ʾ 
 * @retval ��
 */
void LCD_DisplayStringLineEx(uint16_t x, 		//�ַ���ʾλ��x
														 uint16_t y, 				//�ַ���ʾλ��y
														 uint16_t Font_width,	//Ҫ��ʾ��������ȣ�Ӣ���ַ��ڴ˻�����/2��ע��Ϊż��
														 uint16_t Font_Heig,	//Ҫ��ʾ������߶ȣ�ע��Ϊż��
														 uint8_t *ptr,					//��ʾ���ַ�����
														 uint16_t DrawModel)  //�Ƿ�ɫ��ʾ
{
	uint16_t refcolumn = x; //x����
	uint16_t Charwidth;
	uint8_t *psr;
	uint8_t Ascii;	//Ӣ��
	uint16_t usCh;  //����
	uint8_t ucBuffer [ 24*24/8 ];	
	
	while ((refcolumn < LCD_PIXEL_WIDTH) && ((*ptr != 0) & (((refcolumn + LCD_Currentfonts->Width) & 0xFFFF) >= LCD_Currentfonts->Width)))
	{
		if(*ptr > 0x80) //���������
		{
			Charwidth = Font_width;
			usCh = * ( uint16_t * ) ptr;				
			usCh = ( usCh << 8 ) + ( usCh >> 8 );
			macGetGBKCode ( ucBuffer, usCh );	//ȡ��ģ����
			//������ģ����
			LCD_zoomChar(24,24,Charwidth,Font_Heig,(uint8_t *)&ucBuffer,psr,1); 
			//��ʾ�����ַ�
			LCD_DrawChar_Ex(y,refcolumn,Charwidth,Font_Heig,(uint8_t*)&zoomBuff,DrawModel);
			refcolumn+=Charwidth;
			ptr+=2;
		}
		else
		{
				Charwidth = Font_width / 2;
				Ascii = *ptr - 32;
				//������ģ����
				LCD_zoomChar(16,24,Charwidth,Font_Heig,(uint8_t *)&LCD_Currentfonts->table[Ascii * LCD_Currentfonts->Height],psr,0);
			  //��ʾ�����ַ�
				LCD_DrawChar_Ex(y,refcolumn,Charwidth,Font_Heig,(uint8_t*)&zoomBuff,DrawModel);
				refcolumn+=Charwidth;
				ptr++;
		}
	}
}

/**
 * @brief  Sets a display window
 * @param  Xpos: specifies the X bottom left position from 0 to 240.
 * @param  Ypos: specifies the Y bottom left position from 0 to 320.
 * @param  Height: display window height, can be a value from 0 to 320.
 * @param  Width: display window width, can be a value from 0 to 240.
 * @retval None
 */
void LCD_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width)
{

 if (CurrentLayer == LCD_BACKGROUND_LAYER)
 {
   /* reconfigure the layer1 position */
   LTDC_LayerPosition(LTDC_Layer1, Xpos, Ypos);
   LTDC_ReloadConfig(LTDC_IMReload);

   /* reconfigure the layer1 size */
   LTDC_LayerSize(LTDC_Layer1, Width, Height);
   LTDC_ReloadConfig(LTDC_IMReload);
}
else
{
   /* reconfigure the layer2 position */
   LTDC_LayerPosition(LTDC_Layer2, Xpos, Ypos);
   LTDC_ReloadConfig(LTDC_IMReload);

  /* reconfigure the layer2 size */
   LTDC_LayerSize(LTDC_Layer2, Width, Height);
   LTDC_ReloadConfig(LTDC_IMReload);
 }
}

/**
 * @brief  Disables LCD Window mode.
 * @param  None
 * @retval None
 */
void LCD_WindowModeDisable(void)
{
 LCD_SetDisplayWindow(0, 0, LCD_PIXEL_HEIGHT, LCD_PIXEL_WIDTH);
}

/**
 * @brief  Displays a line.
 * @param Xpos: specifies the X position, can be a value from 0 to 240.
 * @param Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param Length: line length.
 * @param Direction: line direction.
 *   This parameter can be one of the following values: LCD_DIR_HORIZONTAL or LCD_DIR_VERTICAL.
 * @retval None
 */
void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction)
{
 DMA2D_InitTypeDef      DMA2D_InitStruct;

 uint32_t  Xaddress = 0;
 uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0;

 Xaddress = CurrentFrameBuffer + 2*(LCD_PIXEL_WIDTH*Ypos + Xpos);

 Red_Value = (0xF800 & CurrentTextColor) >> 11;
 Blue_Value = 0x001F & CurrentTextColor;
 Green_Value = (0x07E0 & CurrentTextColor) >> 5;

 /* Configure DMA2D */
 DMA2D_DeInit();
 DMA2D_InitStruct.DMA2D_Mode = DMA2D_R2M;
 DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB565;
 DMA2D_InitStruct.DMA2D_OutputGreen = Green_Value;
 DMA2D_InitStruct.DMA2D_OutputBlue = Blue_Value;
 DMA2D_InitStruct.DMA2D_OutputRed = Red_Value;
 DMA2D_InitStruct.DMA2D_OutputAlpha = 0x0F;
 DMA2D_InitStruct.DMA2D_OutputMemoryAdd = Xaddress;

 if(Direction == LCD_DIR_HORIZONTAL)
 {
   DMA2D_InitStruct.DMA2D_OutputOffset = 0;
   DMA2D_InitStruct.DMA2D_NumberOfLine = 1;
   DMA2D_InitStruct.DMA2D_PixelPerLine = Length;
 }
 else
 {
   DMA2D_InitStruct.DMA2D_OutputOffset = LCD_PIXEL_WIDTH - 1;
   DMA2D_InitStruct.DMA2D_NumberOfLine = Length;
   DMA2D_InitStruct.DMA2D_PixelPerLine = 1;
 }

 DMA2D_Init(&DMA2D_InitStruct);
 /* Start Transfer */
 DMA2D_StartTransfer();
 /* Wait for CTC Flag activation */
 while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET)
 {
 }

}

/**
 * @brief  Displays a rectangle.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Height: display rectangle height, can be a value from 0 to 320.
 * @param  Width: display rectangle width, can be a value from 0 to 240.
 * @retval None
 */
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
 /* draw horizontal lines */
 LCD_DrawLine(Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);
 LCD_DrawLine(Xpos, (Ypos+ Height), Width, LCD_DIR_HORIZONTAL);

 /* draw vertical lines */
 LCD_DrawLine(Xpos, Ypos, Height, LCD_DIR_VERTICAL);
 LCD_DrawLine((Xpos + Width), Ypos, Height, LCD_DIR_VERTICAL);
}

/**
 * @brief  Draw a circle.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Radius: radius of the circle.
 * @retval None
 */
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
   int x = -Radius, y = 0, err = 2-2*Radius, e2;
   do {
       *(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos-x) + LCD_PIXEL_WIDTH*(Ypos+y)))) = CurrentTextColor;
       *(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos+x) + LCD_PIXEL_WIDTH*(Ypos+y)))) = CurrentTextColor;
       *(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos+x) + LCD_PIXEL_WIDTH*(Ypos-y)))) = CurrentTextColor;
       *(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos-x) + LCD_PIXEL_WIDTH*(Ypos-y)))) = CurrentTextColor;

       e2 = err;
       if (e2 <= y) {
           err += ++y*2+1;
           if (-x == y && e2 <= x) e2 = 0;
       }
       if (e2 > x) err += ++x*2+1;
   }
   while (x <= 0);
}

/**
 * @brief  Draw a full ellipse.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Radius: minor radius of ellipse.
 * @param  Radius2: major radius of ellipse.
 * @retval None
 */
void LCD_DrawFullEllipse(int Xpos, int Ypos, int Radius, int Radius2)
{
 int x = -Radius, y = 0, err = 2-2*Radius, e2;
 float K = 0, rad1 = 0, rad2 = 0;

 rad1 = Radius;
 rad2 = Radius2;

 if (Radius > Radius2)
 {
   do
   {
     K = (float)(rad1/rad2);
     LCD_DrawLine((Xpos+x), (Ypos-(uint16_t)(y/K)), (2*(uint16_t)(y/K) + 1), LCD_DIR_VERTICAL);
     LCD_DrawLine((Xpos-x), (Ypos-(uint16_t)(y/K)), (2*(uint16_t)(y/K) + 1), LCD_DIR_VERTICAL);

     e2 = err;
     if (e2 <= y)
     {
       err += ++y*2+1;
       if (-x == y && e2 <= x) e2 = 0;
     }
     if (e2 > x) err += ++x*2+1;

   }
   while (x <= 0);
 }
 else
 {
   y = -Radius2;
   x = 0;
   do
   {
     K = (float)(rad2/rad1);
     LCD_DrawLine((Xpos-(uint16_t)(x/K)), (Ypos+y), (2*(uint16_t)(x/K) + 1), LCD_DIR_HORIZONTAL);
     LCD_DrawLine((Xpos-(uint16_t)(x/K)), (Ypos-y), (2*(uint16_t)(x/K) + 1), LCD_DIR_HORIZONTAL);

     e2 = err;
     if (e2 <= x)
     {
       err += ++x*2+1;
       if (-y == x && e2 <= y) e2 = 0;
     }
     if (e2 > y) err += ++y*2+1;
   }
   while (y <= 0);
 }
}

/**
 * @brief  Displays an Ellipse.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Radius: specifies Radius.
 * @param  Radius2: specifies Radius2.
 * @retval None
 */
void LCD_DrawEllipse(int Xpos, int Ypos, int Radius, int Radius2)
{
 int x = -Radius, y = 0, err = 2-2*Radius, e2;
 float K = 0, rad1 = 0, rad2 = 0;

 rad1 = Radius;
 rad2 = Radius2;

 if (Radius > Radius2)
 {
   do {
     K = (float)(rad1/rad2);
     *(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos-x) + LCD_PIXEL_WIDTH*(Ypos+(uint16_t)(y/K))))) = CurrentTextColor;
     *(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos+x) + LCD_PIXEL_WIDTH*(Ypos+(uint16_t)(y/K))))) = CurrentTextColor;
     *(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos+x) + LCD_PIXEL_WIDTH*(Ypos-(uint16_t)(y/K))))) = CurrentTextColor;
     *(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos-x) + LCD_PIXEL_WIDTH*(Ypos-(uint16_t)(y/K))))) = CurrentTextColor;

     e2 = err;
     if (e2 <= y) {
       err += ++y*2+1;
       if (-x == y && e2 <= x) e2 = 0;
     }
     if (e2 > x) err += ++x*2+1;
   }
   while (x <= 0);
 }
 else
 {
   y = -Radius2;
   x = 0;
   do {
     K = (float)(rad2/rad1);
     *(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos-(uint16_t)(x/K)) + LCD_PIXEL_WIDTH*(Ypos+y)))) = CurrentTextColor;
     *(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos+(uint16_t)(x/K)) + LCD_PIXEL_WIDTH*(Ypos+y)))) = CurrentTextColor;
     *(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos+(uint16_t)(x/K)) + LCD_PIXEL_WIDTH*(Ypos-y)))) = CurrentTextColor;
     *(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos-(uint16_t)(x/K)) + LCD_PIXEL_WIDTH*(Ypos-y)))) = CurrentTextColor;

     e2 = err;
     if (e2 <= x) {
       err += ++x*2+1;
       if (-y == x && e2 <= y) e2 = 0;
     }
     if (e2 > y) err += ++y*2+1;
   }
   while (y <= 0);
 }
}

/**
 * @brief  Displays a mono-color picture.
 * @param  Pict: pointer to the picture array.
 * @retval None
 */
void LCD_DrawMonoPict(const uint32_t *Pict)
{
 uint32_t index = 0, counter = 0;


 for(index = 0; index < 2400; index++)
 {
   for(counter = 0; counter < 32; counter++)
   {
     if((Pict[index] & (1 << counter)) == 0x00)
     {
       *(__IO uint16_t*)(CurrentFrameBuffer) = CurrentBackColor;
     }
     else
     {
       *(__IO uint16_t*)(CurrentFrameBuffer) = CurrentTextColor;
     }
   }
 }
}

/**
 * @brief  Displays a bitmap picture loaded in the internal Flash.
 * @param  BmpAddress: Bmp picture address in the internal Flash.
 * @retval None
 */
void LCD_WriteBMP(uint32_t BmpAddress)
{
 uint32_t bisize=0,index = 0, size = 0, width = 0, height = 0, bit_pixel = 0,picsize=0;
 uint32_t Address;
 uint32_t currentline = 0, linenumber = 0;
 uint16_t data=0;
 
 Address = CurrentFrameBuffer;

 /* Read bitmap size */
 size = *(__IO uint16_t *) (BmpAddress + 2);
 size |= (*(__IO uint16_t *) (BmpAddress + 4)) << 16;	
 printf("bmp->size: %d\n",size);
	
 data= *(__IO uint16_t *) (BmpAddress + size-2);
	printf("bmp->data: %04X\n",data);
	
 /* Get bitmap data address offset */
 index = *(__IO uint16_t *) (BmpAddress + 10);
 index |= (*(__IO uint16_t *) (BmpAddress + 12)) << 16;
 printf("bmp->index: %d\n",index);
	
 /* Read bisize */
 bisize = *(uint16_t *) (BmpAddress + 14);
 bisize |= (*(uint16_t *) (BmpAddress + 16)) << 16;
 printf("bmp->bisize: %d\n",bisize);
	
 /* Read bitmap width */
 width = *(uint16_t *) (BmpAddress + 18);
 width |= (*(uint16_t *) (BmpAddress + 20)) << 16;
 printf("bmp->width: %d\n",width);
	
 /* Read bitmap height */
 height = *(uint16_t *) (BmpAddress + 22);
 height |= (*(uint16_t *) (BmpAddress + 24)) << 16;
 printf("bmp->height: %d\n",height);
 
 /* Read bit/pixel */
 bit_pixel = *(uint16_t *) (BmpAddress + 28);
 printf("bmp->bit_pixel: %d\n",bit_pixel);
 
 /* Read bitmap height */
 picsize = *(uint16_t *) (BmpAddress + 34);
 picsize |= (*(uint16_t *) (BmpAddress + 36)) << 16;
 printf("bmp->picsize: %d\n",picsize);
 
 if (CurrentLayer == LCD_BACKGROUND_LAYER)
 {
   /* reconfigure layer size in accordance with the picture */
   LTDC_LayerSize(LTDC_Layer1, width, height);
   LTDC_ReloadConfig(LTDC_VBReload);

   /* Reconfigure the Layer pixel format in accordance with the picture */
   if ((bit_pixel/8) == 4)
   {
     LTDC_LayerPixelFormat(LTDC_Layer1, LTDC_Pixelformat_ARGB8888);
     LTDC_ReloadConfig(LTDC_VBReload);
   }
   else if ((bit_pixel/8) == 2)
   {
	 if((bisize==56)&& (data==0))
	   LTDC_LayerPixelFormat(LTDC_Layer1, LTDC_Pixelformat_RGB565);
	 else
       LTDC_LayerPixelFormat(LTDC_Layer1, LTDC_Pixelformat_ARGB1555);
     LTDC_ReloadConfig(LTDC_VBReload);
   }
   else
   {
     LTDC_LayerPixelFormat(LTDC_Layer1, LTDC_Pixelformat_RGB888);
     LTDC_ReloadConfig(LTDC_VBReload);
   }
 }
 else
 {
   /* reconfigure layer size in accordance with the picture */
   LTDC_LayerSize(LTDC_Layer2, width, height);
   LTDC_ReloadConfig(LTDC_VBReload);

   /* Reconfigure the Layer pixel format in accordance with the picture */
   if ((bit_pixel/8) == 4)
   {
     LTDC_LayerPixelFormat(LTDC_Layer2, LTDC_Pixelformat_ARGB8888);
     LTDC_ReloadConfig(LTDC_VBReload);
   }
   else if ((bit_pixel/8) == 2)
   {
     if((bisize==56)&& (data==0))
		LTDC_LayerPixelFormat(LTDC_Layer2, LTDC_Pixelformat_RGB565);
	  else
        LTDC_LayerPixelFormat(LTDC_Layer2, LTDC_Pixelformat_ARGB1555);
     LTDC_ReloadConfig(LTDC_VBReload);
   }
   else
   {
     LTDC_LayerPixelFormat(LTDC_Layer2, LTDC_Pixelformat_RGB888);
     LTDC_ReloadConfig(LTDC_VBReload);
   }
 }
 /* compute the real size of the picture (without the header)) */
 if(picsize && (data==0))
	size = (size - index-2);
 else
	 size = (size - index);

 /* bypass the bitmap header */
 BmpAddress += index;

 /* start copie image from the bottom */
 Address += width*(height-1)*(bit_pixel/8);

 for(index = 0; index < size; index++)
 {
   *(__IO uint8_t*) (Address) = *(__IO uint8_t *)BmpAddress;
   /*jump on next byte */
   BmpAddress++;
   Address++;
   currentline++;

   if((currentline/(bit_pixel/8)) == width)
   {
     if(linenumber < height)
     {
       linenumber++;
       Address -=(2*width*(bit_pixel/8));
       currentline = 0;
     }
   }
 }
}

/**
 * @brief  Displays a full rectangle.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Height: rectangle height.
 * @param  Width: rectangle width.
 * @retval None
 */
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
 DMA2D_InitTypeDef      DMA2D_InitStruct;

 uint32_t  Xaddress = 0;
 uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0;

 Red_Value = (0xF800 & CurrentTextColor) >> 11;
 Blue_Value = 0x001F & CurrentTextColor;
 Green_Value = (0x07E0 & CurrentTextColor) >> 5;

 Xaddress = CurrentFrameBuffer + 2*(LCD_PIXEL_WIDTH*Ypos + Xpos);

 /* configure DMA2D */
 DMA2D_DeInit();
 DMA2D_InitStruct.DMA2D_Mode = DMA2D_R2M;
 DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB565;
 DMA2D_InitStruct.DMA2D_OutputGreen = Green_Value;
 DMA2D_InitStruct.DMA2D_OutputBlue = Blue_Value;
 DMA2D_InitStruct.DMA2D_OutputRed = Red_Value;
 DMA2D_InitStruct.DMA2D_OutputAlpha = 0x0F;
 DMA2D_InitStruct.DMA2D_OutputMemoryAdd = Xaddress;
 DMA2D_InitStruct.DMA2D_OutputOffset = (LCD_PIXEL_WIDTH - Width);
 DMA2D_InitStruct.DMA2D_NumberOfLine = Height;
 DMA2D_InitStruct.DMA2D_PixelPerLine = Width;
 DMA2D_Init(&DMA2D_InitStruct);

 /* Start Transfer */
 DMA2D_StartTransfer();

 /* Wait for CTC Flag activation */
 while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET)
 {
 }

 LCD_SetTextColor(CurrentTextColor);
}

/**
 * @brief  Displays a full circle.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Radius
 * @retval None
 */
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
 int32_t  D;    /* Decision Variable */
 uint32_t  CurX;/* Current X Value */
 uint32_t  CurY;/* Current Y Value */

 D = 3 - (Radius << 1);

 CurX = 0;
 CurY = Radius;

 while (CurX <= CurY)
 {
   if(CurY > 0)
   {
     LCD_DrawLine(Xpos - CurX, Ypos - CurY, 2*CurY, LCD_DIR_VERTICAL);
     LCD_DrawLine(Xpos + CurX, Ypos - CurY, 2*CurY, LCD_DIR_VERTICAL);
   }

   if(CurX > 0)
   {
     LCD_DrawLine(Xpos - CurY, Ypos - CurX, 2*CurX, LCD_DIR_VERTICAL);
     LCD_DrawLine(Xpos + CurY, Ypos - CurX, 2*CurX, LCD_DIR_VERTICAL);
   }
   if (D < 0)
   {
     D += (CurX << 2) + 6;
   }
   else
   {
     D += ((CurX - CurY) << 2) + 10;
     CurY--;
   }
   CurX++;
 }

 LCD_DrawCircle(Xpos, Ypos, Radius);
}

/**
 * @brief  Displays an uni-line (between two points).
 * @param  x1: specifies the point 1 x position.
 * @param  y1: specifies the point 1 y position.
 * @param  x2: specifies the point 2 x position.
 * @param  y2: specifies the point 2 y position.
 * @retval None
 */
void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
 int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
 yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
 curpixel = 0;

 deltax = ABS(x2 - x1);        /* The difference between the x's */
 deltay = ABS(y2 - y1);        /* The difference between the y's */
 x = x1;                       /* Start x off at the first pixel */
 y = y1;                       /* Start y off at the first pixel */

 if (x2 >= x1)                 /* The x-values are increasing */
 {
   xinc1 = 1;
   xinc2 = 1;
 }
 else                          /* The x-values are decreasing */
 {
   xinc1 = -1;
   xinc2 = -1;
 }

 if (y2 >= y1)                 /* The y-values are increasing */
 {
   yinc1 = 1;
   yinc2 = 1;
 }
 else                          /* The y-values are decreasing */
 {
   yinc1 = -1;
   yinc2 = -1;
 }

 if (deltax >= deltay)         /* There is at least one x-value for every y-value */
 {
   xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
   yinc2 = 0;                  /* Don't change the y for every iteration */
   den = deltax;
   num = deltax / 2;
   numadd = deltay;
   numpixels = deltax;         /* There are more x-values than y-values */
 }
 else                          /* There is at least one y-value for every x-value */
 {
   xinc2 = 0;                  /* Don't change the x for every iteration */
   yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
   den = deltay;
   num = deltay / 2;
   numadd = deltax;
   numpixels = deltay;         /* There are more y-values than x-values */
 }

 for (curpixel = 0; curpixel <= numpixels; curpixel++)
 {
   PutPixel(x, y);             /* Draw the current pixel */
   num += numadd;              /* Increase the numerator by the top of the fraction */
   if (num >= den)             /* Check if numerator >= denominator */
   {
     num -= den;               /* Calculate the new numerator value */
     x += xinc1;               /* Change the x as appropriate */
     y += yinc1;               /* Change the y as appropriate */
   }
   x += xinc2;                 /* Change the x as appropriate */
   y += yinc2;                 /* Change the y as appropriate */
 }
}

/**
 * @brief  Displays an triangle.
 * @param  Points: pointer to the points array.
 * @retval None
 */
void LCD_Triangle(pPoint Points, uint16_t PointCount)
{
 int16_t X = 0, Y = 0;
 pPoint First = Points;

 if(PointCount != 3)
 {
   return;
 }

 while(--PointCount)
 {
   X = Points->X;
   Y = Points->Y;
   Points++;
   LCD_DrawUniLine(X, Y, Points->X, Points->Y);
 }
 LCD_DrawUniLine(First->X, First->Y, Points->X, Points->Y);
}

/**
 * @brief  Fill an triangle (between 3 points).
 * @param  x1..3: x position of triangle point 1..3.
 * @param  y1..3: y position of triangle point 1..3.
 * @retval None
 */
void LCD_FillTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3)
{

 int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
 yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
 curpixel = 0;

 deltax = ABS(x2 - x1);        /* The difference between the x's */
 deltay = ABS(y2 - y1);        /* The difference between the y's */
 x = x1;                       /* Start x off at the first pixel */
 y = y1;                       /* Start y off at the first pixel */

 if (x2 >= x1)                 /* The x-values are increasing */
 {
   xinc1 = 1;
   xinc2 = 1;
 }
 else                          /* The x-values are decreasing */
 {
   xinc1 = -1;
   xinc2 = -1;
 }

 if (y2 >= y1)                 /* The y-values are increasing */
 {
   yinc1 = 1;
   yinc2 = 1;
 }
 else                          /* The y-values are decreasing */
 {
   yinc1 = -1;
   yinc2 = -1;
 }

 if (deltax >= deltay)         /* There is at least one x-value for every y-value */
 {
   xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
   yinc2 = 0;                  /* Don't change the y for every iteration */
   den = deltax;
   num = deltax / 2;
   numadd = deltay;
   numpixels = deltax;         /* There are more x-values than y-values */
 }
 else                          /* There is at least one y-value for every x-value */
 {
   xinc2 = 0;                  /* Don't change the x for every iteration */
   yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
   den = deltay;
   num = deltay / 2;
   numadd = deltax;
   numpixels = deltay;         /* There are more y-values than x-values */
 }

 for (curpixel = 0; curpixel <= numpixels; curpixel++)
 {
   LCD_DrawUniLine(x, y, x3, y3);

   num += numadd;              /* Increase the numerator by the top of the fraction */
   if (num >= den)             /* Check if numerator >= denominator */
   {
     num -= den;               /* Calculate the new numerator value */
     x += xinc1;               /* Change the x as appropriate */
     y += yinc1;               /* Change the y as appropriate */
   }
   x += xinc2;                 /* Change the x as appropriate */
   y += yinc2;                 /* Change the y as appropriate */
 }


}
/**
 * @brief  Displays an poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @retval None
 */
void LCD_PolyLine(pPoint Points, uint16_t PointCount)
{
 int16_t X = 0, Y = 0;

 if(PointCount < 2)
 {
   return;
 }

 while(--PointCount)
 {
   X = Points->X;
   Y = Points->Y;
   Points++;
   LCD_DrawUniLine(X, Y, Points->X, Points->Y);
 }
}

/**
 * @brief  Displays an relative poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @param  Closed: specifies if the draw is closed or not.
 *           1: closed, 0 : not closed.
 * @retval None
 */
static void LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount, uint16_t Closed)
{
 int16_t X = 0, Y = 0;
 pPoint First = Points;

 if(PointCount < 2)
 {
   return;
 }
 X = Points->X;
 Y = Points->Y;
 while(--PointCount)
 {
   Points++;
   LCD_DrawUniLine(X, Y, X + Points->X, Y + Points->Y);
   X = X + Points->X;
   Y = Y + Points->Y;
 }
 if(Closed)
 {
   LCD_DrawUniLine(First->X, First->Y, X, Y);
 }
}

/**
 * @brief  Displays a closed poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @retval None
 */
void LCD_ClosedPolyLine(pPoint Points, uint16_t PointCount)
{
 LCD_PolyLine(Points, PointCount);
 LCD_DrawUniLine(Points->X, Points->Y, (Points+PointCount-1)->X, (Points+PointCount-1)->Y);
}

/**
 * @brief  Displays a relative poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @retval None
 */
void LCD_PolyLineRelative(pPoint Points, uint16_t PointCount)
{
 LCD_PolyLineRelativeClosed(Points, PointCount, 0);
}

/**
 * @brief  Displays a closed relative poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @retval None
 */
void LCD_ClosedPolyLineRelative(pPoint Points, uint16_t PointCount)
{
 LCD_PolyLineRelativeClosed(Points, PointCount, 1);
}

/**
 * @brief  Displays a  full poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @retval None
 */
void LCD_FillPolyLine(pPoint Points, uint16_t PointCount)
{

 int16_t X = 0, Y = 0, X2 = 0, Y2 = 0, X_center = 0, Y_center = 0, X_first = 0, Y_first = 0, pixelX = 0, pixelY = 0, counter = 0;
 uint16_t  IMAGE_LEFT = 0, IMAGE_RIGHT = 0, IMAGE_TOP = 0, IMAGE_BOTTOM = 0;

 IMAGE_LEFT = IMAGE_RIGHT = Points->X;
 IMAGE_TOP= IMAGE_BOTTOM = Points->Y;

 for(counter = 1; counter < PointCount; counter++)
 {
   pixelX = POLY_X(counter);
   if(pixelX < IMAGE_LEFT)
   {
     IMAGE_LEFT = pixelX;
   }
   if(pixelX > IMAGE_RIGHT)
   {
     IMAGE_RIGHT = pixelX;
   }

   pixelY = POLY_Y(counter);
   if(pixelY < IMAGE_TOP)
   {
     IMAGE_TOP = pixelY;
   }
   if(pixelY > IMAGE_BOTTOM)
   {
     IMAGE_BOTTOM = pixelY;
   }
 }

 if(PointCount < 2)
 {
   return;
 }

 X_center = (IMAGE_LEFT + IMAGE_RIGHT)/2;
 Y_center = (IMAGE_BOTTOM + IMAGE_TOP)/2;

 X_first = Points->X;
 Y_first = Points->Y;

 while(--PointCount)
 {
   X = Points->X;
   Y = Points->Y;
   Points++;
   X2 = Points->X;
   Y2 = Points->Y;

   LCD_FillTriangle(X, X2, X_center, Y, Y2, Y_center);
   LCD_FillTriangle(X, X_center, X2, Y, Y_center, Y2);
   LCD_FillTriangle(X_center, X2, X, Y_center, Y2, Y);
 }

 LCD_FillTriangle(X_first, X2, X_center, Y_first, Y2, Y_center);
 LCD_FillTriangle(X_first, X_center, X2, Y_first, Y_center, Y2);
 LCD_FillTriangle(X_center, X2, X_first, Y_center, Y2, Y_first);
}




/**
 * @brief  Sets or reset LCD control lines.
 * @param  GPIOx: where x can be B or D to select the GPIO peripheral.
 * @param  CtrlPins: the Control line.
 *   This parameter can be:
 *     @arg LCD_NCS_PIN: Chip Select pin
 *     @arg LCD_NWR_PIN: Read/Write Selection pin
 *     @arg LCD_RS_PIN: Register/RAM Selection pin
 * @param  BitVal: specifies the value to be written to the selected bit.
 *   This parameter can be:
 *     @arg Bit_RESET: to clear the port pin
 *     @arg Bit_SET: to set the port pin
 * @retval None
 */
void LCD_CtrlLinesWrite(GPIO_TypeDef* GPIOx, uint16_t CtrlPins, BitAction BitVal)
{
 /* Set or Reset the control line */
 GPIO_WriteBit(GPIOx, (uint16_t)CtrlPins, (BitAction)BitVal);
}




static void LCD_GPIO_Config(void)
{
 GPIO_InitTypeDef GPIO_InitStruct;

 /* ʹ��LCDʹ�õ�������ʱ�� */
                         //��ɫ������
 RCC_AHB1PeriphClockCmd(LTDC_R0_GPIO_CLK | LTDC_R1_GPIO_CLK | LTDC_R2_GPIO_CLK|
                        LTDC_R3_GPIO_CLK | LTDC_R4_GPIO_CLK | LTDC_R5_GPIO_CLK|
                        LTDC_R6_GPIO_CLK | LTDC_R7_GPIO_CLK |
                         //��ɫ������
                         LTDC_G0_GPIO_CLK|LTDC_G1_GPIO_CLK|LTDC_G2_GPIO_CLK|
                         LTDC_G3_GPIO_CLK|LTDC_G4_GPIO_CLK|LTDC_G5_GPIO_CLK|
                         LTDC_G6_GPIO_CLK|LTDC_G7_GPIO_CLK|
                         //��ɫ������
                         LTDC_B0_GPIO_CLK|LTDC_B1_GPIO_CLK|LTDC_B2_GPIO_CLK|
                         LTDC_B3_GPIO_CLK|LTDC_B4_GPIO_CLK|LTDC_B5_GPIO_CLK|
                         LTDC_B6_GPIO_CLK|LTDC_B7_GPIO_CLK|
                         //�����ź���
                         LTDC_CLK_GPIO_CLK | LTDC_HSYNC_GPIO_CLK |LTDC_VSYNC_GPIO_CLK|
                         LTDC_DE_GPIO_CLK  | LTDC_BL_GPIO_CLK    |LTDC_DISP_GPIO_CLK ,ENABLE);


/* GPIO���� */

/* ��ɫ������ */
 GPIO_InitStruct.GPIO_Pin = LTDC_R0_GPIO_PIN;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
 GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

 GPIO_Init(LTDC_R0_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R0_GPIO_PORT, LTDC_R0_PINSOURCE, LTDC_R0_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_R1_GPIO_PIN;
 GPIO_Init(LTDC_R1_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R1_GPIO_PORT, LTDC_R1_PINSOURCE, LTDC_R1_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_R2_GPIO_PIN;
 GPIO_Init(LTDC_R2_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R2_GPIO_PORT, LTDC_R2_PINSOURCE, LTDC_R2_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_R3_GPIO_PIN;
 GPIO_Init(LTDC_R3_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R3_GPIO_PORT, LTDC_R3_PINSOURCE, LTDC_R3_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_R4_GPIO_PIN;
 GPIO_Init(LTDC_R4_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R4_GPIO_PORT, LTDC_R4_PINSOURCE, LTDC_R4_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_R5_GPIO_PIN;
 GPIO_Init(LTDC_R5_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R5_GPIO_PORT, LTDC_R5_PINSOURCE, LTDC_R5_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_R6_GPIO_PIN;
 GPIO_Init(LTDC_R6_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R6_GPIO_PORT, LTDC_R6_PINSOURCE, LTDC_R6_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_R7_GPIO_PIN;
 GPIO_Init(LTDC_R7_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R7_GPIO_PORT, LTDC_R7_PINSOURCE, LTDC_R7_AF);

 //��ɫ������
 GPIO_InitStruct.GPIO_Pin = LTDC_G0_GPIO_PIN;
 GPIO_Init(LTDC_G0_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G0_GPIO_PORT, LTDC_G0_PINSOURCE, LTDC_G0_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_G1_GPIO_PIN;
 GPIO_Init(LTDC_G1_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G1_GPIO_PORT, LTDC_G1_PINSOURCE, LTDC_G1_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_G2_GPIO_PIN;
 GPIO_Init(LTDC_G2_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G2_GPIO_PORT, LTDC_G2_PINSOURCE, LTDC_G2_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_G3_GPIO_PIN;
 GPIO_Init(LTDC_G3_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G3_GPIO_PORT, LTDC_G3_PINSOURCE, LTDC_G3_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_G4_GPIO_PIN;
 GPIO_Init(LTDC_G4_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G4_GPIO_PORT, LTDC_G4_PINSOURCE, LTDC_G4_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_G5_GPIO_PIN;
 GPIO_Init(LTDC_G5_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G5_GPIO_PORT, LTDC_G5_PINSOURCE, LTDC_G5_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_G6_GPIO_PIN;
 GPIO_Init(LTDC_G6_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G6_GPIO_PORT, LTDC_G6_PINSOURCE, LTDC_G6_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_G7_GPIO_PIN;
 GPIO_Init(LTDC_G7_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G7_GPIO_PORT, LTDC_G7_PINSOURCE, LTDC_G7_AF);

 //��ɫ������
 GPIO_InitStruct.GPIO_Pin = LTDC_B0_GPIO_PIN;
 GPIO_Init(LTDC_B0_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B0_GPIO_PORT, LTDC_B0_PINSOURCE, LTDC_B0_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_B1_GPIO_PIN;
 GPIO_Init(LTDC_B1_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B1_GPIO_PORT, LTDC_B1_PINSOURCE, LTDC_B1_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_B2_GPIO_PIN;
 GPIO_Init(LTDC_B2_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B2_GPIO_PORT, LTDC_B2_PINSOURCE, LTDC_B2_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_B3_GPIO_PIN;
 GPIO_Init(LTDC_B3_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B3_GPIO_PORT, LTDC_B3_PINSOURCE, LTDC_B3_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_B4_GPIO_PIN;
 GPIO_Init(LTDC_B4_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B4_GPIO_PORT, LTDC_B4_PINSOURCE, LTDC_B4_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_B5_GPIO_PIN;
 GPIO_Init(LTDC_B5_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B5_GPIO_PORT, LTDC_B5_PINSOURCE, LTDC_B5_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_B6_GPIO_PIN;
 GPIO_Init(LTDC_B6_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B6_GPIO_PORT, LTDC_B6_PINSOURCE, LTDC_B6_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_B7_GPIO_PIN;
 GPIO_Init(LTDC_B7_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B7_GPIO_PORT, LTDC_B7_PINSOURCE, LTDC_B7_AF);

 //�����ź���
 GPIO_InitStruct.GPIO_Pin = LTDC_CLK_GPIO_PIN;
 GPIO_Init(LTDC_CLK_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_CLK_GPIO_PORT, LTDC_CLK_PINSOURCE, LTDC_CLK_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_HSYNC_GPIO_PIN;
 GPIO_Init(LTDC_HSYNC_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_HSYNC_GPIO_PORT, LTDC_HSYNC_PINSOURCE, LTDC_HSYNC_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_VSYNC_GPIO_PIN;
 GPIO_Init(LTDC_VSYNC_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_VSYNC_GPIO_PORT, LTDC_VSYNC_PINSOURCE, LTDC_VSYNC_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_DE_GPIO_PIN;
 GPIO_Init(LTDC_DE_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_DE_GPIO_PORT, LTDC_DE_PINSOURCE, LTDC_DE_AF);

 //BL DISP
 GPIO_InitStruct.GPIO_Pin = LTDC_DISP_GPIO_PIN;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

 GPIO_Init(LTDC_DISP_GPIO_PORT, &GPIO_InitStruct);


 GPIO_InitStruct.GPIO_Pin = LTDC_BL_GPIO_PIN;
 GPIO_Init(LTDC_BL_GPIO_PORT, &GPIO_InitStruct);

 //����ʹ��lcd
 GPIO_SetBits(LTDC_DISP_GPIO_PORT,LTDC_DISP_GPIO_PIN);
 GPIO_SetBits(LTDC_BL_GPIO_PORT,LTDC_BL_GPIO_PIN);
}

/**
 * @brief  Displays a pixel.
 * @param  x: pixel x.
 * @param  y: pixel y.
 * @retval None
 */
void PutPixel(int16_t x, int16_t y)
{
 if(x < 0 || x > LCD_PIXEL_WIDTH || y < 0 || y > LCD_PIXEL_HEIGHT)
 {
   return;
 }
#if 0
 LCD_DrawLine(x, y, 1, LCD_DIR_HORIZONTAL);
#else
 {
	  uint32_t  Xaddress = 0;
    Xaddress = CurrentFrameBuffer + 2*(LCD_PIXEL_WIDTH*y + x);
    *(__IO uint16_t*) Xaddress= CurrentTextColor;
  }
#endif
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

/**
 * @}
 */

#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
