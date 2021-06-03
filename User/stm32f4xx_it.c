/**
  ******************************************************************************
  * @file    FMC_SDRAM/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   Main Interrupt Service Routines.
  *         This file provides template for all exceptions handler and
  *         peripherals interrupt service routine.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "pbdata.h"
#include "usb_bsp.h"
#include "usb_hcd_int.h"
#include "usbh_core.h"
#include    "bsp_exti.h"
#include "./tim/bsp_basic_tim.h"
#include "stdlib.h"
#include "math.h"

extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
extern USBH_HOST                    USB_Host;
struct MODS_T g_tModS;
u8 g_mods_timeout = 0;
u32 Tick_10ms=0;
char scpinum[20],scpinum1[20];
u8 scpidot,scpiunit,scpidot1,scpiunit1;
u8 scpnum = 0;
u8 scpnum1 = 0;
extern Sort_TypeDef SCPI_SET_R(void),SCPI_SET_V(void),SCPI_SET_R1(void),SCPI_SET_V1(void);
/* Private function prototypes -----------------------------------------------*/
extern void USB_OTG_BSP_TimerIRQ (void);
static void MODS_03H(void);
static void MODS_06H(void);
static void RemTrig(void);
//extern void Read__Convert_read(void);
/** @addtogroup STM32F429I_DISCOVERY_Examples
  * @{
  */

/** @addtogroup FMC_SDRAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
    
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    TimingDelay_Decrement();


}


/*
*********************************************************************************************************
*	? ? ?: MODS_SendAckErr
*	????: ??????
*	��    ?: _ucErrCode : ????
*	? ? ?: ?
*********************************************************************************************************
*/
static void MODS_SendAckErr(uint8_t _ucErrCode)
{
	uint8_t txbuf[3];

	txbuf[0] = g_tModS.RxBuf[0];					/* 485?? */
	txbuf[1] = g_tModS.RxBuf[1] | 0x80;				/* ?????? */
	txbuf[2] = _ucErrCode;							/* ????(01,02,03,04) */

	MODS_SendWithCRC(txbuf, 3);
}


/*
*********************************************************************************************************
*	? ? ?: MODS_SendAckOk
*	????: ???????.
*	��    ?: ?
*	? ? ?: ?
*********************************************************************************************************
*/
static void MODS_SendAckOk(void)
{
	uint8_t txbuf[6];
	uint8_t i;

	for (i = 0; i < 6; i++)
	{
		txbuf[i] = g_tModS.RxBuf[i];
	}
	MODS_SendWithCRC(txbuf, 6);
}

/*
*********************************************************************************************************
*	? ? ?: BEBufToUint16
*	????: ?2????(??Big Endian??��?????)???16��??
*	��    ?: _pBuf : ??
*	? ? ?: 16��???
*
*   ??(Big Endian)?��?(Little Endian)
*********************************************************************************************************
*/
uint16_t BEBufToUint16(uint8_t *_pBuf)
{
    return (((uint16_t)_pBuf[0] << 8) | _pBuf[1]);
}

void uart1SendChars(u8 *str, u16 strlen)
{ 
	  u16 k= 0 ; 
   do { 
       USART_SendData(USART3,*(str + k));
       while (USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
       k++; 
       }   //????,??????   
    while (k < strlen); 
} 


 // CRC ?��????
static const uint8_t s_CRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
} ;
// CRC ?��????
const uint8_t s_CRCLo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
	0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
	0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
	0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
	0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
	0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
	0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
	0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
	0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
	0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
	0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
	0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

uint16_t CRC16(uint8_t *_pBuf, uint16_t _usLen)
{    
    uint8_t ucCRCHi = 0xFF; /* ?CRC????? */
	uint8_t ucCRCLo = 0xFF; /* ?CRC ????? */
	uint16_t usIndex;  /* CRC?????? */

    while (_usLen--)
    {
		usIndex = ucCRCHi ^ *_pBuf++; /* ??CRC */
		ucCRCHi = ucCRCLo ^ s_CRCHi[usIndex];
		ucCRCLo = s_CRCLo[usIndex];
    }
    return ((uint16_t)ucCRCHi << 8 | ucCRCLo);
}


void MODS_SendWithCRC(uint8_t *_pBuf, uint8_t _ucLen)
{
	uint16_t crc;
	uint8_t buf[S_TX_BUF_SIZE];
	memcpy(buf, _pBuf, _ucLen);
	crc = CRC16(_pBuf, _ucLen);
	buf[_ucLen++] = crc >> 8;
	buf[_ucLen++] = crc;
//??485???
//	RS485_SendBuf(buf, _ucLen);

	uart1SendChars(buf, _ucLen);
	
// #if 1									/* ???????????,???????? */
// 	g_tPrint.Txlen = _ucLen;
// 	memcpy(g_tPrint.TxBuf, buf, _ucLen);
// #endif
}


void USART3_IRQHandler(void)
{
	u8 dat;

	if(( USART_GetFlagStatus(USART3, USART_FLAG_RXNE) != RESET))//��������ж�
	{
		g_mods_timeout = 2;
		if (g_tModS.RxCount < S_RX_BUF_SIZE)
		{
			g_tModS.RxBuf[g_tModS.RxCount++] = USART_ReceiveData(USART3);
		}
//		if (!ComBuf.rec.end)//����û����
//		{
//			SetRecTimeOut(REC_TIME_OUT);//���ý��ճ�ʱ����
//			dat=USART_ReceiveData(USART3);
//			if (dat==(u8)(UART_REC_BEGIN))//֡ͷ
//			{
//				if(ComBuf.rec.ptr!=0) //���ֽ�
//				{
//					ComBuf.rec.ptr=0;//���½��� 
//				}
//				else
//				{
//					ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
//				}
//			}
//			else if (dat==(u8)(UART_REC_END))//֡β
//			{
//				ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
//				ComBuf.rec.end=TRUE;//���ս���
//				ComBuf.rec.len=ComBuf.rec.ptr;//��������ݳ���
//				ComBuf.rec.ptr=0;//ָ���������¿�ʼ�µ�һ֡����
//				ComBuf.rec.TimeOut=0;//���ճ�ʱ����
//			}
//			else
//			{
//				if (ComBuf.rec.ptr>=REC_LEN_MAX)//������֡����
//				{
//					ComBuf.rec.ptr=0;//���½���
//				}
//				else
//				{
//					ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
//				}
//			}
//		}	
//        USART_ClearFlag(USART3, USART_FLAG_RXNE);
////		RI=0;//����ձ�־
	}
    USART_ClearFlag(USART3, USART_FLAG_RXNE);//
    USART_ClearFlag(USART3, USART_FLAG_TXE);
    

//	if(( USART_GetFlagStatus(USART3, USART_FLAG_TXE) != RESET))//��������ж�
//	{
//		if (ComBuf.send.begin)//���Ϳ�ʼ
//		{
//			if (ComBuf.send.ptr<ComBuf.send.len)//û���������
//			{
////				SBUF=ComBuf.send.buf[ComBuf.send.ptr++];
//                 USART_SendData(USART3, (uint8_t) ComBuf.send.buf[ComBuf.send.ptr++]); /*����һ���ַ�����*/ 

//                  /* Loop until the end of transmission */
//                  while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)/*�ȴ��������*/
//                  {
//                  
//                  }
//			}
//			else
//			{
//				ComBuf.send.begin=FALSE;//���ͽ���
//				ComBuf.send.ptr=0;//ָ������
//			}
//		}
//USART_ClearFlag(USART3, USART_FLAG_TXE);//		TI=0;//�巢�ͱ�־
//	}	
} 

