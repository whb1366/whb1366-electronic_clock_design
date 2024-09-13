/*******************************************************************************  
* �ļ����ƣ���������ʵ��
* ʵ��Ŀ�ģ�1.���յ�Ƭ��IO�ڲ����Ļ�������
*           2.����C51�ؼ���"sbit"���÷�
*           2.���յ�Ƭ��������Χ������·����Ʒ��� 
* ����˵����ʹ�ó���ǰ��������J2����ΪBTNģʽ
* Ӳ��������IAP15F2K61S2@11.0592MHz
* ���ڰ汾��2012-9-3/V1.0a 
*******************************************************************************/

#include "reg52.h"  //����51��Ƭ�����⹦�ܼĴ���
#include "absacc.h"
#define u8 unsigned char
#define u16 unsigned short int

/** ���������ӿ� */
sfr P4   = 0xC0;
sbit P42 = P4^2;
sbit P44 = P4^4;
sbit buzz  = P0^6;	//�˿�P06���Ʒ�����������ͨ���鿴ԭ��ͼ�ó�����
sbit motor  = P0^5;
bit key_re = 0;
bit add_flag = 0;        	//�Լ�1��־λ
bit subtract_flag = 0;	 	//�Լ�1��־λ
bit naozhong_flag = 0;				 	//���ӿ�����־λ
bit update_nz = 1; 						//�������ӳ�ʼֵ�ı�־λ�������ӳ�ʼֵ����Ҫ���Ҳ������������ʱ����ʾ�ĳ�ʼֵ
bit LED_nz = 0;						//��ǰʱ���������ʱ��ʱ������LED�ı�־λ
bit buffer_1 = 0;					//��2��������ʱ�䣬�ٰ�0ȷ������ʱ�䣬�Ұ�0����������ʱ�Ӽ������в���ʾ����ʱbuffer_1��������1��Ҳ����˵��2�������Ӻ��ٰ�0������£�buffer_1��������1
													//buffer_1����Ҳ��ȷ������ʱ��ı�־λ
bit buffer_2 = 0;    			//buffer_1��buffer_2�����ʹ�õ�
sfr AUXR = 0x8e;
unsigned char bh = 0;
unsigned char Time[]={1,7,22,2,0,22,5,0}; //ʱ��
unsigned char Time_shanshuo[]={0,0,22,0,0,22,0,0}; //������˸��ʱ��
unsigned char Time_naozhong[]={0,0,22,0,0,22,0,0}; //�������ӵ�ʱ�䣬����Time_naozhong����֮��ḳֵ��Time_nz_buffer������Time_nz_buffer��ֵ��Time_nz_finish
																										//���ｫ����Time_nz_buffer��Ϊ��������ԭ���Ǳ���Time_naozhong���ȶ����������ط��������������޸ģ��������һ����������һ��
unsigned char Time_nz_buffer[]={0,0,22,0,0,22,0,0}; //������
unsigned char Time_nz_finish[]={0,0,22,0,0,22,0,0}; //����������ɵ�ʱ��
unsigned char Time_d[]={0,0,22,0,0,22,0,0};	//����ʱʱ��
unsigned char Time_d_finish[]={0,0,22,0,0,22,0,0};	//����ʱ���������õ�ʱ��
unsigned char counter_num = 0;
unsigned char counter_num_2 = 0;
short int counter_num_3 = 0;
unsigned char cnt_d = 0; //����ʱ�����еļ�����
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
unsigned char gongneng = 0; //���ܱ�־λ
unsigned char ss_xh = 2; //��˸λ��0����Сʱ��˸��1���������˸��2���������˸��3������˸
unsigned char nz_xh = 2;	//�������ӹ����µ�ʱ����˸λ
unsigned char d_xh = 2;	//���õ���ʱ�����µ�ʱ����˸λ
bit ss_diaplay_YN = 0; //��˸״̬������ʾ����Ϩ��ı�־λ
void Timer0_Init(void); //��ʱ��0��ʼ������
void Timer0_Isr(void);	//��ʱ��0�жϺ���
void display(unsigned char* ptr);		//��ʾ����ܺ���
void cls_buzz(void); //�ر����������
void Time_proc(void); //ʱ�䴦����
unsigned char read_keyboard(void); //��ȡ�������
void time_add(u8 number, u8* hour, u8* minute, u8* second);
void time_subtract(u8 number, u8* hour, u8* minute, u8* second);
void Time_proc_d(u8* hour, u8* minute, u8* second);		//����ʱ�����е�ʱ�䴦������Ҳ����ÿ��1s��1

