/*******************************************************************************  
* 文件名称：按键控制实验
* 实验目的：1.掌握单片机IO口操作的基本方法
*           2.掌握C51关键字"sbit"的用法
*           2.掌握单片机常见外围驱动电路的设计方法 
* 程序说明：使用程序前，将跳线J2调整为BTN模式
* 硬件环境：IAP15F2K61S2@11.0592MHz
* 日期版本：2012-9-3/V1.0a 
*******************************************************************************/

#include "reg52.h"  //定义51单片机特殊功能寄存器
#include "absacc.h"
#define u8 unsigned char
#define u16 unsigned short int

/** 独立按键接口 */
sfr P4   = 0xC0;
sbit P42 = P4^2;
sbit P44 = P4^4;
sbit buzz  = P0^6;	//端口P06控制蜂鸣器，这是通过查看原理图得出来的
sbit motor  = P0^5;
bit key_re = 0;
bit add_flag = 0;        	//自加1标志位
bit subtract_flag = 0;	 	//自减1标志位
bit naozhong_flag = 0;				 	//闹钟开启标志位
bit update_nz = 1; 						//更新闹钟初始值的标志位，是闹钟初始值，不要搞错，也就是设置闹钟时所显示的初始值
bit LED_nz = 0;						//当前时间等于闹钟时间时，点亮LED的标志位
bit buffer_1 = 0;					//按2调整闹钟时间，再按0确定闹钟时间，且按0可以让正常时钟继续运行并显示，此时buffer_1变量会变成1，也就是说按2调整闹钟后再按0的情况下，buffer_1变量会变成1
													//buffer_1变量也是确定闹钟时间的标志位
bit buffer_2 = 0;    			//buffer_1和buffer_2是配合使用的
sfr AUXR = 0x8e;
unsigned char bh = 0;
unsigned char Time[]={1,7,22,2,0,22,5,0}; //时间
unsigned char Time_shanshuo[]={0,0,22,0,0,22,0,0}; //用来闪烁的时间
unsigned char Time_naozhong[]={0,0,22,0,0,22,0,0}; //设置闹钟的时间，变量Time_naozhong更新之后会赋值给Time_nz_buffer，再由Time_nz_buffer赋值给Time_nz_finish
																										//这里将变量Time_nz_buffer作为缓冲区的原因是变量Time_naozhong不稳定，有两个地方都对它进行了修改，因此用另一个变量缓存一下
unsigned char Time_nz_buffer[]={0,0,22,0,0,22,0,0}; //缓冲区
unsigned char Time_nz_finish[]={0,0,22,0,0,22,0,0}; //闹钟设置完成的时间
unsigned char Time_d[]={0,0,22,0,0,22,0,0};	//倒计时时间
unsigned char Time_d_finish[]={0,0,22,0,0,22,0,0};	//倒计时功能下设置的时间
unsigned char counter_num = 0;
unsigned char counter_num_2 = 0;
short int counter_num_3 = 0;
unsigned char cnt_d = 0; //倒计时功能中的计数器
unsigned char key_value = 0;
unsigned char key_press = 0;
unsigned char shi = 0;
unsigned char fen = 0;
unsigned char miao = 0;
unsigned char shi_n = 0;
unsigned char fen_n = 0;
unsigned char miao_n = 0;
unsigned char hour_d = 0;
unsigned char minute_d = 0;
unsigned char second_d = 0;
unsigned char gongneng = 0; //功能标志位
unsigned char ss_xh = 2; //闪烁位，0代表小时闪烁，1代表分钟闪烁，2代表秒表闪烁，3代表不闪烁
unsigned char nz_xh = 2;	//设置闹钟功能下的时间闪烁位
unsigned char d_xh = 2;	//设置倒计时功能下的时间闪烁位
bit ss_diaplay_YN = 0; //闪烁状态下是显示还是熄灭的标志位
void Timer0_Init(void); //定时器0初始化函数
void Timer0_Isr(void);	//定时器0中断函数
void display(unsigned char* ptr);		//显示数码管函数
void cls_buzz(void); //关闭外设蜂鸣器
void Time_proc(void); //时间处理函数
unsigned char read_keyboard(void); //读取矩阵键盘
void time_add(u8 number, u8* hour, u8* minute, u8* second);
void time_subtract(u8 number, u8* hour, u8* minute, u8* second);
void Time_proc_d(u8* hour, u8* minute, u8* second);		//倒计时功能中的时间处理函数，也就是每隔1s减1