static void GetScpiNum(u8 count,u8 data)
{
	u8 i;
	u8 dflag = 0;
	
	memset(scpinum, 0, sizeof(scpinum));
	memset(scpinum1, 0, sizeof(scpinum1));
	scpnum = 0;
	scpnum1 = 0;
	for(i = count;i < g_tModS.RxCount;i++)
	{
		if(g_tModS.RxBuf[i] == ',')
		{
			dflag = i;
		}
	}
	if(dflag)
	{
		for(i = count;i < dflag;i++)
		{
			if((g_tModS.RxBuf[i] >= '0' && g_tModS.RxBuf[i] <= '9') || g_tModS.RxBuf[i] == '.')
			{
				scpinum[scpnum] = g_tModS.RxBuf[i];
				if(g_tModS.RxBuf[i] == '.')
				{
					scpidot =scpnum;
				}
				scpnum ++;
			}
			scpiunit = 1;
			if(g_tModS.RxBuf[i] == 'm')
			{
				scpiunit = 0;
			}else if(g_tModS.RxBuf[i] == 'k'){
				scpiunit = 2;
			}		
		}
		for(i = dflag+1;i < g_tModS.RxCount;i++)
		{
			if((g_tModS.RxBuf[i] >= '0' && g_tModS.RxBuf[i] <= '9') || g_tModS.RxBuf[i] == '.')
			{
				scpinum1[scpnum1] = g_tModS.RxBuf[i];
				if(g_tModS.RxBuf[i] == '.')
				{
					scpidot1 =scpnum1;
				}
				scpnum1 ++;
			}
			scpiunit1 = 1;
			if(g_tModS.RxBuf[i] == 'm')
			{
				scpiunit1 = 0;
			}else if(g_tModS.RxBuf[i] == 'k'){
				scpiunit1 = 2;
			}		
		}
	}else{
		for(i = count;i < g_tModS.RxCount;i++)
		{
			if((g_tModS.RxBuf[i] >= '0' && g_tModS.RxBuf[i] <= '9') || g_tModS.RxBuf[i] == '.')
			{
				scpinum[scpnum] = g_tModS.RxBuf[i];
				if(g_tModS.RxBuf[i] == '.')
				{
					scpidot =scpnum;
				}
				scpnum ++;
			}
			scpiunit = 1;
			if(g_tModS.RxBuf[i] == 'm')
			{
				scpiunit = 0;
			}else if(g_tModS.RxBuf[i] == 'k'){
				scpiunit = 2;
			}		
		}
	}
	if(data == 0)
	{
		Jk516save.Set_Data.Nominal_V=SCPI_SET_V();
	}else if(data == 1){
		Jk516save.Set_Data.Nominal_Res=SCPI_SET_R();
	}else if(data == 2){
		Jk516save.Set_Data.Res_low=SCPI_SET_R();
		Jk516save.Set_Data.High_Res=SCPI_SET_R1();
	}else if(data == 3){
		Jk516save.Set_Data.V_low=SCPI_SET_V();
		Jk516save.Set_Data.V_high=SCPI_SET_V1();
	}
}
void sendsetres(void)
{
	uart1SendChars("SetOK",5);
}
void RecHandle(void)
{
    u8 crec[6];
    u8 *csend;
    static u8 *sendbuf;
    u8 sendlen;
    static u16 recrc;
    static u16 scrc;
    u8 i,scpisendcount;
	float scpisend1,scpisend2;
	int scpisendE1,scpisendE2;
	char scpisendbuf[24],scpisendbuf1[24];
   
	memset(scpisendbuf,0,24);
	memset(scpisendbuf1,0,24);
	
	if(g_tModS.RxBuf[0] == 'T' && g_tModS.RxBuf[1] == 'R' && g_tModS.RxBuf[2] == 'I' && g_tModS.RxBuf[3] == 'G'  && g_tModS.RxBuf[4] != ':')
	{
		RemTrig();//������������������
	}else if(g_tModS.RxBuf[0] == 'T' && g_tModS.RxBuf[1] == 'R' && g_tModS.RxBuf[2] == 'I' && g_tModS.RxBuf[3] == 'G'
	    && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'S' && g_tModS.RxBuf[6] == 'O'
	    && g_tModS.RxBuf[7] == 'U' && g_tModS.RxBuf[8] == 'R' && g_tModS.RxBuf[9] == ' ')
	{
		if(g_tModS.RxBuf[10] == 'I' && g_tModS.RxBuf[11] == 'N' && g_tModS.RxBuf[12] == 'T')
		{
			Jk516save.Set_Data.trip = 0;
		}else if(g_tModS.RxBuf[10] == 'M' && g_tModS.RxBuf[11] == 'A' && g_tModS.RxBuf[12] == 'N'){
			Jk516save.Set_Data.trip = 1;
		}else if(g_tModS.RxBuf[10] == 'E' && g_tModS.RxBuf[11] == 'X' && g_tModS.RxBuf[12] == 'T'){
			Jk516save.Set_Data.trip = 2;
		}else if(g_tModS.RxBuf[10] == 'R' && g_tModS.RxBuf[11] == 'M' && g_tModS.RxBuf[12] == 'T'){
			Jk516save.Set_Data.trip = 3;
		}
		if(GetSystemStatus() == SYS_STATUS_TEST)
		{
			Disp_Test_value(1);
		}else if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(1);
		}
	}else if(g_tModS.RxBuf[0] == 'D' && g_tModS.RxBuf[1] == 'I' && g_tModS.RxBuf[2] == 'S'
	    && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'M' && g_tModS.RxBuf[6] == 'E'
	    && g_tModS.RxBuf[7] == 'A' && g_tModS.RxBuf[8] == 'S')
	{
		SetSystemStatus(SYS_STATUS_TEST);//������ʾ����
	}else if(g_tModS.RxBuf[0] == 'D' && g_tModS.RxBuf[1] == 'I' && g_tModS.RxBuf[2] == 'S'
	    && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'S' && g_tModS.RxBuf[6] == 'E'
	    && g_tModS.RxBuf[7] == 'T' && g_tModS.RxBuf[8] == 'U')
	{
		SetSystemStatus(SYS_STATUS_SETUP);//�������ý���
	}else if(g_tModS.RxBuf[0] == 'D' && g_tModS.RxBuf[1] == 'I' && g_tModS.RxBuf[2] == 'S'
	    && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'S' && g_tModS.RxBuf[6] == 'S'
	    && g_tModS.RxBuf[7] == 'E' && g_tModS.RxBuf[8] == 'T')
	{
		SetSystemStatus(SYS_STATUS_SYSSET);//ϵͳ���ý���
	}else if(g_tModS.RxBuf[0] == 'D' && g_tModS.RxBuf[1] == 'I' && g_tModS.RxBuf[2] == 'S'
	    && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'S' && g_tModS.RxBuf[6] == 'I'
	    && g_tModS.RxBuf[7] == 'N' && g_tModS.RxBuf[8] == 'F')
	{
		Sys_Process();//ϵͳ��Ϣ����
	}else if(g_tModS.RxBuf[0] == 'D' && g_tModS.RxBuf[1] == 'I' && g_tModS.RxBuf[2] == 'S'
	    && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'P' && g_tModS.RxBuf[6] == 'A'
	    && g_tModS.RxBuf[7] == 'G' && g_tModS.RxBuf[8] == 'E' && g_tModS.RxBuf[9] == '?')
	{
		if(GetSystemStatus() == SYS_STATUS_TEST)
		{
			uart1SendChars("meas",4);
		}else if(GetSystemStatus() == SYS_STATUS_SETUP){
			uart1SendChars("setu",4);
		}else if(GetSystemStatus() == SYS_STATUS_SYSSET){
			uart1SendChars("syst",4);
		}else if(GetSystemStatus() == SYS_STATUS_SYS){
			uart1SendChars("sinf",4);
		}//��ѯ��ǰҳ��
	}else if(g_tModS.RxBuf[0] == 'F' && g_tModS.RxBuf[1] == 'U' && g_tModS.RxBuf[2] == 'N'
	     && g_tModS.RxBuf[3] == 'C' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'R' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'N' && g_tModS.RxBuf[8] == 'G' && g_tModS.RxBuf[9] == ':' && g_tModS.RxBuf[10] == 'M'
		 && g_tModS.RxBuf[11] == 'O' && g_tModS.RxBuf[12] == 'D' && g_tModS.RxBuf[13] == 'E' && g_tModS.RxBuf[14] == ' '
		 && g_tModS.RxBuf[15] == 'A' && g_tModS.RxBuf[16] == 'U' && g_tModS.RxBuf[17] == 'T' && g_tModS.RxBuf[18] == 'O')
	{
		Jk516save.Set_Data.Range_Set=0;
		Jk516save.Set_Data.Range=0;//��������Ϊ�Զ�
		sendsetres();
		if(GetSystemStatus() == SYS_STATUS_TEST)
		{
			Disp_Test_value(4);
		}else if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(7);
		}
	}else if(g_tModS.RxBuf[0] == 'F' && g_tModS.RxBuf[1] == 'U' && g_tModS.RxBuf[2] == 'N'
	     && g_tModS.RxBuf[3] == 'C' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'R' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'N' && g_tModS.RxBuf[8] == 'G' && g_tModS.RxBuf[9] == ':' && g_tModS.RxBuf[10] == 'M'
		 && g_tModS.RxBuf[11] == 'O' && g_tModS.RxBuf[12] == 'D' && g_tModS.RxBuf[13] == 'E' && g_tModS.RxBuf[14] == ' '
		 && g_tModS.RxBuf[15] == 'H' && g_tModS.RxBuf[16] == 'O' && g_tModS.RxBuf[17] == 'L' && g_tModS.RxBuf[18] == 'D')
	{
		Jk516save.Set_Data.Range_Set=1;
		Jk516save.Set_Data.Range=Range;//��������Ϊ�ֶ�
		if(GetSystemStatus() == SYS_STATUS_TEST)
		{
			Disp_Test_value(4);
		}else if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(7);
		}
	}else if(g_tModS.RxBuf[0] == 'F' && g_tModS.RxBuf[1] == 'U' && g_tModS.RxBuf[2] == 'N'
	     && g_tModS.RxBuf[3] == 'C' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'R' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'N' && g_tModS.RxBuf[8] == 'G' && g_tModS.RxBuf[9] == ':' && g_tModS.RxBuf[10] == 'M'
		 && g_tModS.RxBuf[11] == 'O' && g_tModS.RxBuf[12] == 'D' && g_tModS.RxBuf[13] == 'E' && g_tModS.RxBuf[14] == ' '
		 && g_tModS.RxBuf[15] == 'N' && g_tModS.RxBuf[16] == 'O' && g_tModS.RxBuf[17] == 'M')
	{
		Jk516save.Set_Data.Range_Set=2;//���ӵ�λѡ��ļ���
		Range=Jisuan_Range(Jk516save.Set_Data.Nominal_Res);
		Jk516save.Set_Data.Range=Range;
		Range_Control(Range,V_Range);//��������Ϊ���
		if(GetSystemStatus() == SYS_STATUS_TEST)
		{
			Disp_Test_value(4);
		}else if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(7);
		}
	}else if(g_tModS.RxBuf[0] == 'F' && g_tModS.RxBuf[1] == 'U' && g_tModS.RxBuf[2] == 'N'
	     && g_tModS.RxBuf[3] == 'C' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'R' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'N' && g_tModS.RxBuf[8] == 'G' && g_tModS.RxBuf[9] == ':' && g_tModS.RxBuf[10] == 'M'
		 && g_tModS.RxBuf[11] == 'O' && g_tModS.RxBuf[12] == 'D' && g_tModS.RxBuf[13] == 'E' && g_tModS.RxBuf[14] == '?')
	{
		if(Jk516save.Set_Data.Range_Set == 0)
		{
			uart1SendChars("AUTO",4);
		}else if(Jk516save.Set_Data.Range_Set == 1){
			uart1SendChars("HOLD",4);
		}else if(Jk516save.Set_Data.Range_Set == 2){
			uart1SendChars("NOM",3);
		}//��ѯ�л����̷�ʽ
	}
