//==========================================================
//文件名称：User.c
//文件描述：菜单设置子程序
//文件版本：Ver1.0
//创建日期：2014.12.15
//修改日期：2015.09.06 11:15
//文件作者：
//备注说明：无
//注意事项：无
//==========================================================
#include <stdio.h>
#include <string.h>
//#include <stdlib.h>
#include <math.h>
////#include <absacc.h>
////#include <intrins.h>
#include "pbdata.h"
//#include <absacc.h>
#include "./APP/usbh_bsp.h"
#include "cpld.h"
//#include "GlobalValue.h"
//#include "Hardware.h"
#include "gpio.h"
#include "./RTC/bsp_rtc.h"
//#include "Interrupt.h"
//#include "Lcd240128.h"
//#include "Disp.h"
//#include "Keyboard.h"
//#include "Beep.h"
//#include "Isp.h"
//#include "Uart0.h"
//#include "Com.h"
//#include "User.h"
u8 DispBuf[DISP_MAX_LEN];//显示缓冲区定义
extern char scpinum[20],scpinum1[20];
extern u8 scpidot,scpiunit,scpidot1,scpiunit1;
extern u8 scpnum,scpnum1;
//==========================================================
const u32 Port_Select[16][16]=//pos(x,y)x0-15 y0-15
{
    {0x00000000,0X10800000,0X10008000,0X01000080,0X14000000,0X10400000,0X10004000,0X10000040,
     0X12000000,0X10200000,0X10002000,0X10000020,0X11000000,0X10010000,0X10000100,0X10000001},
    {0X08020000,0x00000000,0X00028000,0X00020080,0X04020000,0X00420000,0X00024000,0X10000040,
     0X02020000,0X00220000,0X00022000,0X00020020,0X01020000,0X00030000,0X00020100,0X00020001},
    {0X08000200,0X00800200,0x00000000,0X00000280,0X04000200,0X00400200,0X00004200,0X00000240,
     0X02000200,0X00200200,0X00002200,0X00000220,0X01000200,0X00010200,0X00000300,0X00000201},
    {0X08000002,0X00800002,0X00008002,0x00000000,0X04000002,0X00400002,0X00004002,0X00000042,
     0X02000002,0X00200002,0X00002002,0X00000022,0X01000002,0X00010002,0X00000102,0X00000003},
    {0X28000000,0X20800000,0X20008000,0X20000080,0x00000000,0X20400000,0X20004000,0X20000040,
     0X22000000,0X20200000,0X20002000,0X20000020,0X21000000,0X20010000,0X20000100,0X20000001},
    {0X08040000,0X00840000,0X00048000,0X00040080,0X04040000,0x00000000,0X00044000,0X00040040,
     0X02040000,0X00240000,0X00042000,0X00040020,0X01040000,0X00050000,0X00040100,0X00040001},
    {0X08000400,0X00800400,0X00008400,0X00000480,0X04000400,0X00400400,0x00000000,0X00000440,
     0X02000400,0X00200400,0X00002400,0X00000420,0X01000400,0X00010400,0X00000500,0X00000401},
    {0X08000004,0X00800004,0X00008004,0X00000084,0X04000004,0X00400004,0X00004004,0x00000000,
     0X02000004,0X00200004,0X00002004,0X00000024,0X01000004,0X00010004,0X00000104,0X00000005},
    {0X48000000,0X40800000,0X40008000,0X40000080,0X44000000,0X40400000,0X40004000,0X40000040,
     0x00000000,0X40200000,0X40002000,0X40000020,0X41000000,0X40010000,0X40000100,0X40000001},
    {0X08080000,0X00880000,0X00088000,0X00080080,0X04080000,0X00480000,0X00084000,0X00080040,
     0X02080000,0X00000000,0X00082000,0X00080020,0X01080000,0X00090000,0X00080100,0X00080001},
    {0X08000800,0X00800800,0X00008800,0X00000880,0X04000800,0X00400800,0X00004800,0X00000840,
     0X02000800,0X00200800,0X00000000,0X00000820,0X01000800,0X00010800,0X00000900,0X00000801},
    {0X08000008,0X00800008,0X00008008,0X00000088,0X04000008,0X00400008,0X00004008,0X00000048,
     0X02000008,0X00200008,0X00002008,0X00000000,0X01000008,0X00010008,0X00000108,0X00000009},
    {0X88000000,0X80800000,0X80008000,0X80000080,0X84000000,0X80400000,0X80004000,0X80000040,
     0X82000000,0X80200000,0X80002000,0X80000020,0X00000000,0X80010000,0X80000100,0X80000001},
    {0X08100000,0X00900000,0X00108000,0X00100080,0X04100000,0X00500000,0X00104000,0X00100040,
     0X02100000,0X00300000,0X00102000,0X00100020,0X01100000,0X00000000,0X00100100,0X00100001},
    {0X08001000,0X00801000,0X00009000,0X00001080,0X04001000,0X00401000,0X00005000,0X00001040,
     0X02001000,0X00201000,0X00003000,0X00001020,0X01001000,0X00011000,0X00000000,0X00001001},
    {0X08000010,0X00800010,0X00008010,0X00000090,0X04000010,0X00400010,0X00004010,0X00000050,
     0X02000010,0X00200010,0X00002010,0X00000030,0X01000010,0X00010010,0X00000110,0X00000000}

};
const uint8_t User_Check_main[][12+1]=
{
	{"档号    "},
	{"标称值  "},
	{"参考值  "},

};
const uint8_t User_Check_Item[][12+1]=
{
	{"10.000mΩ"},
	{"100.00mΩ"},
	{"1.0000 Ω"},
	{"10.000 Ω"},
    {"100.00 Ω"},
    {"1.0000kΩ"},
    {"10.000kΩ"},
    
	{"1.0000 V"},
	{"2.0000 V"},


};

const uint8_t User_Check_Item1[][12+1]=
{
	{"4.0000 V"},
	{"6.0000 V"},
	{"15.000 V"},
	{"30.000 V"},
    {"45.000 V"},
    {"60.000 V"},
    {"80.000 V"},
    
	{"100.000 V"},
	{"         "},

};
const uint8_t Test_Setitem[][9+1]=
{
	{"触发    :"},
	{"电阻上限:"},
	{"电阻下限:"},
	{"量程    :"},
	{"电压上限:"},
	{"电压下限:"},
};

const uint8_t Test_Setitem_E[][9+1]=
{
	{"TRIP    :"},
	{"R_HIGH  :"},
	{"R_LOW   :"},
	{"RANGE   :"},
	{"V_HIGH  :"},
	{"V_LOW   :"},
};


const uint8_t All_TopName[][21+1]=
{
	{"< 测量显示 >    "},
	{"< 档号显示 >    "},
	{"< 档计数显示 >  "},
	{"< 列表扫描显示 >"},
	{"< 测量设置 >    "},
	{"< 用户校正 >    "},
	{"< 极限列表设置 >"},
	{"< 列表扫描设置 >"},
	{"< 系统设置 >    "},
	{"[ LCR文件列表 ] "},
	{"< 系统显示 >    "},

};

const uint8_t All_TopName_E[][21+1]=
{
	{"< MEAS DISP >    "},
	{"< 档号显示 >    "},
	{"< 档计数显示 >  "},
	{"< 列表扫描显示 >"},
	{"< MEAS SETUP >  "},
	{"< 用户校正 >    "},
	{"< 极限列表设置 >"},
	{"< 列表扫描设置 >"},
	{"< SYS SETUP >   "},
	{"[ LCR文件列表 ] "},
	{"< SYS DISP >    "},

};

const uint8_t	Set_testitem[][9+1]=
{
	{"触发    :"},
	{"速度    :"},
	{"         "},
	{"电阻比较:"},
	{"电阻下限:"},
	{"         "},
	{"电压比较:"},
	{"电压下限:"},
	{"        "},
	
	{"量程    :"},
	{"讯响    :"},
	{"         "},
	{"电阻标称:"},
	{"电阻上限:"},
	
	{"         "},
	{"电压标称:"},
	{"电压上限:"} ,
    {"         "},
};
const uint8_t	Set_testitem_E[][9+1]=
{
	{"TRIP    :"},
	{"SPEED   :"},
	{"         "},
	{"R_COMP  :"},
	{"R_LOW   :"},
	{"         "},
	{"V_COMP  :"},
	{"V_LOW   :"},
	{"        "},
	
	{"RANGE   :"},
	{"BEEP    :"},
	{"         "},
	{"R_NORM  :"},
	{"R_HIGH  :"},
	
	{"         "},
	{"V_NORM  :"},
	{"V_HIGH:"} ,
    {"         "},
};
const uint8_t Test_Tirpvalue[][6+1]=
{
	{"自动"},
	{"手动"},
	{"外部"},
	{"远程"}

};
const uint8_t Test_Tirpvalue_E[][6+1]=
{
	{"AUTO"},
	{"MAN"},
	{"EXT"},
	{"REMOTE"}

};
const uint8_t Test_Speedvalue[][6+1]=
{
	{"慢速"},
	{"中速"},
    {"快速"},
    {"极速"}

};
const uint8_t Test_Speedvalue_E[][6+1]=
{
	{"SLOW"},
	{"MED"},
    {"FAST"},
    {"TOP"}

};
const uint8_t Test_Compvalue[][6+1]=
{
	{"关闭"},
	{"打开"}

};

const uint8_t Test_VCompvalue[][6+1]=
{
	{"关闭"},
	{"打开"},
	{"绝对值"}
};

const uint8_t Test_VCompvalue_E[][6+1]=
{
	{"CLOSE"},
	{"OPEN"},
	{"ABS"}

};

const uint8_t Test_Compvalue_E[][6+1]=
{
	{"CLOSE"},
	{"OPEN"}

};

const uint8_t Setup_Beep[][6+1]=
{
	"关闭  ",
	"合格  ",
	"不合格",
	"开路ON"
};
const uint8_t Setup_Beep1[][7+1]=
{
	"关闭  ",
	"合格  ",
	"不合格",
	"开路OFF"
};
const uint8_t Setup_Beep_E[][6+1]=
{
	"CLOSE ",
	"PASS  ",
	"FAIL ",
	"OPENON"
};
const uint8_t Setup_Beep_E1[][7+1]=
{
	"CLOSE ",
	"PASS  ",
	"FAIL ",
	"OPENOFF"
};
const uint8_t INPUT[][7]={"输入"};
const uint8_t INPUT_E[][7]={"INPUT"};
    

const uint8_t User_Range[][10+1]=
{
    {"自动  "},
    {"10 mΩ"},
    {"100mΩ"},
    {"1Ω   "},
    {"10Ω  "},
    {"100 Ω"},
    {"1k  Ω"},
    {"10k Ω"},
    

};
const uint8_t User_Range_E[][10+1]=
{
    {"AUTO  "},
    {"10 mΩ"},
    {"100mΩ"},
    {"1Ω   "},
    {"10Ω  "},
    {"100 Ω"},
    {"1k  Ω"},
    {"10k Ω"},
    

};
const u8 DISP_UINT[][4]=
{
    {"mΩ"},
    {" Ω"},
    {"kΩ"}

};
const uint8_t User_Rangeset[][11]=
{
    {"AUTO"},
    {"HAND"},
    {"NORMAL"},
    {"INCR +"},
    {"DECR -"},
    
};
const uint8_t Sys_Setitem[][10+1]=
{
	{"串口开关"},
	{"波特率  "},
	{"U盘开关"},
	{"I/O口   "},
	{"显示语言"},
	
	{"日期"},
    {"时间"},
    {"文件名称"},
	
};
const uint8_t Sys_Setitem_E[][10+1]=
{
	{"SERIAL"},
	{"BAUD "},
	{"U_DISK"},
	{"I/O_PORT"},
	{"LANGUAGE"},
	
	{"DATE"},
    {"TIME"},
    {"FILE"},
	
};

const uint8_t Sys_Buard_Value[][6+1]=
{
	
	{"4800"},
	{"9600"},
	{"14400"},
	{"19200"},
	{"115200"},
	{"38400"},
	{"56000"},
	{"2400"},

};
const uint8_t Sys_Language_Value[][7+1]=
{
	{"中文"},
	{"ENGLISH"}

};
const uint8_t BiasButton_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"DECR -"},
    {"INCR +"},

};

const uint8_t Sys_Sys[][20+1]=
{
	{"仪器型号  JK2520C"},
	{"软件版本  Ver:2.8"},
	{"硬件版本  Ver:1.1"},
	{"仪器编号"},
//	{"账号    "},



};
const uint8_t Sys_Sys_E[][20+1]=
{
	{"INST MODEL  JK2520C"},
	{"SOFT VER   Ver:2.8"},
	{"HARD VER   Ver:1.1"},
	{"SERIALNO"},
//	{"账号    "},



};
const uint8_t Sys_Sys1[][20+1]=
{
	{"仪器型号  2520C"},
	{"软件版本  Ver:2.8"},
	{"硬件版本  Ver:1.1"},
	{"仪器编号"},
//	{"账号    "},



};
const uint8_t Sys_Sys_E1[][20+1]=
{
	{"INST MODEL  2520C"},
	{"SOFT VER   Ver:2.8"},
	{"HARD VER   Ver:1.1"},
	{"SERIALNO"},
//	{"账号    "},



};
const uint8_t Range_Disp_Test[][7][11+1]=
{
    {
        {"AUTO 10mΩ"},
        {"AUTO100mΩ"},
        {"AUTO   1Ω"},
        {"AUTO  10Ω"},
        {"AUTO 100Ω"},
        {"AUTO  1kΩ"},
        {"AUTO 10kΩ"},
       
    },
    {
        {"HAND 10mΩ"},
        {"HAND100mΩ"},
        {"HAND   1Ω"},
        {"HAND  10Ω"},
        {"HAND 100Ω"},
        {"HAND  1kΩ"},
        {"HAND 10kΩ"},
       
    }


};
const u8 RANG_4094[8]={0x48,0x48,0x48,0x80,0x01,0x01,0x02,0x04};//换挡的时候的4094的值
//位操作增减数值
const u16 POW_NUM[4]=
{
	1000,
	100,
	10,
	1,
};
const u32 RANGE_RATE[5]=
{
	1000,
	10000,
	100000,
	1000000,
	10000000,
	


};
const vu32 Debug_Compvalue[][2]=
{
	{8000,12000},
	{8000,12000},
	{8000,12000},
	{8000,12000},
    {8000,12000},
	{8000,12000},
	{8000,12000},
	{50000,80000},
	{50000,80000}


};
const vu32 Debug_Compvaluemind[]=
{
	10000,10000,10000,10000,10000,10000,60000,60000


};
#define FACTORY_MENU_MAX (sizeof(FactoryTab)/sizeof(FactoryTab[0])-1)
    Test_ValueTypedef Test_Value;

Test_ValueTypedef Test_Value_V;
//==========================================================
//设置菜单

//==========================================================
//确认提示

//==========================================================
//数值提示

//==========================================================
//选择提示

//==========================================================
//参数菜单

//==========================================================
//编号

//==========================================================
//系统菜单

//==========================================================
//系统项上、下限
const u16 SystemLimit[][2]=
{
	{500, 1},
	{9999, 0},
	{9999, 0},
	{9999, 0},
};
//==========================================================
//系统项上、下限
const u16 SystemLimitDot[5]=
{
	0,1,0,0
	
};
//==========================================================
//参数项上、下限
const u8 ParameterLimit[][2]=
{
	{ RANGE_MAX, 0},//量程模式
	{ 2, 0},//速度
	{ 255, 1},//平均次数
	{ 1, 0},//扫描次数
	{ 3, 0},//测试模式
    { 14, 0},//温度通道选择
};
const float Debug_Limit[2]={1.3,0.7};
//==========================================================
//系统状态


//==========================================================
//出厂信息

//==========================================================

//==========================================================
//参数菜单项
enum ParameterMenuEnum
{	
	PARAMETER_MENU_RANGE ,
	PARAMETER_MENU_BEEP ,
	PARAMETER_MENU_CLEAR ,
	PARAMETER_MENU_TRIG ,
	PARAMETER_MENU_UART ,
};

//==========================================================
//系统菜单项
enum SYSTEMMenuEnum
{	
	SYSTEM_MENU_VOUT ,
	SYSTEM_MENU_Ttime,
	SYSTEM_MENU_RH	 ,
	SYSTEM_MENU_RL	 ,
	
};
//==========================================================
//函数名称：InitGlobalValue
//函数功能：初始化全局变量
//入口参数：无
//出口参数：无
//创建日期：2015.10.28
//修改日期：2015.10.28 15:33
//备注说明：无
//==========================================================
void InitGlobalValue(void)
{
	u16 len;
	u8 * buf;
						 
	buf=(u8 *)&Jk516save;//数据首地址
	len=sizeof(Jk516save_TypeDef);
	while(len--)
	{
		*buf=0;//清空
		buf++;
	}
	F_Password=FALSE;//密码有效标志	
}
//==========================================================
//函数名称：Parameter_valuecomp
//函数功能：初始化值得比较
//入口参数：无
//出口参数：无
//创建日期：2015.10.28
//修改日期：2015.10.28 15:33
//备注说明：无
//==========================================================
void Parameter_valuecomp(void)
{
	u8 i;
//	u8 *pt;
//	for(i=0;i<PARAMETER_MENU_MAX;i++)
//	{	pt=(u8* )&Jk516save.Set_Data;
//		if(*pt>ParameterLimit[i][0])
//            *pt=ParameterLimit[i][1];
//	}
	if(Jk516save.open>1)
        Jk516save.open=0;
    if(Jk516save.Set_Data.trip>3)
        Jk516save.Set_Data.trip=0;
    if(Jk516save.Set_Data.speed>3)
        Jk516save.Set_Data.speed=0;
    if(Jk516save.Set_Data.Res_comp>1)
        Jk516save.Set_Data.Res_comp=0;
    if(Jk516save.Set_Data.V_low.Dot>5)
        Jk516save.Set_Data.V_low.Dot=3;
    if(Jk516save.Set_Data.V_low.Num>1200000)
        Jk516save.Set_Data.V_low.Num=120000;
    if(Jk516save.Set_Data.V_low.Unit>5)
        Jk516save.Set_Data.V_low.Unit=0;
    if(Jk516save.Set_Data.Range>10)
        Jk516save.Set_Data.Range=0;
    if(Jk516save.Set_Data.Range_Set>10)
        Jk516save.Set_Data.Range_Set=0;
    if(Jk516save.Set_Data.beep>3)
        Jk516save.Set_Data.beep=0;
    if(Jk516save.Set_Data.V_comp>2)
        Jk516save.Set_Data.V_comp=0;
    
    if(Jk516save.Set_Data.Nominal_Res.Dot>5)
        Jk516save.Set_Data.Nominal_Res.Dot=3;
    if(Jk516save.Set_Data.Nominal_Res.Num>1200000)
        Jk516save.Set_Data.Nominal_Res.Num=12000;
    if(Jk516save.Set_Data.Nominal_Res.Unit>5)
        Jk516save.Set_Data.Nominal_Res.Unit=0;
    
    if(Jk516save.Set_Data.High_Res.Dot>5)
        Jk516save.Set_Data.High_Res.Dot=3;
    if(Jk516save.Set_Data.High_Res.Num>120000)
        Jk516save.Set_Data.High_Res.Num=12000;
    if(Jk516save.Set_Data.High_Res.Unit>5)
        Jk516save.Set_Data.High_Res.Unit=0;
    
    if(Jk516save.Set_Data.Res_low.Dot>5)
        Jk516save.Set_Data.Res_low.Dot=3;
    if(Jk516save.Set_Data.Res_low.Num>120000)
        Jk516save.Set_Data.Res_low.Num=12000;
    if(Jk516save.Set_Data.Res_low.Unit>5)
        Jk516save.Set_Data.Res_low.Unit=0;
    
    if(Jk516save.Set_Data.Nominal_V.Dot>5)
        Jk516save.Set_Data.Nominal_V.Dot=3;
    if(Jk516save.Set_Data.Nominal_V.Num>1200000)
        Jk516save.Set_Data.Nominal_V.Num=120000;
    if(Jk516save.Set_Data.Nominal_V.Unit>5)
        Jk516save.Set_Data.Nominal_V.Unit=0;
    
    
    
    if(Jk516save.Set_Data.V_high.Dot>5)
        Jk516save.Set_Data.V_high.Dot=3;
    if(Jk516save.Set_Data.V_high.Num>1200000)
        Jk516save.Set_Data.V_high.Num=120000;
    if(Jk516save.Set_Data.V_high.Unit>5)
        Jk516save.Set_Data.V_high.Unit=0;
    
    if(Jk516save.Sys_Setvalue.buard>10)
        Jk516save.Sys_Setvalue.buard=0;
    if(Jk516save.Sys_Setvalue.lanage>1)
        Jk516save.Sys_Setvalue.lanage=0;
    if(Jk516save.Sys_Setvalue.plc>1)
        Jk516save.Sys_Setvalue.plc=0;
    if(Jk516save.Sys_Setvalue.uart>1)
        Jk516save.Sys_Setvalue.uart=0;
    if(Jk516save.Sys_Setvalue.u_flag>1)
        Jk516save.Sys_Setvalue.u_flag=0;
    for(i=0;i<20;i++)
    {
        if(Jk516save.Sys_Setvalue.textname[i]>128)
            Jk516save.Sys_Setvalue.textname[i]=0;
            
    
    }
    for(i=0;i<10;i++)
    {
        if(Jk516save.fac_num[i]>128)
        {
            if(i<8)
                Jk516save.fac_num[i]='0';
            else
                Jk516save.fac_num[i]=0;
                
        }
    
    }
    
    for(i=0;i<RANGE_MAX+2;i++)
    {
        if((Jk516save.Debug_Value[i].ad_value>Debug_Limit[0])||(Jk516save.Debug_Value[i].ad_value<Debug_Limit[1]))
            Jk516save.Debug_Value[i].ad_value=1;
        if((Jk516save.Debug_Value[i].standard>Debug_Compvalue[i][0])||(Jk516save.Debug_Value[i].standard<Debug_Compvalue[i][1]))
            Jk516save.Debug_Value[i].standard=Debug_Compvaluemind[i];
        if(i<RANGE_MAX)
        {
            if(Jk516save.Clear[i]>100000)
                Jk516save.Clear[i]=0;
        
        }
        else
        {
            if(Jk516save.Clear_V[i-RANGE_MAX]>500000)
                Jk516save.Clear_V[i-RANGE_MAX]=0;
        
        
        }
        
    }


}
//==========================================================
//函数名称：Hex_Format
//函数功能：格式化整数值到DispBuf
//入口参数：整数，小数位数，数值位数，显示零
//出口参数：无
//创建日期：2014.12.15
//修改日期：2015.01.14 13:12
//备注说明：dot=0-XXXXX XXXX.X XXX.XX XX.XXX X.XXXX-Dot=4
//注意事项：最多5位数
//==========================================================
//void Hex_Format(u16 dat , u8 Dot , u8 len , u8 dispzero)
//{
//	u8 i,j;

