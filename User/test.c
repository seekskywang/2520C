#include "pbdata.h"
#include <math.h>
#include "ff.h"
#include "cpld.h"
#include "./APP/usbh_bsp.h"
#include "./RTC/bsp_rtc.h"
#include "bsp_exti.h"
#include "open.h"
#include "flash_if.h"


//�����л�
// ����0 1 2 3  U16_4094
// 0  1000V
//1   100V
//2 10V 
//==========================================================
#define POWERON_DISP_TIME (20)	//������ʾ������ʱ20*100mS=2s
u8 U15_4094,U16_4094;

FRESULT result;
FATFS fs;
FIL file;
u16 startdelay;
//const u8 RANGE_UNIT[11]=
//{
//	4,
//	3,
//	2,
//	1,
//	3,
//	2,
//	1,
//	3,
//	2,
//	1,
//	3
//	
void JumpBoot(u8 flag)
{
  	void (*pUserApp)(void);
  uint32_t JumpAddress;
	if(flag==55)
  {		
	__asm("CPSID  I");
        
		JumpAddress = *(volatile uint32_t*) (USER_FLASH_FIRST_PAGE_ADDRESS+4);
		pUserApp = (void (*)(void)) JumpAddress;
		TIM_Cmd(BASIC_TIM, DISABLE);	
		TIM_DeInit(TIM2);
		TIM_Cmd(TIM2,DISABLE);
		TIM_DeInit(BASIC_TIM);
		TIM_ITConfig(BASIC_TIM,TIM_IT_Update,DISABLE);
		TIM_Cmd(BASIC_TIM, DISABLE);	
		USART_DeInit(DEBUG_USART);
		USART_ITConfig(DEBUG_USART, USART_IT_RXNE, DISABLE);		
		USART_Cmd(DEBUG_USART,DISABLE);
		RCC_DeInit();
		RCC_RTCCLKCmd(DISABLE);
		EXTI_DeInit();
		SysTick->CTRL = 0;
		RTC_DeInit();
		RTC_ITConfig(RTC_IT_WUT,DISABLE);//�ر�WAKE UP ��ʱ���ж�
		RTC_WakeUpCmd( DISABLE);//�ر�WAKE UP ��ʱ����
		Disable_Extiint();
		USBH_DeInit(&USB_OTG_Core,&USB_Host);
		__disable_irq();
		NVIC_DisableIRQ(OTG_FS_IRQn);
		NVIC_DisableIRQ(OTG_FS_WKUP_IRQn);
		NVIC_DisableIRQ(OTG_HS_IRQn);
		NVIC_DisableIRQ(OTG_HS_WKUP_IRQn);
		__ASM volatile ("cpsid i");
		/* Initialize user application's Stack Pointer */
		__set_PSP(*(volatile uint32_t*) USER_FLASH_FIRST_PAGE_ADDRESS);
		__set_CONTROL(0);
		__set_MSP(*(volatile uint32_t*) USER_FLASH_FIRST_PAGE_ADDRESS);
		
        
		
//		NVIC_SystemReset();
		pUserApp();
	}
}

//};
//==========================================================
//�������ƣ�Power_Process
//�������ܣ��ϵ紦��
//��ڲ�������
//���ڲ�������
//�������ڣ�2015.10.26
//�޸����ڣ�2015.10.26 08:53
//��ע˵������������SET����У׼����ģʽ
//==========================================================
//void DIS_Line(void)
//{
//  uint16_t i,j;
//  
//  uint32_t *p = (uint32_t *)(LCD_LAYER2_START_ADDR );
//  
//  for(j=0;j<480;j++)
//  for(i=0;i<272;i++)
//  {
//    //ARGB8888 
//    *p = 0x7Fff00;
//    p++;
//    }
//  
//  
//}  
void Power_Process(void)
{
	u16 i;
    u8 j;

    Disp_Coordinates_Typedef Debug_Cood;
   
	u8 key;
    Int_Pe3flag=0;
    Turnon_Led();
	
    RTC_CLK_Config();
    RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);		//����WAKE UP�ж�,1�����ж�һ��
    GPIO_Configuration();//�˿ڳ�ʼ��
     if (RTC_ReadBackupRegister(RTC_BKP_DRX) != RTC_BKP_DATA)
      {
        /* ����ʱ������� */
            RTC_TimeAndDate_Set();
      }
      else
      {
        /* ����Ƿ��Դ��λ */   
        /* ʹ�� PWR ʱ�� */
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
            /* PWR_CR:DBF��1��ʹ��RTC��RTC���ݼĴ����ͱ���SRAM�ķ��� */
            PWR_BackupAccessCmd(ENABLE);
            /* �ȴ� RTC APB �Ĵ���ͬ�� */
            RTC_WaitForSynchro();   
      } 
    SysTick_Init();
    delay_ms(10);
    
    Keyboard_Init();//������ʼ��


    //////////////////////////////////////////Һ����ʼ��
    LCD_Init();
    LCD_LayerInit();
    LTDC_Cmd(ENABLE);
	/*�ѱ�����ˢ��ɫ*/
    LCD_SetLayer(LCD_BACKGROUND_LAYER);  
    LCD_Clear(LCD_COLOR_RED);
	
  /*��ʼ����Ĭ��ʹ��ǰ����*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
//	/*Ĭ�����ò�͸��	���ú�������Ϊ��͸���ȣ���Χ 0-0xff ��0Ϊȫ͸����0xffΪ��͸��*/
    LCD_SetTransparency(0xff);
	LCD_Clear(LCD_COLOR_TEST_BACK);
    SPI_FLASH_Init();

	InitGlobalValue();//��ʼ��ȫ�ֱ���
	Read_set_flash();
    
	Parameter_valuecomp();//�Ƚ϶���������
	if(Jk516save.Sys_Setvalue.buard == 0)
	{
		Debug_USART_Config(4800);//����1
	}else if(Jk516save.Sys_Setvalue.buard == 1){
		Debug_USART_Config(9600);//����1
	}else if(Jk516save.Sys_Setvalue.buard == 2){
		Debug_USART_Config(14400);//����1
	}else if(Jk516save.Sys_Setvalue.buard == 3){
		Debug_USART_Config(19200);//����1
	}else if(Jk516save.Sys_Setvalue.buard == 4){
		Debug_USART_Config(115200);//����1
	}
	if(Jk516save.open == 1)
	{
		lcd_image((uint8_t *)gImage_open);
	}else{
		
	}
    TIM6_Configuration();//��ʱ��6��ʱ10ms

	MenuIndex=0;//�����˵���
    EXTI_Ads1251_Config();//�ж�û�п���

	i=0;//��ʾ��ʱ
    open_flag=1;
    Range=6;
    Range_Control(Range,0);
    RangeChange_Flag=1;
     USBH_Init(&USB_OTG_Core,
			USB_OTG_HS_CORE_ID,
            &USB_Host,
            &USBH_MSC_cb,
            &USR_cb);
     USBH_Process(&USB_OTG_Core, &USB_Host);
	while(GetSystemStatus()==SYS_STATUS_POWER)
	{
		i++;
        j++;
       
		if(i>POWERON_DISP_TIME && Jk516save.open == 1)//��ʱ20*100mS=2s���Զ��˳�
            SetSystemStatus(SYS_STATUS_TEST);//����״̬
		else
			SetSystemStatus(SYS_STATUS_TEST);//����״̬

        
		key=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/10);//�ȴ�����(100*10ms/10=100ms)

		switch(key)
		{
            case	    Key_F1:
                Trip_On();
                break;
            case		Key_F2:
                Trip_Off();
                break;
            case 	    Key_F3:
                Lock_On();
                break;
            case 	    Key_F4:
                Lock_Off();
                break;
            case		Key_F5:
                break;
            case		Key_Disp:
                Turnon_Led();
                break;
            case		Key_SETUP:
                Turnoff_Led();
                break;
            case		Key_FAST:
                    LCD_Clear(LCD_COLOR_TEST_BACK);
                    Debug_Cood.xpos=180;
                    Debug_Cood.ypos =120;
                    Debug_Cood.lenth=120;
                    input_password(&Debug_Cood);
                break;
            case		Key_LEFT:
                break;
            case		Key_RIGHT:
                break;
            case		Key_UP:
                Led_Pass_On();
                break;
            case		Key_DOWN:
                Led_Fail_On();
                break;
            case		Key_NUM7:
                break;
            case		Key_NUM8:
                break;
            case		Key_NUM9:
                break;
            case		Key_NUM4:
                break;
            case		Key_NUM5:
                break;
            case		Key_NUM6:
                break;
            case		Key_NUM1:
                break;
            case		Key_NUM2:
                break;
            case		Key_NUM3:
                break;
            case		Key_NUM0:
                break;
            case		Key_DOT:
                break;
            case		Key_BACK:
                break;
            case		Key_LOCK:
                break;
            case		Key_BIAS:
                break;
            case		Key_REST:
                break;
            case		Key_TRIG:
                break;
            case		Key_POWER:
                break;
		}
		//Range_Control(3);
	}