//	else if(g_tModS.RxBuf[0] == 'F' && g_tModS.RxBuf[1] == 'U' && g_tModS.RxBuf[2] == 'N'
//	     && g_tModS.RxBuf[3] == 'C' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'R' && g_tModS.RxBuf[6] == 'A'
//		 && g_tModS.RxBuf[7] == 'N' && g_tModS.RxBuf[8] == 'G' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == '1'
//		 )
//	{
//		Jk516save.Set_Data.Range_Set=1;//���ӵ�λѡ��ļ���
//		Jk516save.Set_Data.Range=0;//��������Ϊ0
//		
//		if(GetSystemStatus() == SYS_STATUS_TEST)
//		{
//			Disp_Test_value(4);
//		}else if(GetSystemStatus() == SYS_STATUS_SETUP){
//			DispSet_value(7);
//		}
//	}
	else if(g_tModS.RxBuf[0] == 'F' && g_tModS.RxBuf[1] == 'U' && g_tModS.RxBuf[2] == 'N'
	     && g_tModS.RxBuf[3] == 'C' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'R' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'N' && g_tModS.RxBuf[8] == 'G' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == '1'
		 )
	{
		Jk516save.Set_Data.Range_Set=1;
		Jk516save.Set_Data.Range=0;//��������Ϊ0
		Range=Jk516save.Set_Data.Range;
		sendsetres();
		if(GetSystemStatus() == SYS_STATUS_TEST)
		{
			Disp_Test_value(4);
		}else if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(7);
		}
		Range_Control(Range,V_Range);
	}else if(g_tModS.RxBuf[0] == 'F' && g_tModS.RxBuf[1] == 'U' && g_tModS.RxBuf[2] == 'N'
	     && g_tModS.RxBuf[3] == 'C' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'R' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'N' && g_tModS.RxBuf[8] == 'G' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == '2'
		 )
	{
		Jk516save.Set_Data.Range_Set=1;
		Jk516save.Set_Data.Range=1;//��������Ϊ1
		Range=Jk516save.Set_Data.Range;
		sendsetres();
		if(GetSystemStatus() == SYS_STATUS_TEST)
		{
			Disp_Test_value(4);
		}else if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(7);
		}
		Range_Control(Range,V_Range);
	}else if(g_tModS.RxBuf[0] == 'F' && g_tModS.RxBuf[1] == 'U' && g_tModS.RxBuf[2] == 'N'
	     && g_tModS.RxBuf[3] == 'C' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'R' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'N' && g_tModS.RxBuf[8] == 'G' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == '3'
		 )
	{
		Jk516save.Set_Data.Range_Set=1;
		Jk516save.Set_Data.Range=2;//��������Ϊ2
		Range=Jk516save.Set_Data.Range;
		sendsetres();
		if(GetSystemStatus() == SYS_STATUS_TEST)
		{
			Disp_Test_value(4);
		}else if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(7);
		}
		Range_Control(Range,V_Range);
	}else if(g_tModS.RxBuf[0] == 'F' && g_tModS.RxBuf[1] == 'U' && g_tModS.RxBuf[2] == 'N'
	     && g_tModS.RxBuf[3] == 'C' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'R' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'N' && g_tModS.RxBuf[8] == 'G' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == '4'
		 )
	{
		Jk516save.Set_Data.Range_Set=1;
		Jk516save.Set_Data.Range=3;//��������Ϊ3
		Range=Jk516save.Set_Data.Range;
		sendsetres();
		if(GetSystemStatus() == SYS_STATUS_TEST)
		{
			Disp_Test_value(4);
		}else if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(7);
		}
		Range_Control(Range,V_Range);
	}else if(g_tModS.RxBuf[0] == 'F' && g_tModS.RxBuf[1] == 'U' && g_tModS.RxBuf[2] == 'N'
	     && g_tModS.RxBuf[3] == 'C' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'R' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'N' && g_tModS.RxBuf[8] == 'G' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == '5'
		 )
	{
		Jk516save.Set_Data.Range_Set=1;
		Jk516save.Set_Data.Range=4;//��������Ϊ4
		Range=Jk516save.Set_Data.Range;
		sendsetres();
		if(GetSystemStatus() == SYS_STATUS_TEST)
		{
			Disp_Test_value(4);
		}else if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(7);
		}
		Range_Control(Range,V_Range);
	}else if(g_tModS.RxBuf[0] == 'F' && g_tModS.RxBuf[1] == 'U' && g_tModS.RxBuf[2] == 'N'
	     && g_tModS.RxBuf[3] == 'C' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'R' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'N' && g_tModS.RxBuf[8] == 'G' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == '6'
		 )
	{
		Jk516save.Set_Data.Range_Set=1;
		Jk516save.Set_Data.Range=5;//��������Ϊ5
		Range=Jk516save.Set_Data.Range;
		sendsetres();
		if(GetSystemStatus() == SYS_STATUS_TEST)
		{
			Disp_Test_value(4);
		}else if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(7);
		}
		Range_Control(Range,V_Range);
	}else if(g_tModS.RxBuf[0] == 'F' && g_tModS.RxBuf[1] == 'U' && g_tModS.RxBuf[2] == 'N'
	     && g_tModS.RxBuf[3] == 'C' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'R' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'N' && g_tModS.RxBuf[8] == 'G' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == '7'
		 )
	{
		Jk516save.Set_Data.Range_Set=1;
		Jk516save.Set_Data.Range=6;//��������Ϊ6
		Range=Jk516save.Set_Data.Range;
		sendsetres();
		if(GetSystemStatus() == SYS_STATUS_TEST)
		{
			Disp_Test_value(4);
		}else if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(7);
		}
		Range_Control(Range,V_Range);
	}else if(g_tModS.RxBuf[0] == 'F' && g_tModS.RxBuf[1] == 'U' && g_tModS.RxBuf[2] == 'N'
	     && g_tModS.RxBuf[3] == 'C' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'R' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'N' && g_tModS.RxBuf[8] == 'G' && g_tModS.RxBuf[9] == '?')
	{
		if(Jk516save.Set_Data.Range == 0)
		{
			uart1SendChars("1",1);
		}else if(Jk516save.Set_Data.Range == 1){
			uart1SendChars("2",1);
		}else if(Jk516save.Set_Data.Range == 2){
			uart1SendChars("3",1);
		}else if(Jk516save.Set_Data.Range == 3){
			uart1SendChars("4",1);
		}else if(Jk516save.Set_Data.Range == 4){
			uart1SendChars("5",1);
		}else if(Jk516save.Set_Data.Range == 5){
			uart1SendChars("6",1);
		}else if(Jk516save.Set_Data.Range == 6){
			uart1SendChars("7",1);
		}//��ѯ���̺�
	}else if(g_tModS.RxBuf[0] == 'F' && g_tModS.RxBuf[1] == 'U' && g_tModS.RxBuf[2] == 'N'
	     && g_tModS.RxBuf[3] == 'C' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'R' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'T' && g_tModS.RxBuf[8] == 'E' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == 'S'
		 && g_tModS.RxBuf[11] == 'L' && g_tModS.RxBuf[12] == 'O' && g_tModS.RxBuf[13] == 'W')
	{
		Jk516save.Set_Data.speed=0;//����
		sendsetres();
		if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(2);
		}
	}else if(g_tModS.RxBuf[0] == 'F' && g_tModS.RxBuf[1] == 'U' && g_tModS.RxBuf[2] == 'N'
	     && g_tModS.RxBuf[3] == 'C' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'R' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'T' && g_tModS.RxBuf[8] == 'E' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == 'M'
		 && g_tModS.RxBuf[11] == 'E' && g_tModS.RxBuf[12] == 'D')
	{
		Jk516save.Set_Data.speed=1;//����
		sendsetres();
		if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(2);
		}
	}else if(g_tModS.RxBuf[0] == 'F' && g_tModS.RxBuf[1] == 'U' && g_tModS.RxBuf[2] == 'N'
	     && g_tModS.RxBuf[3] == 'C' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'R' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'T' && g_tModS.RxBuf[8] == 'E' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == 'F'
		 && g_tModS.RxBuf[11] == 'A' && g_tModS.RxBuf[12] == 'S' && g_tModS.RxBuf[13] == 'T')
	{
		Jk516save.Set_Data.speed=2;//����
		sendsetres();
		if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(2);
		}
	}else if(g_tModS.RxBuf[0] == 'F' && g_tModS.RxBuf[1] == 'U' && g_tModS.RxBuf[2] == 'N'
	     && g_tModS.RxBuf[3] == 'C' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'R' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'T' && g_tModS.RxBuf[8] == 'E' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == 'U'
		 && g_tModS.RxBuf[11] == 'L' && g_tModS.RxBuf[12] == 'T' && g_tModS.RxBuf[13] == 'R')
	{
		Jk516save.Set_Data.speed=3;//����
		sendsetres();
		if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(2);
		}
	}else if(g_tModS.RxBuf[0] == 'F' && g_tModS.RxBuf[1] == 'U' && g_tModS.RxBuf[2] == 'N'
	     && g_tModS.RxBuf[3] == 'C' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'R' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'T' && g_tModS.RxBuf[8] == 'E' && g_tModS.RxBuf[9] == '?')
	{
		if(Jk516save.Set_Data.speed == 0)
		{
			uart1SendChars("SLOW",4);
		}else if(Jk516save.Set_Data.speed == 1){
			uart1SendChars("MED",3);
		}else if(Jk516save.Set_Data.speed == 2){
			uart1SendChars("FAST",4);
		}else if(Jk516save.Set_Data.speed == 3){
			uart1SendChars("ULTR",4);
		}//��ѯ�ٶ�
	}else if(g_tModS.RxBuf[0] == 'C' && g_tModS.RxBuf[1] == 'O' && g_tModS.RxBuf[2] == 'M'
	     && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'R' && g_tModS.RxBuf[6] == 'M'
		 && g_tModS.RxBuf[7] == 'O' && g_tModS.RxBuf[8] == 'D' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == 'O'
		 && g_tModS.RxBuf[11] == 'F' && g_tModS.RxBuf[12] == 'F')
	{
		Jk516save.Set_Data.Res_comp=0;//�رյ����ѡ
		if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(3);
		}
	}else if(g_tModS.RxBuf[0] == 'C' && g_tModS.RxBuf[1] == 'O' && g_tModS.RxBuf[2] == 'M'
	     && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'R' && g_tModS.RxBuf[6] == 'M'
		 && g_tModS.RxBuf[7] == 'O' && g_tModS.RxBuf[8] == 'D' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == 'O'
		 && g_tModS.RxBuf[11] == 'N')
	{
		Jk516save.Set_Data.Res_comp=1;//�򿪵����ѡ
		if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(3);
		}
	}else if(g_tModS.RxBuf[0] == 'C' && g_tModS.RxBuf[1] == 'O' && g_tModS.RxBuf[2] == 'M'
	     && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'R' && g_tModS.RxBuf[6] == 'M'
		 && g_tModS.RxBuf[7] == 'O' && g_tModS.RxBuf[8] == 'D' && g_tModS.RxBuf[9] == '?')
	{
		if(Jk516save.Set_Data.Res_comp == 0)
		{
			uart1SendChars("OFF",3);
		}else if(Jk516save.Set_Data.Res_comp == 1){
			uart1SendChars("ON",2);
		}//��ѯ�����ѡ
	}else if(g_tModS.RxBuf[0] == 'C' && g_tModS.RxBuf[1] == 'O' && g_tModS.RxBuf[2] == 'M'
	     && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'V' && g_tModS.RxBuf[6] == 'M'
		 && g_tModS.RxBuf[7] == 'O' && g_tModS.RxBuf[8] == 'D' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == 'O'
		 && g_tModS.RxBuf[11] == 'F' && g_tModS.RxBuf[12] == 'F')
	{
		Jk516save.Set_Data.V_comp=0;//�رյ�ѹ��ѡ
		if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(5);
		}
	}else if(g_tModS.RxBuf[0] == 'C' && g_tModS.RxBuf[1] == 'O' && g_tModS.RxBuf[2] == 'M'
	     && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'V' && g_tModS.RxBuf[6] == 'M'
		 && g_tModS.RxBuf[7] == 'O' && g_tModS.RxBuf[8] == 'D' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == 'O'
		 && g_tModS.RxBuf[11] == 'N')
	{
		Jk516save.Set_Data.V_comp=1;//�򿪵�ѹ��ѡ
		if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(5);
		}
	}else if(g_tModS.RxBuf[0] == 'C' && g_tModS.RxBuf[1] == 'O' && g_tModS.RxBuf[2] == 'M'
	     && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'V' && g_tModS.RxBuf[6] == 'M'
		 && g_tModS.RxBuf[7] == 'O' && g_tModS.RxBuf[8] == 'D' && g_tModS.RxBuf[9] == '?')
	{
		if(Jk516save.Set_Data.V_comp == 0)
		{
			uart1SendChars("OFF",3);
		}else if(Jk516save.Set_Data.V_comp == 1){
			uart1SendChars("ON",2);
		}//��ѯ��ѹ��ѡ
	}else if(g_tModS.RxBuf[0] == 'C' && g_tModS.RxBuf[1] == 'O' && g_tModS.RxBuf[2] == 'M'
	     && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'B' && g_tModS.RxBuf[6] == 'E'
		 && g_tModS.RxBuf[7] == 'E' && g_tModS.RxBuf[8] == 'P' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == 'O'
		 && g_tModS.RxBuf[11] == 'F' && g_tModS.RxBuf[12] == 'F')
	{
		Jk516save.Set_Data.beep=0;//�ر�Ѷ��
		if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(8);
		}
	}else if(g_tModS.RxBuf[0] == 'C' && g_tModS.RxBuf[1] == 'O' && g_tModS.RxBuf[2] == 'M'
	     && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'B' && g_tModS.RxBuf[6] == 'E'
		 && g_tModS.RxBuf[7] == 'E' && g_tModS.RxBuf[8] == 'P' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == 'G'
		 && g_tModS.RxBuf[11] == 'D')
	{
		Jk516save.Set_Data.beep=1;//�ϸ�Ѷ��
		if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(8);
		}
	}else if(g_tModS.RxBuf[0] == 'C' && g_tModS.RxBuf[1] == 'O' && g_tModS.RxBuf[2] == 'M'
	     && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'B' && g_tModS.RxBuf[6] == 'E'
		 && g_tModS.RxBuf[7] == 'E' && g_tModS.RxBuf[8] == 'P' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == 'N'
		 && g_tModS.RxBuf[11] == 'G')
	{
		Jk516save.Set_Data.beep=2;//���ϸ�Ѷ��
		if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(8);
		}
	}else if(g_tModS.RxBuf[0] == 'C' && g_tModS.RxBuf[1] == 'O' && g_tModS.RxBuf[2] == 'M'
	     && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'B' && g_tModS.RxBuf[6] == 'E'
		 && g_tModS.RxBuf[7] == 'E' && g_tModS.RxBuf[8] == 'P' && g_tModS.RxBuf[9] == '?')
	{
		if(Jk516save.Set_Data.beep == 0)
		{
			uart1SendChars("OFF",3);
		}else if(Jk516save.Set_Data.beep == 1){
			uart1SendChars("GD",2);
		}else if(Jk516save.Set_Data.beep == 2){
			uart1SendChars("NG",2);
		}//��ѯѶ��
	}else if(g_tModS.RxBuf[0] == 'C' && g_tModS.RxBuf[1] == 'O' && g_tModS.RxBuf[2] == 'M'
	     && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'T' && g_tModS.RxBuf[6] == 'O'
		 && g_tModS.RxBuf[7] == 'L' && g_tModS.RxBuf[8] == ':' && g_tModS.RxBuf[9] == 'R' && g_tModS.RxBuf[10] == 'N'
		 && g_tModS.RxBuf[11] == 'O' && g_tModS.RxBuf[12] == 'M' && g_tModS.RxBuf[13] == ' ')//���ñ�Ƶ���
	{
		GetScpiNum(13,1);
		if(GetSystemStatus() == SYS_STATUS_SETUP)
			DispSet_value(9);
	}else if(g_tModS.RxBuf[0] == 'C' && g_tModS.RxBuf[1] == 'O' && g_tModS.RxBuf[2] == 'M'
	     && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'T' && g_tModS.RxBuf[6] == 'O'
		 && g_tModS.RxBuf[7] == 'L' && g_tModS.RxBuf[8] == ':' && g_tModS.RxBuf[9] == 'V' && g_tModS.RxBuf[10] == 'N'
		 && g_tModS.RxBuf[11] == 'O' && g_tModS.RxBuf[12] == 'M' && g_tModS.RxBuf[13] == ' ')//���ñ�Ƶ�ѹ
	{
		GetScpiNum(13,0);
		if(GetSystemStatus() == SYS_STATUS_SETUP)
		DispSet_value(11);
	}else if(g_tModS.RxBuf[0] == 'C' && g_tModS.RxBuf[1] == 'O' && g_tModS.RxBuf[2] == 'M'
	     && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'T' && g_tModS.RxBuf[6] == 'O'
		 && g_tModS.RxBuf[7] == 'L' && g_tModS.RxBuf[8] == ':' && g_tModS.RxBuf[9] == 'R' && g_tModS.RxBuf[10] == 'M'
		 && g_tModS.RxBuf[11] == 'L' && g_tModS.RxBuf[12] == 'T' && g_tModS.RxBuf[13] == ' ')//���õ���������
	{
		GetScpiNum(13,2);
		if(GetSystemStatus() == SYS_STATUS_SETUP)
		{
			DispSet_value(4);
			DispSet_value(10);
		}else if(GetSystemStatus() == SYS_STATUS_TEST){
			Disp_Test_value(2);
			Disp_Test_value(3);
		}
	}else if(g_tModS.RxBuf[0] == 'C' && g_tModS.RxBuf[1] == 'O' && g_tModS.RxBuf[2] == 'M'
	     && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'T' && g_tModS.RxBuf[6] == 'O'
		 && g_tModS.RxBuf[7] == 'L' && g_tModS.RxBuf[8] == ':' && g_tModS.RxBuf[9] == 'V' && g_tModS.RxBuf[10] == 'M'
		 && g_tModS.RxBuf[11] == 'L' && g_tModS.RxBuf[12] == 'T' && g_tModS.RxBuf[13] == ' ')//���õ�ѹ������
	{
		GetScpiNum(13,3);
		if(GetSystemStatus() == SYS_STATUS_SETUP)
		{
			DispSet_value(6);
			DispSet_value(12);
		}else if(GetSystemStatus() == SYS_STATUS_TEST){
			Disp_Test_value(5);
			Disp_Test_value(6);
		}
	}else if(g_tModS.RxBuf[0] == 'C' && g_tModS.RxBuf[1] == 'O' && g_tModS.RxBuf[2] == 'M'
	     && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'T' && g_tModS.RxBuf[6] == 'O'
		 && g_tModS.RxBuf[7] == 'L' && g_tModS.RxBuf[8] == ':' && g_tModS.RxBuf[9] == 'R' && g_tModS.RxBuf[10] == 'N'
		 && g_tModS.RxBuf[11] == 'O' && g_tModS.RxBuf[12] == 'M' && g_tModS.RxBuf[13] == '?')//��ѯ��Ƶ���
	{
		scpisend1 = (float)Jk516save.Set_Data.Nominal_Res.Num/pow(10,Jk516save.Set_Data.Nominal_Res.Dot);
	
		if(Jk516save.Set_Data.Nominal_Res.Unit == 0)
		{
			scpisendE1 = -3;
			sprintf(scpisendbuf,"%.4fE%02d",scpisend1,scpisendE1);
		}else if(Jk516save.Set_Data.Nominal_Res.Unit == 1){
			scpisendE1 = 0;
			sprintf(scpisendbuf,"%.4f+E%02d",scpisend1,scpisendE1);
		}else if(Jk516save.Set_Data.Nominal_Res.Unit == 2){
			scpisendE1 = 3;
			sprintf(scpisendbuf,"%.4fE%+02d",scpisend1,scpisendE1);
		}				
		
		for(i=0;i < sizeof(scpisendbuf);i++)
		{
			if(scpisendbuf[i] == 'E')
			{ 
				scpisendcount = i+3;
			}
		}
		uart1SendChars(scpisendbuf,scpisendcount);
	}else if(g_tModS.RxBuf[0] == 'C' && g_tModS.RxBuf[1] == 'O' && g_tModS.RxBuf[2] == 'M'
	     && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'T' && g_tModS.RxBuf[6] == 'O'
		 && g_tModS.RxBuf[7] == 'L' && g_tModS.RxBuf[8] == ':' && g_tModS.RxBuf[9] == 'V' && g_tModS.RxBuf[10] == 'N'
		 && g_tModS.RxBuf[11] == 'O' && g_tModS.RxBuf[12] == 'M' && g_tModS.RxBuf[13] == '?')//��ѯ��Ƶ���
	{
		scpisend1 = (float)Jk516save.Set_Data.Nominal_V.Num/pow(10,Jk516save.Set_Data.Nominal_V.Dot);
	
//		if(Jk516save.Set_Data.Nominal_V.Unit == 0)
//		{
//			scpisendE = -3;
//			sprintf(scpisendbuf,"%.4fE%02d",scpisend1,scpisendE);
//		}else if(Jk516save.Set_Data.Nominal_Res.Unit == 1){
//			scpisendE = 0;
//			sprintf(scpisendbuf,"%.4f+E%02d",scpisend1,scpisendE);
//		}else if(Jk516save.Set_Data.Nominal_Res.Unit == 2){
//			scpisendE = 3;
//			sprintf(scpisendbuf,"%.4fE%+02d",scpisend1,scpisendE);
//		}				
//		
		sprintf(scpisendbuf,"%.4fE+0",scpisend1);
		for(i=0;i < sizeof(scpisendbuf);i++)
		{
			if(scpisendbuf[i] == 'E')
			{
				scpisendcount = i+3;
			}
		}
		
		uart1SendChars(scpisendbuf,scpisendcount);
	}else if(g_tModS.RxBuf[0] == 'C' && g_tModS.RxBuf[1] == 'O' && g_tModS.RxBuf[2] == 'M'
	     && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'T' && g_tModS.RxBuf[6] == 'O'
		 && g_tModS.RxBuf[7] == 'L' && g_tModS.RxBuf[8] == ':' && g_tModS.RxBuf[9] == 'R' && g_tModS.RxBuf[10] == 'L'
		 && g_tModS.RxBuf[11] == 'M' && g_tModS.RxBuf[12] == 'T' && g_tModS.RxBuf[13] == '?')//��ѯ��Ƶ���
	{
		scpisend1 = (float)Jk516save.Set_Data.Res_low.Num/pow(10,Jk516save.Set_Data.Res_low.Dot);						
		scpisend2 = (float)Jk516save.Set_Data.High_Res.Num/pow(10,Jk516save.Set_Data.High_Res.Dot);
		
		if(Jk516save.Set_Data.Res_low.Unit == 0)
		{
			scpisendE1 = -3;
		}else if(Jk516save.Set_Data.Res_low.Unit == 1){
			scpisendE1 = 0;
		}else if(Jk516save.Set_Data.Res_low.Unit == 2){
			scpisendE1 = 3;
		}
	
		if(Jk516save.Set_Data.High_Res.Unit == 0)
		{
			scpisendE2 = -3;
		}else if(Jk516save.Set_Data.High_Res.Unit == 1){
			scpisendE2 = 0;
		}else if(Jk516save.Set_Data.High_Res.Unit == 2){
			scpisendE2 = 3;
		}
		if(scpisendE1 >= 0 && scpisendE2 >= 0)
		{
			sprintf(scpisendbuf,"%.4fE+%02d,%.4fE+%02d",scpisend1,scpisendE1,scpisend2,scpisendE2);
		}else if(scpisendE1 >= 0 && scpisendE2 < 0)
		{
			sprintf(scpisendbuf,"%.4fE+%02d,%.4fE%02d",scpisend1,scpisendE1,scpisend2,scpisendE2);
		}else if(scpisendE1 < 0 && scpisendE2 >= 0)
		{
			sprintf(scpisendbuf,"%.4fE%02d,%.4fE+%02d",scpisend1,scpisendE1,scpisend2,scpisendE2);
		}else{
			sprintf(scpisendbuf,"%.4fE%02d,%.4fE%02d",scpisend1,scpisendE1,scpisend2,scpisendE2);
		}
		for(i=0;i < sizeof(scpisendbuf);i++)
		{
			if(scpisendbuf[i] == 'E')
			{
				scpisendcount = i+4;
			}
		}
		uart1SendChars(scpisendbuf,scpisendcount);
	}else if(g_tModS.RxBuf[0] == 'C' && g_tModS.RxBuf[1] == 'O' && g_tModS.RxBuf[2] == 'M'
	     && g_tModS.RxBuf[3] == 'P' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'T' && g_tModS.RxBuf[6] == 'O'
		 && g_tModS.RxBuf[7] == 'L' && g_tModS.RxBuf[8] == ':' && g_tModS.RxBuf[9] == 'V' && g_tModS.RxBuf[10] == 'L'
		 && g_tModS.RxBuf[11] == 'M' && g_tModS.RxBuf[12] == 'T' && g_tModS.RxBuf[13] == '?')//��ѯ��Ƶ���
	{
		scpisend1 = (float)Jk516save.Set_Data.V_low.Num/pow(10,Jk516save.Set_Data.V_low.Dot);
		scpisend2 = (float)Jk516save.Set_Data.V_high.Num/pow(10,Jk516save.Set_Data.V_high.Dot);
	
//		if(Jk516save.Set_Data.Nominal_V.Unit == 0)
//		{
//			scpisendE = -3;
//			sprintf(scpisendbuf,"%.4fE%02d",scpisend1,scpisendE);
//		}else if(Jk516save.Set_Data.Nominal_Res.Unit == 1){
//			scpisendE = 0;
//			sprintf(scpisendbuf,"%.4f+E%02d",scpisend1,scpisendE);
//		}else if(Jk516save.Set_Data.Nominal_Res.Unit == 2){
//			scpisendE = 3;
//			sprintf(scpisendbuf,"%.4fE%+02d",scpisend1,scpisendE);
//		}				
//		
		sprintf(scpisendbuf,"%.4fE+00,%.4fE+00",scpisend1,scpisend2);
		for(i=0;i < sizeof(scpisendbuf);i++)
		{
			if(scpisendbuf[i] == 'E')
			{
				scpisendcount = i+4;
			}
		}
		
		uart1SendChars(scpisendbuf,scpisendcount);
	}else if(g_tModS.RxBuf[0] == 'S' && g_tModS.RxBuf[1] == 'Y' && g_tModS.RxBuf[2] == 'S'
	     && g_tModS.RxBuf[3] == 'T' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'L' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'N' && g_tModS.RxBuf[8] == 'G' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == 'E'
		 && g_tModS.RxBuf[11] == 'N')//������ʾ����
	{
		Jk516save.Sys_Setvalue.lanage=1;
	}else if(g_tModS.RxBuf[0] == 'S' && g_tModS.RxBuf[1] == 'Y' && g_tModS.RxBuf[2] == 'S'
	     && g_tModS.RxBuf[3] == 'T' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'L' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'N' && g_tModS.RxBuf[8] == 'G' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == 'C'
		 && g_tModS.RxBuf[11] == 'N')//������ʾ����
	{
		Jk516save.Sys_Setvalue.lanage=0;
	}else if(g_tModS.RxBuf[0] == 'S' && g_tModS.RxBuf[1] == 'Y' && g_tModS.RxBuf[2] == 'S'
	     && g_tModS.RxBuf[3] == 'T' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'L' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'N' && g_tModS.RxBuf[8] == 'G' && g_tModS.RxBuf[9] == ' ' && g_tModS.RxBuf[10] == 'E'
		 && g_tModS.RxBuf[11] == 'N')//������ʾ����
	{
		Jk516save.Sys_Setvalue.lanage=1;
	}else if(g_tModS.RxBuf[0] == 'S' && g_tModS.RxBuf[1] == 'Y' && g_tModS.RxBuf[2] == 'S'
	     && g_tModS.RxBuf[3] == 'T' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'L' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'N' && g_tModS.RxBuf[8] == 'G' && g_tModS.RxBuf[9] == '?')//������ʾ����
	{
		if(Jk516save.Sys_Setvalue.lanage == 0)
		{
			uart1SendChars("CN",2);
		}else if(Jk516save.Sys_Setvalue.lanage == 1){
			uart1SendChars("EN",2);
		}//��ѯ��ʾ����
	}else if(g_tModS.RxBuf[0] == 'S' && g_tModS.RxBuf[1] == 'Y' && g_tModS.RxBuf[2] == 'S'
	     && g_tModS.RxBuf[3] == 'T' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'B' && g_tModS.RxBuf[6] == 'A'
		 && g_tModS.RxBuf[7] == 'U' && g_tModS.RxBuf[8] == 'D' && g_tModS.RxBuf[9] == ' ')//���ò�����
	{
		if(g_tModS.RxBuf[10] == '2' && g_tModS.RxBuf[11] == '4' && g_tModS.RxBuf[12] == '0' && g_tModS.RxBuf[13] == '0')
		{
			Jk516save.Sys_Setvalue.buard=0;
			if(GetSystemStatus() == SYS_STATUS_SYSSET)
			Disp_Sys_value(2);
		}else if(g_tModS.RxBuf[10] == '4' && g_tModS.RxBuf[11] == '8' && g_tModS.RxBuf[12] == '0' && g_tModS.RxBuf[13] == '0')
		{
			Jk516save.Sys_Setvalue.buard=1;
			if(GetSystemStatus() == SYS_STATUS_SYSSET)
			Disp_Sys_value(2);
		}else if(g_tModS.RxBuf[10] == '9' && g_tModS.RxBuf[11] == '6' && g_tModS.RxBuf[12] == '0' && g_tModS.RxBuf[13] == '0')
		{
			Jk516save.Sys_Setvalue.buard=2;
			if(GetSystemStatus() == SYS_STATUS_SYSSET)
			Disp_Sys_value(2);
		}else if(g_tModS.RxBuf[10] == '1' && g_tModS.RxBuf[11] == '4' && g_tModS.RxBuf[12] == '4' && g_tModS.RxBuf[13] == '0' && g_tModS.RxBuf[14] == '0')
		{
			Jk516save.Sys_Setvalue.buard=3;
			if(GetSystemStatus() == SYS_STATUS_SYSSET)
			Disp_Sys_value(2);
		}else if(g_tModS.RxBuf[10] == '1' && g_tModS.RxBuf[11] == '9' && g_tModS.RxBuf[12] == '2' && g_tModS.RxBuf[13] == '0' && g_tModS.RxBuf[14] == '0')
		{
			Jk516save.Sys_Setvalue.buard=4;
			if(GetSystemStatus() == SYS_STATUS_SYSSET)
			Disp_Sys_value(2);
		}
	}else if(g_tModS.RxBuf[0] == 'S' && g_tModS.RxBuf[1] == 'Y' && g_tModS.RxBuf[2] == 'S'
	     && g_tModS.RxBuf[3] == 'T' && g_tModS.RxBuf[4] == ':' && g_tModS.RxBuf[5] == 'U' && g_tModS.RxBuf[6] == 'D'
		 && g_tModS.RxBuf[7] == 'S' && g_tModS.RxBuf[8] == 'K' && g_tModS.RxBuf[9] == ' ')//����U�̿���
	{
		if(g_tModS.RxBuf[10] == 'O' && g_tModS.RxBuf[11] == 'N')
		{
			Jk516save.Sys_Setvalue.u_flag=1;
			if(GetSystemStatus() == SYS_STATUS_SYSSET)
			Disp_Sys_value(3);
		}else if(g_tModS.RxBuf[10] == 'O' && g_tModS.RxBuf[11] == 'F' && g_tModS.RxBuf[11] == 'F')
		{
			Jk516save.Sys_Setvalue.u_flag=0;
			if(GetSystemStatus() == SYS_STATUS_SYSSET)
			Disp_Sys_value(3);
		}
	}
	
	
	
	
	
    switch(g_tModS.RxBuf[1])
    {
        case 0x03:
        {
            MODS_03H();
        }break;
        case 0x06:
        {
            MODS_06H();
        }break;
        default:break;
    }

		memset(g_tModS.RxBuf,0,30);
}