//	if(len>5)len=5;//最大长度5
//	if(len==0)len=1;
//	if(Dot>4)Dot=4;//最大小数点4
//	if(Dot>len)Dot=len;

//	for(i=0;i<sizeof(DispBuf);i++)	//清空缓冲区
//		DispBuf[i]=' ';

//	for(i=len;i>0;i--)	//数制转换
//	{
//		DispBuf[i-1] = dat%10 + '0';//转为ASCII
//		dat/=10;
//	}
//	
//	if(Dot)//有小数点
//	{
//		j=len;
//		i=Dot;//取小数点位置
//		while(i--)
//		{
//			DispBuf[j]=DispBuf[j-1];
//			j--;
//		}
//		DispBuf[j]='.';//加入小数点
//	}

//	if(dispzero==FALSE)//不显示无效零
//	{
//		for(i=0;i<(len-1);i++)
//		{
//			if((DispBuf[i]=='0')&&(DispBuf[i+1]!='.'))
//				DispBuf[i]=' ';
//			else
//				break;
//		}
//	}
//	if(Dot)len++;
//	for(i=len;i<sizeof(DispBuf);i++)//末尾补结束符'\0'
//	{
//		DispBuf[i]=0;
//	}			
//}

void Hex_Format(u32 dat , u8 Dot , u8 len , u8 dispzero)
{
	u8 i,j;

	if(len>8)len=8;//最大长度8
	if(len==0)len=1;
	if(Dot>7)Dot=7;//最大小数点7
	if(Dot>len)Dot=len;

	for(i=0;i<sizeof(DispBuf);i++)	//清空缓冲区
		DispBuf[i]=' ';

	for(i=len;i>0;i--)	//数制转换
	{
		DispBuf[i-1] = dat%10 + '0';//转为ASCII
		dat/=10;
	}
	
	if(Dot)//有小数点
	{
		j=len;
		i=Dot;//取小数点位置
		while(i--)
		{
			DispBuf[j]=DispBuf[j-1];
			j--;
		}
		DispBuf[j]='.';//加入小数点
	}

	if(dispzero==FALSE)//不显示无效零
	{
		for(i=0;i<(len-1);i++)
		{
			if((DispBuf[i]=='0')&&(DispBuf[i+1]!='.'))
				DispBuf[i]=' ';
			else
				break;
		}
	}
	if(Dot)len++;
	for(i=len;i<sizeof(DispBuf);i++)//末尾补结束符'\0'
	{
		DispBuf[i]=0;
	}			
}
//==========================================================
//函数名称：Disp_Product_Model
//函数功能：显示产品型号
//入口参数：无
//出口参数：无
//创建日期：2015.10.27
//修改日期：2015.10.27 16:00
//备注说明：无
//==========================================================


//==========================================================
//函数名称：Disp_Software_Version
//函数功能：显示软件版本
//入口参数：无
//出口参数：无
//创建日期：2015.10.27
//修改日期：2015.10.27 16:00
//备注说明：无
//==========================================================

//==========================================================
//函数名称：Disp_Compile_Date
//函数功能：显示编译日期
//入口参数：无
//出口参数：无
//创建日期：2015.10.27
//修改日期：2015.10.27 16:00
//备注说明：无
//==========================================================


//==========================================================
//函数名称：Disp_PowerOn_Menu
//函数功能：显示开机界面
//入口参数：无
//出口参数：无
//创建日期：2015.10.26
//修改日期：2015.10.26 15:00
//备注说明：无
//==========================================================

//==========================================================
//函数名称：Number_Setup
//函数功能：数值框设置处理
//入口参数：NumBox_TypeDef
//出口参数：键值
//创建日期：2015.01.07
//修改日期：2015.08.19 13:53
//备注说明：无
//==========================================================

//==========================================================
//函数名称：ListBox_Setup
//函数功能：列表框设置
//入口参数：无
//出口参数：键值
//创建日期：2015.01.08
//修改日期：2015.08.19 13:48
//备注说明：列表项标题长度固定为8个字符
//==========================================================


//==========================================================
//函数名称：Disp_Frame
//函数功能：显示界面框架
//入口参数：无
//出口参数：无
//创建日期：2014.12.26
//修改日期：2015.08.18 16:30
//备注说明：整屏更新
//==========================================================

//==========================================================
//函数名称：Disp_Boot_Number
//函数功能：显示开机次数
//入口参数：无
//出口参数：无
//创建日期：2014.11.13
//修改日期：2014.11.13 12:42
//备注说明：无
//==========================================================

//==========================================================
//函数名称：Disp_Software_Version
//函数功能：显示软件版本
//入口参数：无
//出口参数：无
//创建日期：2015.01.09
//修改日期：2015.01.09 15:13
//备注说明：无
//==========================================================

//==========================================================
//函数名称：Disp_Hardware_Version
//函数功能：显示硬件版本
//入口参数：无
//出口参数：无
//创建日期：2015.01.09
//修改日期：2015.01.09 15:13
//备注说明：无
//==========================================================

//==========================================================
//函数名称：Disp_Compile_Date
//函数功能：显示编译日期
//入口参数：无
//出口参数：无
//创建日期：2015.01.09
//修改日期：2015.01.09 15:13
//备注说明：无
//==========================================================

//==========================================================
//函数名称：Disp_Product_Model
//函数功能：显示产品型号
//入口参数：无
//出口参数：无
//创建日期：2015.01.09
//修改日期：2015.01.09 15:13
//备注说明：无
//==========================================================

//==========================================================
//函数名称：Disp_Company_Name
//函数功能：显示公司名称
//入口参数：无
//出口参数：无
//创建日期：2015.01.09
//修改日期：2015.01.09 15:13
//备注说明：无
//==========================================================

//==========================================================
//函数名称：Disp_Company_Telephone
//函数功能：显示厂商电话
//入口参数：无
//出口参数：无
//创建日期：2015.01.09
//修改日期：2015.09.06 10:07
//备注说明：无
//==========================================================

//==========================================================
//函数名称：Disp_Company_Http
//函数功能：显示厂商主页
//入口参数：无
//出口参数：无
//创建日期：2015.01.09
//修改日期：2015.09.06 10:07
//备注说明：无
//==========================================================


//==========================================================
//函数名称：Disp_Chip_ID
//函数功能：显示芯片ID号
//入口参数：无
//出口参数：无
//创建日期：2014.11.13
//修改日期：2014.11.13 13:31
//备注说明：无
//==========================================================
//STC12系列CPU内部ID号首地址定义(F1-F7为ID号)
#define MCU_ID_ADDR (0xF1)
//==========================================================

//==========================================================
////无序列号的显示固定值


//==========================================================
//函数名称：Disp_Product_ID
//函数功能：显示出厂编号
//入口参数：无
//出口参数：无
//创建日期：2015.01.09
//修改日期：2015.08.19 10:03
//备注说明：无
//==========================================================


//==========================================================
//函数名称：Disp_PowerOn_Menu
//函数功能：显示开机界面
//入口参数：无
//出口参数：无
//创建日期：2014.12.26
//修改日期：2015.08.18 09:38
//备注说明：无
//==========================================================

//==========================================================
//函数名称：Disp_Group
//函数功能：显示组别
//入口参数：无
//出口参数：无
//创建日期：2015.01.05
//修改日期：2015.01.05 13:18
//备注说明：无
//==========================================================

//==========================================================
//函数名称：Disp_Parameter_Value
//函数功能：显示设置值
//入口参数：step:第几步骤；index:第几项参数；highlight:非零高亮显示
//出口参数：无
//创建日期：2015.01.05
//修改日期：2015.09.06 09:14
//备注说明：数值位数+小数点+单位的长度固定为7个字符
//==========================================================


//==========================================================
//函数名称：Disp_SysMessage
//函数功能：显示系统信息
//入口参数：无
//出口参数：无
//创建日期：2014.10.15
//修改日期：2014.10.15 15:17
//备注说明：无
//==========================================================


//==========================================================
//函数名称：Disp_Idle_Menu
//函数功能：显示待测菜单
//入口参数：无
//出口参数：无
//创建日期：2015.10.26
//修改日期：2015.10.29 10:27
//备注说明：无
//==========================================================

//==========================================================
//函数名称：Check_Parameter_Limit
//函数功能：参数值检查
//入口参数：无
//出口参数：无
//创建日期：2015.10.28 
//修改日期：2015.10.28 13:50
//备注说明：上限、下限检查,	L<=x<=H
//==========================================================
void Check_Parameter_Limit(void)
{
	u8 i;
	u8 *pt;

	pt=(u8* )&SaveData.Parameter;
	for(i=0;i<PARAMETER_MENU_MAX;i++)//共5项
	{
		if(*pt>ParameterLimit[i][0])//Max
			*pt= ParameterLimit[i][0];//Max
		if(*pt<ParameterLimit[i][1])//Min
			*pt=ParameterLimit[i][1];//Min
		pt++;
	}
}
//==========================================================
//函数名称：Range_Control
//函数功能：量程控制
//入口参数：u8 dat
//出口参数：无
//创建日期：2014.09.25
//修改日期：2015.09.04 15:35
//备注说明：量程(0-4) 100 1k 10k 100k 1M
//==========================================================
//u8 _4051value[]={0x48,0x48,0x48,0x88,0x09,0x01,0x02,0x04};
//u8 _gpio_sel[]={2,2,1,1,1,0,0,0};
//u8 _pga_sel[]={3,3,1,1,1,0,0,0};
u8 Pga_value[]={0x30,0x34,0x32,0x20,0x28,0x08,0x09};
void Range_Control(u8 dat,u8 V_Range)
{
//    u8 buff;
//    RangeChange_Flag=1;
//    cpld_buff[0]=0xaa;
//    if(dat>RANGE_MAX)
//        dat=RANGE_MAX;
//    buff=Pga_value[dat];
//    if(open_flag==1||dat==6)
//       buff|=0x01;
//    else
//        buff&=0xfe;
//  
//    if(dat<5)
//        buff|=0x40;    
//    if(V_Range)
//    {
//        buff|=0x20;
//    
//    }
//    else
//        buff&=0xdf;
//    if(open_flag)   //开路 
//        buff|=0x80;
//    else
//        buff&=0x7f;
//    cpld_buff[1]=buff;
//    Send_To_Cpld(cpld_buff,2);
//    delay_ms(120);
    u8 buff;
    RangeChange_Flag=1;
    cpld_buff[0]=0xaa;
    if(dat>RANGE_MAX)
        dat=RANGE_MAX;
    buff=Pga_value[dat];
    if(Jk516save.Set_Data.Range_Set==0)//自动
    {
    if(open_flag==1||dat==6)
       buff|=0x01;
    else
        buff&=0xfe;
    }
    else
    {
        if(dat==6)
           buff|=0x01;
        else
            buff&=0xfe;
    
    }
  
    if(dat<5)
        buff|=0x40;    
    if(V_Range)
    {
        buff|=0x20;
    
    }
    else
        buff&=0xdf;
//    if(open_flag)   //开路 
//        buff|=0x80;
//    else
//        buff&=0x7f;
    cpld_buff[1]=buff;
    Send_To_Cpld(cpld_buff,2);
    delay_ms(120);
}
//void V_Range_Control(u8 dat)
//{
//    u8 buff;
//    cpld_buff[0]=0xaa;
//    
//   
//        
//    if(dat)
//    {
//        cpld_buff[1]|=0x20;
//    
//    }
//    else
//        cpld_buff[1]&=0xdf;
//        
//    Send_To_Cpld(cpld_buff,2);
//    //delay_ms(80);


//}


//==========================================================
//函数名称：Disp_Setup_Menu
//函数功能：显示设置菜单
//入口参数：index:第几项
//出口参数：无
//创建日期：2015.10.28
//修改日期：2015.10.28 13:25
//备注说明：无
//==========================================================

//==========================================================
//函数名称：Disp_Parameter_Value
//函数功能：显示参数
//入口参数：index:第几项参数
//出口参数：无
//创建日期：2015.10.28
//修改日期：2015.10.28 11:25
//备注说明：无
//==========================================================

//==========================================================
//函数名称：ListBox_Setup
//函数功能：列表框设置
//入口参数：无
//出口参数：键值
//创建日期：2015.10.28
//修改日期：2015.10.28 09:50
//备注说明：标题栏字符长度,TITLE_LEN_MAX=8
//==========================================================

//==========================================================
//函数名称：Disp_Parameter_Menu
//函数功能：显示参数界面
//入口参数：无
//出口参数：无
//创建日期：2015.10.28
//修改日期：2015.10.28 13:20
//备注说明：无
//==========================================================


//==========================================================
//函数名称：Parameter_Setup
//函数功能：参数菜单设置处理
//入口参数：无
//出口参数：无
//创建日期：2015.10.28
//修改日期：2015.10.28 11:49
//备注说明：无
//==========================================================

//==========================================================
//函数名称：Disp_System_Value
//函数功能：显示系统值
//入口参数：index:第几项参数
//出口参数：无
//创建日期：2015.10.28
//修改日期：2015.10.28 14:20
//备注说明：无
//==========================================================

//==========================================================
//函数名称：Disp_System_Menu
//函数功能：显示系统界面
//入口参数：index:第几项参数
//出口参数：无
//创建日期：2015.10.28
//修改日期：2015.10.28 14:23
//备注说明：无
//==========================================================

//==========================================================
//函数名称：Number_Setup
//函数功能：数值框设置处理
//入口参数：NumBox_TypeDef
//出口参数：键值
//创建日期：2015.10.28
//修改日期：2015.10.28 09:44
//备注说明：无
//==========================================================
//==========================================================
//函数名称：Range_Changecomp
//函数功能：电阻测试换挡
//入口参数：无
//出口参数：无
//创建日期：2015.10.26 
//修改日期：2015.10.29 13:35
//备注说明：无
//==========================================================
void Range_Changecomp(void)
{
	
//	bool f_disp=FALSE;//显示标志
	bool f_upper=FALSE;//量程过压标志
	bool f_below=FALSE;//量程欠压标志
	bool f_switch=FALSE;//量程切换标志
    bool V_upper=FALSE;//量程过压标志
	bool V_below=FALSE;//量程欠压标志
	bool V_switch=FALSE;//量程切换标志
	u8 i;
	//量程自动换挡处?
	if(Range_value>RANGR_LIMIT_HIGH)//高于量程上限Ad_value
	{
		f_upper=TRUE;//量程过压标志
		f_below=FALSE;//量程欠压标志
		for(i=0;i<FIT_INUM;i++)
			i_buff[i]=0;
			ffit_data1=0;
			count_ffit=0;
	}
	else if(Range_value<RANGR_LIMIT_LOW)//低于量程下限
	{
		f_below=TRUE;//量程低压标志
		f_upper=FALSE;//量程欠压标志
	}
	else
	{
		f_upper=FALSE;//量程过压标志
		f_below=FALSE;//量程欠压标志
	}
	f_switch=FALSE;//量程切换标志
	//f_switch=TRUE;
	if(Jk516save.Set_Data.Range_Set==0)
	{
		if((Range<RANGE_MAX)&&(f_upper==TRUE))//量程非最高且低压
		{
			Range++;
			f_switch=TRUE;//量程切换标志
			for(i=0;i<FIT_INUM;i++)
			i_buff[i]=0;
			ffit_data1=0;
			count_ffit=0;
		} 
        else
		if((Range>0)&&(f_below==TRUE))//量程非最低且过压
		{
			Range--;
			f_switch=TRUE;//量程切换标志
			for(i=0;i<FIT_INUM;i++)
			i_buff[i]=0;
			ffit_data1=0;
			count_ffit=0;
		}
//        if(Range_Value_V>RANGE_LIMIT_VHIGH)//高于量程上限Ad_value
//        {
//            V_upper=TRUE;//量程过压标志
//            V_below=FALSE;//量程欠压标志
//            
//        }
//        else if(Range_Value_V<RANGE_LIMIT_VLOW)//低于量程下限
//        {
//            V_below=TRUE;//量程低压标志
//            V_upper=FALSE;//量程欠压标志
//        }
//        else
//        {
//            V_upper=FALSE;//量程过压标志
//            V_below=FALSE;//量程欠压标志
//        }
//        V_switch=FALSE;//量程切换标志
//        //f_switch=TRUE;
//        
//        if((V_Range<RANGE_V_MAX)&&(V_upper==TRUE))//量程非最高且低压
//        {
//            V_Range++;
//            V_switch=TRUE;//量程切换标志
//            
//        } 
//        else
//        if((V_Range>0)&&(V_below==TRUE))//量程非最低且过压
//        {
//            V_Range--;
//            V_switch=TRUE;//量程切换标志
//           
//        }
    }
	if((f_switch==TRUE)||(V_switch==TRUE))//量程切换标志
	{
        range_over=1;//增加不换档标志，用于查看是否在稳定的量程测量的
//        Range=0;
		Range_Control(Range,V_Range);//量程控制
        f_switch=FALSE;
        V_switch=FALSE;
	}
//    V_Range_Control(0);
    else
        range_over=0;
        
}
//==========================================================
//函数名称：Range_Changecomp
//函数功能：电阻测试换挡
//入口参数：无
//出口参数：无
//创建日期：2015.10.26 
//修改日期：2015.10.29 13:35
//备注说明：无
//==========================================================
void VRange_Changecomp(void)
{
	
//	bool f_disp=FALSE;//显示标志
	bool f_upper=FALSE;//量程过压标志
	bool f_below=FALSE;//量程欠压标志
	bool f_switch=FALSE;//量程切换标志
    bool V_upper=FALSE;//量程过压标志
	bool V_below=FALSE;//量程欠压标志
	bool V_switch=FALSE;//量程切换标志
	u8 i;
//	//量程自动换挡处?
//	if(Range_value>RANGR_LIMIT_HIGH)//高于量程上限Ad_value
//	{
//		f_upper=TRUE;//量程过压标志
//		f_below=FALSE;//量程欠压标志
//		for(i=0;i<FIT_INUM;i++)
//			i_buff[i]=0;
//			ffit_data1=0;
//			count_ffit=0;
//	}
//	else if(Range_value<RANGR_LIMIT_LOW)//低于量程下限
//	{
//		f_below=TRUE;//量程低压标志
//		f_upper=FALSE;//量程欠压标志
//	}
//	else
//	{
//		f_upper=FALSE;//量程过压标志
//		f_below=FALSE;//量程欠压标志
//	}
//	f_switch=FALSE;//量程切换标志
	//f_switch=TRUE;
	if(Jk516save.Set_Data.Range_Set==0)
	{
//		if((Range<RANGE_MAX)&&(f_upper==TRUE))//量程非最高且低压
//		{
//			Range++;
//			f_switch=TRUE;//量程切换标志
//			for(i=0;i<FIT_INUM;i++)
//			i_buff[i]=0;
//			ffit_data1=0;
//			count_ffit=0;
//		} 
//        else
//		if((Range>0)&&(f_below==TRUE))//量程非最低且过压
//		{
//			Range--;
//			f_switch=TRUE;//量程切换标志
//			for(i=0;i<FIT_INUM;i++)
//			i_buff[i]=0;
//			ffit_data1=0;
//			count_ffit=0;
//		}
        if(Range_Value_V>RANGE_LIMIT_VHIGH)//高于量程上限Ad_value
        {
            V_upper=TRUE;//量程过压标志
            V_below=FALSE;//量程欠压标志
            
        }
        else if(Range_Value_V<RANGE_LIMIT_VLOW)//低于量程下限
        {
            V_below=TRUE;//量程低压标志
            V_upper=FALSE;//量程欠压标志
        }
        else
        {
            V_upper=FALSE;//量程过压标志
            V_below=FALSE;//量程欠压标志
        }
        V_switch=FALSE;//量程切换标志
        //f_switch=TRUE;
        
        if((V_Range<RANGE_V_MAX)&&(V_upper==TRUE))//量程非最高且低压
        {
            V_Range++;
            V_switch=TRUE;//量程切换标志
            
        } 
        else
        if((V_Range>0)&&(V_below==TRUE))//量程非最低且过压
        {
            V_Range--;
            V_switch=TRUE;//量程切换标志
           
        }
    }
	if((f_switch==TRUE)||(V_switch==TRUE))//量程切换标志
	{
        range_over=1;//增加不换档标志，用于查看是否在稳定的量程测量的
//        Range=0;
		Range_Control(Range,V_Range);//量程控制
        f_switch=FALSE;
        V_switch=FALSE;
	}
//    V_Range_Control(0);
    else
        range_over=0;
        
}
//==========================================================
//函数名称：Get_FFT
//函数功能：计算测试值结果
//入口参数：无
//出口参数：无
//创建日期：2014.11.08 
//修改日期：2015.08.27 14:43
//备注说明：更新Voltage
//==========================================================
void Get_FFT(void)
{
//	float dat=0;
//	u8 i;
	ffit_data1++;	

	if(((Test_value.Test_I>40000)||(Test_value.Test_I<2000))&&(Range!=RANGE_MAX))
		ffit_data1=0;

	Float.Digit=Float.Digit/1000;

}