//	BeepOff();
}

//==========================================================
//�������ƣ�Idle_Process
//�������ܣ�����������
//��ڲ�������
//���ڲ�������
//�������ڣ�2015.10.26
//�޸����ڣ�2015.10.26 08:59
//��ע˵������
//==========================================================
//void Idle_Process(void)
//{
//	u8 key;
//	u8 disp_flag=0;
//	u8 group;

//	//ϵͳ��Ϣ����
//	SetSystemMessage(MSG_IDLE);//ϵͳ��Ϣ-����
//	Test_value.Test_Time=0;

//	
//	while(GetSystemStatus()==SYS_STATUS_IDLE)
//	{
//		//Uart_Process();//���ڴ���
////		Led_Pass_On();
////		Led_Pass_Off();
////		Led_Fail_On();
////		Led_Fail_Off();
////		Led_HV_On();
//		//Range_Control(0);
//		//Range_Control(1);
//		//Range_Control(2);
//		//Range_Control(3);
////		LcdAddr.x=6;//��ʾ��ַ
////			LcdAddr.y=0;
////			Hex_Format(Test_Time,2,4,TRUE);//��ֵ��ʽ����4λ��ֵ
////			Disp_StrAt(DispBuf);//��ʾ�˵�ֵ
//		if(disp_flag)
//		{
//			disp_flag=0;
////			Read_compvalue(group-1);
////			Disp_Idle_Menu();//��ʾ�������
//			//Store_set_flash(SaveData.group-1);
//		
//		}
//		key=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/10);//�ȴ�����(100*10ms/10=100ms)
//		switch(key)
//		{
//			case KEY_SET:	//���ü�
//			case L_KEY_SET:	//�������ü�
//				SetSystemStatus(SYS_STATUS_SETUP);//����״̬
//				break;
//	
//			case KEY_UP:	//�ϼ�
//				group=group<5?++group:1;
//				SaveData.group=group;
//				disp_flag=1;
////			case L_KEY_UP:	//�����ϼ�
//				break;
//	
//			case KEY_DOWN:		//�¼�
//				group=group>1?--group:5;
//				SaveData.group=group;
//				disp_flag=1;
////			case L_KEY_DOWN:	//�����¼�
//				break;
//	
//			case KEY_LEFT:		//���
//			case L_KEY_LEFT:	//�������
//				break;

//			case KEY_RIGHT:		//�Ҽ�
//			case L_KEY_RIGHT:	//�����Ҽ�
//				break;

//			case KEY_START:		//������
//			case L_KEY_START:	//����������
////				if(SaveData.System.Uart!=TRUE)//���ڿ�ʼʱ��������Ч
//					SetSystemStatus(SYS_STATUS_TEST);//��������״̬
//				break;
//	
//			case KEY_ENTER:		//ȷ�ϼ�
//			case L_KEY_ENTER:	//����ȷ�ϼ�
//				break;
//	
//			case KEY_RESET:		//��λ��
//			case L_KEY_RESET:	//������λ��
////				Disp_Clr( );//����
////				Disp_Idle_Menu();//��ʾ�������
//				break;
//			
//			default:
//				break;
//		}
//	}
//}


//==========================================================
//�������ƣ�Setup_Process
//�������ܣ����ó���
//��ڲ�������
//���ڲ�������
//�������ڣ�2015.10.28
//�޸����ڣ�2015.10.28 10:45
//��ע˵������
//==========================================================
void Setup_Process(void)
{

	u8 keynum=0;
	Disp_Coordinates_Typedef  Coordinates;
//	Send_Ord_Typedef Uart;
	
	vu8 key;
	vu8 Disp_Flag=1;
	keynum=0;
    LCD_Clear(LCD_COLOR_TEST_BACK);
    Disp_Test_Set_Item();
 	while(GetSystemStatus()==SYS_STATUS_SETUP)
	{
	    
		if(Disp_Flag==1)
		{
			DispSet_value(keynum);
           
			Disp_Flag=0;
		
		}

        key=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/10);
        if(key!=KEY_NONE)
		{	Disp_Flag=1;
			switch(key)
			{
				case Key_F1:

					switch(keynum)
					{
						case 0:
							//if(Button_Page.page==0)
								SetSystemStatus(SYS_STATUS_TEST);//
//							else
//								SetSystemStatus(SYS_STATUS_FILE);
								
							break;
						case 1:
							Jk516save.Set_Data.trip=0;
							
							break;
						case 2:
							Jk516save.Set_Data.speed=0;//����
							
								
							
							break;
						case 3:
							Jk516save.Set_Data.Res_comp=0;
							//SaveData.Main_Func.Level=0;
							break;
						case 4:
                            Coordinates.xpos=LIST1+88;
                            Coordinates.ypos=FIRSTLINE+SPACE1*4;
                            Coordinates.lenth=76;
                            Jk516save.Set_Data.Res_low=Disp_Set_Num(&Coordinates);
                            
                    
							
							break;
						case 5:
							Jk516save.Set_Data.V_comp=0;
							break;
						case 6:
                            Coordinates.xpos=LIST1+88;
                            Coordinates.ypos=FIRSTLINE+SPACE1*7;
                            Coordinates.lenth=76;
                            Jk516save.Set_Data.V_low=Disp_Set_CompNum(&Coordinates);
							
							
							break;
						case 7:
                            Jk516save.Set_Data.Range_Set=0;
							Jk516save.Set_Data.Range=0;//�Զ�
							break;
						case 8:
							Jk516save.Set_Data.beep=0;
							break;
						case 9:
                            Coordinates.xpos=LIST2+88;
                            Coordinates.ypos=FIRSTLINE+SPACE1*3;
                            Coordinates.lenth=76;
                            Jk516save.Set_Data.Nominal_Res=Disp_Set_Num(&Coordinates);

							break;
						case 10:
                            Coordinates.xpos=LIST2+88;
                            Coordinates.ypos=FIRSTLINE+SPACE1*4;
                            Coordinates.lenth=76;
                            Jk516save.Set_Data.High_Res=Disp_Set_Num(&Coordinates);

							break;
						case 11:
                            Coordinates.xpos=LIST2+88;
                            Coordinates.ypos=FIRSTLINE+SPACE1*6;
                            Coordinates.lenth=76;
                            Jk516save.Set_Data.Nominal_V=Disp_Set_CompNum(&Coordinates);

							break;
						case 12:
                            Coordinates.xpos=LIST2+88;
                            Coordinates.ypos=FIRSTLINE+SPACE1*7;
                            Coordinates.lenth=76;
                            Jk516save.Set_Data.V_high=Disp_Set_CompNum(&Coordinates);

							break;
						
							
						default:
							break;
					
					
					}

				break;
				case Key_F2:
					

					switch(keynum)
					{
						case 0:
							//if(Button_Page.page==0)
								SetSystemStatus(SYS_STATUS_SETUP);
//							else
//								SetSystemStatus(SYS_STATUS_SYSSET);
								
							break;
						case 1:
							
							Jk516save.Set_Data.trip=1;//����
							
							
							break;
						case 2:
							Jk516save.Set_Data.speed=1;
							
							
							
							break;
						case 3:
							Jk516save.Set_Data.Res_comp=1;
							break;
						case 4:
//							if(Save_Res.Set_Data.Res_low.Num%1000<900)
//								Save_Res.Set_Data.Res_low.Num+=100;
//							else
//							{
//								//Save_Res.Set_Data.Res_low.Num%=100;
//								Save_Res.Set_Data.Res_low.Num-=900;
//								
//							}
							break;
						case 5:
							Jk516save.Set_Data.V_comp=1;
							break;
						case 6:
//							if(Save_Res.Set_Data.V_low.Num%1000<900)
//								Save_Res.Set_Data.V_low.Num+=100;
//							else
//								Save_Res.Set_Data.V_low.Num-=900;
//							Uart_Send_Flag=2;
							break;
						case 7:
                            Jk516save.Set_Data.Range_Set=1;
							//Jk516save.Set_Data.Range=10;//�ֶ�
							break;
						case 8:
							Jk516save.Set_Data.beep=1;
							break;

						default:
							break;
					
					
					}				
				

				break;
				case Key_F3:
					switch(keynum)
					{
						case 0:
								SetSystemStatus(SYS_STATUS_SYSSET);
							break;
						case 1:
                            Jk516save.Set_Data.trip=2;//����
							break;
						case 2:
                            Jk516save.Set_Data.speed=2;
							break;
						case 3:
							break;
						case 4:

							break;
						case 5:
							break;
						case 6:

							break;
						case 7:
//                            Jk516save.Set_Data.Range_Set=0;
//                            Range=Jisuan_Range(Jk516save.Set_Data.Nominal_Res);
                            
                            Jk516save.Set_Data.Range_Set=2;//���ӵ�λѡ��ļ���
                            Range=Jisuan_Range(Jk516save.Set_Data.Nominal_Res);
                            Jk516save.Set_Data.Range=Range;
                            Range_Control(Range,V_Range);
							//Jk516save.Set_Data.Range=11;//��ǰ���̼�������λ��
							break;
						case 8:
							Jk516save.Set_Data.beep=2;
							break;

						
						default:
							break;
					
					
					}	
					
				break;
				case Key_F4:
					switch(keynum)
					{
						case 0:
								SetSystemStatus(SYS_STATUS_SYS);
							break;
						case 1:
                            Jk516save.Set_Data.trip=3;//����
							
									
							break;
						case 2:
                            Jk516save.Set_Data.speed=3;
							
							break;
						case 3:
							break;
						case 4:

							break;
						case 5:
							break;
						case 6:
							if(Jk516save.Set_Data.V_low.Num%10<9)
								Jk516save.Set_Data.V_low.Num+=1;
							else
								Jk516save.Set_Data.V_low.Num-=9;
							break;
						case 7:
                            Jk516save.Set_Data.Range_Set=1;
                            if(Jk516save.Set_Data.Range<3)
                                Jk516save.Set_Data.Range++;
                            else
                                Jk516save.Set_Data.Range=0;
                            Range=Jk516save.Set_Data.Range;
							break;
						case 8:
							if(Jk516save.Set_Data.openbeep==0)
							{
								Jk516save.Set_Data.openbeep=1;
							}else{
								Jk516save.Set_Data.openbeep=0;
							}
							break;
						
						default:
							break;					
					}	
				
				break;
				case Key_F5:
					switch(keynum)
					{
						case 0:
							JumpBoot(55);
						break;
						case 4:
						case 9:
							break;
						case 7:	
							Jk516save.Set_Data.Range_Set=1;
                            if(Jk516save.Set_Data.Range)
							Jk516save.Set_Data.Range--;
                            else
                                Jk516save.Set_Data.Range=3;
                                
                            Range=Jk516save.Set_Data.Range;
						break;
						
						default:
							break;
					
					
					}
                    
					
				break;
				case Key_Disp:
                        SetSystemStatus(SYS_STATUS_TEST);
				break;
				case Key_SETUP:
                        //SetSystemStatus(SYS_STATUS_SETUPTEST);
				break;
				
				case Key_LEFT:
					if(keynum==0)
						keynum=12;
					else
					if(keynum>6)
						keynum-=6;
					else
						keynum+=5;
					
						
				break;
				case Key_RIGHT:
					if(keynum==0)
						keynum=1;
					else
					if(keynum<=6)
						keynum+=6;
					else
						keynum-=5;
					
						
				break;
				case Key_DOWN:
					if(keynum>11)
						keynum=0;
					else
						keynum++;
					
					
				break;
				case Key_UP:
					if(keynum<1)
						keynum=11;
					else
						keynum--;
					
				break;
				case Key_DOT:

					break;
				case Key_NUM1:
				//break;
				case Key_NUM2:
				//break;
				case Key_NUM3:
				//break;
				case Key_NUM4:
				//break;
				case Key_NUM5:
				//break;
				case Key_NUM6:
				//break;
				case Key_NUM7:
				//break;
				case Key_NUM8:
				//break;
				case Key_NUM9:
				//break;
				case Key_NUM0:
				break;
                case Key_FAST:
				
				
																							
					break;
				case Key_BACK:
				break;
				case Key_LOCK:
				break;
				case Key_BIAS:
				break;
				case Key_REST:
				break;
				case Key_TRIG:
				break;
				default:
				break;
					
			}
		
		
		}
	 	
	
	
	
	}
	Store_set_flash();
}