static uint8_t MODS_ReadRegValue(uint16_t reg_addr, uint8_t *reg_value)
{
    uint16_t value;
	static u32 sendunit;
	static u32 sendrvalue;
	static u32 sendvvalue;
	switch(Range)
	{
		case 0:
		{
			sendrvalue = Test_Value.res;
			sendunit = 0;
		}break;
		case 1:
		{
			sendrvalue = Test_Value.res * 10;
			sendunit = 0;
		}break;
		case 2:
		{
			sendrvalue = Test_Value.res * 100;
			sendunit = 0;
		}break;
		case 3:
		{
			sendrvalue = Test_Value.res;
			sendunit = 1;
		}break;
		case 4:
		{
			sendrvalue = Test_Value.res * 10;
			sendunit = 1;
		}break;
		case 5:
		{
			sendrvalue = Test_Value.res * 100;
			sendunit = 1;
		}break;
		case 6:
		{
			sendrvalue = Test_Value.res;
			sendunit = 2;
		}break;
		
	}
	sendvvalue = Test_Value_V.res;
	if(open_flag == 1)
	{
		sendrvalue = 0xffffffff;	
		sendvvalue = 0xffffffff;
	}
//	lock = 1;
	switch (reg_addr)									/* ??????? */
	{
        case SLAVE_REG_P00:
			value = (u16)(sendrvalue >> 16);	
			break;
		case SLAVE_REG_P01:
            value = (u16)(sendrvalue);	
			break;

		case SLAVE_REG_P02:
			value = (u16)(sendvvalue >> 16);
			break;
		case SLAVE_REG_P03: 
			value = (u16)(sendvvalue);
			break;

		case SLAVE_REG_P04:
			value = Test_Value_V.polar;		/* ??????? */
			break;
		case SLAVE_REG_P05:
			value = sendunit;
			break;
		case SLAVE_REG_P06:
			value = 0;							/* ??????? */
			break;
		case SLAVE_REG_P07:
			value =	0;							/* ??????? */
			break;
		case SLAVE_REG_P08:
			value =	0;	
			break;

		case SLAVE_REG_P09:
			value =	0;							/* ??????? */
			break;
		case SLAVE_REG_P10:
			value =	0;	
			break;

		case SLAVE_REG_P11:
			value =	0;
			break;
		case SLAVE_REG_P12:
			value =	0;	
			break;

		case SLAVE_REG_P13:
			value =	0;							/* ??????? */
			break;
		case SLAVE_REG_P14:
			value =	0;
			break;

		case SLAVE_REG_P15:
			value =	0;
			break;
		case SLAVE_REG_P16:
			value =	0;	
			break;

		case SLAVE_REG_P17:
			value =	0;							/* ??????? */
			break;
		case SLAVE_REG_P18:
			value =	0;							/* ??????? */
			break;	
		default:
			return 0;
    }
	reg_value[0] = value >> 8;
	reg_value[1] = value;

	return 1;											/* ???? */
}