void Test_Debug(void)
{
    float a,b,vmid;
    

     if(Range ==6)
    {
        if(Res_count.r<9000)
        {
            a=Res_count.r;
            b=0.778f*Res_count.r;
            Res_count.r=a*a*2/100000+b+292+70;
        }
        else
        {
            a=Res_count.r;
            b=0.4818f*Res_count.r;
            Res_count.r=a*a*3/100000+b+2410;
        
        }
    
    
    }
	vmid = V_ad;
	//Range_Value_V
//        Res_count.r=Res_count.r/Jk516save.Debug_Value[Range].ad_value;
//        V_ad=(float)V_ad/Jk516save.Debug_Value[7+V_Range].ad_value;
	if(Jk516save.Set_Data.speed == 0)
	{
        Res_count.r=Res_count.r/Jk516save.Debug_Value[Range].ad_value;
		if(V_Range == 0)
		{
			V_ad=(float)vmid/Jk516save.Debug_Value[7].ad_value;
			if(V_ad>10000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value[8].ad_value;
			}
			if(V_ad>20000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value[9].ad_value;
			}
			if(V_ad>40000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value[10].ad_value;
			}
		}else{
			V_ad=(float)V_ad/Jk516save.Debug_Value[11].ad_value;
			if(V_ad>15000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value[12].ad_value;
			}
			if(V_ad>30000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value[13].ad_value;
			}
			if(V_ad>45000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value[14].ad_value;
			}
			if(V_ad>60000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value[15].ad_value;
			}
			if(V_ad>80000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value[16].ad_value;
			}

		}
	}else if(Jk516save.Set_Data.speed == 1){
		Res_count.r=Res_count.r/Jk516save.Debug_Value1[Range].ad_value;
		if(V_Range == 0)
		{
			V_ad=(float)vmid/Jk516save.Debug_Value1[7].ad_value;
			if(V_ad>10000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value1[8].ad_value;
			}
			if(V_ad>20000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value1[9].ad_value;
			}
			if(V_ad>40000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value1[10].ad_value;
			}
			
		}else{
			V_ad=(float)vmid/Jk516save.Debug_Value1[11].ad_value;
			if(V_ad>15000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value1[12].ad_value;
			}
			if(V_ad>30000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value1[13].ad_value;
			}
			if(V_ad>45000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value1[14].ad_value;
			}
			if(V_ad>60000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value1[15].ad_value;
			}
			if(V_ad>80000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value1[16].ad_value;
			}
		}
	}else if(Jk516save.Set_Data.speed == 2){
		Res_count.r=Res_count.r/Jk516save.Debug_Value2[Range].ad_value;
		if(V_Range == 0)
		{
			V_ad=(float)vmid/Jk516save.Debug_Value2[7].ad_value;
			if(V_ad>10000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value2[8].ad_value;
			}
			if(V_ad>20000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value2[9].ad_value;
			}
			if(V_ad>40000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value2[10].ad_value;
			}
		}else{
			V_ad=(float)vmid/Jk516save.Debug_Value2[11].ad_value;
			if(V_ad>15000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value2[12].ad_value;
			}
			if(V_ad>30000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value2[13].ad_value;
			}
			if(V_ad>45000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value2[14].ad_value;
			}
			if(V_ad>60000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value2[15].ad_value;
			}
			if(V_ad>80000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value2[16].ad_value;
			}
		}
	}else if(Jk516save.Set_Data.speed == 3){
		Res_count.r=Res_count.r/Jk516save.Debug_Value3[Range].ad_value;
		if(V_Range == 0)
		{
			V_ad=(float)vmid/Jk516save.Debug_Value3[7].ad_value;
			if(V_ad>10000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value3[8].ad_value;
			}
			if(V_ad>20000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value3[9].ad_value;
			}
			if(V_ad>40000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value3[10].ad_value;
			}
		}else{
			V_ad=(float)vmid/Jk516save.Debug_Value3[11].ad_value;
			if(V_ad>15000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value3[12].ad_value;
			}
			if(V_ad>30000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value3[13].ad_value;
			}
			if(V_ad>45000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value3[14].ad_value;
			}
			if(V_ad>60000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value3[15].ad_value;
			}
			if(V_ad>80000)
			{
				V_ad=(float)vmid/Jk516save.Debug_Value3[16].ad_value;
			}
		}
	}
   
}



//==========================================================
//函数名称：Read_Ad
//函数功能：读取A/D值
//入口参数：无
//出口参数：无
//创建日期：2014.11.08 
//修改日期：2015.08.26 10:32
//备注说明：无
//==========================================================
void Read_Ad(void)
{

	//Test_value.Test_v.Test_v=Read_Channel(1);//电流AD值
    AD_Convert_read();
		
	
	
}
//==========================================================
//函数名称：System_Setup
//函数功能：系统设置处理
//入口参数：无
//出口参数：无
//创建日期：2015.10.28
//修改日期：2015.10.28 14:27
//备注说明：无
//==========================================================

//==========================================================
//函数名称：Resistance_Setup
//函数功能：电阻数设置处理
//入口参数：NumBox_TypeDef
//出口参数：键值
//创建日期：2016.03.10
//修改日期：2016.03.10 13:44
//备注说明：无
//==========================================================

//==========================================================
//系统菜单项

//==========================================================

//==========================================================

//==========================================================

//==========================================================

//==========================================================
//函数名称：Check_System_Limit
//函数功能：系统值检查
//入口参数：步骤
//出口参数：无
//创建日期：2015.01.22 
//修改日期：2015.08.18 13:28
//备注说明：含上限、下限检查
//==========================================================


//==========================================================
//函数名称：Disp_System_Info
//函数功能：显示系统信息
//入口参数：u8 index-第几项
//出口参数：无
//创建日期：2015.01.21
//修改日期：2015.08.19 09:48
//备注说明：按index进行分页显示
//==========================================================

//==========================================================
//函数名称：Disp_System_Menu
//函数功能：显示系统界面
//入口参数：无
//出口参数：无
//创建日期：2015.01.22
//修改日期：2015.08.19 09:00
//备注说明：无
//==========================================================


//==========================================================
//函数名称：Disp_System_Value
//函数功能：显示系统值
//入口参数：index:第几项参数
//出口参数：无
//创建日期：2015.01.21
//修改日期：2015.09.06 09:06
//备注说明：数值位数+小数点+单位的长度固定为4个字符
//==========================================================

//==========================================================
//函数名称：System_Setup
//函数功能：系统设置处理
//入口参数：无
//出口参数：无
//创建日期：2015.01.21
//修改日期：2015.09.06 09:10
//备注说明：无
//==========================================================


//==========================================================
//函数名称：Password_Check
//函数功能：密码校验
//入口参数：无
//出口参数：TRUE:有效
//创建日期：2015.01.29 
//修改日期：2015.08.24 11:40
//备注说明：无
//==========================================================

//==========================================================

//==========================================================
//密码项

//==========================================================
//函数名称：Disp_Password_Info
//函数功能：显示密码信息
//入口参数：无
//出口参数：无
//创建日期：2015.01.23
//修改日期：2015.08.19 10:45
//备注说明：无
//==========================================================

//==========================================================
//函数名称：Password_Setup
//函数功能：密码设置处理
//入口参数：无
//出口参数：无
//创建日期：2015.01.23
//修改日期：2015.08.20 11:03
//备注说明：无
//==========================================================

//==========================================================
//函数名称：FactoryInfo_Setup
//函数功能：出厂信息设置处理
//入口参数：无
//出口参数：无
//创建日期：2015.01.14
//修改日期：2015.08.24 11:54
//备注说明：
//按上下键：显示仪器信息；
//长按右键：进入出厂配置设置界面
//长按左键：加载默认出厂设置值
//长按设置：系统复位处理
//==========================================================


//==========================================================
//函数名称：Disp_Factory_Info
//函数功能：显示出厂信息
//入口参数：无
//出口参数：无
//创建日期：2015.01.09
//修改日期：2015.08.19 10:01
//备注说明：无
//==========================================================


//==========================================================
//函数名称：Disp_Instrument_Info
//函数功能：显示仪器信息
//入口参数：无
//出口参数：无
//创建日期：2015.01.22
//修改日期：2015.08.20 15:52
//备注说明：无
//==========================================================


//==========================================================
//配置项

//==========================================================
//配置项

//==========================================================
//配置项上、下限

 
//==========================================================
//函数名称：Check_Config_Limit
//函数功能：配置值检查
//入口参数：步骤
//出口参数：无
//创建日期：2015.01.22 
//修改日期：2015.09.06 11:15
//备注说明：含上限、下限检查
//==========================================================

//==========================================================
//函数名称：Disp_Config_Info
//函数功能：显示配置信息
//入口参数：无
//出口参数：无
//创建日期：2015.01.21
//修改日期：2015.08.19 10:17
//备注说明：无
//==========================================================


//==========================================================
//函数名称：Disp_Config_Menu
//函数功能：显示配置界面
//入口参数：无
//出口参数：无
//创建日期：2015.01.22
//修改日期：2015.08.19 10:16
//备注说明：无
//==========================================================


//==========================================================
//函数名称：Disp_Config_Value
//函数功能：显示配置值
//入口参数：index:第几项参数
//出口参数：无
//创建日期：2015.01.22
//修改日期：2015.08.19 10:22
//备注说明：数值位数+小数点+单位的长度固定为4个字符
//==========================================================


//==========================================================
//函数名称：Config_Setup
//函数功能：配置设置处理
//入口参数：无
//出口参数：无
//创建日期：2015.01.22
//修改日期：2015.08.19 10:29
//备注说明：无
//==========================================================

////==========================================================

////==========================================================
////函数名称：Disp_SysStatus
////函数功能：显示系统状态
////入口参数：无
////出口参数：无
////创建日期：2014.10.15
////修改日期：2014.10.15 15:17
////备注说明：无
////==========================================================

//==========================================================

//==========================================================
//函数名称：Disp_SysMessage
//函数功能：显示系统信息
//入口参数：无
//出口参数：无
//创建日期：2014.10.15
//修改日期：2014.10.15 15:17
//备注说明：无
//==========================================================

//==========================================================
//开机界面-整屏图片格式
//常州扬子(48*48)
//常州扬子电子有限公司(16*16)
//常州市新北区庆阳路2号
//电话：88226706 8826707

void _4094_data(void)
{
	_4094_databuff[0]=U16_4094;
	_4094_databuff[1]=U15_4094;
//	_4094_databuff[2]=U22_4094;
//	_4094_databuff[3]=_U21_4094;

}


//电阻计算，电压/电流*倍率
//void Get_Res(void)
//{
//	Res_count.r=Res_count.v*Res_count.rate/Res_count.i;
//}
uint32_t  ReadBE32U (volatile  uint8_t  *pmem)
{
    uint32_t   val;

    ((uint8_t *)&val)[0] = pmem[3];
    ((uint8_t *)&val)[1] = pmem[2];
    ((uint8_t *)&val)[2] = pmem[1];
    ((uint8_t *)&val)[3] = pmem[0];
    return (val);
}
Sort_TypeDef StrToint(vu8 *list)
{
    u32 date;
    Sort_TypeDef returndate;
    date=ReadBE32U(list);
    returndate.Num=date;
    returndate.Dot=list[4];
    returndate.Unit=list[5];
    return returndate;
    
    
    


}
void ComToSave(Rev_Combuff_Typedef *buff)
{
    Jk516save.Set_Data.trip=buff->Trip;
    Jk516save.Set_Data.speed=buff->Speed;
    Jk516save.Set_Data.Res_comp=buff->R_comp;
    
    Jk516save.Set_Data.Res_low=StrToint((buff->Rev_LoRes));
    Jk516save.Set_Data.V_comp=buff->Rev_V_comp;
    Jk516save.Set_Data.V_comp=buff->Rev_V_comp;
    Jk516save.Set_Data.V_low=StrToint((buff->Rev_V_Lo));
    Jk516save.Set_Data.Range=buff->Range;
    Jk516save.Set_Data.beep=buff->Beep;
    Jk516save.Set_Data.Nominal_Res=StrToint((buff->Norm_Res));
    Jk516save.Set_Data.High_Res=StrToint((buff->Rev_Hires));
    Jk516save.Set_Data.Nominal_V=StrToint((buff->Norm_V));
    Jk516save.Set_Data.V_high=StrToint((buff->Rev_VHi));



}
void Uart_Process(void)
{
	u8 i;
    u8 king;
    Rev_Combuff_Typedef *pt;
    pt=&Rev_Combuff;
        
    if (ComBuf.rec.end)
    {
         ComBuf.rec.end=FALSE;
         for(i=0;i<REC_LEN_MAX;i++)
             debug_buff[i]=ComBuf.rec.buf[i];
         king=debug_buff[1];
         switch(king)
         {
             case 0xF1://写设置数据
                 memcpy(pt,&ComBuf.rec.buf[2],sizeof(Rev_Combuff));//这里错误 要改正
                ComToSave(&Rev_Combuff);               
             break;
             case 0xf2://写校正命令
                debug_flag=1;
                Set_flag=1;
                 
             break;
             case 0xfe://清零命令
             break;
             case 0xf3://写校正数据
                 
             break;
             case 0xf4://读测试值
                 Send_Uart3str((char *)&Send_ComBuff);
                 
                 
             break;
             case 0xf5://设置扫描板地址
                
             break;
             case 0xf6://
                 break;
             case 0xfa://启动，单次或者扫描的时候用
                 test_start=1;
             break;
             case 0xfb://复位 扫描时候用
                test_start=0;
                
             break;
             default:
             break;
         
         }
         
        
        
    }

}

const u8 send_dot[]={3,2,4,3,2,4,3,2,4,3,2};
const u8 Send_uint[]={0,0,1,1,1,2,2,2,3,6,6};
Test_ValueTypedef Datacov(s32 value,u8 range)
{
    Test_ValueTypedef midvalue;
    
    midvalue.polar=polarity_r;
    midvalue.res=value;
    midvalue.dot=send_dot[range];
    midvalue.uint=Send_uint[range];
    return midvalue;

}


Test_ValueTypedef V_Datacov(s32 value ,u8 range)
{
    Test_ValueTypedef midvalue;
    
    midvalue.polar=polarity_v;
        
    midvalue.res=value;
    if(range)
        midvalue.dot=3;
    else
        midvalue.dot=4;
    midvalue.uint=0;
    return midvalue;

}

u8 Comp_choice(void)//三种模式分选
{
    u8 comp=0;// 0 合格  1  失败
    u32 testvalue,hi_value,lo_value;
//    testvalue=(u32)Test_Value.res*pow(10,Test_Value.uint-3)/pow(10,Test_Value.dot);//减3以后是m欧姆为单位
//    hi_value=(u32)Jk516save.Compset.Hi_limit.value*pow(10,Jk516save.Compset.Hi_limit.unit-3)/pow(10,Jk516save.Compset.Hi_limit.dot);
//    lo_value=(u32)Jk516save.Compset.Low_limit.value*pow(10,Jk516save.Compset.Low_limit.unit-3)/pow(10,Jk516save.Compset.Low_limit.dot);
    if(testvalue>=hi_value)
        comp=1;//上限失败
    else if(testvalue<=lo_value)
        comp=2;//下限失败
    else
        comp=0;//合格
    return comp;
    
    
}
void Beep_Out(u8 flag)
{
    if(Jk516save.Set_Data.beep==0)
        Beep_Off();
    else
    {
        if(flag==0)//合格
        {
            if(Jk516save.Set_Data.beep==1)//合格响
            {
                Beep_On();
            }
            else
                Beep_Off();
        }
        else//不合格
        {
            if(Jk516save.Set_Data.beep==2)//不合格响
            {
                Beep_On();
            }
            else
                Beep_Off();
        }
    }

}