//==========================================================
//�������ƣ�Test_Process
//�������ܣ�����������
//��ڲ�������
//���ڲ�������
//�������ڣ�2015.10.26
//�޸����ڣ�2015.10.29 14:18
//��ע˵����x.xx ms(12MHz/12T)
//==========================================================
void Test_Process(void)
{   
	static float RemoteR,RemoteV;
	static u8 Extrigflag = 0;
	static u8 skipkey;
	static u32 oldskip;
	static u8 vropen;
	u32 sendE;
	char sendbuf[24];
	u8 csendlen;
	vu8 key;
    vu16 USB_Count=0;
    UINT fnum;
    vu8 test_Vsorting,test_Rsorting;
    u32 color;
    u8 keynum=0;
    u8 test_over=0;
    u8 OpenRangflag=0;
    vu8 Trip_Over_usb=0;

    u8 ry_flag=0;
	vu16 i;
    Send_To_UTypedef pt;
    Disp_Coordinates_Typedef  Coordinates;
	u8 Disp_Flag=1;//��ʾ��־
//    u8 Disp_Testflag=0;
//    u8 fit_num=0;
    vu8 range_flag=0;
//	Check_Parameter_Limit();//����ֵ���
	ffit_data1=0;
    Ad_over=0;
    Set_flag=0;
    test_start=0;
	F_100ms=FALSE;//100ms��ʱ	
    debug_flag=0;//	blinkT=0;//��˸ʱ���ʱ
    LCD_Clear(LCD_COLOR_TEST_BACK);
    pt=Send_To_U;
    if(Jk516save.Set_Data.Range_Set==1)//�̶�����
    {
        Range=Jk516save.Set_Data.Range;
    
    }
    else if(Jk516save.Set_Data.Range_Set==0)//�Զ�
    {
        Range=6;
    }
    else if(Jk516save.Set_Data.Range_Set==2)//�������ĸ���
    {
        Range=Jisuan_Range(Jk516save.Set_Data.Nominal_Res);
    }
    else
    {
        Range=Jk516save.Set_Data.Range;
    
    }
    Disp_Test_Item();  
    open_flag=0;    
    V_Range=Jisuan_V_Range(Jk516save.Set_Data.Nominal_V);
    Range_Control(Range,V_Range);	
    open_flag=1; 
    Select_V_I(1);
    delay_us(120);
    i=0;
    range_over=0;
    Send_ComBuff.send_head=0xaa;
    Send_ComBuff.sendend=0xbf;
    Send_ComBuff.over=0;
    EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE); 
    NVIC_EnableIRQ(EXTI3_IRQn);