static void RemTrig(void)
{
	test_start = 1;
}

static void MODS_03H(void)
{
    uint16_t reg;
	uint16_t num;
	uint16_t i;
	uint8_t reg_value[64];

    
    g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)								/* 03H????��8??? */
	{
		g_tModS.RspCode = RSP_ERR_VALUE;					/* ?????? */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 				/* ???? */
	num = BEBufToUint16(&g_tModS.RxBuf[4])*2;					/* ????? */
	if (num > sizeof(reg_value) / 2)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;					/* ?????? */
		goto err_ret;
	}

	for (i = 0; i < num; i++)
	{
		if (MODS_ReadRegValue(reg, &reg_value[2 * i]) == 0)	/* ????????reg_value */
		{
			g_tModS.RspCode = RSP_ERR_REG_ADDR;				/* ??????? */
			break;
		}
		reg++;
	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)							/* ???? */
	{
		g_tModS.TxCount = 0;
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[0];
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1];
		g_tModS.TxBuf[g_tModS.TxCount++] = num * 2;			/* ????? */

		for (i = 0; i < num; i++)
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = reg_value[2*i];
			g_tModS.TxBuf[g_tModS.TxCount++] = reg_value[2*i+1];
		}
//         if(usartocflag == 0)
//         {
             MODS_SendWithCRC(g_tModS.TxBuf, g_tModS.TxCount);	/* ?????? */