//共阳极数码管        0     1    2    3    4    5    6    7    8    9    A    B    C    D    E    F    P    U    Y    L   全亮 熄灭  -
unsigned char smg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e,0x8c,0xc1,0x91,0x7c,0x00,0xff,0xbf};


//主函数
void main(void)
{    
	shi = Time[0]*10+Time[1];
	fen = Time[3]*10+Time[4];
	miao = Time[6]*10+Time[7];
	cls_buzz();
	Timer0_Init();
	EA = 1;
	
	
	
    while(1)
    {
			if(gongneng == 0) //正常运行//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			{
				display(Time);
			}
			if(gongneng == 1)  //调整时间//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			{
				if(ss_diaplay_YN) 						//闪烁情况下有亮和灭两种状态，ss_diaplay_YN等于1时表示“亮”
				{
					Time_shanshuo[0] = Time[0];	Time_shanshuo[1] = Time[1];
					Time_shanshuo[3] = Time[3];	Time_shanshuo[4] = Time[4];
					Time_shanshuo[6] = Time[6];	Time_shanshuo[7] = Time[7];
				}
				else													//闪烁情况下有亮和灭两种状态，ss_diaplay_YN等于0时表示“灭”
				{
					switch(ss_xh)
					{
						case 0:Time_shanshuo[0] = 21;	Time_shanshuo[1] = 21; break;	//数组smg的第21个元素表示熄灭，ss_xh == 0代表“小时”数字闪烁，ss_xh是shanshuo_xuhao的简写
						case 1:Time_shanshuo[3] = 21; Time_shanshuo[4] = 21; break;	//“分钟”数字闪烁
						case 2:Time_shanshuo[6] = 21; Time_shanshuo[7] = 21; break;	//“秒表”数字闪烁
					}
				}
				display(Time_shanshuo);
				
				if(add_flag == 1) 											//时间的某一位自加1
				{
					add_flag = 0;
					time_add(ss_xh, &shi, &fen, &miao);
				}
				if(subtract_flag == 1) 											//时间的某一位自减1
				{
					subtract_flag = 0;
					time_subtract(ss_xh, &shi, &fen, &miao);
				}
				shi>=10?(Time[0]=shi/10):(Time[0]=0);
				Time[1]=shi%10;
				fen>=10?(Time[3]=fen/10):(Time[3]=0);
				Time[4]=fen%10;
				miao>=10?(Time[6]=miao/10):(Time[6]=0);
				Time[7]=miao%10;
				
			}
			if(gongneng == 2) //闹钟功能//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			{
				u8 cnt1 = 0;
				if(add_flag == 1) 											//时间的某一位自加1
				{
					add_flag = 0;
					time_add(nz_xh, &shi_n, &fen_n, &miao_n);
				}
				if(subtract_flag == 1) 											//时间的某一位自减1
				{
					subtract_flag = 0;
					time_subtract(nz_xh, &shi_n, &fen_n, &miao_n);
				}
				shi_n>=10?(Time_naozhong[0]=shi_n/10):(Time_naozhong[0]=0);
				Time_naozhong[1]=shi_n%10;
				fen_n>=10?(Time_naozhong[3]=fen_n/10):(Time_naozhong[3]=0);
				Time_naozhong[4]=fen_n%10;
				miao_n>=10?(Time_naozhong[6]=miao_n/10):(Time_naozhong[6]=0);
				Time_naozhong[7]=miao_n%10;
				
				for(cnt1 = 0; cnt1<8; cnt1++)
					Time_nz_buffer[cnt1] = Time_naozhong[cnt1];
				
				if(~ss_diaplay_YN)													//闪烁情况下有亮和灭两种状态，ss_diaplay_YN等于0时表示“灭”
				{
					switch(nz_xh)
					{
						case 0:Time_naozhong[0] = 21;	Time_naozhong[1] = 21; break;	//数组smg的第21个元素表示熄灭，ss_xh == 0代表“小时”数字闪烁，ss_xh是shanshuo_xuhao的简写
						case 1:Time_naozhong[3] = 21; Time_naozhong[4] = 21; break;	//“分钟”数字闪烁
						case 2:Time_naozhong[6] = 21; Time_naozhong[7] = 21; break;	//“秒表”数字闪烁
					}
				}
				display(Time_naozhong);
			}
			if(gongneng == 3) //倒计时设置功能
			{
				u8 cnt1 = 0;
				if(add_flag == 1) 											//时间的某一位自加1
				{
					add_flag = 0;
					time_add(d_xh, &hour_d, &minute_d, &second_d);
				}
				if(subtract_flag == 1) 											//时间的某一位自减1
				{
					subtract_flag = 0;
					time_subtract(d_xh, &hour_d, &minute_d, &second_d);
				}
				hour_d>=10?(Time_d[0]=hour_d/10):(Time_d[0]=0);
				Time_d[1]=hour_d%10;
				minute_d>=10?(Time_d[3]=minute_d/10):(Time_d[3]=0);
				Time_d[4]=minute_d%10;
				second_d>=10?(Time_d[6]=second_d/10):(Time_d[6]=0);
				Time_d[7]=second_d%10;
				
				for(cnt1 = 0; cnt1<8; cnt1++)
					Time_d_finish[cnt1] = Time_d[cnt1];
				
				if(~ss_diaplay_YN)													//闪烁情况下有亮和灭两种状态，ss_diaplay_YN等于0时表示“灭”
				{
					switch(d_xh)
					{
						case 0:Time_d[0] = 21; Time_d[1] = 21; break;	//数组smg的第21个元素表示熄灭，ss_xh == 0代表“小时”数字闪烁，ss_xh是shanshuo_xuhao的简写
						case 1:Time_d[3] = 21; Time_d[4] = 21; break;	//“分钟”数字闪烁
						case 2:Time_d[6] = 21; Time_d[7] = 21; break;	//“秒表”数字闪烁
					}
				}
				display(Time_d);
			}
			if(gongneng == 7) //倒计时开始功能
			{
				display(Time_d_finish);
			}
    }
}