//    if(Jk516save.Sys_Setvalue.u_flag)
//    {
//        i=200;
//        while(i--)
//        {
//        USBH_Process(&USB_OTG_Core, &USB_Host);
//            delay_ms(1);
//            
//        }
//    }
	while(GetSystemStatus()==SYS_STATUS_TEST)
	{
        USB_Count++;
        
        Colour.Fword=White;
        if(Disp_RTCflag)
        {
            Disp_RTCflag=0;
            Disp_dateandtime();//1�����һ��
            Int_Pe3flag=0;
            
        }
         if(Disp_Flag==1 )//��ʾ���õ�ֵ
		{
			Disp_Test_value(keynum);
            Colour.black=LCD_COLOR_TEST_MID;
            Disp_R_V();
					
		}  
		
		Extrigflag = Read_ExtTrig();
		if(Jk516save.Set_Data.trip==1)
		{
			
			if(Extrigflag == 0)
			{
				skipkey = 1;
			}else{
				skipkey = 0;
			}
		}
		if(Jk516save.Set_Data.trip==2)
		{
			if(Extrigflag == 0)
			{
				test_start = 1;
			}
		}
         if(Disp_usbflag||Disp_Flag)
        {
            Disp_Usbflag(USB_Openflag);
            Disp_Flag=0;
            Disp_usbflag=0;
            
        }
		
		
		OpenRangflag=read_cpld();//�ж��Ƿ�·
        if(OpenRangflag)
        {
            
             ry_flag++;
            if(ry_flag>3)
                ry_flag=3;
            OpenRangflag=0;
            if(Jk516save.Set_Data.Range_Set!=1)
            if(ry_flag==1)
            {
                Range_Control(Range,V_Range);
                
            }       
        
        }else
        ry_flag=0;
            
        if(Jk516save.Set_Data.trip==0)
            test_start=1;
        if(open_flag==0)//����·
        {
            if(test_start==1)
            {
                range_flag=1;
                switch(Jk516save.Set_Data.speed)
                {
                    case 0:
                        Select_V_I(1);
                        read_adI_1();//
                        Range_value=I_ad;
                        Range_Changecomp();	//���� �Ƚ�
                        while(range_over)
                        {
                            Range_Changecomp();	//���� �Ƚ�
                            
                            read_adI_1();//
                            Range_value=I_ad;
                        
                        
                        }
                        Select_V_I(0);
                        
                        read_adV_1();
                        Range_Value_V=V_ad;
                        VRange_Changecomp();	//���� �Ƚ�
                        while(range_over)
                        {
                            VRange_Changecomp();	//���� �Ƚ�
                            
                            read_adV_1();//
                           Range_Value_V=V_ad;
                        
                        
                        }
                        break;
                    
                    case 1:
                        Select_V_I(1);
                        read_adI_2();//
                        Range_value=I_ad;
                        Range_Changecomp();	//���� �Ƚ�
                        while(range_over)
                        {
                            Range_Changecomp();	//���� �Ƚ�
                            
                            read_adI_2();//
                            Range_value=I_ad;
                        
                        
                        }
                        Select_V_I(0);
                        
                        read_adV_2();
                        Range_Value_V=V_ad;
                        VRange_Changecomp();	//���� �Ƚ�
                        while(range_over)
                        {
                            VRange_Changecomp();	//���� �Ƚ�
                            
                            read_adV_2();//
                           Range_Value_V=V_ad;
                        
                        
                        }
                        break;
                    
                    case 2:
                        Select_V_I(1);
                        read_adI_3();//
                        Range_value=I_ad;
                        Range_Changecomp();	//���� �Ƚ�
                        while(range_over)
                        {
                            Range_Changecomp();	//���� �Ƚ�
                            
                            read_adI_3();//
                            Range_value=I_ad;
                        
                        
                        }
                        Select_V_I(0);
                        
                        read_adV_4();
                        Range_Value_V=V_ad;
                        VRange_Changecomp();	//���� �Ƚ�
                        while(range_over)
                        {
                            VRange_Changecomp();	//���� �Ƚ�
                            
                            read_adV_4();//
                           Range_Value_V=V_ad;
                        
                        
                        }
                        break;
                    
                    case 3:
                        Select_V_I(1);
                        read_adI_4();//
                        Range_value=I_ad;
                        Range_Changecomp();	//���� �Ƚ�
                        while(range_over)
                        {
                            Range_Changecomp();	//���� �Ƚ�
                            
                            read_adI_4();//
                            Range_value=I_ad;
                        
                        
                        }
                        Select_V_I(0);
                        
                        read_adV_4();
                        Range_Value_V=V_ad;
                        VRange_Changecomp();	//���� �Ƚ�
                        while(range_over)
                        {
                            VRange_Changecomp();	//���� �Ƚ�
                            
                            read_adV_4();//
                           Range_Value_V=V_ad;
                        
                        
                        }
                        break;
                    default:
                        Select_V_I(1);
                        read_adI_1();//
                        Range_value=I_ad;
                        Range_Changecomp();	//���� �Ƚ�
                        while(range_over)
                        {
                            Range_Changecomp();	//���� �Ƚ�
                            
                            read_adI_1();//
                            Range_value=I_ad;
                        
                        
                        }
                        Select_V_I(0);
                        
                        read_adV_1();
                        Range_Value_V=V_ad;
                        VRange_Changecomp();	//���� �Ƚ�
                        while(range_over)
                        {
                            VRange_Changecomp();	//���� �Ƚ�
                            
                            read_adV_1();//
                           Range_Value_V=V_ad;
                        
                        
                        }
                        break;
                
                
                
                }
                
                test_over=1;
               
                               
            }
            
           
        }
        else
        {
           if(range_flag) //��·��ʱ��ĵ�λ����
           {
               if(Jk516save.Set_Data.Range_Set==0)
               {
                   Range=6;
                    
               }
               else
               {
               
               }
               if(Jk516save.Set_Data.Range_Set == 0)
               Range_Control(Range,V_Range);
                   i=0;
                   range_flag=0;
                   RangeChange_Flag=1;
                            
           }
		   if(Jk516save.Set_Data.trip==0)
			{
				Disp_Open();
//				if(Jk516save.Set_Data.openbeep == 1)
//				{
//					Colour.black=LCD_COLOR_RED;
//					Beep_Out(1);
//					Led_Fail_On();
//					memcpy(DispBuf,"RV FL",5);
//					Send_ComBuff.comp=3;                                   
//					memcpy((void *)Send_To_U.comp,DispBuf,5);
//					DispBuf[5]=0;
//					LCD_DrawFullRect(SORTING_XDISP, SORTING_Y_DISP, 60, 22);
//					WriteString_16(SORTING_XDISP, SORTING_Y_DISP, DispBuf,  0);
//				}
			}
			if(test_start == 1)
			{
				Disp_Open();
//				if(Jk516save.Set_Data.openbeep == 1)
//				{
//					Colour.black=LCD_COLOR_RED;
//					Beep_Out(1);
//					Led_Fail_On();
//					memcpy(DispBuf,"RV FL",5);
//					Send_ComBuff.comp=3;
//                               
//					memcpy((void *)Send_To_U.comp,DispBuf,5);
//					DispBuf[5]=0;
//					LCD_DrawFullRect(SORTING_XDISP, SORTING_Y_DISP, 60, 22);
//					WriteString_16(SORTING_XDISP, SORTING_Y_DISP, DispBuf,  0);
//				}
				test_start = 0;
				if(Jk516save.Set_Data.trip == 3)
			    {
//				    uart1SendChars("+0.0000E+0,+0.0000E+0+",22);
					uart1SendChars("OPEN",4);
					test_start = 0;
			    }
			}
        }
            
       
	 	if(test_start&&test_over==1)//��ʾ����ֵ
		{   
            
            //test_over=0;
            if(Jk516save.Set_Data.trip)
            Trip_Over_usb=1;
            
            Ad_over=0; 
            I_ad=I_ad-Jk516save.Clear[Range];//���жϼ���
               if(I_ad<0)
               {
                   I_ad=-I_ad;
                   polarity_r=0;
               }
               else
               {
                polarity_r=1;
               
               }
               I_ad/=169;
               if(I_ad>38000)
               {
                   i=0;
                   open_flag=1;
           
                    Disp_Open();
				  if(Jk516save.Set_Data.trip != 0)
				  {
					  test_start = 0;
					  if(Jk516save.Set_Data.trip == 3)
					  {
//						  uart1SendChars("+0.0000E+0,+0.0000E+0+",22);
						  uart1SendChars("OPEN",4);
					  }
				  }
               }
                V_ad=V_ad-Jk516save.Clear_V[V_Range];
                if(V_ad<0)
                {
                    polarity_v=0;
                    V_ad=-V_ad;
                
                }else
                {
                    polarity_v=1;
                
                }
                V_ad=V_ad/45;
                Range_Value_V=V_ad;
                Res_count.r=I_ad;              
                Test_Debug();//У��
                I_ad=Res_count.r;
                if(open_flag==0)
                {
                    
                    if(RangeChange_Flag)
                    {
                        Disp_Range(Jk516save.Set_Data.Range_Set,Range);
                        Send_ComBuff.Range=Range;
                        RangeChange_Flag=0;
                        i=0;
                    }
                   
                    Colour.black=LCD_COLOR_TEST_MID;
                    if(range_over==0 && startdelay == 0)
                    {
                        test_start=0;
                        Test_Value_V=V_Datacov(V_ad ,V_Range);//�����ݵ�С����͵�λ �ͼ��Զ�����
                        Test_Value=Datacov(I_ad,Range);
                        
                       if((Test_Value.uint == 1 && Test_Value.res > 33000 && Test_Value.dot == 3) || (Test_Value.uint == 1 && Test_Value.dot == 2) || Test_Value.uint > 1 || (Test_Value_V.dot == 3 && Test_Value_V.res > 60000))
						{
							vropen = 1;
							open_flag=1;
							Disp_Open();
						}else{
							vropen = 0;
							Disp_Testvalue(Test_Value,Test_Value_V,0);//��ʾ����͵�ѹ
						}
						
						if(Jk516save.Set_Data.trip==3)//��Զ�̴������������ݵ���λ��
						{
							RemoteR = (float)Test_Value.res/pow(10,Test_Value.dot);
							RemoteV = (float)Test_Value_V.res/10000;
						
							if(Test_Value.uint == 0)
							{
								sendE = -3;
							}else if(Test_Value.uint == 1){
								sendE = 0;
							}else if(Test_Value.uint == 2){
								sendE = 3;
							}
							
							if(Test_Value_V.polar == 1)
							{
								sprintf(sendbuf,"+%.4fE+%d,+%.4fE+00",RemoteR,sendE,RemoteV);
							}else if(Test_Value_V.polar == 0){
								sprintf(sendbuf,"+%.4fE+%d,-%.4fE+00",RemoteR,sendE,RemoteV);
							}
							uart1SendChars(sendbuf,sizeof(sendbuf));
							Disp_Testvalue(Test_Value,Test_Value_V,0);//��ʾ����͵�ѹ
						}
						
                        if(Jk516save.Set_Data.V_comp)
                        {
                            test_Vsorting= V_Comp();
                            if(polarity_v==0)
                                test_Vsorting=2;
                            
                        }
                        if(Jk516save.Set_Data.Res_comp)
                            test_Rsorting=R_Comp();
						
						Colour.black=LCD_COLOR_TEST_MID;
						if(vropen == 0)
						{
							Disp_R_Uint();
						}else{
							LCD_ShowFontCN_40_55(60+40*6,92,40,55, (uint8_t*)Out_Assic+22*40*55/8);//m
						}
                        color=Colour.black;
                        //�����Ƿ�ѡ
                       
                        if(Jk516save.Set_Data.Res_comp==1&&Jk516save.Set_Data.V_comp==1)
                        {
                             
                            Colour.black=LCD_COLOR_RED;
                            if(test_Vsorting==0&&test_Rsorting==0)
                            {
                                Colour.black=LCD_COLOR_GREEN;
                                memcpy(DispBuf,"RV GD",5);  
                                
                                Send_ComBuff.comp=0;
                                Beep_Out(0);
                                Led_Pass_On();
                                
                            
                            }
                            else 
                            {
                                Beep_Out(1);
                                Led_Fail_On();
                                if(test_Vsorting==0)
                                {
                                    memcpy(DispBuf,"R  FL",5);
                                    Send_ComBuff.comp=1;
                                }
                                else if(test_Rsorting==0)
                                {
                                    memcpy(DispBuf,"V  FL",5);
                                    Send_ComBuff.comp=2;
                                    
                                }
                                else
                                {
                                    memcpy(DispBuf,"RV FL",5);
                                    Send_ComBuff.comp=3;

                                }                                    
                            }
                            memcpy((void *)Send_To_U.comp,DispBuf,5);
                            DispBuf[5]=0;
                            LCD_DrawFullRect(SORTING_XDISP, SORTING_Y_DISP, 60, 22);
                            WriteString_16(SORTING_XDISP, SORTING_Y_DISP, DispBuf,  0);
                         Colour.black=color;
                        }
                        else
                            if(Jk516save.Set_Data.Res_comp==1)
                            {
                                if(test_Rsorting)//���ϸ�
                                {
                                    Colour.black=LCD_COLOR_RED;
                                    memcpy(DispBuf,"R  FL",5);
                                    Send_ComBuff.comp=1;                                    
                                    Beep_Out(1);
                                    Led_Fail_On();
                                }
                                else//�ϸ�
                                {
                                    Colour.black=LCD_COLOR_GREEN;
                                    memcpy(DispBuf,"R  GD",5);
                                    Send_ComBuff.comp=0;                                    
                                    Beep_Out(0);
                                    Led_Pass_On();
                                
                                }
                            
                             memcpy((void *)Send_To_U.comp,DispBuf,5);
                             DispBuf[5]=0;
                            LCD_DrawFullRect(SORTING_XDISP, SORTING_Y_DISP, 60, 22);    
                            WriteString_16(SORTING_XDISP, SORTING_Y_DISP, DispBuf,  0);
                            
                            }else if(Jk516save.Set_Data.V_comp==1)
                            {
                                if(test_Vsorting)//���ϸ�
                                {
                                    Colour.black=LCD_COLOR_RED;
                                    memcpy(DispBuf,"V  FL",5);
                                    Send_ComBuff.comp=3;
                                    Beep_Out(1);
                                    Led_Fail_On();
                                }
                                else//�ϸ�
                                {
                                    Beep_Out(0);
                                    Led_Pass_On();
                                    Colour.black=LCD_COLOR_GREEN;
                                    memcpy(DispBuf,"V  GD",5);
                                    Send_ComBuff.comp=0;
                                
                                }
                             memcpy((void *)Send_To_U.comp,DispBuf,5);
                             DispBuf[5]=0; 
                             LCD_DrawFullRect(SORTING_XDISP, SORTING_Y_DISP, 60, 22);   
                             WriteString_16(SORTING_XDISP, SORTING_Y_DISP, DispBuf,  0);
                            }
                            else
                            {
                                Close_Compled();
//                                Beep_Out(0);
                                Colour.black=LCD_COLOR_TEST_MID;
                                LCD_DrawFullRect(SORTING_XDISP, SORTING_Y_DISP, 60, 22);
                            
                            }
                      
                        
                        
                    }else{
						startdelay --;
					}
                    
                }
 
		}
        if(Jk516save.Sys_Setvalue.u_flag)
        {
            i=1001;
            while(i--)
            USBH_Process(&USB_OTG_Core, &USB_Host);
            if(Jk516save.Set_Data.trip)
                USB_Count=0;
            if(Jk516save.Set_Data.trip==0&&test_over==1)   
            {
                test_over=0;
                result = f_mount(&fs,"0:",1);
                if(result == FR_OK)
                {
                    vu8 copybuff[100];
                    memset((void *)copybuff,0,100);
                    
                    memcpy ((void *)copybuff,"0:/JK/",6);
                    strcat((char *)copybuff,(char *)Jk516save.Sys_Setvalue.textname);
                    strcat((char *)copybuff,(char *)".xls");
                    result=f_mkdir("0://JK");
                    result = f_open(&file, (char *)copybuff,FA_CREATE_NEW | FA_WRITE );
                         if ( result == FR_OK ) 
                         {
                              f_lseek (&file,file.fsize);
                             result=f_write(&file,"���\t����\t��ѹ\t��ѡ\r\n",sizeof("���\t����\t��ѹ\t��ѡ\r\n"),&fnum);
                             Trip_On();
                             //result = f_open(&file, (char *)copybuff, FA_WRITE );
                             f_lseek (&file,file.fsize);
                             pt=Send_To_U;
                            result=f_write(&file,(u8 *)&pt,sizeof(Send_To_U),&fnum);
                             f_close(&file);
                             //f_close(&file);
                         
                         
                         }else
                         if(result==FR_EXIST)
                         {
                             Trip_On();
                             result = f_open(&file, (char *)copybuff, FA_WRITE );
                             f_lseek (&file,file.fsize);
                             pt=Send_To_U;
                            result=f_write(&file,(u8 *)&pt,sizeof(Send_To_U),&fnum);
                             f_close(&file);
                         
                         }
                         Trip_Off();
                    }
            
            }                
            if(Trip_Over_usb)
            {
                Trip_Over_usb=0;

                result = f_mount(&fs,"0:",1);
                if(result == FR_OK)
                {
                    vu8 copybuff[100];
                    memset((void *)copybuff,0,100);
                    
                    memcpy ((void *)copybuff,"0:/JK/",6);
                    strcat((char *)copybuff,(char *)Jk516save.Sys_Setvalue.textname);
                    strcat((char *)copybuff,(char *)".xls");
                    result=f_mkdir("0://JK");
                    result = f_open(&file, (char *)copybuff,FA_CREATE_NEW | FA_WRITE );
                         if ( result == FR_OK ) 
                         {
                              f_lseek (&file,file.fsize);
                             result=f_write(&file,"���\t����\t��ѹ\t��ѡ\r\n",sizeof("���\t����\t��ѹ\t��ѡ\r\n"),&fnum);
                             Trip_On();
                             //result = f_open(&file, (char *)copybuff, FA_WRITE );
                             f_lseek (&file,file.fsize);
                             pt=Send_To_U;
                            result=f_write(&file,(u8 *)&pt,sizeof(Send_To_U),&fnum);
                             f_close(&file);
                             //f_close(&file);
                         
                         
                         }else
                         if(result==FR_EXIST)
                         {
                             Trip_On();
                             result = f_open(&file, (char *)copybuff, FA_WRITE );
                             f_lseek (&file,file.fsize);
                             pt=Send_To_U;
                            result=f_write(&file,(u8 *)&pt,sizeof(Send_To_U),&fnum);
                             f_close(&file);
                         
                         }
                         Trip_Off();
                    }
                }
            
        }
             
		Uart_Process();//���ڴ���
        if(Keyboard.state==TRUE)
        {
			if(skipkey == 0)
			{
				Disp_Flag=1;
				key=Key_Read();
				if(key!=KEY_NONE)
				switch(key)
				{
						case Key_F1:
							
							switch(keynum)
							{
								case 0:
									keynum=0;
										//SetSystemStatus(SYS_STATUS_TEST);
									break;
								case 1:
									Jk516save.Set_Data.trip=0;
								
								
		
									
									break;
								case 2:
									
									Coordinates.xpos=LIST1+88;
									Coordinates.ypos=FIRSTLINE+SPACE1*1;
									Coordinates.lenth=76;
									Jk516save.Set_Data.High_Res=Disp_Set_Num(&Coordinates);
								break;
								case 3:
									Coordinates.xpos=LIST1+88;
									Coordinates.ypos=FIRSTLINE+SPACE1*2;
									Coordinates.lenth=76;
									Jk516save.Set_Data.Res_low=Disp_Set_Num(&Coordinates);
									
								break;
								case 4:
									Jk516save.Set_Data.Range=RANGE_MAX;//0  �Զ�  10   �������
									Jk516save.Set_Data.Range_Set=0;
	   
									break;
								case 5:
									Coordinates.xpos=LIST2+88;
									Coordinates.ypos=FIRSTLINE+SPACE1*1;
									Coordinates.lenth=76;
									Jk516save.Set_Data.V_high=Disp_Set_CompNum(&Coordinates);
									
								break;
								
								
								case 6:
									Coordinates.xpos=LIST2+88;
									Coordinates.ypos=FIRSTLINE+SPACE1*2;
									Coordinates.lenth=76;
									Jk516save.Set_Data.V_low=Disp_Set_CompNum(&Coordinates);
									
								break;
							
								default:
									break;
							
							
							}
		//					Savetoeeprom();
						break;
						case Key_F2:
							switch(keynum)
							{
								case 0:
									
										SetSystemStatus(SYS_STATUS_SETUP);
									break;
								case 1:
									Jk516save.Set_Data.trip=1;
									
									break;
						
								case 4:
									//Jk516save.Set_Data.Range=10;//0  �Զ�  10   �������
									Jk516save.Set_Data.Range_Set=1;
									Jk516save.Set_Data.Range=Range;
								//Range=

									break;
	 
								
								default:
									break;
							
							
							}
		//					Savetoeeprom();
						break;
						case Key_F3:
							switch(keynum)
							{
								case 0:
										SetSystemStatus(SYS_STATUS_SYSSET);
									break;
								case 1:
									Jk516save.Set_Data.trip=2;
									
									break;
						
								case 4://MAX_R_RANGE
									//Jk516save.Set_Data.Range=Jk516save.Set_Data.Range>=RANGE_MAX? 0:Jk516save.Set_Data.Range++;
									Jk516save.Set_Data.Range_Set=2;//���ӵ�λѡ��ļ���
									Range=Jisuan_Range(Jk516save.Set_Data.Nominal_Res);
									Jk516save.Set_Data.Range=Range;
									Range_Control(Range,V_Range);
									

									break;

								
								default:
									break;
							
							
							}
		//					Savetoeeprom();
						break;
						case Key_F4:
							switch(keynum)//
							{
							   case 0:
										SetSystemStatus(SYS_STATUS_SYS);
									break;
							   case 1:
									Jk516save.Set_Data.trip=3;
									test_start = 0;
								break;
							   
								case 4:
									//Jk516save.Set_Data.Range=Jk516save.Set_Data.Range>0? Jk516save.Set_Data.Range--:RANGE_MAX;
									if(Range>=3)
										Range=0;
									else
										Range++;
									Jk516save.Set_Data.Range_Set=1;
									Jk516save.Set_Data.Range=Range;
									Range_Control(Range,V_Range);
									
								break;
	  
								default:
									break;
							
							
							}	
		//					Savetoeeprom();
						break;
						case Key_F5:
							switch(keynum)
							{
								case 0:
									//if(page==1)
									//	SetSystemStatus(SYS_STATUS_DEBUG);
									break;
								default:
									break;
							}
	  
						break;
						case Key_Disp:
							//SetSystemStatus(SYS_STATUS_TEST);
						break;
						case Key_SETUP:
							keynum=0;
							SetSystemStatus(SYS_STATUS_SETUP);
						break;
						case Key_FAST:
						break;
						case Key_LEFT:
							if(keynum<1)
								keynum=6;
							else
								keynum--;
						  
						break;
						case Key_RIGHT:
							if(keynum>5)
								keynum=0;
							else
								keynum++;
						   
					 
								
						break;
						case  Key_UP:
							if(keynum<1)
								keynum=6;
							else
								keynum--;
							
						break;
						case Key_DOWN:
							if(keynum>5)
								keynum=0;
							else
								keynum++;
						break;
						
						case Key_NUM1:
						//break;
						case Key_NUM2:
						//break;
						case Key_NUM3:
						//break;
						case Key_NUM4:
						//break;
						case Key_NUM5:
						//break;
						case Key_NUM6:
						//break;
						case Key_NUM7:
						//break;
						case Key_NUM8:
						//break;
						case Key_NUM9:
						//break;
						case Key_NUM0:
						break;
						case Key_DOT:

						break;
						case Key_BACK:
							
						break;
						case Key_LOCK:

		//					if(lock_flag)
		//					{
		//						Lock_Control_On();
		//						Lock_LedOn();
		//						
		//					}
		//					else
		//					{
		//						Lock_LedOff();
		//						Lock_Control_Off();
		//					
		//					
		//					}
						break;
						case Key_BIAS:
							SetSystemStatus(SYS_STATUS_CLEAR);
						break;
						case Key_REST:
						break;
						case Key_TRIG:
							if(Jk516save.Set_Data.trip == 1 && Extrigflag == 1)
							{
								test_start=1;
							}
							
						break;
						default:
							SetSystemStatus(SYS_STATUS_TEST);
						break;
							
					}
				}else{
//					skipkey = 0;
				}
         }
     }
    Store_set_flash();
    f_mount(NULL, "0:", 0);
}
//==========================================================
//�������ƣ�TestAbort_Process
//�������ܣ�������ֹ����
//��ڲ�������
//���ڲ�������
//�������ڣ�2015.10.26 
//�޸����ڣ�2015.10.29 13:35
//��ע˵������
//==========================================================
//void TestAbort_Process(void)
//{
//	u8 key;