//         }
	}
	else
	{
//		MODS_SendAckErr(g_tModS.RspCode);					/* ?????? */
	}
    
}

static void MODS_06H(void)
{
    uint16_t reg;
	uint16_t value;

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ?????? */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* ???? */
	value = BEBufToUint16(&g_tModS.RxBuf[4]);	/* ???? */
    
    if(reg == 0x0E)
    {
        if(value == 00)
        {
            Jk516save.Set_Data.Range=RANGE_MAX;//0  �Զ�  10   �������
			Jk516save.Set_Data.Range_Set=0;
        }else{
            Jk516save.Set_Data.Range_Set=1;
			Jk516save.Set_Data.Range=value-1;
			Range=Jk516save.Set_Data.Range;
			Range_Control(Range,V_Range);
        }
		if(GetSystemStatus() == SYS_STATUS_TEST)
		{
			Disp_Test_value(4);
		}else if(GetSystemStatus() == SYS_STATUS_SETUP){
			DispSet_value(7);
		}
    }
// 	if (MODS_WriteRegValue(reg, value) == 1)	/* ????�ק�???????? */
// 	{
// 		;
// 	}
// 	else
// 	{
// 		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* ??????? */
// 	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)				/* ???? */
	{
//		MODS_SendAckOk();
	}
	else
	{
//		MODS_SendAckErr(g_tModS.RspCode);		/* ???????? */
	}
}


