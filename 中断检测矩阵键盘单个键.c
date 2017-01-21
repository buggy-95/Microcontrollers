#include <reg52.h>

sbit KEY1 = P1^3;
sbit KEY2 = P1^2;
sbit KEY3 = P1^1;
sbit KEY4 = P1^7;


sbit CtrA =	P2^2;
sbit CtrB =	P2^3;
sbit CtrC =	P2^4;

unsigned char code Chars[] = {
  0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
  0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71
};
bit KeySta = 1;

void main()
{
	bit backup = 1;
	unsigned char cnt = 0;
	
	CtrA = 0;
	CtrB = 0;
	CtrC = 0;
	
	TMOD = 0x01;
	EA = 1;
	ET0 = 1;
	TH0 = 0xF8;
	TL0 = 0x30;
	TR0 = 1;
	
	P1 = 0xfe;
	P0 = Chars[cnt];
	
	while(1)
	{
		if(KeySta != backup)
		{
			if(backup == 0)
			{
				cnt++;
				if(cnt >= 10)
				{
					cnt = 0;
				}
				P0 = Chars[cnt];
			}
			backup = KeySta;
		}
	}

	

}

void InterruptTimer0() interrupt 1
{
	static unsigned char keybuf = 0xFF;
	
	TH0 = 0xF8;
	TL0 = 0x30;
	
	keybuf = keybuf << 1 | KEY4;
	if(keybuf == 0x00)
	{
		KeySta = 0;
	}
	else if(keybuf == 0xFF)
	{
		KeySta = 1;
	}
	else
	{
	}
}