#include <reg52.h>

sbit CtrA = P2^2;
sbit CtrB = P2^3;
sbit CtrC = P2^4;

unsigned int cnt = 0;
unsigned char i = 0;

unsigned char code LedChar[] = {
	0x3F, 0x06, 0x5B, 0x4F, 0x66,
	0x6D, 0x7D, 0x07, 0x7F, 0x6F
};
unsigned char LEDstatus[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
void main()
{
	unsigned long sec = 0;
	
	TMOD = 0x01;  //定时器工作方式为1
	TH0 = 0xFC;
	TL0 = 0x00;
	TR0 = 1;  //打开T0计时器
	EA = 1;   //打开总中断
	ET0 = 1;  //打开定时器T0中断
	
	while(1)
	{
		if(cnt >= 1000)
		{
			cnt = 0;
			sec++;
			LEDstatus[0] = LedChar[sec%10];
			LEDstatus[1] = LedChar[sec/10%10];
			LEDstatus[2] = LedChar[sec/100%10];
			LEDstatus[3] = LedChar[sec/1000%10];
			LEDstatus[4] = LedChar[sec/10000%10];
			LEDstatus[5] = LedChar[sec/100000%10];
			LEDstatus[6] = LedChar[sec/1000000%10];
			LEDstatus[7] = LedChar[sec/10000000%10];
		}
	}
}

void InterruptTime()	interrupt 1 //1ms时间到了的中断程序
{
	TH0 = 0xFC;
	TL0 = 0x00;
	cnt++;
	P0 = 0x00; //消除鬼影
	switch (i)
	{
		case 0: CtrC = 0; CtrB = 0; CtrA = 0; i++; P0 = LEDstatus[0]; break;
		case 1: CtrC = 0; CtrB = 0; CtrA = 1; i++; P0 = LEDstatus[1]; break;
		case 2: CtrC = 0; CtrB = 1; CtrA = 0; i++; P0 = LEDstatus[2]; break;
		case 3: CtrC = 0; CtrB = 1; CtrA = 1; i++; P0 = LEDstatus[3]; break;
		case 4: CtrC = 1; CtrB = 0; CtrA = 0; i++; P0 = LEDstatus[4]; break;
		case 5: CtrC = 1; CtrB = 0; CtrA = 1; i++; P0 = LEDstatus[5]; break;
		case 6: CtrC = 1; CtrB = 1; CtrA = 0; i++; P0 = LEDstatus[6]; break;
		case 7: CtrC = 1; CtrB = 1; CtrA = 1; i=0; P0 = LEDstatus[7]; break;
		default: break;
	}
}