void  TIM2_IRQHandler (void)//U�����˶�ʱ��2
{
    USB_OTG_BSP_TimerIRQ();
	 	
}

void  BASIC_TIM_IRQHandler (void)
{
    static u8 num=0;
    u8 i;
	if ( TIM_GetITStatus( BASIC_TIM, TIM_IT_Update) != RESET ) 
	{	
//		LED1_TOGGLE;
		TIM_ClearITPendingBit(BASIC_TIM , TIM_IT_Update);  		
//        Key_Scan();
        //????
//        if(SystemStatus==SYS_STATUS_CLEAR)
//        {
//            num++;
//            if(num>9)//10mS??
//            {
//                num=0;
//                F_100ms=TRUE;//100ms????
//                //Test_value.Clear_Time++;
//            }
//        
//        }else
		Tick_10ms ++;
        MODS_Poll();
        if(SystemStatus==SYS_STATUS_TEST)
        {
            num++;
            if(num>9)//10mS??
            {
                num=0;
                F_100ms=TRUE;//100ms????
                Test_value.Test_Time++;
                //Voltage++;
            }
//            if(Jk516save.Sys_Setvalue.u_flag)
//            {
//                
//                USBH_Process(&USB_OTG_Core, &USB_Host);
//            }
        }
        else
        {
            if(num!=0)
                num=0;
        }
        Key_Scan();	 
       
        for (i=0;i<MAXTSOFTTIMER;i++)
        {
            if (SoftTimer[i])
                SoftTimer[i]--;
            
        }	
        
	
	}		 	
}
void KEY1_IRQHandler(void)//���ػ�
{
  //ȷ���Ƿ������EXTI Line�ж�
//    u32 i;
	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
	{
		delay_ms(1000);
       
        if(GPIO_ReadInputDataBit( GPIOE,  GPIO_Pin_3)==0)
        {
            Int_Pe3flag=0;
            if(softswitch)
            {
                softswitch=0;
                SetSystemStatus(SYS_STATUS_TOOL);
            
            }else
            {
                softswitch=1;
                
                SetSystemStatus(SYS_STATUS_POWER);
            
            }
            NVIC_DisableIRQ(EXTI3_IRQn);
        }
		//GPIO_ClearInt(0, 1<<19);
		
		EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);     
	}  
}

void EXTI15_10_IRQHandler(void)
{
  //ȷ���Ƿ������EXTI Line�ж�
	if(EXTI_GetITStatus(EXTI_Line13) != RESET) 
	{
		// LED2 ȡ��		
		//LED2_TOGGLE;
    //����жϱ�־λ
        //Read__Convert_read();
		EXTI_ClearITPendingBit(EXTI_Line13);     
	}  
}
#ifdef USE_USB_OTG_FS  
void OTG_FS_IRQHandler(void)
#else
void OTG_HS_IRQHandler(void)
    
#endif
{
  USBH_OTG_ISR_Handler(&USB_OTG_Core);
}

/**	������⣬Ӳ����֧��
  * @brief  EXTI1_IRQHandler
  *         This function handles External line 1 interrupt request.
  * @param  None
  * @retval None
  */
#if 0
void EXTI1_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line1) != RESET)
  {
      USB_Host.usr_cb->OverCurrentDetected();
      EXTI_ClearITPendingBit(EXTI_Line1);
  }
}


#endif
/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                         */
/******************************************************************************/

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