void Timer0_Isr(void) interrupt 1 //每10ms中断一次
{
	unsigned char key_temp = read_keyboard();	//每10ms扫描一次键盘
	if(key_temp == 0) //按0代表功能0，也就是时钟正常运行功能
	{
		gongneng = 0;
		update_nz = 1;
		if(buffer_2)
		{
			unsigned char buffer_3 = 0;   //局部变量的声明必须在语句体的开头
			buffer_2 = 0;
			buffer_1 = 1;		//通过buffer_1和buffer_2来确定闹钟时间是否设置完成，换句话说，想要将闹钟时间完全确定下来必须先按2再按0，在按2和按0之间可以通过按5和9调整闹钟时间
			for(buffer_3 = 0;buffer_3<8; buffer_3++)
				Time_nz_finish[buffer_3] = Time_nz_buffer[buffer_3];
		}
	}
	if(key_temp == 1)	//按1代表功能1，也就是调整时间功能
	{
		unsigned char buffer = 0;
		for(buffer = 0;buffer<8; buffer++)
			Time_shanshuo[buffer] = Time[buffer];
		gongneng = 1;
		switch(ss_xh)
		{
			case 0:ss_xh ++;break;
			case 1:ss_xh ++;break;  //0或1的情况下均执行自加1的程序
			case 2:ss_xh = 0;break;	//ss_xh为2的情况下被赋值0，也就是说，变量ss_xh的变化过程为0、1、2、0、1、2、0、1、2
		}
	}
	if(key_temp == 2) //按2代表闹钟功能
	{
		buffer_2 = 1;
		gongneng = 2;
		if (update_nz)
		{
			unsigned char buffer = 0;
			for(buffer = 0;buffer<8; buffer++)
				Time_naozhong[buffer] = Time[buffer];  //闹钟初始化时间，每次调整闹钟时间时只初始化一次
			update_nz = 0;
			shi_n = shi;
			fen_n = fen;
			miao_n = miao;
		}
		switch(nz_xh)			//调整数码管闪烁的位置
		{
			case 0:nz_xh ++;break;	//这行代码执行之前是第0个数码管，自加1之后显示保险
			case 1:nz_xh ++;break;  //0或1的情况下均执行自加1的程序
			case 2:nz_xh = 0;break;	//ss_xh为2的情况下被赋值0，也就是说，变量ss_xh的变化过程为0、1、2、0、1、2、0、1、2
		}
	}
	if(key_temp == 3) //按3代表倒计时功能
	{
		gongneng = 3;
		switch(d_xh)
		{
			case 0:d_xh ++;break;
			case 1:d_xh ++;break;  //0或1的情况下均执行自加1的程序
			case 2:d_xh = 0;break;	//ss_xh为2的情况下被赋值0，也就是说，变量ss_xh的变化过程为0、1、2、0、1、2、0、1、2
		}
	}
	if(key_temp == 5)	//自加1功能
		add_flag = 1;
	if(key_temp == 9)	//自减1功能
		subtract_flag = 1;
	if(key_temp == 7)	//倒计时开始功能
		gongneng = 7;
	
	
	if (gongneng == 0)					//每间隔1000ms执行一次秒表自加1的程序，也就是Time_proc函数
	{
		counter_num++;
		if(counter_num == 100) //这里不能设置为1000，因为counter_num是unsigned char类型，最大能达到的数值是255
		{
			counter_num = 0;
			Time_proc();
		}
	}
	if (gongneng == 1)				//每间隔500ms执行一次数码管取反的程序，这意味着数码管处于“闪烁”状态，提醒用户调整数值
	{
		counter_num_2 ++;
		if(counter_num_2 == 50)
		{
			counter_num_2 = 0;
			ss_diaplay_YN = ~ss_diaplay_YN;
		}
	}
	if (gongneng == 2)
	{
		counter_num++;
		if(counter_num == 100)   //设置闹钟情况下，原来的时钟继续计时，也就是功能0下的时钟继续计时
		{
			counter_num = 0;
			Time_proc();
		}
		counter_num_2 ++;
		if(counter_num_2 == 50) 	//每间隔500ms执行一次数码管取反的程序，这意味着数码管处于“闪烁”状态，提醒用户调整数值
		{
			counter_num_2 = 0;
			ss_diaplay_YN = ~ss_diaplay_YN;
		}
	}
	if (gongneng == 3)
	{
		counter_num++;
		if(counter_num == 100)   //设置倒计时情况下，原来的时钟继续计时，也就是功能0下的时钟继续计时
		{
			counter_num = 0;
			Time_proc();
		}
		counter_num_2 ++;
		if(counter_num_2 == 50) 	//每间隔500ms执行一次数码管取反的程序，这意味着数码管处于“闪烁”状态，提醒用户调整数值
		{
			counter_num_2 = 0;
			ss_diaplay_YN = ~ss_diaplay_YN;
		}
	}
	if(LED_nz)														//当前时间等于闹钟时间时，点亮LED
	{
		P2 = (P2&0x1F|0x80);
		P0 = 0x00;
		P2 &= 0x1F;
		counter_num_3 ++;
	}
	if (counter_num_3 == 500)			//LED亮5s之后自动熄灭
	{
		counter_num_3 = 0;
		P2 = (P2&0x1F|0x80);
		P0 = 0xff;													//熄灭LED
		P2 &= 0x1F;
		LED_nz = 0;
	}
	//判断当前时间和闹钟时间是否相等
	if ((Time[0] == Time_nz_finish[0]) &&
			(Time[1] == Time_nz_finish[1]) &&
			(Time[3] == Time_nz_finish[3]) &&
			(Time[4] == Time_nz_finish[4]) &&
			(Time[6] == Time_nz_finish[6]) &&
			(Time[7] == Time_nz_finish[7]) &&
			(buffer_1 == 1))
	{
		buffer_1 = 0;
		LED_nz = 1;
	}
	if (gongneng == 7)
	{
		cnt_d ++;
		if (cnt_d == 100)
		{
			cnt_d = 0;
			if((Time_d_finish[0] != 0)||
					(Time_d_finish[1] != 0)||
					(Time_d_finish[3] != 0)||
					(Time_d_finish[4] != 0)||
					(Time_d_finish[6] != 0)||
					(Time_d_finish[7] != 0))
			{
				Time_proc_d(&hour_d, &minute_d, &second_d);
				hour_d>=10?(Time_d_finish[0]=hour_d/10):(Time_d_finish[0]=0);
				Time_d_finish[1]=hour_d%10;
				minute_d>=10?(Time_d_finish[3]=minute_d/10):(Time_d_finish[3]=0);
				Time_d_finish[4]=minute_d%10;
				second_d>=10?(Time_d_finish[6]=second_d/10):(Time_d_finish[6]=0);
				Time_d_finish[7]=second_d%10;
			}
		}
	}
}