const u8 Test_Uint[][4]=
{
    {"mΩ"},
    {" Ω"},
    {"kΩ"},
    {"MΩ"},

};
void Disp_Open(void)
{
    u32 colour;
    colour= Colour.black;
    Colour.black=LCD_COLOR_TEST_MID;
	V_POS();
    if(Jk516save.Set_Data.speed!=3)
    {
        if(Jk516save.Set_Data.speed==0)
        {
            WriteString_Big ( TESTVALUE_X-32, TESTVALUE_Y, "--------",0 );
            WriteString_Big ( TESTVALUE_X-32, TESTVALUE_Y+TEST_VALUE_YMID, "--------",0 );
        }
        else
        {
            WriteString_Big ( TESTVALUE_X-16, TESTVALUE_Y+16, "--------",1 );
            WriteString_Big ( TESTVALUE_X-16, TESTVALUE_Y+TEST_VALUE_YMID+16, "--------",1 );
        
        }
    }
    else
    {
         WriteString_16 ( TESTVALUE_X, TESTVALUE_Y+32, "--------",0);
         WriteString_16 ( TESTVALUE_X, TESTVALUE_Y+TEST_VALUE_YMID+32, "--------",0 ); 
    
    
    }
	Test_Value.res = 0;
	Test_Value_V.res = 0;
	if(Jk516save.Set_Data.openbeep == 0)
	{
		Close_Compled();
		Beep_Off();
	//    Beep_Out(0);
		LCD_DrawFullRect(SORTING_XDISP, SORTING_Y_DISP, 60, 22);
		LCD_ShowFontCN_40_55(60+40*6,92,40,55, (uint8_t*)Out_Assic+14*40*55/8);
	}
	if(Jk516save.Set_Data.V_comp!=0 || Jk516save.Set_Data.Res_comp==1)
	{
		if(Jk516save.Set_Data.openbeep == 0)
		{
			Close_Compled();
			Beep_Off();
		//    Beep_Out(0);
			LCD_DrawFullRect(SORTING_XDISP, SORTING_Y_DISP, 60, 22);
			LCD_ShowFontCN_40_55(60+40*6,92,40,55, (uint8_t*)Out_Assic+14*40*55/8);
		}else{
			Colour.black=LCD_COLOR_RED;
			Beep_Out(1);
			Led_Fail_On();
			memcpy(DispBuf,"RV FL",5);
			Send_ComBuff.comp=3;                                   
			memcpy((void *)Send_To_U.comp,DispBuf,5);
			DispBuf[5]=0;
			LCD_DrawFullRect(SORTING_XDISP, SORTING_Y_DISP, 60, 22);
			WriteString_16(SORTING_XDISP, SORTING_Y_DISP, DispBuf,  0);
		}
	}else{
		Close_Compled();
		Beep_Off();
	//    Beep_Out(0);
		LCD_DrawFullRect(SORTING_XDISP, SORTING_Y_DISP, 60, 22);
		LCD_ShowFontCN_40_55(60+40*6,92,40,55, (uint8_t*)Out_Assic+14*40*55/8);
	}
    //WriteString_16 ( TESTVALUE_X, SORTING_Y_DISP+30, "RV_OPEN",0 ); 
    Colour.black=colour;
    Disp_Range(Jk516save.Set_Data.Range_Set,Range);
    startdelay = 2;

}
void Disp_Testvalue(Test_ValueTypedef value,Test_ValueTypedef value_v,u8 speed)
{
    u8 i;
//    u32 date;
    u32 Res_disp;
    Res_disp=value.res;
    for(i=0;i<9;i++)
    DispBuf[i]=0;
    #ifdef BASTARDLY
    if(value.res>last_R_disp)
    {
       date= value.res-last_R_disp;
    
    }
    else
    {
        date=last_R_disp-value.res;
    
    
    }
    if(date>=3)
    {
        last_R_disp=value.res;
        Res_disp=value.res;
    
    }
    else
    {
        Res_disp=last_R_disp;
    
    }
    #endif
    if(Res_disp>33000)
    {
        for(i=0;i<6;i++)
        DispBuf[i]='-';
        
    }
    else
    Hex_Format(Res_disp , value.dot , 5, FALSE);  
    memcpy((void *)Send_ComBuff.send_res,DispBuf,6);
    memcpy((void *)Send_To_U.Send_res,DispBuf,6);//电阻
    memcpy((void *)&Send_To_U.Send_res[6],DISP_UINT[Test_Value.uint],3);//单位
   Send_To_U.back=9;
    DispBuf[6]=' ';
    if(Jk516save.Set_Data.speed!=3)
    {
        if(Jk516save.Set_Data.speed==0)
        {
            if(value.polar)
                WriteString_Big ( TESTVALUE_X-32, TESTVALUE_Y, " ",0); 
            else
                WriteString_Big ( TESTVALUE_X-32, TESTVALUE_Y, "-",0 );
            
              WriteString_Big ( TESTVALUE_X, TESTVALUE_Y, DispBuf ,0); 
        }
        else
        {
            if(value.polar)
                WriteString_Big ( TESTVALUE_X-16, TESTVALUE_Y+16, " ",1); 
            else
                WriteString_Big ( TESTVALUE_X-16, TESTVALUE_Y+16, "-",1 );
              WriteString_Big ( TESTVALUE_X, TESTVALUE_Y+16, DispBuf ,1); 
        
        
        }
    }
    else
    {
        if(value.polar)
            WriteString_16 ( TESTVALUE_X, TESTVALUE_Y+32, " ",0 ); 
        else
            WriteString_16 ( TESTVALUE_X, TESTVALUE_Y+32, "-",0 );
        
          WriteString_16 ( TESTVALUE_X+10, TESTVALUE_Y+32, DispBuf,0 ); 
    
    
    }
        
    
        Hex_Format(value_v.res , value_v.dot , 6, FALSE);
        memcpy((void *)&Send_ComBuff.send_V[1],DispBuf,7);//
        memcpy((void *)&Send_To_U.Send_V[1],DispBuf,7);//存U盘
        Send_To_U.Send_V[8]='V';
        Send_To_U.back1=9;
        memcpy((void *)&Send_To_U.ret[0],"\r\n",2);
        //Send_To_U.ret="/r/n";
     if(Jk516save.Set_Data.speed!=3)
     {
         if(Jk516save.Set_Data.speed==0)
         {
        
           if(value_v.polar)
               WriteString_Big ( TESTVALUE_X-32, TESTVALUE_Y+TEST_VALUE_YMID, " ",0 ); 
           else
               WriteString_Big ( TESTVALUE_X-32, TESTVALUE_Y+TEST_VALUE_YMID, "-",0 );
          WriteString_Big ( TESTVALUE_X, TESTVALUE_Y+TEST_VALUE_YMID, DispBuf,0 ); 
        }
         else
         {
             if(value_v.polar)
               WriteString_Big ( TESTVALUE_X-16, TESTVALUE_Y+TEST_VALUE_YMID+16, " ",1 ); 
           else
               WriteString_Big ( TESTVALUE_X-16, TESTVALUE_Y+TEST_VALUE_YMID+16, "-",1 );
          WriteString_Big ( TESTVALUE_X, TESTVALUE_Y+TEST_VALUE_YMID+16, DispBuf,1 ); 
         }
    }
     else
     {
         if(value_v.polar)
            WriteString_16 ( TESTVALUE_X, TESTVALUE_Y+TEST_VALUE_YMID+32, " ",0 ); 
        else
            WriteString_16 ( TESTVALUE_X, TESTVALUE_Y+TEST_VALUE_YMID+32, "-",0 );
          WriteString_16 ( TESTVALUE_X+10, TESTVALUE_Y+TEST_VALUE_YMID+32, DispBuf,0 ); 
     
     }
     if(value_v.polar)
     {
         Send_ComBuff.send_V[0]=' ';
         Send_To_U.Send_V[0]=' ';
         V_POS();
     }
     else
     {
         Send_ComBuff.send_V[0]='-';
         Send_To_U.Send_V[0]='-';
         V_NEG();
     }
     Send_ComBuff.send_res[6]=Test_Value.uint;
         
        
        
   
    
//    if(value_v.polar)
//        WriteString_16( TESTVALUE_X-8, TESTVALUE_Y, "-",1);
//    else
//        WriteString_16( TESTVALUE_X-8, TESTVALUE_Y," ",1);
//        
//    WriteString_16( TESTVALUE_X+100, TESTVALUE_Y, DispBuf,1 );
    
}
void Disp_xxx(u16 data,u16 pos)
{
    Hex_Format(data , 0 , 5, FALSE);
    LCD_DispString_EN_CH( TESTVALUE_X+pos, TESTVALUE_Y, DispBuf );



}


//显示 下面的快捷键按键  画矩形的底层函数要移植
void Disp_Fastbutton(void)
{
	uint32_t i;
	for(i=0;i<6;i++)
	{
        Colour.black=LCD_COLOR_TEST_BUTON;
		LCD_DrawFullRect( 4*i+i*76, 228, 76, 43   ) ;
        Colour.black=LCD_COLOR_TEST_LINE;
        LCD_DrawFullRect( 80*i+1, 227, 76, 1   );
        LCD_DrawFullRect( 76+i*80, 227, 1, 44   );
        Colour.black=Black;
        LCD_DrawFullRect( 76+i*80+1, 227, 3, 44   );

	}

}//118
void Disp_Button_value1(uint32_t value)
{
	Disp_Fastbutton();
	if(value==0)
	{
		Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
        if(Jk516save.Sys_Setvalue.lanage)
        {
            WriteString_16(84+18, 271-40, "MEAS",  0);
            WriteString_16(84+18, 271-20, "DISP",  0);
            WriteString_16(84+80+18, 271-40, "MEAS",  0);
            WriteString_16(84+80+18, 271-20, "SETUP",  0);
            WriteString_16(84+80+80+18, 271-40, " SYS",  0);
            WriteString_16(84+80+80+18, 271-20, "SETUP",  0);
            WriteString_16(84+80+80+80+18, 271-40, "SYS",  0);
            WriteString_16(84+80+80+80+18, 271-20, "INFO",  0);
//			WriteString_16(84+80+80+80+80+18, 271-40, "SYS",  0);
//            WriteString_16(84+80+80+80+80+18, 271-20, "UPDATE",  0);
        
        }
        else
        {
            WriteString_16(84+18, 271-40, "测量",  0);
            WriteString_16(84+18, 271-20, "显示",  0);
            WriteString_16(84+80+18, 271-40, "测量",  0);
            WriteString_16(84+80+18, 271-20, "设置",  0);
            WriteString_16(84+80+80+18, 271-40, "系统",  0);
            WriteString_16(84+80+80+18, 271-20, "设置",  0);
            WriteString_16(84+80+80+80+18, 271-40, "系统",  0);
            WriteString_16(84+80+80+80+18, 271-20, "信息",  0);
//			WriteString_16(84+80+80+80+80+18, 271-40, "固件",  0);
//			WriteString_16(84+80+80+80+80+18, 271-20, "升级",  0);
        }
	}
    else if(value==1)
    {
        Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
		WriteString_16(84+18, 271-40, "文件",  0);
		WriteString_16(84+18, 271-20, "管理",  0);
		WriteString_16(84+80+18, 271-40, "保存",  0);
		WriteString_16(84+80+18, 271-20, "数据",  0);
        WriteString_16(84+80+80+80+80+18, 271-40, "更多",  0);
		WriteString_16(84+80+80+80+80+18, 271-20, " 2/2",  0);
    
    }else if(value==2){
		if(Jk516save.Sys_Setvalue.lanage)
        {
            WriteString_16(84+18, 271-40, "MEAS",  0);
            WriteString_16(84+18, 271-20, "DISP",  0);
            WriteString_16(84+80+18, 271-40, "MEAS",  0);
            WriteString_16(84+80+18, 271-20, "SETUP",  0);
            WriteString_16(84+80+80+18, 271-40, " SYS",  0);
            WriteString_16(84+80+80+18, 271-20, "SETUP",  0);
            WriteString_16(84+80+80+80+18, 271-40, "SYS",  0);
            WriteString_16(84+80+80+80+18, 271-20, "INFO",  0);
			WriteString_16(84+80+80+80+80+18, 271-40, "SYS",  0);
            WriteString_16(84+80+80+80+80+18, 271-20, "UPDATE",  0);
        
        }
        else
        {
            WriteString_16(84+18, 271-40, "测量",  0);
            WriteString_16(84+18, 271-20, "显示",  0);
            WriteString_16(84+80+18, 271-40, "测量",  0);
            WriteString_16(84+80+18, 271-20, "设置",  0);
            WriteString_16(84+80+80+18, 271-40, "系统",  0);
            WriteString_16(84+80+80+18, 271-20, "设置",  0);
            WriteString_16(84+80+80+80+18, 271-40, "系统",  0);
            WriteString_16(84+80+80+80+18, 271-20, "信息",  0);
			WriteString_16(84+80+80+80+80+18, 271-40, "固件",  0);
			WriteString_16(84+80+80+80+80+18, 271-20, "升级",  0);
        }
	}

}
void Disp_Button_TestSet(uint32_t value)
{
//	Disp_Fastbutton();
 	if(value==0)
	{
		Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
		WriteString_16(84+18, 271-40, "测量",  0);
		WriteString_16(84+18, 271-20, "显示",  0);
		WriteString_16(84+80+18, 271-40, "测量",  0);
		WriteString_16(84+80+18, 271-20, "设置",  0);
		WriteString_16(84+80+80+18, 271-40, "系统",  0);
		WriteString_16(84+80+80+18, 271-20, "设置",  0);
		WriteString_16(84+80+80+80+18, 271-40, "系统",  0);
		WriteString_16(84+80+80+80+18, 271-20, "信息",  0);
//		WriteString_16(84+80+80+80+80, 271-40, "更多",  0);
//		WriteString_16(84+80+80+80+80, 271-20, " 1/2",  0);
	}
    else if(value==1)
    {
        Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
		WriteString_16(84+18, 271-40, "文件",  0);
		WriteString_16(84+18, 271-20, "管理",  0);
		WriteString_16(84+8+180, 271-40, "系统",  0);
		WriteString_16(84+80+18, 271-20, "设置",  0);
        WriteString_16(84+80+80+18, 271-30, "工具",  0);
//		WriteString_16(84+80+80, 271-20, "设置",  0);
        WriteString_16(84+80+80+80+80+18, 271-40, "更多",  0);
		WriteString_16(84+80+80+80+80+18, 271-20, " 2/2",  0);
    
    
    }


}
void Disp_Button_SysSet(void)
{

		Disp_Button_value1(0);


}
void Disp_TopBar_Color(void)
{
    Colour.black=LCD_COLOR_TEST_BAR;
    LCD_DrawFullRect(0, 0, 170, 22);
	//LCD_DrawFullRect( 0, 0, 240, 22 );


}
void Disp_MidRect(void)
{
    Colour.black=LCD_COLOR_TEST_MID;
	LCD_DrawFullRect( 0, 92,479 , 202-92 ) ;
}
void Disp_TestScreen(void)
{
	Disp_TopBar_Color();
	Disp_Fastbutton();//显示快速按键
	Disp_MidRect();//显示中间色块
}


//
//测量显示主菜单的项目显示
//
void Disp_Test_Item(void)
{
	uint32_t i;
    const u8 (*pt)[sizeof(Test_Setitem[0])];
    const u8 (*ppt)[sizeof(All_TopName[0])];
    
	Disp_TestScreen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
    if(Jk516save.Sys_Setvalue.lanage)
        ppt=All_TopName_E;
    else
        ppt=All_TopName;
	WriteString_16(0, 4, ppt[0],  0);
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
    if(Jk516save.Sys_Setvalue.lanage)
        pt=Test_Setitem_E;
    else
        pt=Test_Setitem;
	for(i=0;i<6;i++)
	{
		if(i<3)
			WriteString_16(0, 26+i*22, pt[i],  0);
		else
			WriteString_16(250, 26+(i-3)*22, pt[i],  0);

	
	}
	Disp_Button_value1(0);
	

}

//测试的时候显示的设置值
void Disp_Test_value(u8 num)
{
//	vu32 flag;
	vu32 i;
    const u8 (*ppt)[11];
    const u8 (*pt)[7];
//	vu32 xpose;
	vu32 Black_Select;
//	vu32 Select_color;
	if(Jk516save.Sys_Setvalue.lanage)
    {
        pt=Test_Tirpvalue_E;
    }
    else
    {
        pt=Test_Tirpvalue;
    
    }
	Black_Select=(num==1)?1:0;//触发
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	LCD_DrawFullRect( LIST1+88, FIRSTLINE-2,88+4 , SPACE1-2  ) ;//SPACE1
    Colour.Fword=White;
	WriteString_16(LIST1+88, FIRSTLINE, pt[Jk516save.Set_Data.trip],  0);//增加算法  把顺序改过来
	
//电阻上限
	Black_Select=(num==2)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		

	Hex_Format(Jk516save.Set_Data.High_Res.Num , Jk516save.Set_Data.High_Res.Dot , 5 , 0);
    LCD_DrawFullRect( LIST1+88, FIRSTLINE+SPACE1,88+4 , SPACE1-2  ) ;//SPACE1
	WriteString_16(LIST1+88, FIRSTLINE+SPACE1, DispBuf,  0);
    WriteString_16(LIST1+90+7*8+2, FIRSTLINE+SPACE1,DISP_UINT[Jk516save.Set_Data.High_Res.Unit],  0);
	

//电阻下限
	Black_Select=(num==3)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}

	
	Hex_Format(Jk516save.Set_Data.Res_low.Num , Jk516save.Set_Data.Res_low.Dot , 5 , 0);
    LCD_DrawFullRect( LIST1+88, FIRSTLINE+SPACE1*2,88+4 , SPACE1-2  ) ;
	WriteString_16(LIST1+88, FIRSTLINE+SPACE1*2, DispBuf,  0);
    WriteString_16(LIST1+90+8*7+2, FIRSTLINE+SPACE1*2,DISP_UINT[Jk516save.Set_Data.Res_low.Unit],  0);
	
//量程
	Black_Select=(num==4)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	if(Jk516save.Sys_Setvalue.lanage)
    {
        ppt=User_Range_E;
    }
    else
    {
        ppt=User_Range;
    
    }	
	LCD_DrawFullRect( LIST2+88, FIRSTLINE-2,SELECT_2END-LIST2-88 , SPACE1-2) ;//SPACE1
    if(Jk516save.Set_Data.Range_Set==0)
        
        WriteString_16(LIST2+88, FIRSTLINE, ppt[0],  0);//待改
    else
        WriteString_16(LIST2+88, FIRSTLINE, ppt[Jk516save.Set_Data.Range+1],  0);//待改
        
	
	
//电压上限
	Black_Select=(num==5)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}

	Hex_Format(Jk516save.Set_Data.V_high.Num , Jk516save.Set_Data.V_high.Dot , 6 , 0);
    LCD_DrawFullRect( LIST2+88, FIRSTLINE-2+SPACE1,SELECT_2END-LIST2-88 , SPACE1-2) ;//SPACE1
	WriteString_16(LIST2+88, FIRSTLINE+SPACE1*1, DispBuf,  0);
	WriteString_16(LIST2+88+8*9, FIRSTLINE+SPACE1, " V",  0);
	
//电压下限

	Black_Select=(num==6)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}

	Hex_Format(Jk516save.Set_Data.V_low.Num , Jk516save.Set_Data.V_low.Dot , 6 , 0);
    LCD_DrawFullRect( LIST2+88, FIRSTLINE-2+SPACE1*2,SELECT_2END-LIST2-88 , SPACE1-2) ;//SPACE1
	WriteString_16(LIST2+88, FIRSTLINE+SPACE1*2, DispBuf,  0);
	WriteString_16(LIST2+88+8*9, FIRSTLINE+SPACE1*2, " V",  0);
	Disp_Fastbutton();
	
	switch(num)
	{
		case 0:
				Disp_Button_value1(0);
			break;
		case 1:
		
				Colour.Fword=White;//
				Colour.black=LCD_COLOR_TEST_BUTON;
				
				if(Jk516save.Sys_Setvalue.lanage)
                {
                    pt=Test_Tirpvalue_E;
                }
                else
                {
                    pt=Test_Tirpvalue;
                
                }
				for(i=0;i<4;i++)
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, pt[i],  0);
				
				
			
				
				
				
			
			
		
		break;
		case 2:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
            if(Jk516save.Sys_Setvalue.lanage)
            {
                pt=INPUT_E;
            }
            else
            {
                pt=INPUT;
            
            }
			for(i=0;i<1;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, pt[i],  0);
			}
		break;
		case 3:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
        if(Jk516save.Sys_Setvalue.lanage)
            {
                pt=INPUT_E;
            }
            else
            {
                pt=INPUT;
            
            }
			for(i=0;i<1;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, pt[i],  0);
			}
			break;

		case 4:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<4;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, User_Rangeset[i],  0);
			}
			break;
		case 5:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
            if(Jk516save.Sys_Setvalue.lanage)
                {
                    pt=INPUT_E;
                }
                else
                {
                    pt=INPUT;
                
                }
			for(i=0;i<1;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, pt[i],  0);
			}
			break;
		case 6:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
            if(Jk516save.Sys_Setvalue.lanage)
            {
                pt=INPUT_E;
            }
            else
            {
                pt=INPUT;
            
            }
			for(i=0;i<1;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, pt[i],  0);
			}
			break;
		default:
			
		break;
		
			

		
			
	
	
	}

}

void Disp_TestSet_MidRect(void)
{

    Disp_MidRect();
}
void Disp_TestSetScreen(void)
{
	Disp_TopBar_Color();
	Disp_Fastbutton();//显示快速按键
	Disp_TestSet_MidRect();//显示中间色块
}
//列表显示界面	 
void Disp_Test_Set_Item(void)
{
	uint32_t i;
    const u8 (*pt)[sizeof(Set_testitem[0])];
    const u8 (*ppt)[sizeof(All_TopName[0])];
    
	Disp_TestSetScreen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(Jk516save.Sys_Setvalue.lanage)
        ppt=All_TopName_E;
    else
        ppt=All_TopName;
	WriteString_16(0, 4, ppt[4],  0);
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
    if(Jk516save.Sys_Setvalue.lanage)
        pt=Set_testitem_E;
    else
        pt=Set_testitem;
        
	for(i=0;i<(sizeof(Set_testitem)/(sizeof(Set_testitem[0])));i++)
	if(i<sizeof(Set_testitem)/(sizeof(Set_testitem[0]))/2)
	{
		if(i>2&&i<8)
			Colour.black=LCD_COLOR_TEST_MID;
		else
			Colour.black=LCD_COLOR_TEST_BACK;	
		
		WriteString_16(LIST1, FIRSTLINE+SPACE1*i, pt[i],  0);

	}
	else
	{
	  	if((i-sizeof(Set_testitem)/(sizeof(Set_testitem[0]))/2)>2&&(i-sizeof(Set_testitem)/(sizeof(Set_testitem[0]))/2)<8)
			Colour.black=LCD_COLOR_TEST_MID;
		else
			Colour.black=LCD_COLOR_TEST_BACK;

		WriteString_16(LIST2, FIRSTLINE+SPACE1*(i-sizeof(Set_testitem)/(sizeof(Set_testitem[0]))/2), pt[i],  0);
		
		
	}	
	//Disp_Button_TestSet(0);
    Disp_Button_value1(0);

}


//显示设置参数的值Setup_Valueall
void DispSet_value(u8 keynum)
{
	vu32 i;
    const u8 (*pt)[7];
	const u8 (*pt1)[8];
    const u8 (*ppt)[11];
	vu32 Black_Select;
	Black_Select=(keynum==1)?1:0;
	if(Black_Select)//触发
	{
		Colour.black=LCD_COLOR_SELECT;
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	if(Jk516save.Sys_Setvalue.lanage)
    {
        pt=Test_Tirpvalue_E;
    
    }
    else
    {
        pt=Test_Tirpvalue;
    
    }
	LCD_DrawFullRect(LIST1+88, FIRSTLINE, SELECT_1END-(LIST1+88), SPACE1-4);	
	WriteString_16(LIST1+88, FIRSTLINE, pt[Jk516save.Set_Data.trip],  0);//增加算法  把顺序改过来
	
//速度	
	Black_Select=(keynum==2)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
    if(Jk516save.Sys_Setvalue.lanage)
    {
        pt=Test_Speedvalue_E;
    
    }
    else
    {
        pt=Test_Speedvalue;
    
    }
	LCD_DrawFullRect(LIST1+88, FIRSTLINE+SPACE1-2, SELECT_1END-(LIST1+88), SPACE1-4);
	WriteString_16(LIST1+88, FIRSTLINE+SPACE1, pt[Jk516save.Set_Data.speed],  0);
	
	
//电阻比较	
	Black_Select=(keynum==3)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_MID;
	}
    if(Jk516save.Sys_Setvalue.lanage)
    {
        pt=Test_Compvalue_E;
    
    }
    else
    {
        pt=Test_Compvalue;
    
    }
	LCD_DrawFullRect(LIST1+88, FIRSTLINE+SPACE1*3, SELECT_1END-(LIST1+88), SPACE1-4);
	WriteString_16(LIST1+88, FIRSTLINE+SPACE1*3, pt[Jk516save.Set_Data.Res_comp],  0);
	
	
//电阻下限	
	Black_Select=(keynum==4)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
		
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_MID;
	}
	
	
	Hex_Format(Jk516save.Set_Data.Res_low.Num , Jk516save.Set_Data.Res_low.Dot , 5 , 0);
	LCD_DrawFullRect(LIST1+88, FIRSTLINE+SPACE1*4-2, SELECT_1END-(LIST1+70), SPACE1-4);
	WriteString_16(LIST1+88, FIRSTLINE+SPACE1*4, DispBuf,  0);
	WriteString_16(LIST1+90+8*7+2, FIRSTLINE+SPACE1*4,DISP_UINT[Jk516save.Set_Data.Res_low.Unit],  0);

	