//�����������        0     1    2    3    4    5    6    7    8    9    A    B    C    D    E    F    P    U    Y    L   ȫ�� Ϩ��  -
unsigned char smg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e,0x8c,0xc1,0x91,0x7c,0x00,0xff,0xbf};


//������
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
			if(gongneng == 0) //��������//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			{
				display(Time);
			}
			if(gongneng == 1)  //����ʱ��//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			{
				if(ss_diaplay_YN) 						//��˸�����������������״̬��ss_diaplay_YN����1ʱ��ʾ������
				{
					Time_shanshuo[0] = Time[0];	Time_shanshuo[1] = Time[1];
					Time_shanshuo[3] = Time[3];	Time_shanshuo[4] = Time[4];
					Time_shanshuo[6] = Time[6];	Time_shanshuo[7] = Time[7];
				}
				else													//��˸�����������������״̬��ss_diaplay_YN����0ʱ��ʾ����
				{
					switch(ss_xh)
					{
						case 0:Time_shanshuo[0] = 21;	Time_shanshuo[1] = 21; break;	//����smg�ĵ�21��Ԫ�ر�ʾϨ��ss_xh == 0����Сʱ��������˸��ss_xh��shanshuo_xuhao�ļ�д
						case 1:Time_shanshuo[3] = 21; Time_shanshuo[4] = 21; break;	//�����ӡ�������˸
						case 2:Time_shanshuo[6] = 21; Time_shanshuo[7] = 21; break;	//�����������˸
					}
				}
				display(Time_shanshuo);
				
				if(add_flag == 1) 											//ʱ���ĳһλ�Լ�1
				{
					add_flag = 0;
					time_add(ss_xh, &shi, &fen, &miao);
				}
				if(subtract_flag == 1) 											//ʱ���ĳһλ�Լ�1
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
			if(gongneng == 2) //���ӹ���//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			{
				u8 cnt1 = 0;
				if(add_flag == 1) 											//ʱ���ĳһλ�Լ�1
				{
					add_flag = 0;
					time_add(nz_xh, &shi_n, &fen_n, &miao_n);
				}
				if(subtract_flag == 1) 											//ʱ���ĳһλ�Լ�1
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
				
				if(~ss_diaplay_YN)													//��˸�����������������״̬��ss_diaplay_YN����0ʱ��ʾ����
				{
					switch(nz_xh)
					{
						case 0:Time_naozhong[0] = 21;	Time_naozhong[1] = 21; break;	//����smg�ĵ�21��Ԫ�ر�ʾϨ��ss_xh == 0����Сʱ��������˸��ss_xh��shanshuo_xuhao�ļ�д
						case 1:Time_naozhong[3] = 21; Time_naozhong[4] = 21; break;	//�����ӡ�������˸
						case 2:Time_naozhong[6] = 21; Time_naozhong[7] = 21; break;	//�����������˸
					}
				}
				display(Time_naozhong);
			}
			if(gongneng == 3) //����ʱ���ù���
			{
				u8 cnt1 = 0;
				if(add_flag == 1) 											//ʱ���ĳһλ�Լ�1
				{
					add_flag = 0;
					time_add(d_xh, &hour_d, &minute_d, &second_d);
				}
				if(subtract_flag == 1) 											//ʱ���ĳһλ�Լ�1
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
				
				if(~ss_diaplay_YN)													//��˸�����������������״̬��ss_diaplay_YN����0ʱ��ʾ����
				{
					switch(d_xh)
					{
						case 0:Time_d[0] = 21; Time_d[1] = 21; break;	//����smg�ĵ�21��Ԫ�ر�ʾϨ��ss_xh == 0����Сʱ��������˸��ss_xh��shanshuo_xuhao�ļ�д
						case 1:Time_d[3] = 21; Time_d[4] = 21; break;	//�����ӡ�������˸
						case 2:Time_d[6] = 21; Time_d[7] = 21; break;	//�����������˸
					}
				}
				display(Time_d);
			}
			if(gongneng == 7) //����ʱ��ʼ����
			{
				display(Time_d_finish);
			}
    }
}

