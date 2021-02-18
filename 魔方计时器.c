#include<reg52.h>
sbit ADDR3=P1^3;
sbit ENLED=P1^4;
sbit KEY1=P2^4;
sbit KEY2=P2^5;
sbit KEY3=P2^6;
sbit KEY4=P2^7;
unsigned char code LedChar[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,
0x82,0xF8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E};
unsigned char LedBuff[6]={
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};
unsigned char KeySta[4]={
1,1,1,1};
unsigned char KS=1;
unsigned char T0RH=0;
unsigned char T0RL=0;
bit StopwatchRefresh=1;
bit StopwatchRunning=0;
unsigned char DecimalPart=0;
unsigned int IntegerPart=0;
unsigned char Dresult;
unsigned int Iresult;
unsigned char number=0;
void ConfigTimer0(unsigned int ms);
void StopwatchDisplay(unsigned char D,unsigned int I);
void KeyDriver();
void MagicCube();
void main(){
	EA=1;
	ENLED=0;
	ADDR3=1;
	P2=0xFE;
	ConfigTimer0(2);
	while(1)
	{
		if(StopwatchRefresh)
		{
			StopwatchRefresh=0;
			StopwatchDisplay(DecimalPart,IntegerPart);
		}
		KeyDriver();
	}
}
void StopwatchAction()
{
	if(StopwatchRunning)
	{
		StopwatchRunning=0;
	}
	else
	{
		StopwatchRunning=1;
	}
}
void StopwatchReset()
{
	
	StopwatchRunning=0;
	DecimalPart=0;
	IntegerPart=0;
	StopwatchRefresh=1;
}
void SaveResult()
{
	Dresult+=DecimalPart;
	Iresult+=IntegerPart;
	number++;
} 
void MagicCube()
{
	StopwatchDisplay(Dresult/number,Iresult/number);
	StopwatchRunning=0;
}
void KeyDriver()
{
	unsigned char i;
	static unsigned char backup[4]={1,1,1,1};
	for(i=0;i<4;i++)
	{
		if(backup[i]!=KeySta[i])
		{
			if(backup[i]!=0)
			{
				if(i==1) StopwatchReset();//esc
				else if(i==2) StopwatchAction();//??
				else if(i==3) SaveResult(); 
				else if(i==0) MagicCube();
			}
			backup[i]=KeySta[i];
		}
	}
}
void ConfigTimer0(unsigned int ms)
{
	unsigned long tmp;
	tmp=11059200/12;
	tmp=(tmp*ms)/1000;//???????
	tmp=65536-tmp;
	tmp+=18;
	T0RH=(unsigned char)(tmp>>8);
	T0RL=(unsigned char)(tmp);
	TMOD&=0xF0;
	TMOD|=0x01;
	TH0=T0RH;
	TL0=T0RL;
	ET0=1;
	TR0=1;
}
void StopwatchDisplay(unsigned char D,unsigned int I)
{
	signed char i;
	unsigned char buf[4];
	LedBuff[0]=LedChar[D%10];
	LedBuff[1]=LedChar[D/10];
	buf[0]=I%10;
	buf[1]=I/10%10;
	buf[2]=I/100%10;
	buf[3]=I/1000%10;
	for(i=3;i>=1;i--)//???????
	{
		if(buf[i]==0)
		{
			LedBuff[i+2]=0xFF;
		}
		else
		{
			break;
		}
		for(;i>=0;i--)
		{
			LedBuff[i+2]=LedChar[buf[i]];
		}
		LedBuff[2]&=0x7F;
		
	}
}
void StopwatchCount()
{
	if(StopwatchRunning)
	{
		DecimalPart++;
		if(DecimalPart>=100)
		{
			DecimalPart=0;
			IntegerPart++;
			if(IntegerPart>=10000)
			{
				IntegerPart=0;
			}
		}
		StopwatchRefresh=1;
	}
	
}
void LedScan()
{
	static unsigned char i =0 ;
	P0=0xFF;
	P1=(P1&0xF8)|i;
	P0=LedBuff[i];
	if(i<5)
	{
		i++;
	}
	else{
		i=0;	
	}
	
}
void KeyScan()
{
	signed char i=0;
	static unsigned char keybuf[4]={
	0xFF,0xFF,0xFF,0xFF
	};
	keybuf[0]=(keybuf[0]<<1)|KEY1;
	keybuf[1]=(keybuf[1]<<1)|KEY2;
	keybuf[2]=(keybuf[2]<<1)|KEY3;
	keybuf[3]=(keybuf[3]<<1)|KEY4;
	for(i=0;i<4;++i)
	{
		if(keybuf[i]==0x00)
			KeySta[i]=0;
		else if(keybuf[i]==0x0F)
			KeySta[i]=1;
	}
}
void InterruptTimer0() interrupt 1
{
	static unsigned char tmr10ms=0;
	TH0=T0RH;
	TL0=T0RL;
	LedScan();
	KeyScan();
	tmr10ms++;
	if(tmr10ms>=5)
	{
		tmr10ms=0;
		StopwatchCount();
	}
}