//电压比较	
	Black_Select=(keynum==5)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_MID;
	}
    if(Jk516save.Sys_Setvalue.lanage)
    {
        pt=Test_VCompvalue_E;
    
    }
    else
    {
        pt=Test_VCompvalue;
    
    }
	LCD_DrawFullRect(LIST1+88, FIRSTLINE+SPACE1*6, SELECT_1END-(LIST1+88), SPACE1-4);
	WriteString_16(LIST1+88, FIRSTLINE+SPACE1*6+2, pt[Jk516save.Set_Data.V_comp],  0);
	//电压下限
	Black_Select=(keynum==6)?1:0;
	
	
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_MID;
	}
	
	Hex_Format(Jk516save.Set_Data.V_low.Num , Jk516save.Set_Data.V_low.Dot , 6 , 0);
	LCD_DrawFullRect(LIST1+88, FIRSTLINE+SPACE1*7-2, SELECT_1END-(LIST1+70), SPACE1-4);
	WriteString_16(LIST1+88, FIRSTLINE+SPACE1*7, DispBuf,  0);
	WriteString_16(LIST1+88+8*9, FIRSTLINE+SPACE1*7, " V",  0);
	

//量程
	Black_Select=(keynum==7)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
    if(Jk516save.Sys_Setvalue.lanage)
    {
        ppt=User_Range_E;
    
    }
    else
    {
        ppt=User_Range;
    
    }
    LCD_DrawFullRect(LIST2+88, FIRSTLINE-2, SELECT_1END-(LIST1+88), SPACE1-4);
    if(Jk516save.Set_Data.Range_Set==0)
        
        WriteString_16(LIST2+88, FIRSTLINE, ppt[0],  0);//待改
    else
        WriteString_16(LIST2+88, FIRSTLINE, ppt[Jk516save.Set_Data.Range+1],  0);//待改
    
	//WriteString_16(LIST2+88, FIRSTLINE, User_Range[Jk516save.Set_Data.Range],  0);
	
//讯响
	Black_Select=(keynum==8)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
     if(Jk516save.Sys_Setvalue.lanage)
    {
        pt=Setup_Beep_E;
    
    }
    else
    {
        pt=Setup_Beep;
    
    }
	LCD_DrawFullRect(LIST2+88, FIRSTLINE+SPACE1-2, SELECT_1END-(LIST1+88), SPACE1-4);
	WriteString_16(LIST2+88, FIRSTLINE+SPACE1, pt[Jk516save.Set_Data.beep],  0);
	
	//电阻标称
	Black_Select=(keynum==9)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_MID;
	}
	
	
	Hex_Format(Jk516save.Set_Data.Nominal_Res.Num , Jk516save.Set_Data.Nominal_Res.Dot , 5 , 0);
	LCD_DrawFullRect(LIST2+88, FIRSTLINE+SPACE1*3, SELECT_1END-(LIST1+70), SPACE1-4);
	WriteString_16(LIST2+88, FIRSTLINE+SPACE1*3, DispBuf,  0);
    WriteString_16(LIST2+90+8*7+2, FIRSTLINE+SPACE1*3+2,DISP_UINT[Jk516save.Set_Data.Nominal_Res.Unit],  0);
	
		
	

//电阻上限
	
	Black_Select=(keynum==10)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_MID;
	}
	
	Hex_Format(Jk516save.Set_Data.High_Res.Num , Jk516save.Set_Data.High_Res.Dot , 5 , 0);
	LCD_DrawFullRect(LIST2+88, FIRSTLINE+SPACE1*4-2, SELECT_1END-(LIST1+70), SPACE1-4);
	WriteString_16(LIST2+88, FIRSTLINE+SPACE1*4, DispBuf,  0);
	WriteString_16(LIST2+90+8*7+2, FIRSTLINE+SPACE1*4,DISP_UINT[Jk516save.Set_Data.High_Res.Unit],  0);
		
	
//电压标称
	
	Black_Select=(keynum==11)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_MID;
	}
	
	Hex_Format(Jk516save.Set_Data.Nominal_V.Num, Jk516save.Set_Data.Nominal_V.Dot ,6 , 0);
	LCD_DrawFullRect(LIST2+88, FIRSTLINE+SPACE1*6-2, SELECT_1END-(LIST1+70), SPACE1-4);
	WriteString_16(LIST2+88, FIRSTLINE+SPACE1*6, DispBuf,  0);
	
	WriteString_16(LIST2+88+8*9, FIRSTLINE+SPACE1*6, " V",  0);
	
	
//电压上限
	
	Black_Select=(keynum==12)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_MID;
	}
	
	
	Hex_Format(Jk516save.Set_Data.V_high.Num, Jk516save.Set_Data.V_high.Dot , 6 , 0);
	LCD_DrawFullRect(LIST2+88, FIRSTLINE+SPACE1*7-2, SELECT_1END-(LIST1+70), SPACE1-4);
	WriteString_16(LIST2+88, FIRSTLINE+SPACE1*7, DispBuf,  0);
	
	WriteString_16(LIST2+88+8*9, FIRSTLINE+SPACE1*7, " V",  0);
	
	
	Disp_Fastbutton();
	switch(keynum)
	{
		case 0:
				Disp_Button_value1(2);
			break;
		case 1:
			
				Colour.Fword=White;
				Colour.black=LCD_COLOR_TEST_BUTON;
                if(Jk516save.Sys_Setvalue.lanage)
                {
                    pt=Test_Tirpvalue_E;
                
                }
                else
                {
                    pt=Test_Tirpvalue;
                
                }
				for(i=0;i<4;i++)
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, pt[i],  0);
				
//				
			
			
		break;
		case 2:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
            if(Jk516save.Sys_Setvalue.lanage)
            {
                pt=Test_Speedvalue_E;
            
            }
            else
            {
                pt=Test_Speedvalue;
            
            }
			for(i=0;i<4;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, pt[i],  0);
			}
		break;
		case 3:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
            if(Jk516save.Sys_Setvalue.lanage)
            {
                pt=Test_Compvalue_E;
            
            }
            else
            {
                pt=Test_Compvalue;
            
            }
			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, pt[i],  0);
			}
			break;
		case 4:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
            if(Jk516save.Sys_Setvalue.lanage)
            {
                pt=INPUT_E;
            
            }
            else
            {
                pt=INPUT;
            
            }
			for(i=0;i<1;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, pt[i],  0);
			}
			break;
		case 5:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
            if(Jk516save.Sys_Setvalue.lanage)
            {
                pt=Test_VCompvalue_E;
            
            }
            else
            {
                pt=Test_VCompvalue;
            
            }
			for(i=0;i<3;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, pt[i],  0);
			}
			break;
		case 6:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
            if(Jk516save.Sys_Setvalue.lanage)
            {
                pt=INPUT_E;
            
            }
            else
            {
                pt=INPUT;
            
            }
			for(i=0;i<1;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, pt[i],  0);
			}
			break;
		case 7:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
            if(Jk516save.Sys_Setvalue.lanage)
            {
                ppt=User_Rangeset;
            
            }
            else
            {
                ppt=User_Rangeset;
            
            }
			for(i=0;i<5;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, ppt[i],  0);
			}
			break;
		case 8:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
            if(Jk516save.Sys_Setvalue.lanage)
            {
				if(Jk516save.Set_Data.openbeep==1)
				{
					pt=Setup_Beep_E;
				}else{
					pt1=Setup_Beep_E1;
				}
            
            }
            else
            {
				if(Jk516save.Set_Data.openbeep==1)
				{
					pt=Setup_Beep;
				}else{
					pt1=Setup_Beep1;
				}
                
            
            }
			if(Jk516save.Set_Data.openbeep==1)
			{
				for(i=0;i<4;i++)
				{
					
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, pt[i],  0);
				}
			}else{
				for(i=0;i<4;i++)
				{
					
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, pt1[i],  0);
				}
			}
			break;
		case 9:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
            if(Jk516save.Sys_Setvalue.lanage)
            {
                pt=INPUT_E;
            
            }
            else
            {
                pt=INPUT;
            
            }
			for(i=0;i<1;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, pt[i],  0);
			}
			break;
		case 10:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
            if(Jk516save.Sys_Setvalue.lanage)
            {
                pt=INPUT_E;
            
            }
            else
            {
                pt=INPUT;
            
            }
			for(i=0;i<1;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, pt[i],  0);
			}
			break;
		case 11:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
            if(Jk516save.Sys_Setvalue.lanage)
            {
                pt=INPUT_E;
            
            }
            else
            {
                pt=INPUT;
            
            }
			for(i=0;i<1;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, pt[i],  0);
			}
			break;
		case 12:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
            if(Jk516save.Sys_Setvalue.lanage)
            {
                pt=INPUT_E;
            
            }
            else
            {
                pt=INPUT;
            
            }
			for(i=0;i<1;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, pt[i],  0);
			}
			break;
		
			
		default:
			break;
		
			
	
	
	}

}
void Disp_Sys_Screen(void)
{
	Disp_TopBar_Color();
	Disp_Fastbutton();//显示快速按键	  
//	Disp_ListScan_MidRect();//显示中间色块	 User_LimitList_Item  
}
//系统设置子函数
void Disp_SysLine(void)
{
 	uint32_t i;
	for(i=0;i<7;i++)
	{
		//if(i<=13/2)
			LCD_DrawFullRect( 90, FIRSTLINE+(i+1)*SPACE1-2, 100,1);
//		else
//			LCD_DrawLine( 90+250, FIRSTLINE+(i-13/2+1)*SPACE1-2, 90+100+250,FIRSTLINE+(i-13/2+1)*SPACE1-2, White );
	}

}
void Disp_Sys_Item(void)
{
	uint32_t i;
    const u8 (*pt)[sizeof(Sys_Setitem[0])];
    const u8 (*ppt)[sizeof(All_TopName[0])];
	Disp_Sys_Screen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
    if(Jk516save.Sys_Setvalue.lanage)
        ppt=All_TopName_E;
    else
        ppt=All_TopName;
	WriteString_16(0, 4, ppt[8],  0);
	//WriteString_16(0, 4, All_TopName[8],  0);
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
    if(Jk516save.Sys_Setvalue.lanage)
    pt=Sys_Setitem_E;
    else
        pt=Sys_Setitem;
//	WriteString_16(LIST1, FIRSTLINE, User_ListScan_Item[0],  0);
    
	for(i=0;i<(sizeof(Sys_Setitem)/(sizeof(Sys_Setitem[0])));i++)
	//if(i<=sizeof(Sys_Setitem)/(sizeof(Sys_Setitem[0]))/2)
	{
		WriteString_16(LIST1, FIRSTLINE+SPACE1*i, pt[i],  0);
        

	}
//	else
//	{
//		WriteString_16(LIST2,FIRSTLINE+SPACE1*(i-sizeof(Sys_Setitem)/(sizeof(Sys_Setitem[0]))/2), Sys_Setitem[i],  0);
//	}
    Colour.black=White;
	Disp_SysLine();
	//Disp_Button_SysSet();
}

//显示设置参数的值Setup_Valueall
void Disp_Sys_value(u8 keynum)
{
//	vu32 flag;
	vu32 i;
    const u8 (*pt)[7];
//	vu32 xpose;
	vu32 Black_Select;
    RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
    RTC_TimeStructure.RTC_H12 = RTC_HourFormat_24;
//	vu32 Select_color;
//串口
//    keynum=1;
	Black_Select=(keynum==1)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	if(Jk516save.Sys_Setvalue.lanage)
    {
        pt=Test_Compvalue_E;
    
    }
    else
    {
        pt=Test_Compvalue;
    
    }	
	LCD_DrawFullRect( LIST1+90, FIRSTLINE,SELECT_1END-(LIST1+90) , SPACE1-4 ) ;//SPACE1
	WriteString_16(LIST1+90, FIRSTLINE+2, pt[Jk516save.Sys_Setvalue.uart],  0);//
	
//波特率
	Black_Select=(keynum==2)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawFullRect( LIST1+90, FIRSTLINE+SPACE1,SELECT_1END-(LIST1+90) , SPACE1-4  ) ;//SPACE1
	WriteString_16(LIST1+90, FIRSTLINE+SPACE1+2, Sys_Buard_Value[Jk516save.Sys_Setvalue.buard],  0);
	//U盘开关
    Black_Select=(keynum==3)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	if(Jk516save.Sys_Setvalue.lanage)
    {
        pt=Test_Compvalue_E;
    
    }
    else
    {
        pt=Test_Compvalue;
    
    }	
	LCD_DrawFullRect( LIST1+90, FIRSTLINE+SPACE1*2,SELECT_1END-(LIST1+90) , SPACE1-4  ) ;//SPACE1
	WriteString_16(LIST1+90, FIRSTLINE+SPACE1*2+2, pt[Jk516save.Sys_Setvalue.u_flag],  0);
	//IO接口开关
    Black_Select=(keynum==4)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	if(Jk516save.Sys_Setvalue.lanage)
    {
        pt=Test_Compvalue_E;
    
    }
    else
    {
        pt=Test_Compvalue;
    
    }	
	LCD_DrawFullRect( LIST1+90, FIRSTLINE+SPACE1*3,SELECT_1END-(LIST1+90) , SPACE1-4  ) ;//SPACE1
	WriteString_16(LIST1+90, FIRSTLINE+SPACE1*3+2, pt[Jk516save.Sys_Setvalue.plc],  0);
	
//显示语言
	Black_Select=(keynum==5)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawFullRect( LIST1+90, FIRSTLINE+SPACE1*4,SELECT_1END-(LIST1+90) , SPACE1 -4 ) ;//SPACE1
	WriteString_16(LIST1+90, FIRSTLINE+SPACE1*4+2, Sys_Language_Value[Jk516save.Sys_Setvalue.lanage],  0);
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
//年
	Black_Select=(keynum==6)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	//SaveData.Sys_Setup.Data_Value.Year=17;
	LCD_DrawFullRect( LIST1+90, FIRSTLINE+SPACE1*5,LIST1+90+18-(LIST1+90) , SPACE1-4 ) ;//SPACE1

	sprintf((char *)DispBuf,"%2d",RTC_DateStructure.RTC_Year);
	WriteString_16(LIST1+90, FIRSTLINE+SPACE1*5+2, DispBuf,  0);
	Colour.black=LCD_COLOR_TEST_BACK;
	WriteString_16(LIST1+90+20, FIRSTLINE+SPACE1*5+2, "-",  0);
//月
	
	Black_Select=(keynum==7)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	//SaveData.Sys_Setup.Data_Value.Mon=17;
	LCD_DrawFullRect( LIST1+90+30, FIRSTLINE+SPACE1*5,18 , SPACE1-4  ) ;//SPACE1

	sprintf((char *)DispBuf,"%2d",RTC_DateStructure.RTC_Month);
	WriteString_16(LIST1+90+30, FIRSTLINE+SPACE1*5+2, DispBuf,  0);
	Colour.black=LCD_COLOR_TEST_BACK;
	WriteString_16(LIST1+90+20+30, FIRSTLINE+SPACE1*5+2, "-",  0);
	
	
	//日
	
	Black_Select=(keynum==8)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	//SaveData.Sys_Setup.Data_Value.data=17;
	LCD_DrawFullRect( LIST1+90+30+32, FIRSTLINE+SPACE1*5,18 ,SPACE1-4 ) ;//SPACE1

	sprintf((char *)DispBuf,"%2d",RTC_DateStructure.RTC_Date);
	WriteString_16(LIST1+90+30+32, FIRSTLINE+SPACE1*5+2, DispBuf,  0);

//时
	Black_Select=(keynum==9)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	//SaveData.Sys_Setup.Timer_Value.Hour=1;
	LCD_DrawFullRect( LIST1+90, FIRSTLINE+SPACE1*6,18 , SPACE1-4 ) ;//SPACE1

	sprintf((char *)DispBuf,"%2d",RTC_TimeStructure.RTC_Hours);
	WriteString_16(LIST1+90, FIRSTLINE+SPACE1*6+2, DispBuf,  0);
	Colour.black=LCD_COLOR_TEST_BACK;
	WriteString_16(LIST1+90+20, FIRSTLINE+SPACE1*6+2, ":",  0);
//分
	
	Black_Select=(keynum==10)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	//SaveData.Sys_Setup.Timer_Value.Min=17;
	LCD_DrawFullRect( LIST1+90+30, FIRSTLINE+SPACE1*6,18 , SPACE1-4 ) ;//SPACE1

	sprintf((char *)DispBuf,"%2d",RTC_TimeStructure.RTC_Minutes);
	WriteString_16(LIST1+90+30, FIRSTLINE+SPACE1*6+2, DispBuf,  0);
	Colour.black=LCD_COLOR_TEST_BACK;
	WriteString_16(LIST1+90+20+30, FIRSTLINE+SPACE1*6+2, ":",  0);
	
	
	//秒
	
	Black_Select=(keynum==11)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	LCD_DrawFullRect( LIST1+90+30+32, FIRSTLINE+SPACE1*6,18 ,SPACE1-4 ) ;//SPACE1
	sprintf((char *)DispBuf,"%2d",RTC_TimeStructure.RTC_Seconds);
	WriteString_16(LIST1+90+30+32, FIRSTLINE+SPACE1*6+2, DispBuf,  0);
	//文件名称
   Black_Select=(keynum==12)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawFullRect( LIST1+90, FIRSTLINE+SPACE1*7,SELECT_1END-(LIST1+90) ,SPACE1 -4 ) ;//SPACE1
	WriteString_16(LIST1+90, FIRSTLINE+SPACE1*7+2,(const uint8_t *) Jk516save.Sys_Setvalue.textname,  0);

    
	Disp_Fastbutton();
    Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BUTON;
	switch(keynum)
	{
		case 0:
				Disp_Button_SysSet();
			break;
		case 1:
            if(Jk516save.Sys_Setvalue.lanage)
            {
                pt=Test_Compvalue_E;
            
            }
            else
            {
                pt=Test_Compvalue;
            
            }
            for(i=0;i<2;i++)
            WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, pt[i],  0);


		break;
		case 2:
			for(i=0;i<5;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, Sys_Buard_Value[i],  0);
			}
		break;
		case 3:
			if(Jk516save.Sys_Setvalue.lanage)
            {
                pt=Test_Compvalue_E;
            
            }
            else
            {
                pt=Test_Compvalue;
            
            }
			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, pt[i],  0);
			}
			break;
		case 4:
            if(Jk516save.Sys_Setvalue.lanage)
            {
                pt=Test_Compvalue_E;
            
            }
            else
            {
                pt=Test_Compvalue;
            
            }
			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, pt[i],  0);
			}
			break;
		case 5:

			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, Sys_Language_Value[i],  0);
			}
			break;
		case 6:
		case 7:
        case 8:
        case 9:
            
        case 10:
            
        case 11:

			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, BiasButton_Tip[i],  0);
			}
			break;
	
//		case 12:
//			Colour.Fword=White;
//			Colour.black=LCD_COLOR_TEST_BUTON;
//			for(i=0;i<5;i++)
//			{
//				
//				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, Sys_Addr_value[i],  0);
//			}
//			break;
//		case 13:
//			Colour.Fword=White;
//			Colour.black=LCD_COLOR_TEST_BUTON;
//			for(i=0;i<2;i++)
//			{
//				
//				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, BiasButton_Tip[i],  0);
//			}
//			break;
//		case 14://
//			Colour.Fword=White;
//			Colour.black=LCD_COLOR_TEST_BUTON;
//			for(i=0;i<2;i++)
//			{
//				
//				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, CompButton_Tip[i],  0);
//			}
//			break;
//		case 15:
//			Colour.Fword=White;
//			Colour.black=LCD_COLOR_TEST_BUTON;
//			for(i=0;i<2;i++)
//			{
//				
//				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, BiasButton_Tip[i],  0);
//			}
//			break;
//		case 16:
//			Colour.Fword=White;
//			Colour.black=LCD_COLOR_TEST_BUTON;
//			for(i=0;i<2;i++)
//			{
//				
//				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, BiasButton_Tip[i],  0);
//			}
//			break;
//		case 17:
//			Colour.Fword=White;
//			Colour.black=LCD_COLOR_TEST_BUTON;
//			for(i=0;i<2;i++)
//			{
//				
//				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, BiasButton_Tip[i],  0);
//			}
//			break;
//		case 18:
//			break;
//		case 19:
//			break;
//		case 20:
//			break;

		default:
			break;
		
			
	
	
	}

}