void Timer0_Isr(void) interrupt 1 //ÿ10ms�ж�һ��
{
	unsigned char key_temp = read_keyboard();	//ÿ10msɨ��һ�μ���
	if(key_temp == 0) //��0������0��Ҳ����ʱ���������й���
	{
		gongneng = 0;
		update_nz = 1;
		if(buffer_2)
		{
			unsigned char buffer_3 = 0;   //�ֲ����������������������Ŀ�ͷ
			buffer_2 = 0;
			buffer_1 = 1;		//ͨ��buffer_1��buffer_2��ȷ������ʱ���Ƿ�������ɣ����仰˵����Ҫ������ʱ����ȫȷ�����������Ȱ�2�ٰ�0���ڰ�2�Ͱ�0֮�����ͨ����5��9��������ʱ��
			for(buffer_3 = 0;buffer_3<8; buffer_3++)
				Time_nz_finish[buffer_3] = Time_nz_buffer[buffer_3];
		}
	}
	if(key_temp == 1)	//��1������1��Ҳ���ǵ���ʱ�书��
	{
		unsigned char buffer = 0;
		for(buffer = 0;buffer<8; buffer++)
			Time_shanshuo[buffer] = Time[buffer];
		gongneng = 1;
		switch(ss_xh)
		{
			case 0:ss_xh ++;break;
			case 1:ss_xh ++;break;  //0��1������¾�ִ���Լ�1�ĳ���
			case 2:ss_xh = 0;break;	//ss_xhΪ2������±���ֵ0��Ҳ����˵������ss_xh�ı仯����Ϊ0��1��2��0��1��2��0��1��2
		}
	}
	if(key_temp == 2) //��2�������ӹ���
	{
		buffer_2 = 1;
		gongneng = 2;
		if (update_nz)
		{
			unsigned char buffer = 0;
			for(buffer = 0;buffer<8; buffer++)
				Time_naozhong[buffer] = Time[buffer];  //���ӳ�ʼ��ʱ�䣬ÿ�ε�������ʱ��ʱֻ��ʼ��һ��
			update_nz = 0;
			shi_n = shi;
			fen_n = fen;
			miao_n = miao;
		}
		switch(nz_xh)			//�����������˸��λ��
		{
			case 0:nz_xh ++;break;	//���д���ִ��֮ǰ�ǵ�0������ܣ��Լ�1֮����ʾ����
			case 1:nz_xh ++;break;  //0��1������¾�ִ���Լ�1�ĳ���
			case 2:nz_xh = 0;break;	//ss_xhΪ2������±���ֵ0��Ҳ����˵������ss_xh�ı仯����Ϊ0��1��2��0��1��2��0��1��2
		}
	}
	if(key_temp == 3) //��3������ʱ����
	{
		gongneng = 3;
		switch(d_xh)
		{
			case 0:d_xh ++;break;
			case 1:d_xh ++;break;  //0��1������¾�ִ���Լ�1�ĳ���
			case 2:d_xh = 0;break;	//ss_xhΪ2������±���ֵ0��Ҳ����˵������ss_xh�ı仯����Ϊ0��1��2��0��1��2��0��1��2
		}
	}
	if(key_temp == 5)	//�Լ�1����
		add_flag = 1;
	if(key_temp == 9)	//�Լ�1����
		subtract_flag = 1;
	if(key_temp == 7)	//����ʱ��ʼ����
		gongneng = 7;
	
	
	if (gongneng == 0)					//ÿ���1000msִ��һ������Լ�1�ĳ���Ҳ����Time_proc����
	{
		counter_num++;
		if(counter_num == 100) //���ﲻ������Ϊ1000����Ϊcounter_num��unsigned char���ͣ�����ܴﵽ����ֵ��255
		{
			counter_num = 0;
			Time_proc();
		}
	}
	if (gongneng == 1)				//ÿ���500msִ��һ�������ȡ���ĳ�������ζ������ܴ��ڡ���˸��״̬�������û�������ֵ
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
		if(counter_num == 100)   //������������£�ԭ����ʱ�Ӽ�����ʱ��Ҳ���ǹ���0�µ�ʱ�Ӽ�����ʱ
		{
			counter_num = 0;
			Time_proc();
		}
		counter_num_2 ++;
		if(counter_num_2 == 50) 	//ÿ���500msִ��һ�������ȡ���ĳ�������ζ������ܴ��ڡ���˸��״̬�������û�������ֵ
		{
			counter_num_2 = 0;
			ss_diaplay_YN = ~ss_diaplay_YN;
		}
	}
	if (gongneng == 3)
	{
		counter_num++;
		if(counter_num == 100)   //���õ���ʱ����£�ԭ����ʱ�Ӽ�����ʱ��Ҳ���ǹ���0�µ�ʱ�Ӽ�����ʱ
		{
			counter_num = 0;
			Time_proc();
		}
		counter_num_2 ++;
		if(counter_num_2 == 50) 	//ÿ���500msִ��һ�������ȡ���ĳ�������ζ������ܴ��ڡ���˸��״̬�������û�������ֵ
		{
			counter_num_2 = 0;
			ss_diaplay_YN = ~ss_diaplay_YN;
		}
	}
	if(LED_nz)														//��ǰʱ���������ʱ��ʱ������LED
	{
		P2 = (P2&0x1F|0x80);
		P0 = 0x00;
		P2 &= 0x1F;
		counter_num_3 ++;
	}
	if (counter_num_3 == 500)			//LED��5s֮���Զ�Ϩ��
	{
		counter_num_3 = 0;
		P2 = (P2&0x1F|0x80);
		P0 = 0xff;													//Ϩ��LED
		P2 &= 0x1F;
		LED_nz = 0;
	}
	//�жϵ�ǰʱ�������ʱ���Ƿ����
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