//	//����ر�
////	Plc_Start_Off();//��Զ������

// 	//Ѷ�����
////	if(SaveData.Parameter.Beep)
////		Beep_One();//��һ��

//	//��ʾ��Ϣ
////	LcdAddr.x=MSG_ADDR_X; LcdAddr.y=0;
////	Lcd_SetAddr();//��������
////	Disp_SysMessage();//��ʾ��Ϣ
//	
//	//DAC_SetChannel1Data(DAC_Align_12b_R,0);
////	Led_HV_Off();//�ز��Ե�
////	delay_ms(30);
////	Electro_Test(0);
////	delay_ms(30);
////	
////	Electro_discharge(0);
////	delay_ms(30);
////	BeepOff();
////	
//	while(GetSystemStatus()==SYS_STATUS_TEST_ABORT)
//	{
////		Uart_Process();//���ڴ���
//		
//		key=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/20);//�ȴ�����(100*10ms/20=50ms)
//		switch(key)
//		{
//			case KEY_SET:	//���ü�
//			case L_KEY_SET:	//�������ü�
//				break;
//	
//			case KEY_UP:	//�ϼ�
//			case L_KEY_UP:	//�����ϼ�
//				break;
//	
//			case KEY_DOWN:		//�¼�
//			case L_KEY_DOWN:	//�����¼�
//				break;