void Timer0_Init(void)		//10毫秒@11.0592MHz
{
	AUXR &= 0x7F;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0x00;				//设置定时初始值
	TH0 = 0xDC;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	ET0 = 1;				//使能定时器0中断
}

void display(unsigned char* ptr)
{	
	//消隐
	P2 = (P2&0x1F|0xe0);
	P0 = 0xff;
	P2 &= 0x1F;
	
	//位选
	P0 = 1<<bh;
	P2 = (P2&0x1F|0xc0);
	P2 &= 0x1F;
	
	//段选
	P0 = smg[ptr[bh]];
	P2 = (P2&0x1F|0xe0);
	P2 &= 0x1F;
	
	//下一位
	bh++;
	if(bh == 8)
		bh = 0;
	
}
void Time_proc(void) //时间处理函数
{
	miao++;
	if(miao==60)
	{
		miao=0;
		fen++;
		if(fen==60)
		{
			fen=0;
			shi++;
			if (shi==24)
			{
				shi=0;
			}
		}
	}
	shi>=10?(Time[0]=shi/10):(Time[0]=0);
	Time[1]=shi%10;
	fen>=10?(Time[3]=fen/10):(Time[3]=0);
	Time[4]=fen%10;
	miao>=10?(Time[6]=miao/10):(Time[6]=0);
	Time[7]=miao%10;
}