void Timer0_Init(void)		//10����@11.0592MHz
{
	AUXR &= 0x7F;			//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;			//���ö�ʱ��ģʽ
	TL0 = 0x00;				//���ö�ʱ��ʼֵ
	TH0 = 0xDC;				//���ö�ʱ��ʼֵ
	TF0 = 0;				//���TF0��־
	TR0 = 1;				//��ʱ��0��ʼ��ʱ
	ET0 = 1;				//ʹ�ܶ�ʱ��0�ж�
}

void display(unsigned char* ptr)
{	
	//����
	P2 = (P2&0x1F|0xe0);
	P0 = 0xff;
	P2 &= 0x1F;
	
	//λѡ
	P0 = 1<<bh;
	P2 = (P2&0x1F|0xc0);
	P2 &= 0x1F;
	
	//��ѡ
	P0 = smg[ptr[bh]];
	P2 = (P2&0x1F|0xe0);
	P2 &= 0x1F;
	
	//��һλ
	bh++;
	if(bh == 8)
		bh = 0;
	
}
void Time_proc(void) //ʱ�䴦����
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
//��ȡ������̼�ֵ:ת�Ӱ���ʹ��P42��P44����8051����
//˳���е�P36��P37����
unsigned char read_keyboard(void)
{
    static unsigned char col;
    
	//��ⱻ���µ�����
	P3 = 0xf0; P42 = 1; P44 = 1;	//P3����ֵ11110000������а�����Ȼ���ڱ����µ�״̬����ôP3��ֵ��˲��ı�
    if((P3 != 0xf0)||(P42 != 1)||(P44 != 1)) //���д����ʾ�а������£����д���ԭ����if((P3 != 0xf0)||(P42 != 0)||(P44 != 0))����ԭ���Ĵ���Ҳ�ǿ����������еġ�
        key_press++;
	else
		key_press = 0;  //����
    
    if(key_press == 3)
    {
		key_press = 0;
		key_re = 1;
		
		if(P44 == 0)			col = 1;
		if(P42 == 0)			col = 2;
		if((P3 & 0x20) == 0)	col = 3;
		if((P3 & 0x10) == 0)	col = 4;
        
			//��ⱻ���µ�����
        P3 = 0x0F; P42 = 0; P44 = 0; //�����ж�û�б����µ�״̬��Ҳ����P3�ĺ�4λ��1111

		if((P3&0x01) == 0)	key_value = (col-1);
		if((P3&0x02) == 0)	key_value = (col+3);
		if((P3&0x04) == 0)	key_value = (col+7);
		if((P3&0x08) == 0)	key_value = (col+11);
    }
    
	//�������μ�⵽���������£����Ҹð����Ѿ��ͷ�
	P3 = 0x0f; P42 = 0; P44 = 0;
	
    if(((key_re == 1) && (P3 == 0x0f))&&(P42 == 0)&&(P44 == 0))
    {
        key_re = 0;
        return key_value;
    }
    
    return 0xff;  //�ް������»򱻰��µİ���δ���ͷ� 
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