//系统设置
void Use_SysSetProcess(void)
{	
	//vu32 keynum=0;
	vu8 key,i;
//    uint8_t Disp_buff[12];
    char key_count=0;
	vu8 Disp_flag=1;
//    u8 keynum1=0;
    u8 keynum=0;
    RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
    LCD_Clear(LCD_COLOR_TEST_BACK);
	//Button_Page_Typedef Button_Page;
	//keynum=0;
	//Button_Page.page=0;
    //lcd_Clear(LCD_COLOR_TEST_BACK);
	Disp_Sys_Item();
	//Delay_Key();
 	while(GetSystemStatus()==SYS_STATUS_SYSSET)
	{
	  	
		if(Disp_flag==1)
		{
           
            
            Jk516save.Sys_Setvalue.textname[7]=0;
			Disp_Sys_value(keynum);
			Disp_flag=0;
            
		}
		key=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/10);
        RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
        RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
		if(key!=KEY_NONE)
		{
			Disp_flag=1;
           //Key_Beep();
			switch(key)
			{
				case Key_F1:
					switch(keynum)
					{
						case 0:
							SetSystemStatus(SYS_STATUS_TEST);
							break;
						case 1:
							Jk516save.Sys_Setvalue.uart=0;
							break;
						case 2:
							Jk516save.Sys_Setvalue.buard=0;
							Debug_USART_Config(4800);
							break;
						case 3:
                            Jk516save.Sys_Setvalue.u_flag=0;
							
							break;
						case 4:
							Jk516save.Sys_Setvalue.plc=0;
							break;
						case 5:
							Jk516save.Sys_Setvalue.lanage=0;
                            LCD_Clear(LCD_COLOR_TEST_BACK);
                            Disp_Sys_Item();
							break;
						case 6:
							if(RTC_DateStructure.RTC_Year<1)
								RTC_DateStructure.RTC_Year=RTC_YEAR_MAX;
							else
                                if(RTC_DateStructure.RTC_Year>=RTC_YEAR_MAX)
                                    RTC_DateStructure.RTC_Year=0;
                                else
								RTC_DateStructure.RTC_Year--;
                           // RTC_SetTime (LPC_RTC, RTC_TIMETYPE_YEAR, RTC_TIME_DISP.YEAR);
                            
                            //RTC_CalibConfig(LPC_RTC, 0, RTC_CALIB_DIR_FORWARD);
                            //RTC_CalibCounterCmd(LPC_RTC, ENABLE);
                                RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
                                RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
							break;
						case 7:
							if(RTC_DateStructure.RTC_Month<=1)
								RTC_DateStructure.RTC_Month=RTC_MONTH_MAX;
							else
								RTC_DateStructure.RTC_Month--;
                            RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
                            RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
							break;
						case 8:
							if(RTC_DateStructure.RTC_Date<=1)
								RTC_DateStructure.RTC_Date=RTC_DAYOFMONTH_MAX;
							else
								RTC_DateStructure.RTC_Date--;
                            RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
                            RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
                           
							break;
						case 9:
							if(RTC_TimeStructure.RTC_Hours<1)
								RTC_TimeStructure.RTC_Hours=RTC_HOUR_MAX;
							else
								RTC_TimeStructure.RTC_Hours--;
//                            
                            RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
                            RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
							break;
						case 10:
							if(RTC_TimeStructure.RTC_Minutes<1)
								RTC_TimeStructure.RTC_Minutes=RTC_MINUTE_MAX;
							else
								RTC_TimeStructure.RTC_Minutes--;
                            RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
                            RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
							break;
						case 11:
							if(RTC_TimeStructure.RTC_Seconds<1)
								RTC_TimeStructure.RTC_Seconds=RTC_SECOND_MAX;
							else
								RTC_TimeStructure.RTC_Seconds--;
                            RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
                            RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
							break;
						case 12://
							//SaveData.Sys_Setup.Bias=0;
							break;
						case 18:
							//SaveData.Sys_Setup.Bus_Mode=0;
							break;
						case 13:
//							if(SaveData.Sys_Setup.GP_Addr<1)
//								SaveData.Sys_Setup.GP_Addr=99;
//							else
//								SaveData.Sys_Setup.GP_Addr--;
							break;
//						case 14:
//							SaveData.Sys_Setup.Talk_Only=1;
//							break;
//						case 15:
//							if(SaveData.Sys_Setup.Timer_Value.Hour<1)
//								SaveData.Sys_Setup.Timer_Value.Hour=12;
//							else
//								SaveData.Sys_Setup.Timer_Value.Hour--;
//							break;
//						case 16:
//							if(SaveData.Sys_Setup.Timer_Value.Min<1)
//								SaveData.Sys_Setup.Timer_Value.Min=59;
//							else
//								SaveData.Sys_Setup.Timer_Value.Min--;
//							break;
//						case 17:
//							if(SaveData.Sys_Setup.Timer_Value.Sec<1)
//								SaveData.Sys_Setup.Timer_Value.Sec=59;
//							else
//								SaveData.Sys_Setup.Timer_Value.Sec--;
//							break;
						default:
							break;
					
					}
                    
				break;
				case Key_F2:
					switch(keynum)
					{
						case 0:
							SetSystemStatus(SYS_STATUS_SETUP);
							break;
						case 1:
							Jk516save.Sys_Setvalue.uart=1;
							break;
						case 2:
							Jk516save.Sys_Setvalue.buard=1;
							Debug_USART_Config(9600);
							break;
						case 3:
							Jk516save.Sys_Setvalue.u_flag=1;
							break;
						case 4:
							Jk516save.Sys_Setvalue.plc=1;
							break;
						case 5:
							Jk516save.Sys_Setvalue.lanage=1;
                            LCD_Clear(LCD_COLOR_TEST_BACK);
                            Disp_Sys_Item();
							break;
						case 6:
							if(RTC_DateStructure.RTC_Year>=RTC_YEAR_MAX)
								RTC_DateStructure.RTC_Year=0;
							else
								RTC_DateStructure.RTC_Year++;
                            RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
                            RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
							break;
						case 7:
							if(RTC_DateStructure.RTC_Month>=RTC_MONTH_MAX)
								RTC_DateStructure.RTC_Month=1;
							else
								RTC_DateStructure.RTC_Month++;
                            RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
                            RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
							break;
						case 8:
							if(RTC_DateStructure.RTC_Date>=RTC_DAYOFMONTH_MAX)
								RTC_DateStructure.RTC_Date=1;
							else
								RTC_DateStructure.RTC_Date++;
                            RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
                            RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
							break;
						case 9:
							if(RTC_TimeStructure.RTC_Hours>=RTC_HOUR_MAX)
								RTC_TimeStructure.RTC_Hours=0;
							else
								RTC_TimeStructure.RTC_Hours++;
                            RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
                            RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
							break;
						case 10:
							if(RTC_TimeStructure.RTC_Minutes>=RTC_MINUTE_MAX)
								RTC_TimeStructure.RTC_Minutes=0;
							else
								RTC_TimeStructure.RTC_Minutes++;
                            RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
                            RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
							break;
						case 11:
							if(RTC_TimeStructure.RTC_Seconds>=RTC_SECOND_MAX)
								RTC_TimeStructure.RTC_Seconds=0;
							else
								RTC_TimeStructure.RTC_Seconds++;
                            RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
                            RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
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
						case 2:
							Jk516save.Sys_Setvalue.buard=2;
							Debug_USART_Config(14400);
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
						case 2:
							Jk516save.Sys_Setvalue.buard=3;
							Debug_USART_Config(19200);
						break;
						default:
						break;
					
					}
				break;
				case Key_F5:
					switch(keynum)
					{
						case 0:
							break;//恢复系统复位
						case 2:
							Jk516save.Sys_Setvalue.buard=4;
							Debug_USART_Config(115200);
						break;
						default:
						break;
					
					}
				break;
				case Key_Disp:
                    SetSystemStatus(SYS_STATUS_TEST);
				break;
				case Key_SETUP:
                    SetSystemStatus(SYS_STATUS_SETUP);
				break;
				case Key_FAST:
                    if(keynum==12)
                    {
                        //dispflag=0;
//                        for(i=0;i<8;i++)
//                        {
//                            Save_Res.Sys_Setvalue.textname[i]=Disp_buff[i];
//                        
//                        }
                        key_count=0;
                        keynum=0;
                        //Savetoeeprom();
                    }
				break;
				case Key_LEFT:
				break;
				case Key_RIGHT:
				break;
				case Key_DOWN:
					if(keynum>SYS_MAX-1)
						keynum=0;
					else
						keynum++;
						
				break;
				case Key_UP:
					if(keynum<1)
						keynum=SYS_MAX;
					else
						keynum--;
				break;
				case Key_NUM1:
                    if(keynum==12)
                    {
                        if(key_count<PASSWORD_LENTH-1)
                        {
                            Jk516save.Sys_Setvalue.textname[key_count]='1';
                            key_count++;
                                
                        }
                    
                    }
                        //Save_Res.Sys_Setvalue
				break;
				case Key_NUM2:
                    if(keynum==12)
                    {
                        if(key_count<PASSWORD_LENTH-1)
                        {
                            Jk516save.Sys_Setvalue.textname[key_count]='2';
                            key_count++;
                                
                        }
                    
                    }
				break;
				case Key_NUM3:
                    if(keynum==12)
                    {
                        if(key_count<PASSWORD_LENTH-1)
                        {
                            Jk516save.Sys_Setvalue.textname[key_count]='3';
                            key_count++;
                                
                        }
                    
                    }
				break;
				case Key_NUM4:
                    if(keynum==12)
                    {
                        if(key_count<PASSWORD_LENTH-1)
                        {
                            Jk516save.Sys_Setvalue.textname[key_count]='4';
                            key_count++;
                                
                        }
                    
                    }
				break;
				case Key_NUM5:
                    if(keynum==12)
                    {
                        if(key_count<PASSWORD_LENTH-1)
                        {
                            Jk516save.Sys_Setvalue.textname[key_count]='5';
                            key_count++;
                                
                        }
                    
                    }
				break;
				case Key_NUM6:
                    if(keynum==12)
                    {
                        if(key_count<PASSWORD_LENTH-1)
                        {
                            Jk516save.Sys_Setvalue.textname[key_count]='6';
                            key_count++;
                                
                        }
                    
                    }
				break;
				case Key_NUM7:
                    if(keynum==12)
                    {
                        if(key_count<PASSWORD_LENTH-1)
                        {
                            Jk516save.Sys_Setvalue.textname[key_count]='7';
                            key_count++;
                                
                        }
                    
                    }
				break;
				case Key_NUM8:
                    if(keynum==12)
                    {
                        if(key_count<PASSWORD_LENTH-1)
                        {
                            Jk516save.Sys_Setvalue.textname[key_count]='8';
                            key_count++;
                                
                        }
                    
                    }
				break;
				case Key_NUM9:
                    if(keynum==12)
                    {
                        if(key_count<PASSWORD_LENTH-1)
                        {
                            Jk516save.Sys_Setvalue.textname[key_count]='9';
                            key_count++;
                                
                        }
                    
                    }
				break;
				case Key_NUM0:
                    if(keynum==12)
                    {
                        if(key_count<PASSWORD_LENTH-1)
                        {
                            Jk516save.Sys_Setvalue.textname[key_count]='0';
                            key_count++;
                                
                        }
                    
                    }
				break;
				case Key_DOT:
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

void Disp_Sys(void)
{
	uint32_t i;
    const u8 (*pt)[(sizeof(Sys_Sys[0]))];
    const u8 (*ppt)[(sizeof(All_TopName[0]))];
	Disp_Sys_Screen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
    if(Jk516save.Sys_Setvalue.lanage)
        ppt=All_TopName_E;
    else
        ppt=All_TopName;
	WriteString_16(0, 4, ppt[10],  0);
	//WriteString_16(0, 4, All_TopName[10],  0);
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
	if(Jk516save.open)
	{
		if(Jk516save.Sys_Setvalue.lanage)
			pt=Sys_Sys_E;
		else
			pt=Sys_Sys;
	}else{
		if(Jk516save.Sys_Setvalue.lanage)
			pt=Sys_Sys_E1;
		else
			pt=Sys_Sys1;
	}
    
//	WriteString_16(LIST1, FIRSTLINE, User_ListScan_Item[0],  0);

	for(i=0;i<(sizeof(Sys_Sys)/(sizeof(Sys_Sys[0])));i++)
	//if(i<=sizeof(Sys_Setitem)/(sizeof(Sys_Setitem[0]))/2)
	{
		WriteString_16(LIST1, FIRSTLINE+SPACE1*i, pt[i],  0);

	}//Save_Res.fac_num
    WriteString_16(LIST1+82, FIRSTLINE+SPACE1*3, (const uint8_t*)Jk516save.fac_num,  0);
//	else
//	{
//		WriteString_16(LIST2,FIRSTLINE+SPACE1*(i-sizeof(Sys_Setitem)/(sizeof(Sys_Setitem[0]))/2), Sys_Setitem[i],  0);
//	}
//	Disp_SysLine();
	Disp_Button_SysSet();
}

void Sys_Process(void)
{
	vu32 keynum=0;
	vu8 key;
	vu8 Disp_flag=1;
    LCD_Clear(LCD_COLOR_TEST_BACK);

	while(GetSystemStatus()==SYS_STATUS_SYS)
	{
		if(Disp_flag==1)
		{
			Disp_Sys();
			Disp_flag=0;	
		}
		key=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/10);
		if(key!=KEY_NONE)
		{
			Disp_flag=1;
//            Key_Beep();
			switch(key)
			{
				case Key_F1:
					switch(keynum)
					{
						case 0:
							SetSystemStatus(SYS_STATUS_TEST);
							break;
						
						default:
							break;
					
					}
                    
				break;
				case Key_F2:
					switch(keynum)
					{
						case 0:
							SetSystemStatus(SYS_STATUS_SETUP);
							break;
						
					
					}
                    
				break;
				case Key_F3:
					switch(keynum)
					{
						case 0:
							SetSystemStatus(SYS_STATUS_SYSSET);
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
						
							
						
						default:
						break;
					
					}
				break;
				
				
				case Key_Disp:
                    SetSystemStatus(SYS_STATUS_TEST);
				break;
				case Key_SETUP:
                    SetSystemStatus(SYS_STATUS_SETUP);
				break;
				
				
				default:
				break;
					
			}
		
		
		}
	
	
	
	
	}

}
void Disp_button_Num_time(void)
	
{
	Disp_Fastbutton();
	
	Colour.black=LCD_COLOR_TEST_BUTON;
	Colour.Fword=White;
	WriteString_16(84, 271-30, " mΩ  ",  0);
	WriteString_16(84+80, 271-30, " Ω ",  0);
    WriteString_16(84+80+80, 271-30, "kΩ ",  0);


}

//数字键输入显示
Sort_TypeDef Disp_NumKeyboard_Set(Disp_Coordinates_Typedef *Coordinates,u8 flag )
{
    u8 lenth;
	vu8 While_flag=1;
	vu8 Disp_buff[10];
	vu8 key,i;
	vu8 dispflag=1;
	vu8 dot_num=0,dot_num1=0;
//	vu8 page=0;
	vu32 keynum=0;
	vu8 key_count=0;
//	vu32 Num[6]={1,10,100,1e3,1e4,1e5};
	Sort_TypeDef   Sort_set;
	Sort_set.Dot=0;
	Sort_set.Num=0;
	Sort_set.Unit=0;
	Sort_set.Num=0;
	for(i=0;i<7;i++)
	Disp_buff[i]=' ';
	Disp_buff[7]=0;
	if(flag)
        lenth=NUM_LENTH_V;
    else
        lenth=NUM_LENTH;
	while(While_flag)
	{
		key=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/10);
        if(key!=KEY_NONE)
		{
			dispflag=1;
//            Key_Beep();
			switch(key)
			{
				case Key_F1:
					
					Sort_set.Unit=0;
					
					While_flag=0;
					if(key_count<lenth)
					{
						if(dot_num==0)
						{
							if(key_count>0)
							{
								Disp_buff[key_count]='.';
								dot_num1=key_count;
								key_count++;
							
							
							}
							dot_num++;
						}
					
					}
						
					
					Sort_set.Updata_flag=1;
					
				break;
				case Key_F2:
					Sort_set.Unit=1;
					
					While_flag=0;
					if(key_count<lenth)
					{
						if(dot_num==0)
						{
							if(key_count>0)
							{
								Disp_buff[key_count]='.';
								dot_num1=key_count;
								key_count++;
							
							
							}
							dot_num++;
						}
					
					}
						
					
					Sort_set.Updata_flag=1;
					
				break;
				case Key_F3:
                    Sort_set.Unit=2;
					
					While_flag=0;
					if(key_count<lenth)
					{
						if(dot_num==0)
						{
							if(key_count>0)
							{
								Disp_buff[key_count]='.';
								dot_num1=key_count;
								key_count++;
							
							
							}
							dot_num++;
						}
					
					}
						
					
					Sort_set.Updata_flag=1;
					
				break;
				case Key_F4:
					
				break;
				case Key_F5:
					
				break;
				case Key_Disp:
					SetSystemStatus(SYS_STATUS_TEST);
					While_flag=0;
					Sort_set.Updata_flag=0;
				break;
				case Key_SETUP:
					While_flag=0;
					Sort_set.Updata_flag=0;
				SetSystemStatus(SYS_STATUS_SETUP);
				break;
				case Key_FAST:
				break;
				case Key_LEFT:
				break;
				case Key_RIGHT:
				break;
				case Key_UP:
				break;
				case Key_DOWN:
				break;
				case Key_NUM1:
					if(key_count<lenth)
					{
						Disp_buff[key_count]='1';
						
						
							
						
						key_count++;
							
					}
				break;
				case Key_NUM2:
					if(key_count<lenth)
					{
						Disp_buff[key_count]='2';
						key_count++;
					}
				break;
				case Key_NUM3:
					if(key_count<lenth)
					{
						Disp_buff[key_count]='3';
						key_count++;
					}
				break;
				case Key_NUM4:
					if(key_count<lenth)
					{
						Disp_buff[key_count]='4';
						key_count++;
					}
					
				break;
				case Key_NUM5:
					if(key_count<lenth)
					{
						Disp_buff[key_count]='5';
						key_count++;
					}
				break;
				case Key_NUM6:
					if(key_count<lenth)
					{
						Disp_buff[key_count]='6';
						key_count++;
					}
				break;
				case Key_NUM7:
					if(key_count<lenth)
					{
						Disp_buff[key_count]='7';
						key_count++;
					}
				break;
				case Key_NUM8:
					if(key_count<lenth)
					{
						Disp_buff[key_count]='8';
						key_count++;
					}
				break;
				case Key_NUM9:
					if(key_count<lenth)
					{
						Disp_buff[key_count]='9';
						key_count++;
					}
				break;
				case Key_NUM0:
					if(key_count<lenth)
					{
						Disp_buff[key_count]='0';
						key_count++;
					}
				break;
				case Key_DOT:
					
					if(key_count<lenth&&key_count>0)
					{
						if(dot_num==0)
						{
							if(key_count>0)
							{
								Disp_buff[key_count]='.';
								dot_num1=key_count;
								key_count++;
							
							
							}
							dot_num++;
						}
					}
//					else 
//					{
//						if(Disp_buff[key_count]==0)
//							Disp_buff[key_count]='-';
//						else if(Disp_buff[key_count]=='-')
//							Disp_buff[key_count]='+';
//						else
//							Disp_buff[key_count]='-';
//						key_count++;
//							
//					
//					
//					
//					
//					}
				break;
				case Key_BACK:
					if(key_count>0)
					{	key_count--;
						Disp_buff[key_count]=' ';
						if(dot_num1==key_count)
						{
							dot_num=0;
							dot_num1=0;
							
						}
					
					}
					else
					{
						if(Disp_buff[key_count]==0)
								Disp_buff[key_count]='-';
							else if(Disp_buff[key_count]=='-')
								Disp_buff[key_count]='+';
							else
								Disp_buff[key_count]='-';
							key_count++;
					}
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
		if(dispflag)
		{
			dispflag=0;
            Colour.Fword=White;
			Colour.black=LCD_COLOR_RED;
			LCD_DrawFullRect( Coordinates->xpos, Coordinates->ypos,Coordinates->lenth+16 , 16  );
			
			WriteString_16(Coordinates->xpos, Coordinates->ypos, (const uint8_t *)Disp_buff,  0);
			//dispflag=0;
		}
	
	}
	for(i=key_count;i<lenth;i++)
		Disp_buff[i]='0';
	for(i=0;i<lenth;i++)
	{
		if(Disp_buff[0]>='0'&&(Disp_buff[0]<='9'))
		{
			if(Disp_buff[i]>='0'&&(Disp_buff[i]<='9'))
			{
			
				if(dot_num1>i)
				{
					Sort_set.Num*=10;
					Sort_set.Num+=Disp_buff[i]-'0';
				
				}
				else
				{
					Sort_set.Num*=10;
					Sort_set.Num+=Disp_buff[i]-'0';
				
				
				}
			}
			
			
			//Sort_set.Num+=(Disp_buff[key_count-1]-'0');
		
		
		}
	
	
	
	}
	Sort_set.Dot=dot_num1;
//	if(Disp_buff[0]>='0'&&(Disp_buff[0]<'9'))
//		{
//			if(Disp_buff[key_count-1]!='.')		
//			{
//				Sort_set.Num*=Num[key_count-dot_num-1];
//				Sort_set.Num+=(Disp_buff[key_count-1]-'0');//*Num[key_count-dot_num-1];
//				
//			}				
//			//*(Disp_buff[key_count-1]-'0'))+=Num[key_count-dot_num-1];
//			else
//			{
//			
//			}
//		
//		
//		}
//			
//		else 
//			;//(Disp_buff[key_count-1]-'0')*Sort_set.Num+=Num[key_count-dot_num-2];
	return Sort_set;

}
Sort_TypeDef Time_Set_Cov(Sort_TypeDef *Time)
{
	float value;
	if(Time->Num>99999)
	{
		Time->Num/=10;
		Time->Dot=MAX_DOTNUM;
		
	}
	Time->Dot=MAX_DOTNUM-Time->Dot;
	value=Time->Num;
	if(Time->Unit==0)
		value*=(float)1e4;
	else if(Time->Unit==1)
		value*=(float)1e7;
    else if(Time->Unit==2)
		value*=(float)1e10;
        
	value/=pow(10,Time->Dot);
	if(value>(float)3e11)
	{
		//value=3e10;
		Time->Num=30000;
		Time->Dot=3;
		Time->Unit=2;
		
	}else
    if(value>=(float)3e10)
	{
		Time->Num=value/(float)1e7;
		Time->Dot=3;
		Time->Unit=2;
		
	
	
	
	}else if(value>=(float)3e9)
	{
		Time->Num=value/(float)1e6;
		Time->Dot=4;
		Time->Unit=2;
	
	
	}else if(value>=(float)3e8)
	{
		Time->Num=value/(float)1e5;
		Time->Dot=2;
		Time->Unit=1;
	
	
	}
	else if(value>=(float)3e7)
	{
		Time->Num=value/(float)10000;
		Time->Dot=3;
		Time->Unit=1;
	
	}else if(value>=(float)3e6)
	{
		Time->Num=value/(float)1000;
		Time->Dot=4;
		Time->Unit=1;
	
	
	}else if(value>=(float)3e5)
	{
		Time->Num=value/(float)100;
		Time->Dot=2;
		Time->Unit=0;
	
	}
    else if(value>=(float)3e4)
        {
		Time->Num=value/10;
		Time->Dot=3;
		Time->Unit=0;
	
	}
        else
        {
        Time->Num=value;
		Time->Dot=4;
		Time->Unit=0;
        
        }
	Time->mid_data=	value;
	
	

	return *Time;
}
//电阻设置
Sort_TypeDef Disp_Set_Num(Disp_Coordinates_Typedef *Coordinates)
{
	Sort_TypeDef Sort_num,Sort_num1;
	Disp_button_Num_time();
	Sort_num=Disp_NumKeyboard_Set(Coordinates,0);
	Sort_num1=Time_Set_Cov(&Sort_num);
	if(Sort_num1.Updata_flag==0)
	{
		Sort_num1.Dot=0;
		Sort_num1.Num=0;
		Sort_num1.Unit=0;
	
	}
		
	return Sort_num1;

}

//电阻设置
Sort_TypeDef SCPI_SET_R(void)
{
	u8 i;
	static Sort_TypeDef Scpi_num,Scpi_num1;
	Scpi_num.Dot=0;
	Scpi_num.Num=0;
	Scpi_num.Unit=0;
	
	Scpi_num1.Dot=0;
	Scpi_num1.Num=0;
	Scpi_num1.Unit=0;
	
	
	Scpi_num.Unit=scpiunit;
	if(scpidot == 0)
	{
		scpinum[scpnum]='.';
		scpidot = scpnum;
		for(i=scpnum+1;i<6;i++)
		scpinum[i]='0';
	}else{
		for(i=scpnum;i<6;i++)
		scpinum[i]='0';
	}
	
	
	for(i=0;i<6;i++)
	{
		if(scpinum[0]>='0'&&(scpinum[0]<='9'))
		{
			if(scpinum[i]>='0'&&(scpinum[i]<='9'))
			{
			
				if(scpidot>i)
				{
					Scpi_num.Num*=10;
					Scpi_num.Num+=scpinum[i]-'0';
				
				}
				else
				{
					Scpi_num.Num*=10;
					Scpi_num.Num+=scpinum[i]-'0';
				
				
				}
			}
			
			
			//Sort_set.Num+=(Disp_buff[key_count-1]-'0');
		
		
		}			
	}
	Scpi_num.Dot=scpidot;
	Scpi_num1=Time_Set_Cov(&Scpi_num);
	scpidot = 0;	
		
	return Scpi_num1;

}
//电阻设置
Sort_TypeDef SCPI_SET_R1(void)
{
	u8 i;
	static Sort_TypeDef Scpi_num,Scpi_num1;
	Scpi_num.Dot=0;
	Scpi_num.Num=0;
	Scpi_num.Unit=0;
	
	Scpi_num1.Dot=0;
	Scpi_num1.Num=0;
	Scpi_num1.Unit=0;
	
	
	Scpi_num.Unit=scpiunit1;
	if(scpidot1 == 0)
	{
		scpinum1[scpnum1]='.';
		scpidot1 = scpnum1;
		for(i=scpnum1+1;i<6;i++)
		scpinum1[i]='0';
	}else{
		for(i=scpnum1;i<6;i++)
		scpinum1[i]='0';
	}
	
	
	for(i=0;i<6;i++)
	{
		if(scpinum1[0]>='0'&&(scpinum1[0]<='9'))
		{
			if(scpinum1[i]>='0'&&(scpinum1[i]<='9'))
			{
			
				if(scpidot1>i)
				{
					Scpi_num.Num*=10;
					Scpi_num.Num+=scpinum1[i]-'0';
				
				}
				else
				{
					Scpi_num.Num*=10;
					Scpi_num.Num+=scpinum1[i]-'0';
				
				
				}
			}
			
			
			//Sort_set.Num+=(Disp_buff[key_count-1]-'0');
		
		
		}			
	}
	Scpi_num.Dot=scpidot1;
	Scpi_num1=Time_Set_Cov(&Scpi_num);
	scpidot1 = 0;	
		
	return Scpi_num1;

}


Sort_TypeDef Input_Set_Cov(Sort_TypeDef *Input_Ref)//
{
//	vu8 i;
//	vu32 buff[]={10000,1000,100,10,1};
	float value;
//	if(Input_Ref->Num>120000)
//	{
//		Input_Ref->Num=12000;
//		Input_Ref->Dot=3;
//		
//	}
//	value=(float)Input_Ref->Num*1000000;
//	value/=(pow(10,7-Input_Ref->Dot));
	value=Input_Ref->Num/(pow(10,7-Input_Ref->Dot));
	value*=(float)1000000;
	
	if(value>12000000)
	{
		value=12000000;
	}
	if(value>=(float)1e7)
	{
		Input_Ref->Dot=3;
		Input_Ref->Num=value/100;
	
	
	}else if(value>=(float)1e6)
	{
		Input_Ref->Dot=4;
		Input_Ref->Num=value/10;
	
	
	}
	else
	{
		Input_Ref->Dot=5;
		Input_Ref->Num=value;
	
	
	}
	Input_Ref->mid_data=value;
//	value=pow(10.0,4-(float)Input_Ref->Dot);
//	value=Input_Ref->Num/value;
//	if(value>=1000)
//	{
//		Input_Ref->Unit++;
//		Input_Ref->Dot+=3;
//	
//	
//	}else if(value==0)
//	{
//		if(Input_Ref->Unit>0)
//		{
//			Input_Ref->Unit--;
//			if(Input_Ref->Dot>=3)
//				Input_Ref->Dot-=3;
//		
//		
//		}
//	
//	
//	}
	
	return *Input_Ref;
}
void Disp_button_Num_Freq(void)
{
	Disp_Fastbutton();
	
	Colour.black=LCD_COLOR_TEST_BUTON;
	Colour.Fword=White;
	WriteString_16(84, 271-30, " V ",  0);
	//WriteString_16(84+80, 271-30, " Ω ",  0);


}
//电压设置
Sort_TypeDef Disp_Set_CompNum(Disp_Coordinates_Typedef *Coordinates)
{

        
    Sort_TypeDef Sort_num,Sort_num1;
	Disp_button_Num_Freq();
	Sort_num=Disp_NumKeyboard_Set(Coordinates,1);
	Sort_num1=Input_Set_Cov(&Sort_num);
	if(Sort_num1.Updata_flag==0)
	{
		Sort_num1.Dot=0;
		Sort_num1.Num=0;
		Sort_num1.Unit=0;
	
	}
		
	return Sort_num1;

}

//电压设置
Sort_TypeDef SCPI_SET_V(void)
{
	u8 i;
	static Sort_TypeDef ScpiV_num,ScpiV_num1;
	ScpiV_num.Dot=0;
	ScpiV_num.Num=0;
	ScpiV_num.Unit=0;
	
	ScpiV_num1.Dot=0;
	ScpiV_num1.Num=0;
	ScpiV_num1.Unit=0;
	
	
	ScpiV_num.Unit=0;
	if(scpidot == 0)
	{
		scpinum[scpnum]='.';
		scpidot = scpnum;
		for(i=scpnum+1;i<7;i++)
		scpinum[i]='0';
	}else{
		for(i=scpnum;i<7;i++)
		scpinum[i]='0';
	}
	
	
	for(i=0;i<7;i++)
	{
		if(scpinum[0]>='0'&&(scpinum[0]<='9'))
		{
			if(scpinum[i]>='0'&&(scpinum[i]<='9'))
			{
			
				if(scpidot>i)
				{
					ScpiV_num.Num*=10;
					ScpiV_num.Num+=scpinum[i]-'0';
				
				}
				else
				{
					ScpiV_num.Num*=10;
					ScpiV_num.Num+=scpinum[i]-'0';
				
				
				}
			}
			
			
			//Sort_set.Num+=(Disp_buff[key_count-1]-'0');
		
		
		}			
	}
	ScpiV_num.Dot=scpidot;
	ScpiV_num1=Input_Set_Cov(&ScpiV_num);
	scpidot = 0;	
		
	return ScpiV_num1;

}

//电压设置
Sort_TypeDef SCPI_SET_V1(void)
{
	u8 i;
	static Sort_TypeDef ScpiV_num,ScpiV_num1;
	ScpiV_num.Dot=0;
	ScpiV_num.Num=0;
	ScpiV_num.Unit=0;
	
	ScpiV_num1.Dot=0;
	ScpiV_num1.Num=0;
	ScpiV_num1.Unit=0;
	
	
	ScpiV_num.Unit=0;
	if(scpidot1 == 0)
	{
		scpinum1[scpnum1]='.';
		scpidot1 = scpnum1;
		for(i=scpnum1+1;i<7;i++)
		scpinum1[i]='0';
	}else{
		for(i=scpnum1;i<7;i++)
		scpinum1[i]='0';
	}
	
	
	for(i=0;i<7;i++)
	{
		if(scpinum1[0]>='0'&&(scpinum1[0]<='9'))
		{
			if(scpinum1[i]>='0'&&(scpinum1[i]<='9'))
			{
			
				if(scpidot1>i)
				{
					ScpiV_num.Num*=10;
					ScpiV_num.Num+=scpinum1[i]-'0';
				
				}
				else
				{
					ScpiV_num.Num*=10;
					ScpiV_num.Num+=scpinum1[i]-'0';
				
				
				}
			}
			
			
			//Sort_set.Num+=(Disp_buff[key_count-1]-'0');
		
		
		}			
	}
	ScpiV_num.Dot=scpidot1;
	ScpiV_num1=Input_Set_Cov(&ScpiV_num);
	scpidot1 = 0;	
		
	return ScpiV_num1;

}
void Disp_Range(u8 hand,u8 range)
{//Range_Disp_Test
//    if(hand==1)
//        hand=0;
//    else
//        hand=1;
    if(hand>1)
        hand=1;
    if(range>6)
        range=6;
    Colour.black =LCD_COLOR_TEST_BACK;
    WriteString_16(SORTING_XDISP-20, SORTING_Y_DISP+30, Range_Disp_Test[hand][range],  0);


}

void Swap(uint32_t A[], uint16_t i, uint16_t j)
{
    int temp = A[i];
    A[i] = A[j];
    A[j] = temp;
}
//冒泡排序
void BubbleSort(uint32_t A[], uint16_t n)
{
	int i,j;
    for (j = 0; j < n - 1; j++)         // 每次最大元素就像气泡一样"浮"到数组的最后
    {
        for (i = 0; i < n - 1 - j; i++) // 依次比较相邻的两个元素,使较大的那个向后移
        {
            if (A[i] > A[i + 1])            // 如果条件改成A[i] >= A[i + 1],则变为不稳定的排序算法
            {
                Swap(A, i, i + 1);
            }
        }
    }
}

uint8_t R_Comp(void)
{
    float testvalue;
    float set_highvalue;
    float set_lowvalue;
    u8 flag;
    testvalue=Test_Value.res*pow(10,3*(Test_Value.uint))/pow(10,Test_Value.dot);
    set_highvalue=Jk516save.Set_Data.High_Res.Num*pow(10,3*(Jk516save.Set_Data.High_Res.Unit))/pow(10,Jk516save.Set_Data.High_Res.Dot);
    set_lowvalue=Jk516save.Set_Data.Res_low.Num*pow(10,3*(Jk516save.Set_Data.Res_low.Unit))/pow(10,Jk516save.Set_Data.Res_low.Dot);
    if(testvalue>set_highvalue)//大于上限
        flag=1;
    else if(testvalue<set_lowvalue)//小于下限
    {
        flag=2;
        
            
    
    
    }
    else
        flag=0;//合格
    return flag;



}
uint8_t V_Comp(void)
{
    u32 testvalue;
    u32 set_highvalue;
    u32 set_lowvalue;
    u8 flag;
    u8 dot=1;
    if(Test_Value_V.dot==3)
        dot=10;
    testvalue=Test_Value_V.res*dot;
    
    if(Jk516save.Set_Data.V_high.Dot==3)
        set_highvalue=Jk516save.Set_Data.V_high.Num*10;
    else if(Jk516save.Set_Data.V_high.Dot==5)
        set_highvalue=Jk516save.Set_Data.V_high.Num/10;
    else
        set_highvalue=Jk516save.Set_Data.V_high.Num;
        
    
    if(Jk516save.Set_Data.V_low.Dot==3)
        set_lowvalue=Jk516save.Set_Data.V_low.Num*10;
    else if(Jk516save.Set_Data.V_low.Dot==5)
        set_lowvalue=Jk516save.Set_Data.V_low.Num/10;
    else
        set_lowvalue=Jk516save.Set_Data.V_low.Num;
    
    if(testvalue>set_highvalue)//大于上限
        flag=1;
    else if(testvalue<set_lowvalue)//小于下限
        flag=2;
    else
        flag=0;
    return flag;



}
uint8_t Jisuan_Range(Sort_TypeDef date)//0  m  1    2 k
{
    uint8_t range;
    if(date.Dot==3&&date.Unit==0)
        range=0;
    if(date.Dot==2&&date.Unit==0)
        range=1;
    if(date.Dot==4&&date.Unit==1)
        range=2;
    if(date.Dot==3&&date.Unit==1)
        range=3;
    if(date.Dot==2&&date.Unit==1)
        range=4;
    if(date.Dot==4&&date.Unit==2)
        range=5;
    if(date.Dot==3&&date.Unit==2)
        range=6;
    
    return range;


}
uint8_t Jisuan_V_Range(Sort_TypeDef date)
{
    uint8_t range;
    if(date.Dot==3&&date .Num>120000)
        range=1;
    else
        range=0;


    return range;
}
void Disp_dateandtime(void)
{
    
    RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
    char LCDTemp[30];
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);//改1秒扫描一次
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
    sprintf(LCDTemp,"20%0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d", 
    RTC_DateStructure.RTC_Year,
    RTC_DateStructure.RTC_Month, 
    RTC_DateStructure.RTC_Date,
    RTC_TimeStructure.RTC_Hours, 
    RTC_TimeStructure.RTC_Minutes,
    RTC_TimeStructure.RTC_Seconds);
    
    
    //);
    Colour.black =LCD_COLOR_TEST_BACK;
    WriteString_16(LIST2, LIST1+4, (const uint8_t *)LCDTemp,  0);
//    sprintf(LCDTemp,"The Time :  %0.2d:%0.2d:%0.2d", 
//    RTC_TimeStructure.RTC_Hours, 
//    RTC_TimeStructure.RTC_Minutes, 
//    RTC_TimeStructure.RTC_Seconds);
//    WriteString_16(SORTING_XDISP-20, LIST1+4, Range_Disp_Test[hand][range],  0);
    memcpy((void*)Send_To_U.No,LCDTemp,19);
    Send_To_U.No[19]=9;

}

void Disp_Debug_Reference( vu8 list,Test_ValueTypedef eee,Test_ValueTypedef ddd)
{
	if(list)
	{
		if(debugpage == 0)
		{
			if(list>7)
			{
				
				//V_BCD_Int(eee);
				Hex_Format(eee.res,eee.dot,6,FALSE);
				
				
			}
			else
			{
				
				 Hex_Format(ddd.res,ddd.dot,7,FALSE);
				
				
			}
			WriteString_16(LIST1+160+160, FIRSTLINE+(SPACE1-2)*(list), DispBuf,  1);
		}else if(debugpage == 1){
			Hex_Format(eee.res,eee.dot,6,FALSE);
			WriteString_16(LIST1+160+160, FIRSTLINE+(SPACE1-2)*(list), DispBuf,  1);
		}
	}


}
const vu8 Debug_Dot[9]={3,2,4,3,2,4,3,4,4};
const vu8 Debug_Dot1[8]={4,4,3,3,3,3,3,3};
//显示设置参数的值Setup_Valueall
void Disp_Debug_value(vu8 list,vu8 page)
{
//	vu32 flag;
	vu32 i;
//	vu32 xpose;
	vu32 Black_Select;
//	vu32 Select_color;
	if(page == 0)
	{
		for(i=0;i<DEBUG_RANGE-8;i++)
		{
			if(list==(i+1))
			{
				Colour.black=LCD_COLOR_SELECT;
			
			}
			else
			{
				Colour.black=LCD_COLOR_TEST_BACK;
			}
		
		
			LCD_DrawFullRect( LIST1+160, FIRSTLINE+(SPACE1-2)*(i+1),90 , SPACE1-2 ) ;//SPACE1
			Hex_Format(Jk516save.Debug_Value[i].standard, Debug_Dot[i] , 6 , 0);//sprintf((char *)DispBuf,"%4",Save_Res;
			WriteString_16(LIST1+160, FIRSTLINE+(SPACE1-2)*(i+1), DispBuf,  1);
			
	//		LCD_DrawRect( LIST1+160+160, FIRSTLINE+SPACE1*(i+1)-2,SELECT_1END+40+160 , FIRSTLINE+SPACE1*(i+2)-4 , Colour.black ) ;//SPACE1
			//Hex_Format(Save_Res.Debug_Value[i].ad_value, Debug_Dot[i] , 4 , 0);//sprintf((char *)DispBuf,"%4",Save_Res;
			
			
				
		}
		Colour.black=LCD_COLOR_TEST_BACK;
	}else if(page == 1){
		for(i=0;i<8;i++)
		{
			if(list==(i+1))
			{
				Colour.black=LCD_COLOR_SELECT;
			
			}
			else
			{
				Colour.black=LCD_COLOR_TEST_BACK;
			}
		
		
			LCD_DrawFullRect( LIST1+160, FIRSTLINE+(SPACE1-2)*(i+1),90 , SPACE1-2 ) ;//SPACE1
			if(i < 8)
			{
				Hex_Format(Jk516save.Debug_Value[i+9].standard, Debug_Dot1[i] , 6 , 0);
			}else{
				Hex_Format(Jk516save.Debug_Value[i+9].standard, Debug_Dot1[i] , 7 , 0);
			}				
			WriteString_16(LIST1+160, FIRSTLINE+(SPACE1-2)*(i+1), DispBuf,  1);
			
	//		LCD_DrawRect( LIST1+160+160, FIRSTLINE+SPACE1*(i+1)-2,SELECT_1END+40+160 , FIRSTLINE+SPACE1*(i+2)-4 , Colour.black ) ;//SPACE1
			//Hex_Format(Save_Res.Debug_Value[i].ad_value, Debug_Dot[i] , 4 , 0);//sprintf((char *)DispBuf,"%4",Save_Res;
			
			
				
		}
		LCD_DrawFullRect( LIST1+160, FIRSTLINE+(SPACE1-2)*(8+1),90 , SPACE1-2 ) ;//SPACE1
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	


}
//用户校正界面子函数
void Disp_Usercheck_Screen(void)
{
	Disp_TopBar_Color();
	Disp_Fastbutton();//显示快速按键
//	Disp_TestSet_MidRect();//显示中间色块
}
void Disp_UserCheck_Item(void)
{
	uint32_t i;
	Disp_Usercheck_Screen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	WriteString_16(0, 4, All_TopName[5],  0);
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;//User_Check_main
	
	for(i=0;i<(sizeof(User_Check_main)/(sizeof(User_Check_main[0])));i++)
	{
		WriteString_16(LIST1+160*i, FIRSTLINE, User_Check_main[i],  0);

	}
	if(debugpage == 0)
	{
		for(i=0;i<(sizeof(User_Check_Item)/(sizeof(User_Check_Item[0])));i++)
		{
			WriteString_16(LIST1, FIRSTLINE+(SPACE1-2)*(i+1), User_Check_Item[i],  0);

		}
	}else{
		for(i=0;i<(sizeof(User_Check_Item1)/(sizeof(User_Check_Item1[0])));i++)
		{
			WriteString_16(LIST1, FIRSTLINE+(SPACE1-2)*(i+1), User_Check_Item1[i],  0);

		}
	}
//	else
//	{
//		WriteString_16(LIST2, FIRSTLINE+SPACE1*(i-sizeof(User_Check_Item)/(sizeof(User_Check_Item[0]))/2), User_Check_Item[i],  0);
//	}	
//	Disp_Button_TestSet(0);
}
int32_t Debug_Valuedata(Sort_TypeDef *Debug_value)
{
    if(Debug_value->Num>20000 && Debug_value->Dot != 3)
        Debug_value->Num/=10;
	return Debug_value->Num;//(pow(10,Debug_value->Dot));


}
vu32 Debug_Set_Num(Disp_Coordinates_Typedef *Coordinates)//
{
	Sort_TypeDef Sort_num;
//	vu8 i;
	vu32 num;
	Disp_button_Num_Freq();
	Sort_num=Disp_NumKeyboard_Set(Coordinates,1);
	num= Debug_Valuedata(&Sort_num);
	
	
		
	return num;
	
}
vu32 Debug_Set_Res(Disp_Coordinates_Typedef *Coordinates)
{
   Sort_TypeDef Sort_num;
    vu32 num; 
    Disp_button_Num_time();
    Sort_num=Disp_NumKeyboard_Set(Coordinates,0);
    num= Debug_Valuedata(&Sort_num);
    return num;
    

}
void Debug_stanedcomp(void)
{
    vu8 i;
   for(i=0;i<DEBUG_RANGE;i++)
    {
        if(Jk516save.Debug_Value[i].standard>Debug_Compvalue[i][1]||Jk516save.Debug_Value[i].standard<Debug_Compvalue[i][1])
        {
            if(i<DEBUG_RANGE-2)
            {
                
                 Jk516save.Debug_Value[i].standard=10000;
                    
                
            }
            else
            {
            
                Jk516save.Debug_Value[i].standard=60000;
            
            }
        }
		if(Jk516save.Debug_Value1[i].standard>Debug_Compvalue[i][1]||Jk516save.Debug_Value1[i].standard<Debug_Compvalue[i][1])
        {
            if(i<DEBUG_RANGE-2)
            {
                
                 Jk516save.Debug_Value1[i].standard=10000;
                    
                
            }
            else
            {
            
                Jk516save.Debug_Value1[i].standard=60000;
            
            }
        }
		if(Jk516save.Debug_Value2[i].standard>Debug_Compvalue[i][1]||Jk516save.Debug_Value2[i].standard<Debug_Compvalue[i][1])
        {
            if(i<DEBUG_RANGE-2)
            {
                
                 Jk516save.Debug_Value2[i].standard=10000;
                    
                
            }
            else
            {
            
                Jk516save.Debug_Value2[i].standard=60000;
            
            }
        }
        if(Jk516save.Debug_Value3[i].standard>Debug_Compvalue[i][1]||Jk516save.Debug_Value3[i].standard<Debug_Compvalue[i][1])
        {
            if(i<DEBUG_RANGE-2)
            {
                
                 Jk516save.Debug_Value3[i].standard=10000;
                    
                
            }
            else
            {
            
                Jk516save.Debug_Value3[i].standard=60000;
            
            }
        }
            
    
    }


}
//输入仪器编号
void input_num(Disp_Coordinates_Typedef *Coordinates )
{
  	char While_flag=1;
	uint8_t Disp_buff[12];
	uint8_t  key,i;
	char dispflag=1;

	char key_count=0;

	
	for(i=0;i<8;i++)
	Disp_buff[i]=' ';
	Disp_buff[8]=0;
    Disp_buff[9]=0;
    
	
	while(While_flag)
	{
		key=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/10);
		
		if(key!=KEY_NONE)
		{
 //           password_flag=0;
           // Key_Beep();
			dispflag=1;
			switch(key)
			{
				case Key_F1://退出
					 
                       //SetSystemStatus(SYS_STATUS_USERDEBUG);

					
				break;
				case Key_F2://取消
                    
					
				break;
				case Key_F3://确认保存机号和日期

					
				break;
				case Key_F4:
					
				break;
				case Key_F5:
                   //return  &Disp_buff[0];
					
				break;
				case Key_Disp:

				break;
				case Key_SETUP:

				break;
				case Key_FAST:
                    While_flag=0;//保存
                    dispflag=0;
                    for(i=0;i<9;i++)
                    {
                        Jk516save.fac_num[i]=Disp_buff[i];
                    
                    }
                    //Savetoeeprom();
                    Store_set_flash();
                     LCD_DrawFullRect( 0, Coordinates->ypos-20,200 , Coordinates->ypos+20 );//待查
				break;
				case Key_LEFT:
				break;
				case Key_RIGHT:
				break;
				case Key_UP:
				break;
				case Key_DOWN:
				break;
				case Key_NUM1:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='1';
						
						
							
						
						key_count++;
							
					}
				break;
				case Key_NUM2:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='2';
						key_count++;
					}
				break;
				case Key_NUM3:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='3';
						key_count++;
					}
				break;
				case Key_NUM4:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='4';
						key_count++;
					}
					
				break;
				case Key_NUM5:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='5';
						key_count++;
					}
				break;
				case Key_NUM6:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='6';
						key_count++;
					}
				break;
				case Key_NUM7:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='7';
						key_count++;
					}
				break;
				case Key_NUM8:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='8';
						key_count++;
					}
				break;
				case Key_NUM9:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='9';
						key_count++;
					}
				break;
				case Key_NUM0:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='0';
						key_count++;
					}
				break;
				case Key_DOT:
                    if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='/';
						key_count++;
					}
					

				break;
				case Key_BACK:
					if(key_count)
					{	key_count--;
						Disp_buff[key_count]=' ';

					
					}

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
		if(dispflag)
		{
			dispflag=0;
            Colour.Fword=Red;
            LCD_DrawLine( Coordinates->xpos+13+80, Coordinates->ypos-2, 80, 0  );
            LCD_DrawLine( Coordinates->xpos+13+80, Coordinates->ypos+18, 80,0 );
            for(i=0;i<9;i++)
            LCD_DrawLine( Coordinates->xpos+13+i*10, Coordinates->ypos-2+20, 20, 1  );
            
			//LCD_DrawRect( Coordinates->xpos, Coordinates->ypos,Coordinates->xpos+Coordinates->lenth , Coordinates->ypos+16 , Red );
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BACK;
            WriteString_16(Coordinates->xpos-70, Coordinates->ypos, "出厂编号:",  0);
			WriteString_16(Coordinates->xpos+13, Coordinates->ypos, ( uint8_t *)Disp_buff,  0);
//            if(password_flag)
//                WriteString_16(Coordinates->xpos, Coordinates->ypos+20, "密码错误",  0);
//            else
//               LCD_DrawRect( Coordinates->xpos, Coordinates->ypos+20,Coordinates->xpos+100 , Coordinates->ypos+40, LCD_COLOR_TEST_BACK );
               // WriteString_16(Coordinates->xpos, Coordinates->ypos, "         ",  0);
            WriteString_16(Coordinates->xpos, Coordinates->ypos-20, "请输入8位出厂编号",  0);
		}
	
	}

}
void input_password(Disp_Coordinates_Typedef *Coordinates )
{
  	char While_flag=1;
	char Disp_buff[10];
	u8 key,i;
	char dispflag=1;
	char dot_num=0,dot_num1=0;
    char password_flag=0;
	char  key_count=0;
	for(i=0;i<9;i++)
	Disp_buff[i]=0;
	Disp_Button_value1(2);
	while(While_flag)
	{
		key=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/10);
		if(key!=KEY_NONE)
		{
            //Key_Beep()
            password_flag=0;
//            Key_Beep();
			dispflag=1;
			switch(key)
			{
				case Key_F1://退出
					 While_flag=0;
                       SetSystemStatus(SYS_STATUS_TEST);

					
				break;
				case Key_F3://取消
                    
					
				break;
				case Key_F2://确认
                   if(strcmp(PASSWORD,Disp_buff))//比较函数  当相等时  返回0
                   {//密码错误
                       key_count=0;
                       for(i=0;i<8;i++)
                        Disp_buff[i]=' ';
                       password_flag=1;
                       
                   
                   
                   }
                   else //密码正确
                   {
                        While_flag=0;
                       SetSystemStatus(SYS_STATUS_DEBUG);
                   
                   }
                       
					
				break;
				case Key_F4:
					
				break;
				case Key_F5:
					
				break;
				case Key_Disp:
//					SetSystemStatus(SYS_STATUS_TEST);
//					While_flag=0;
//					Sort_set.Updata_flag=0;
				break;
				case Key_SETUP:
//					While_flag=0;
//					Sort_set.Updata_flag=0;
//				SetSystemStatus(SYS_STATUS_SETUPTEST);
				break;
				case Key_FAST:
				break;
				case Key_LEFT:
				break;
				case Key_RIGHT:
				break;
				case Key_UP:
				break;
				case Key_DOWN:
				break;
				case Key_NUM1:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='1';
						
						
							
						
						key_count++;
							
					}
				break;
				case Key_NUM2:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='2';
						key_count++;
					}
				break;
				case Key_NUM3:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='3';
						key_count++;
					}
				break;
				case Key_NUM4:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='4';
						key_count++;
					}
					
				break;
				case Key_NUM5:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='5';
						key_count++;
					}
				break;
				case Key_NUM6:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='6';
						key_count++;
					}
				break;
				case Key_NUM7:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='7';
						key_count++;
					}
				break;
				case Key_NUM8:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='8';
						key_count++;
					}
				break;
				case Key_NUM9:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='9';
						key_count++;
					}
				break;
				case Key_NUM0:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='0';
						key_count++;
					}
				break;
				case Key_DOT:
					
					if(key_count<PASSWORD_LENTH&&key_count>0)
					{
						if(dot_num==0)
						{
							if(key_count>0)
							{
								Disp_buff[key_count]='.';
								dot_num1=key_count;
								key_count++;
							
							
							}
							dot_num++;
						}
					}