void cls_buzz(void)
{
	P2 = (P2&0x1F|0xA0);
	P0 = 0x00;
	P2 &= 0x1F;
}
//读取矩阵键盘键值:转接板中使用P42和P44代替8051引脚
//顺序中的P36和P37引脚
unsigned char read_keyboard(void)
{
    static unsigned char col;
    
	//检测被按下的列数
	P3 = 0xf0; P42 = 1; P44 = 1;	//P3被赋值11110000，如果有按键依然处于被按下的状态，那么P3的值会瞬间改变
    if((P3 != 0xf0)||(P42 != 1)||(P44 != 1)) //这行代码表示有按键按下，这行代码原来是if((P3 != 0xf0)||(P42 != 0)||(P44 != 0))，且原来的代码也是可以正常运行的。
        key_press++;
	else
		key_press = 0;  //抖动
    
    if(key_press == 3)
    {
		key_press = 0;
		key_re = 1;
		
		if(P44 == 0)			col = 1;
		if(P42 == 0)			col = 2;
		if((P3 & 0x20) == 0)	col = 3;
		if((P3 & 0x10) == 0)	col = 4;
        
			//检测被按下的行数
        P3 = 0x0F; P42 = 0; P44 = 0; //所有行都没有被按下的状态，也就是P3的后4位是1111

		if((P3&0x01) == 0)	key_value = (col-1);
		if((P3&0x02) == 0)	key_value = (col+3);
		if((P3&0x04) == 0)	key_value = (col+7);
		if((P3&0x08) == 0)	key_value = (col+11);
    }
    
	//连续三次检测到按键被按下，并且该按键已经释放
	P3 = 0x0f; P42 = 0; P44 = 0;
	
    if(((key_re == 1) && (P3 == 0x0f))&&(P42 == 0)&&(P44 == 0))
    {
        key_re = 0;
        return key_value;
    }
    
    return 0xff;  //无按键按下或被按下的按键未被释放 
}
void time_add(u8 number, u8* hour, u8* minute, u8* second)
{
	switch(number)
		{
			case 0:*hour = *hour + 1;if(*hour == 24) *hour = 0; break;
			case 1:*minute = *minute + 1;if(*minute == 60) *minute = 0; break;
			case 2:*second = *second + 1;if(*second == 60) *second = 0; break;
		}
}
void time_subtract(u8 number, u8* hour, u8* minute, u8* second)
{
	switch(number)
		{
			case 0:if(*hour == 0) *hour=23; else *hour = *hour - 1; break;
			case 1:if(*minute == 0) *minute=59; else *minute = *minute - 1; break;
			case 2:if(*second == 0) *second=59; else *second = *second - 1; break;
		}
}
void Time_proc_d(u8* hour, u8* minute, u8* second)
{
	if((*second) == 0)
	{
		*second = 59;
		if((*minute) == 0)
		{
			*minute = 59;
			*hour = *hour - 1;
		}
		else
			*minute = *minute - 1;
	}
	else
		*second = *second - 1;
}