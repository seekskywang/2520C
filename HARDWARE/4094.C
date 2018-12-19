#include "pbdata.h"

u8 _4094_databuff[_4094NUM];

void HC595_Send(u8 *buff)
{
    u8 i;
    u8 j;
    //_SPI_595_SHCPL;
	for(j=0;j<HC595HC;j++)
	{
		for(i=0;i<8;i++)     //д��λ���ݽ�CD4094;
		{
			if(*(buff+j)&0x80)     //�жϲ�����������;
			{
				_SPI_595_DSH;      //���ߵ�ƽ;
			}
			else
			{
				_SPI_595_DSL;      //���͵�ƽ;
			}
			
			_SPI_595_SHCPL;       //�γ�����������;
			delay_us(5);
			_SPI_595_SHCPH;
			*(buff+j)<<=1;      //������λ,����һλ����;
			
		}
        
	}
    _SPI_595_CSH;
    _SPI_595_STCPL;       //�γ�����������;
    delay_us(5);
    _SPI_595_STCPH;
    
    _SPI_595_CSL;
    delay_ms(30);


}
void Out_4094(u8 *buff) //дCD4094�Ӻ���;
{
	u8 i;       //�����Ӻ���;
	u8 j;
//   _SPI_4094_STROH;
	//_4094_STBL;       //����Ȼ����״̬����;
//	_SPI_4094_SCLKH;
//    delay_us(20);
	for(j=0;j<_4094NUM;j++)
	{
		for(i=0;i<8;i++)     //д��λ���ݽ�CD4094;
		{

			if(*(buff+j)&0x80)     //�жϲ�����������;
			{
				_SPI_4094_DATAH;      //���ߵ�ƽ;
			}
			else
			{
				_SPI_4094_DATAL;      //���͵�ƽ;
			}
			
			_SPI_4094_SCLKL;       //�γ�����������;
			delay_us(10);
			_SPI_4094_SCLKH;
			*(buff+j)<<=1;      //������λ,����һλ����;
            delay_us(10);
			
		}
	}
//	_SPI_4094_STROL;       //��������;������ʾ;
//	delay_us(1);
//	_SPI_4094_STROL; 
//    delay_us(100);
//    _SPI_4094_SCLKL;
//     delay_us(100);
//    _SPI_4094_STROH; 
//     delay_us(100);
////    _SPI_4094_SCLKH;
////    
////    delay_us(50);
//    _SPI_4094_STROL; 
//     delay_us(100);
//    _SPI_4094_STROH; 
}