//					else 
//					{
//						if(Disp_buff[key_count]==0)
//							Disp_buff[key_count]='-';
//						else if(Disp_buff[key_count]=='-')
//							Disp_buff[key_count]='+';
//						else
//							Disp_buff[key_count]='-';
//						key_count++;
//							
//					
//					
//					
//					
//					}
				break;
				case Key_BACK:
					if(key_count>0)
					{	key_count--;
						Disp_buff[key_count]=' ';
						if(dot_num1==key_count)
						{
							dot_num=0;
							dot_num1=0;
							
						}
					
					}
					else
					{
						if(Disp_buff[key_count]==0)
								Disp_buff[key_count]='-';
							else if(Disp_buff[key_count]=='-')
								Disp_buff[key_count]='+';
							else
								Disp_buff[key_count]='-';
							key_count++;
					}
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
		if(dispflag)
		{
			dispflag=0;
            Colour.Fword=Red;
            LCD_DrawLine( Coordinates->xpos+13+80, Coordinates->ypos-2, 80, 0);
            LCD_DrawLine( Coordinates->xpos+13+80, Coordinates->ypos+18, 80, 0  );
            for(i=0;i<9;i++)
            LCD_DrawLine( Coordinates->xpos+13+i*10, Coordinates->ypos-2+20, 20, 1 );
            
			//LCD_DrawRect( Coordinates->xpos, Coordinates->ypos,Coordinates->xpos+Coordinates->lenth , Coordinates->ypos+16 , Red );
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BACK;
            WriteString_16(Coordinates->xpos-38, Coordinates->ypos, "密码:",  0);
			WriteString_16(Coordinates->xpos+13, Coordinates->ypos, ( uint8_t *)Disp_buff,  0);
            if(password_flag)
                WriteString_16(Coordinates->xpos, Coordinates->ypos+20, "密码错误",  0);
            else
               LCD_DrawFullRect( Coordinates->xpos, Coordinates->ypos+20,100 ,40);
               // WriteString_16(Coordinates->xpos, Coordinates->ypos, "         ",  0);
            WriteString_16(Coordinates->xpos, Coordinates->ypos-20, "请输入8位密码",  0);
			//dispflag=0;
		}
	
	}

}
void AD_ads2516(uint16_t i)
{
//     while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)==Bit_RESET);
//    I_ad=Read__Convert_read(0);
//    scan_I[i]=I_ad;
//    delay_1us(10);
    while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)==Bit_RESET);
    I_ad=Read__Convert_read(0);
    scan_I[i]=I_ad;
    //Select_V_I(0);
    while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)==Bit_RESET);
    V_ad=Read__Convert_read(1);
   // Select_V_I(1);
    scan_V[i]=V_ad;


}
void lvbo_jisu(void)
{
    if(V_ad>0x800000)
    {
        V_ad=V_ad-0xffffff;
        //V_ad=0-V_ad;
    }
    if(I_ad>0x800000)
    {
        I_ad=I_ad-0xffffff;
        //I_ad=0-I_ad;
        
    }


}
void lvbo_pingjun(void)
{
    uint16_t i;
    Ad_value=0;
    V_ad=0;
    I_ad=0;
    BubbleSort(scan_V, fit_allnum[Jk516save.Set_Data.speed]);
    for(i=fit_allnum[Jk516save.Set_Data.speed]/4;
    i<fit_allnum[Jk516save.Set_Data.speed]-fit_allnum[Jk516save.Set_Data.speed]/4;i++)
    {
        if(scan_V[i]>0x800000)
        {
            scan_V[i]=0xffffff-scan_V[i];
            V_ad-=scan_V[i];
        }
        else
        {
            V_ad+=scan_V[i];
        
        }
        
    }
    V_ad/=fit_allnum[Jk516save.Set_Data.speed]/2;
    BubbleSort(scan_I, fit_allnum[Jk516save.Set_Data.speed]);
    for(i=fit_allnum[Jk516save.Set_Data.speed]/4;
    i<fit_allnum[Jk516save.Set_Data.speed]-fit_allnum[Jk516save.Set_Data.speed]/4;i++)
    {
        if(scan_I[i]>0x800000)
        {
            scan_I[i]=0xffffff-scan_I[i];
            I_ad-=scan_I[i];
            
        }
        else
        {
            I_ad+=scan_I[i];
        
        }
        
    }
    I_ad/=fit_allnum[Jk516save.Set_Data.speed]/2;
                    
                   

}
void Soft_Turnon(void)
{
//	uint32_t i;

	
	Power_On_led();

	All_LedOff();
	Beep_Off();
    GPIO_SetBits(GPIOI,GPIO_Pin_6);
    delay_ms(1000);
    delay_ms(1000);
    EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);
	NVIC_EnableIRQ(EXTI3_IRQn);    
    
	while(GetSystemStatus()==SYS_STATUS_TOOL)
	{
		

	
	
	}
	NVIC_DisableIRQ(EXTI3_IRQn);
	Power_Off_led();

}
void read_adI_4(void)
{
    u8 i;
//    u32 temp;
    I_ad=0;
    for(i=0;i<50;i++)
    {
        while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)==Bit_RESET);
       scan_I[i]= Read_Convert_read();
        if(i>2)
        if(scan_I[i]>0x800000)
        {
            scan_I[i]=0xffffff-scan_I[i];
            I_ad-=scan_I[i];
        }
        else
            I_ad+=scan_I[i];
            
        
    }
    I_ad/=47;
    //Select_V_I(0);
}
void read_adV_4(void)
{
    u8 i;
//    u32 temp;
    V_ad=0;
    delay_1us(20);
    for(i=0;i<10;i++)
    {
        while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)==Bit_RESET);
       scan_V[i]= Read_Convert_read();
        if(i>2)
        if(scan_V[i]>0x800000)
        {
            scan_V[i]=0xffffff-scan_V[i];
            V_ad-=scan_V[i];
        }
        else
            V_ad+=scan_V[i];
    }
    V_ad/=7;
    //Select_V_I(1);

}
void read_adI_3(void)
{
//    u8 i;
////    u32 temp;
//    I_ad=0;
//    for(i=0;i<100;i++)
//    {
//        while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)==Bit_RESET);
//       scan_I[i]= Read_Convert_read();
//        if(i>2)
//        if(scan_I[i]>0x800000)
//        {
//            scan_I[i]=0xffffff-scan_I[i];
//            I_ad-=scan_I[i];
//        }
//        else
//            I_ad+=scan_I[i];
//          
//        
//    }
//    I_ad/=98;
    //Select_V_I(0);
		u16 i;
//    u32 temp;
    I_ad=0;
    for(i=0;i<580;i++)
    {
        while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)==Bit_RESET);
       scan_I[i]= Read_Convert_read();
        if(i>550)
        if(scan_I[i]>0x800000)
        {
            scan_I[i]=0xffffff-scan_I[i];
            I_ad-=scan_I[i];
        }
        else
            I_ad+=scan_I[i];
          
        
    }
    I_ad/=29;
}
void read_adV_3(void)
{
    u8 i;
//    u32 temp;
    V_ad=0;
    delay_1us(20);
    for(i=0;i<50;i++)
    {
        while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)==Bit_RESET);
       scan_V[i]= Read_Convert_read();
        if(i>2)
        if(scan_V[i]>0x800000)
        {
            scan_V[i]=0xffffff-scan_V[i];
            V_ad-=scan_V[i];
        }
        else
            V_ad+=scan_V[i];
       
    }
    V_ad/=48;
    //Select_V_I(1);
}
void read_adI_2(void)
{
    u8 i,j;
//    u32 temp;
    u32 buff[5];
    I_ad=0;
    for(j=0;j<5;j++)
    {
        for(i=0;i<100;i++)
        {
            while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)==Bit_RESET);
                scan_I[i]= Read_Convert_read();
            if(i>2)
            if(scan_I[i]>0x800000)
            {
                scan_I[i]=0xffffff-scan_I[i];
                I_ad-=scan_I[i];
            }
            else
                I_ad+=scan_I[i];
               
            
        }
        I_ad/=98;
        buff[j]=I_ad;
        
    }
    I_ad=0;
    for(i=1;i<5;i++)
    {
        I_ad+=buff[i];
    
    
    }
    I_ad/=4;
    //Select_V_I(0);
}
void read_adV_2(void)
{
    u8 i,j;
//    u32 temp;
    float buff[5];
    V_ad=0;
    delay_1us(20);
    for(j=0;j<5;j++)
    {
        for(i=0;i<100;i++)
        {
            while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)==Bit_RESET);
           scan_V[i]= Read_Convert_read();
            if(i>2)
            if(scan_V[i]>0x800000)
            {
                scan_V[i]=0xffffff-scan_V[i];
                V_ad-=scan_V[i];
            }
            else
                V_ad+=scan_V[i];
        
            
        }
        V_ad/=98;
        buff[j]=V_ad;
    }
    V_ad=0;
    for(i=0;i<5;i++)
    {
        V_ad+=buff[i];
    
    
    }
    V_ad/=5;
    //Select_V_I(1);
}
void read_adI_1(void)
{
    u8 i,j;
//    u32 temp;
    float buff[10];
    I_ad=0;
    for(j=0;j<10;j++)
    {
        for(i=0;i<100;i++)
        {
            while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)==Bit_RESET);
                scan_I[i]= Read_Convert_read();
            if(i>2)
            if(scan_I[i]>0x800000)
            {
                scan_I[i]=0xffffff-scan_I[i];
                I_ad-=scan_I[i];
            }
            else
                I_ad+=scan_I[i];
               
            
        }
        I_ad/=98;
        
        buff[j]=I_ad;
        
    }
    I_ad=0;
    for(i=1;i<10;i++)
    {
        I_ad+=buff[i];
    
    
    }
    I_ad/=9;
    
}
void read_adV_1(void)
{
    u8 i,j;
//    u32 temp;
    float buff[10];
    V_ad=0;
    delay_1us(20);
    for(j=0;j<10;j++)
    {
        for(i=0;i<100;i++)
        {
            while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)==Bit_RESET);
           scan_V[i]= Read_Convert_read();
            if(i>2)
            if(scan_V[i]>0x800000)
            {
                scan_V[i]=0xffffff-scan_V[i];
                V_ad-=scan_V[i];
            }
            else
                V_ad+=scan_V[i];
            
        }
        V_ad/=98;
        buff[j]=V_ad;
    }
    V_ad=0;
    for(i=0;i<10;i++)
    {
        V_ad+=buff[i];
    
    
    }
    V_ad/=10;
    
}

//(x-100)/0.385  Pt100的计算公式  X是电阻值
/********************************
函数名称: void DispF(float f)
功能 ：用科学记数法显示浮点数，在float全范围内精确显示，超出范围给出提示。
说明 ：浮点数表示范围为+-1.175494E-38到+-3.402823E+38
入口参数：f为要显示的浮点数
返回值 ：无
********************************/

//（2）显示十进制尾数和阶的子函数。
/********************************
函数名称: void PrintW(unsigned long w,char j)
功能 ：科学记数法，显示十进制尾数和阶码
入口参数：w为尾数，j为阶码
返回值 ：无
********************************/

//==========================================================
//END
//==========================================================




