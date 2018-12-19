/****************************************************************************
* Copyright (C), 2011 �ܶ�Ƕ��ʽ������ www.ourstm.net
*
* �������� �ܶ���STM32������MINI��V2,2.1,V3�ϵ���ͨ��           
* QQ: 9191274, ������sun68, Email: sun68@163.com 
* �Ա����̣�ourstm.taobao.com  
*
* �ļ���: Fun.c
* ���ݼ���:
*       �������ṩ��FSMC�ӿڵĳ�ʼ��
		
*
* �ļ���ʷ:
* �汾��  ����       ����    ˵��
* v0.2    2011-07-04 sun68  �������ļ�
*
*/
//#include "fsmc_sram.h"
//#include "stm32f10x_fsmc.h"
#include "pbdata.h"
void FSMC_LCD_Init(void);
/****************************************************************************
* ��    �ƣ�void FSMC_LCD_Init(void)
* ��    �ܣ�����FSMC�ӿڵ�8λLCD�ӿڳ�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void FSMC_LCD_Init(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;
  GPIO_InitTypeDef  GPIO_InitStructure; 

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);	   //ʹ��FSMC�ӿ�ʱ��
  	
  /*  ����FSMC���ù��ܣ� ����FSMC D0---D15��nWE, nOE��Ӧ������  */
  /* ���� PD.00(D2), PD.01(D3), PD.04(nOE), PD.05(nWE), PD.08(D13), PD.09(D14), PD.10(D15),
     PD.14(D0), PD.15(D1) Ϊ�������� */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                 GPIO_Pin_14 | GPIO_Pin_15;
                                
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		   //����ٶ�10MHz
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		   //����ģʽ
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* ���� PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
     PE.14(D11), PE.15(D12) Ϊ�������� */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10  ;
                                  
  GPIO_Init(GPIOE, &GPIO_InitStructure); 


  
  /* FSMC  NE4 ���� PG12 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  /* FSMC RS���� PD11-A1 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 
  GPIO_Init(GPIOF, &GPIO_InitStructure); 

  p.FSMC_AddressSetupTime = 0x05;
  p.FSMC_AddressHoldTime = 0x02;//02
  p.FSMC_DataSetupTime = 0xF;
  p.FSMC_BusTurnAroundDuration = 0x00;//01
  p.FSMC_CLKDivision = 0x00;
  p.FSMC_DataLatency = 0x01;
  p.FSMC_AccessMode = FSMC_AccessMode_A;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;//FSMC_MemoryType_NOR;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Enable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

  /* ʹ��FSMC BANK1_SRAM ģʽ */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  
}

/****************************************************************************
* ��    �ƣ�void FSMC_RAM_Init(void)
* ��    �ܣ�����FSMC�ӿڵ�16λRAM�ӿڳ�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void FSMC_RAM_Init(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;
  GPIO_InitTypeDef  GPIO_InitStructure; 

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);	   //ʹ��FSMC�ӿ�ʱ��
  
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;			   //�������	       
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		   //ͨ���������ģʽ
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		   //���ģʽ����ٶ�50MHz
//  GPIO_Init(GPIOD, &GPIO_InitStructure);    
//  GPIO_SetBits(GPIOD, GPIO_Pin_13);			               //�򿪱���


//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;                //TFT ��λ��
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		   //ͨ���������ģʽ
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		   //���ģʽ����ٶ�50MHz
//  GPIO_Init(GPIOE, &GPIO_InitStructure);  
//  
  	
  /*  ����FSMC���ù��ܣ� ����FSMC D0---D15��nWE, nOE��Ӧ������  */
  /* ���� PD.00(D2), PD.01(D3), PD.04(nOE), PD.05(nWE), PD.08(D13), PD.09(D14), PD.10(D15),
     PD.14(D0), PD.15(D1) Ϊ�������� */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                 GPIO_Pin_14 | GPIO_Pin_15;
                                
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		   //����ٶ�10MHz
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		   //����ģʽ
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* ���� PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
     PE.14(D11), PE.15(D12) Ϊ�������� */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10  ;
                                  
  GPIO_Init(GPIOE, &GPIO_InitStructure); 


  
  /* FSMC  NE4 ���� PG12 */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 
//  GPIO_Init(GPIOG, &GPIO_InitStructure);
//  
//  /* FSMC RS���� PD11-A1 */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 
//  GPIO_Init(GPIOF, &GPIO_InitStructure); 

  p.FSMC_AddressSetupTime = 0x02;
  p.FSMC_AddressHoldTime = 0x00;
  p.FSMC_DataSetupTime = 0x05;
  p.FSMC_BusTurnAroundDuration = 0x00;
  p.FSMC_CLKDivision = 0x00;
  p.FSMC_DataLatency = 0x00;
  p.FSMC_AccessMode = FSMC_AccessMode_B;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

  /* ʹ��FSMC BANK1_SRAM ģʽ */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  
}


/******************* (C) COPYRIGHT 2011 �ܶ�STM32 *****END OF FILE****/