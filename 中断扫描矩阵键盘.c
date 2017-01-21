#include <reg52.h>
#define uchar unsigned char

sbit CtrA =	P2^2;  //数码管位选控制
sbit CtrB =	P2^3;
sbit CtrC =	P2^4;

uchar code Chars[] = {  //数码管字符“0”~“F”
  0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
  0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71
};

uchar KeySta = 1;  //按键状态变量，1为未按下，0为按下，使用1ms的定时器中断改变状态值

void main()
{
	bit backup = 1, errorA = 0, errorB = 0;  //按键是否弹起变量，列检测错误变量，行检测错误变量
	uchar KeyVal = 0, oldKeyVal;  //按键处于按下状态时扫描的结果与错误结果
	
	CtrA = 0;  //数码管位选
	CtrB = 0;
	CtrC = 0;
	
	TMOD = 0x01;  //打开定时器与定时器中断
	EA = 1;
	ET0 = 1;
	TH0 = 0xF8;
	TL0 = 0x30;
	TR0 = 1;
	
	P0 = Chars[KeyVal];  //初始化数码管值为“0”
	P1 = 0xF0;           //*初始化按键IO口电平，以便进行矩阵扫描
	                     //*因为中断扫描需要，所以不能省略
	while(1)
	{
		if(KeySta != backup)  //*按键状态与弹起状态不同时进入判断
		{                     //*(按键从弹起变为按下、按键从按下变为弹起都会进入)
			if(!backup) //当按键一直处于按下状态(0)时不会进入，只有按键弹起的时候才进入
			{
				P0 = Chars[KeyVal];  //按键弹起瞬间更新数码管
			}
			else  //当按键处于按下状态(backup=1)时进入，并扫描数码管应显示的值
			{
				oldKeyVal = KeyVal;  //将当前的显示值备份，以便error发生时使用
				
				P1 = 0x0F;  //列扫描，扫描第几列被按下，多按键按下时error置1
				switch(P1)
				{
					case(0x07): KeyVal = 0; break;
					case(0x0B): KeyVal = 1; break;
					case(0x0D): KeyVal = 2; break;
					case(0x0E): KeyVal = 3; break;
					default: errorA = 1; break;
				}
				
				P1 = 0xF0;  //行扫描，扫描第几行被按下，多按键按下时error置1
				switch(P1)
				{
					case(0x70): KeyVal += 0; break;
					case(0xB0): KeyVal += 4; break;
					case(0xD0): KeyVal += 8; break;
					case(0xE0): KeyVal += 12; break;
					default: errorB = 1; break;
				}
				
				if(errorA | errorB)  //判断是否有多个按键被按下，若发生则显示原值
				{
					errorA = errorB = 0;  //重置errorA和errorB
					KeyVal = oldKeyVal;
				}
			}

			backup = KeySta;  //将按键弹起状态与按键当前状态同步
		}
	}
}

void InterruptTimer0() interrupt 1
{
	static uchar KeyBuf = 0xFF;  //* 111111111111110101010000000000000010101011111111111111
	                             //* 若连续8位的值都相同则状态稳定，并根据这8个相同的值判断状态
	TH0 = 0xF8;  //重置定时器高、低位
	TL0 = 0x30;
	
	KeyBuf = KeyBuf << 1 | 0xF0 == P1;  //将KeyBuf左推一位，并将按键当前状态置位最低位
	if(KeyBuf == 0x00)  //若8位稳定为0，则判断按键为按下状态
	{
		KeySta = 0;
	}
	else if(KeyBuf == 0xFF)  //若8位稳定为1，则判断按键为弹起状态
	{
		KeySta = 1;
	}
}