#include <reg52.h>
#define uchar unsigned char

sbit CtrA =	P2^2;  //�����λѡ����
sbit CtrB =	P2^3;
sbit CtrC =	P2^4;

uchar code Chars[] = {  //������ַ���0��~��F��
  0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
  0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71
};

uchar KeySta = 1;  //����״̬������1Ϊδ���£�0Ϊ���£�ʹ��1ms�Ķ�ʱ���жϸı�״ֵ̬

void main()
{
	bit backup = 1, errorA = 0, errorB = 0;  //�����Ƿ���������м�����������м��������
	uchar KeyVal = 0, oldKeyVal;  //�������ڰ���״̬ʱɨ��Ľ���������
	
	CtrA = 0;  //�����λѡ
	CtrB = 0;
	CtrC = 0;
	
	TMOD = 0x01;  //�򿪶�ʱ���붨ʱ���ж�
	EA = 1;
	ET0 = 1;
	TH0 = 0xF8;
	TL0 = 0x30;
	TR0 = 1;
	
	P0 = Chars[KeyVal];  //��ʼ�������ֵΪ��0��
	P1 = 0xF0;           //*��ʼ������IO�ڵ�ƽ���Ա���о���ɨ��
	                     //*��Ϊ�ж�ɨ����Ҫ�����Բ���ʡ��
	while(1)
	{
		if(KeySta != backup)  //*����״̬�뵯��״̬��ͬʱ�����ж�
		{                     //*(�����ӵ����Ϊ���¡������Ӱ��±�Ϊ���𶼻����)
			if(!backup) //������һֱ���ڰ���״̬(0)ʱ������룬ֻ�а��������ʱ��Ž���
			{
				P0 = Chars[KeyVal];  //��������˲����������
			}
			else  //���������ڰ���״̬(backup=1)ʱ���룬��ɨ�������Ӧ��ʾ��ֵ
			{
				oldKeyVal = KeyVal;  //����ǰ����ʾֵ���ݣ��Ա�error����ʱʹ��
				
				P1 = 0x0F;  //��ɨ�裬ɨ��ڼ��б����£��ఴ������ʱerror��1
				switch(P1)
				{
					case(0x07): KeyVal = 0; break;
					case(0x0B): KeyVal = 1; break;
					case(0x0D): KeyVal = 2; break;
					case(0x0E): KeyVal = 3; break;
					default: errorA = 1; break;
				}
				
				P1 = 0xF0;  //��ɨ�裬ɨ��ڼ��б����£��ఴ������ʱerror��1
				switch(P1)
				{
					case(0x70): KeyVal += 0; break;
					case(0xB0): KeyVal += 4; break;
					case(0xD0): KeyVal += 8; break;
					case(0xE0): KeyVal += 12; break;
					default: errorB = 1; break;
				}
				
				if(errorA | errorB)  //�ж��Ƿ��ж�����������£�����������ʾԭֵ
				{
					errorA = errorB = 0;  //����errorA��errorB
					KeyVal = oldKeyVal;
				}
			}

			backup = KeySta;  //����������״̬�밴����ǰ״̬ͬ��
		}
	}
}

void InterruptTimer0() interrupt 1
{
	static uchar KeyBuf = 0xFF;  //* 111111111111110101010000000000000010101011111111111111
	                             //* ������8λ��ֵ����ͬ��״̬�ȶ�����������8����ͬ��ֵ�ж�״̬
	TH0 = 0xF8;  //���ö�ʱ���ߡ���λ
	TL0 = 0x30;
	
	KeyBuf = KeyBuf << 1 | 0xF0 == P1;  //��KeyBuf����һλ������������ǰ״̬��λ���λ
	if(KeyBuf == 0x00)  //��8λ�ȶ�Ϊ0�����жϰ���Ϊ����״̬
	{
		KeySta = 0;
	}
	else if(KeyBuf == 0xFF)  //��8λ�ȶ�Ϊ1�����жϰ���Ϊ����״̬
	{
		KeySta = 1;
	}
}