//			case KEY_START:		//������
//			case L_KEY_START:	//����������
//				SetSystemStatus(SYS_STATUS_TEST);//���Կ�ʼ
//				break;
//	
//			case KEY_ENTER:		//ȷ�ϼ�
//			case L_KEY_ENTER:	//����ȷ�ϼ�
//				break;
//	
////			case KEY_RESET:		//��λ��
////			case L_KEY_RESET:	//������λ��
////				SetSystemStatus(SYS_STATUS_IDLE);//ϵͳ״̬-����
////				break;
//			
//			default:
//				break;
//		}
//	}
//	Test_value.Test_Time=0;
//}
//==========================================================
//�������ƣ�TestFinish_Process
//�������ܣ����Խ�������
//��ڲ�������
//���ڲ�������
//�������ڣ�2015.10.26
//�޸����ڣ�2015.10.29 13:38
//��ע˵������
//ע�������
//==========================================================
//void TestFinish_Process(void)
//{
//	u8 key;

//	//����ر�
////	Led_HV_Off();//�رո�ѹ��
////	Plc_Start_Off();//��Զ������
//	//��ʾ��Ϣ
//	//SetSystemMessage(MSG_PASS);
//	
////	LcdAddr.x=MSG_ADDR_X; LcdAddr.y=0;
////	Lcd_SetAddr();//��������
////	Disp_SysMessage();//��ʾ��Ϣ
//	//��ѡѶ�����

//	
//	Electro_discharge(0);
//	while(GetSystemStatus()==SYS_STATUS_TEST_FINISH)
//	{
////		Uart_Process();//���ڴ���
//		key=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/50);//�ȴ�����(100*10ms/50=20ms)
//		switch(key)
//		{
//			case KEY_SET:	//���ü�
//			case L_KEY_SET:	//�������ü�
//				SetSystemStatus(SYS_STATUS_SETUP);//����״̬
//				break;
//	
//			case KEY_UP:	//�ϼ�
//			case L_KEY_UP:	//�����ϼ�
//				break;
//	
//			case KEY_DOWN:		//�¼�
//			case L_KEY_DOWN:	//�����¼�
//				break;
//	
//			case KEY_START:		//������
//			case L_KEY_START:	//����������
//				SetSystemStatus(SYS_STATUS_TEST);//����״̬
//				break;
//	
//			case KEY_ENTER:		//ȷ�ϼ�
////			case L_KEY_ENTER:	//����ȷ�ϼ�
////				SetSystemStatus(SYS_STATUS_IDLE);//����״̬
////				break;
////	
////			case KEY_RESET:		//��λ��
////			case L_KEY_RESET:	//������λ��
////				SetSystemStatus(SYS_STATUS_IDLE);//����״̬
////				break;
//			
//			default:
//				break;
//		}
//	}
//}	
void Electro_discharge(u8 data)//data=1  ����  0  �ŵ�
{
	u8 value;
	value=U16_4094;
	if(!data)
		value|=HW_GYBFD_ON;
	else
		value&=HW_GYBFD_OFF;
	U16_4094=value;
	_4094_data( );
	Out_4094(_4094_databuff);
	delay_ms(30);

}
void Electro_Test(u8 data)
{
	u8 value;
	value=U15_4094;
	if(data)
		value|=HW_FD_TEST;
	else
		value&=HW_FD_NOTEST;
	U15_4094=value;
	_4094_data( );
	Out_4094(_4094_databuff);
	delay_ms(30);

}

void Feedback_RY(u8 dat)//��������
{
	
	//U16_4094
	u8 value;
	value=U15_4094;
	//value&=HW_GYB_1000VFEEDMASK;
	switch(dat)
	{
		case 0:
			value|=HW_GYB_1000VFEED;
			break;
		case 1:
			value&=HW_GYB_100VFEED;
			break;
		
		default:
			//value&=HW_GYB_1000VFEED;
			break;
	}
	U15_4094=value;
	_4094_data( );
	Out_4094(_4094_databuff);

}
//��ѹ���������л�
//���� �� 0 1 2 3
// 0  1000V  1  100V  2  10V
//
//
//
void V_Samping(u8 dat)
{
	vu8 value;
	value=U16_4094;
	switch(dat)
	{
		case 0:
			value&=HW_GYB_100V;
			break;
		case 1:
			value|=HW_GYB_1000V;
			break;
		
			
		default:
			//value&=HW_GYB_1000V;
			break;
	}
	U16_4094=value;
	_4094_data( );
	Out_4094(_4094_databuff);
	delay_ms(20);

}

//�û�У��
void Use_DebugProcess(void)
{
	
	vu32 keynum=0;
    float a,b;
	vu8 key;
//    u8 Disp_Testflag=0;
    vu8 range_flag=0;
//    float i_mid;
//	float ddd,eee;
	Disp_Coordinates_Typedef Coordinates;
    Disp_Coordinates_Typedef Debug_Cood;
    vu8 i;
	vu8 Disp_flag=1;
    vu8 list=0;
 //   uint32_t  scan_I[200],scan_V[200];
    test_start=0;
    LCD_Clear(LCD_COLOR_TEST_BACK);
	Disp_UserCheck_Item();
    Debug_stanedcomp();//У��ֵ�Ƚ�
    EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE); 
    NVIC_EnableIRQ(EXTI3_IRQn);
    Range_Control(0,0);
 	while(GetSystemStatus()==SYS_STATUS_DEBUG)
	{

//         Select_V_I(1);
//        read_adI_1();//
//        Range_value=I_ad;

//        Select_V_I(0);
//        
//        read_adV_1();
//        Range_Value_V=V_ad;
		
		if(Jk516save.Set_Data.speed == 0)
		{
			 Select_V_I(1);
			read_adI_1();//
			Range_value=I_ad;
		
			Select_V_I(0);
			
			read_adV_1();
			Range_Value_V=V_ad;
		}else if(Jk516save.Set_Data.speed == 1){
			Select_V_I(1);
			read_adI_2();//
			Range_value=I_ad;
		
			Select_V_I(0);
			
			read_adV_2();
			Range_Value_V=V_ad;
		}else if(Jk516save.Set_Data.speed == 2){
			Select_V_I(1);
			read_adI_3();//
			Range_value=I_ad;
		
			Select_V_I(0);
			
			read_adV_3();
			Range_Value_V=V_ad;
		}else if(Jk516save.Set_Data.speed == 3){
			Select_V_I(1);
			read_adI_4();//
			Range_value=I_ad;
		
			Select_V_I(0);
			
			read_adV_4();
			Range_Value_V=V_ad;
		}


		if(test_start)
		{
            {
               // test_start=0;
                Ad_over=0; 
                //���жϼ���
                //���ݼ��� ��ȷ������ֵ
                //�ټ������
                
                if(I_ad<0)
                {
                    polarity_r=0;//��
                    I_ad=-I_ad;
                }
                else
                {
                    polarity_r=1;
                    
                                        
                }
                
                I_ad=I_ad-Jk516save.Clear[list-1];//���жϼ���
                I_ad=(I_ad)/169;
                 if(list-1 ==6)
                {
                     
                
                   if(I_ad<9000)
                    {
                        a=Res_count.r;
                        b=0.778f*I_ad;
                        I_ad=a*a*2/100000+b+292;
                    }
                    else
                    {
                        a=I_ad;
                        b=0.4818f*I_ad;
                        I_ad=a*a*3/100000+b+2410;
                    
                    }
                
    
    
                
                
                }
                
                if(V_ad<0)
                {
                    V_ad=-V_ad;
                    
                }
                if(list>7)
                V_ad=V_ad-Jk516save.Clear_V[list-8];
                V_ad=V_ad/45;
                {
                    Test_Value_V=V_Datacov(V_ad ,V_Range);//�����ݵ�С����͵�λ �ͼ��Զ�����
                    Test_Value=Datacov(I_ad,list-1);
                    Disp_Debug_Reference(list,Test_Value_V,Test_Value);
                    
                }

               
            }

            
		}else
		if(Disp_flag==1)//��������ʱ��
		{
			Disp_Debug_value(list);
			Disp_flag=0;	
		}
    
		key=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/10);
		
		if(key!=KEY_NONE)
		{
			Disp_flag=1;
			//Key_Beep();
			switch(key)
			{
				case Key_F1:

				break;
				case Key_F2:

				break;
				case Key_F3:

				break;
				case Key_F4:
                    if(list==0)
                    {
                        Store_set_flash();
                        SetSystemStatus(SYS_STATUS_TEST);
                    }

				break;
				case Key_F5:

				break;
				case Key_Disp:
                    if(list)
                        test_start=1;
                    
					//Savetoeeprom();
                    //SetSystemStatus(SYS_STATUS_TEST);
				break;
				case Key_SETUP:
                        test_start=0;
                    if(list==0)
                    {
                        LCD_Clear(LCD_COLOR_TEST_BACK);
                        
                        Debug_Cood.xpos=70;
                        Debug_Cood.ypos =272-70;
                        Debug_Cood.lenth=120;
                        input_num(&Debug_Cood);
                         LCD_Clear(LCD_COLOR_TEST_BACK);
                        Disp_UserCheck_Item();
                    
                    }

				break;
				
				case Key_LEFT:
				break;
				case Key_RIGHT:
				break;
				case Key_UP:
                    if(test_start==0)
                        {
                    open_flag=0;
                    
                    if(test_start==0)
                    {
                        if(list<1)
                            list=DEBUG_RANGE;
                        else
                            list--;
                        if(list)
                        {
                            if(list<7)
                            {
                                Range_Control(list-1,1);
                                Range=list-1;
                                
                            }
                            else
                            {
                                V_Range=DEBUG_RANGE-list-1;
                                Range_Control(RANGE_MAX,V_Range);
                                
                            }
                        }
                    
                    }
                }

							
				break;
				case Key_DOWN:
                    if(test_start==0)
                    {
                        open_flag=0;
                        if(list<DEBUG_RANGE)
                            list++;
                        else
                            list=0;
                        if(list)
                        {
                            if(list<7)
                            {
                                Range_Control(list-1,1);
                                Range=list-1;
                                
                            }
                            else
                                
                                {
                                    V_Range=DEBUG_RANGE-list-1;
                                    Range_Control(RANGE_MAX,V_Range);
                                }
                        }
                    }
					
				break;
					case Key_DOT:
						break;
				case Key_NUM1:
				//break;
				case Key_NUM2:
				//break;
				case Key_NUM3:
				//break;
				case Key_NUM4:
				//break;
				case Key_NUM5:
				//break;
				case Key_NUM6:
				//break;
				case Key_NUM7:
				//break;
				case Key_NUM8:
				//break;
				case Key_NUM9:
				//break;
				case Key_NUM0:
				break;
				case Key_FAST:
                    
                
                                
                                
                                //Jk516save.Set_Data.High_Res=Disp_Set_Num(&Coordinates);
                
				Coordinates.xpos=LIST1+160;
				Coordinates.ypos=FIRSTLINE+(SPACE1-2)*(list);
				Coordinates.lenth=70;
                if(test_start)
                {
                    if(list<8)
                    {
						if(Jk516save.Set_Data.speed == 0)
						{
							Jk516save.Debug_Value[list-1].standard=Debug_Set_Res(&Coordinates);//����
							Jk516save.Debug_Value[list-1].ad_value=(float)Test_Value.res/Jk516save.Debug_Value[list-1].standard;
						}else if(Jk516save.Set_Data.speed == 1){
							Jk516save.Debug_Value1[list-1].standard=Debug_Set_Res(&Coordinates);//����
							Jk516save.Debug_Value1[list-1].ad_value=(float)Test_Value.res/Jk516save.Debug_Value1[list-1].standard;
						}else if(Jk516save.Set_Data.speed == 2){
							Jk516save.Debug_Value2[list-1].standard=Debug_Set_Res(&Coordinates);//����
							Jk516save.Debug_Value2[list-1].ad_value=(float)Test_Value.res/Jk516save.Debug_Value2[list-1].standard;
						}else if(Jk516save.Set_Data.speed == 3){
							Jk516save.Debug_Value3[list-1].standard=Debug_Set_Res(&Coordinates);//����
							Jk516save.Debug_Value3[list-1].ad_value=(float)Test_Value.res/Jk516save.Debug_Value3[list-1].standard;
						}
                    }
                    else
                    {
						if(Jk516save.Set_Data.speed == 0)
						{
							Jk516save.Debug_Value[list-1].standard=Debug_Set_Num(&Coordinates);//��ѹ
							Jk516save.Debug_Value[list-1].ad_value=(float)Test_Value_V.res/Jk516save.Debug_Value[list-1].standard;
						}else if(Jk516save.Set_Data.speed == 1){
							Jk516save.Debug_Value1[list-1].standard=Debug_Set_Num(&Coordinates);//��ѹ
							Jk516save.Debug_Value1[list-1].ad_value=(float)Test_Value_V.res/Jk516save.Debug_Value1[list-1].standard;
						}else if(Jk516save.Set_Data.speed == 2){
							Jk516save.Debug_Value2[list-1].standard=Debug_Set_Num(&Coordinates);//��ѹ
							Jk516save.Debug_Value2[list-1].ad_value=(float)Test_Value_V.res/Jk516save.Debug_Value2[list-1].standard;
						}else if(Jk516save.Set_Data.speed == 3){
							Jk516save.Debug_Value3[list-1].standard=Debug_Set_Num(&Coordinates);//��ѹ
							Jk516save.Debug_Value3[list-1].ad_value=(float)Test_Value_V.res/Jk516save.Debug_Value3[list-1].standard;
						}
                      
                        
                    }
                    
                }
                test_start=0;

				break;
				case Key_BACK:
				break;
				case Key_LOCK:
				break;
				case Key_BIAS:
				break;
				case Key_REST:
                    if(Jk516save.open == 1)
					{
						Jk516save.open = 0;
					}else{
						Jk516save.open = 1;
					}
					Store_set_flash();
				break;
				case Key_TRIG:
				break;
				default:
				break;
					
			}
		
		
		}
	
	
	}
}
void Clear_Process(void)
{

    u8 range_v,range;
    u8 list=0;
    u8 Clear_num=0;

    range=Jk516save.Set_Data.Range;
    Range_Control(0,0);
    Disp_Range(0,list);
    while(GetSystemStatus()==SYS_STATUS_CLEAR)
    {
        Select_V_I(1);
       if(Jk516save.Set_Data.speed == 0)
		{
			read_adI_1();//
		}else if(Jk516save.Set_Data.speed == 1){
			read_adI_2();//
		}else if(Jk516save.Set_Data.speed == 2){
			read_adI_3();//
		}else if(Jk516save.Set_Data.speed == 3){
			read_adI_4();//
		}
        Range_value=I_ad;
        Range_Changecomp();	//���� �Ƚ�
        while(range_over)
        {
            Range_Changecomp();	//���� �Ƚ�
            
            if(Jk516save.Set_Data.speed == 0)
			{
				read_adI_1();//
			}else if(Jk516save.Set_Data.speed == 1){
				read_adI_2();//
			}else if(Jk516save.Set_Data.speed == 2){
				read_adI_3();//
			}else if(Jk516save.Set_Data.speed == 3){
				read_adI_4();//
			}
            Range_value=I_ad;
        
        
        }
        Select_V_I(0);
        
		if(Jk516save.Set_Data.speed == 0)
		{
			read_adV_1();
		}else if(Jk516save.Set_Data.speed == 1){
			read_adV_2();
		}else if(Jk516save.Set_Data.speed == 2){
			read_adV_4();
		}else if(Jk516save.Set_Data.speed == 3){
			read_adV_4();
		}
//        read_adV_1();
        Range_Value_V=V_ad;
        Range_Changecomp();	//���� �Ƚ�
       

           
        if(V_ad>0x800000)
        {
            V_ad=0xffffff-V_ad;
            V_ad=-V_ad;
        }

    
    
        if(I_ad>0x800000)
        {
            I_ad=0xffffff-I_ad;
            I_ad=-I_ad;
            
        }
              

            Clear_num++;
       
        //if(Clear_num>2)
        {
            Clear_num=0;
            Jk516save.Clear[list-1]=I_ad;
            if(list<3)
            {
                Jk516save.Clear_V[list-1]=V_ad;
                range_v=list;
                
            }
            Range_Control(list,range_v);
            Disp_Range(0,list);
        
            if(list>=RANGE_MAX+1)
            {
                SetSystemStatus(SYS_STATUS_TEST);
               
                Jk516save.Set_Data.Range=range;
                Store_set_flash();
                
            }
            list++;
        }
    
    }

}
//��������л�
//���� �� 0 1 2 3
// 0  0R  1  10K  2  100K
//
//
//
//void Internal_ResChange(u8 dat)
//{
//	u8 value;
//	value=U16_4094;
//	value&=HW_GYB_10KR;
//	switch(dat)
//	{
//		case 0:
//			value|=HW_GYB_0R;
//			break;
//		case 1:
//			
//			break;
//		case 2:
//			value|=HW_GYB_100KR;
//			break;
//			
//		default:
//			value&=HW_GYB_10KR;
//			break;
//	}
//	U16_4094=value;
//	_4094_data( );
//	Out_4094(_4094_databuff);
//	delay_ms(20);;

//}
//void Disp_Overtest(void)
//{


//}

