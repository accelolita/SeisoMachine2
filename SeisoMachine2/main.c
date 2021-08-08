/*
 * SeisoMachine2.c
 *
 * Created: 2021/08/04 0:32:36
 * Author : SIMPSONMAN
 */ 

#define	F_CPU	8000000UL		// CKSEL[1:0]=10, CKDIV8=0

#define sbi(a,b) a|=1<<b;
#define cbi(a,b) a&=~(1<<b);
#define roopup(a,b,c) a= a<c ? a+1 : b 
#define roopdown(a,b,c) a= a>c ? a-1 : b

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define PlaySound(x) dfCommand[6]=x;SendCommand(&dfCommand[0],(unsigned char)0x08);

void Set7Seg(unsigned char num,unsigned char dot);
uint16_t trackNum=500;
unsigned char trackDigit[4];
void SetTrackNumber(uint16_t num);

int main(void)
{

	//�|�[�gB�@2 3 GP F �� G
	DDRB= 
	  (1<<DDB7) //PB7 7seg_2
	| (1<<DDB6) //PB6 7seg_3
	| (0<<DDB5) //PB5 SCK
	| (0<<DDB4) //PB4 MISO
	| (1<<DDB3) //PB3 MOSI 7seg_GP
	| (1<<DDB2) //PB2 7seg_F
	| (1<<DDB1) //PB1 blue
	| (1<<DDB0);//PB0 7seg_G
	//�|�[�gC�@E D C B A �X�C�b�`
	DDRC=
	  (1<<DDC5) //PC5 7seg_E
	| (1<<DDC4) //PC4 7seg_D
	| (1<<DDC3) //PC3 7seg_C
	| (1<<DDC2) //PC2 7seg_B
	| (1<<DDC1) //PC1 7seg_A
	| (0<<DDC0);//PC0 switch
	//�|�[�gD�@1 �� �� 4 INT INT TX RX
	DDRD=
	  (1<<DDD7) //PD7 7seg_1
	| (1<<DDD6) //PD6 green
	| (1<<DDD5) //PD5 red
	| (1<<DDD4) //PD4 7seg_4
	| (0<<DDD3) //PD3 INT1 rotary encoder
	| (0<<DDD2) //PD2 INT2 rotary encoder
	| (0<<DDD1) //PD1 UART TX
	| (0<<DDD0);//PD0 UART RX
	
	PORTD|=0b10001100;
	
	//8bit�^�C�}�[�J�E���^�O//PWM
	TCCR0A= (0b11<<COM1A0)	//����PWM��rA���]�o��
	| (0b11<<COM0B0)		//����PWM��rB���]�o��
	| (0b11<<WGM00);  		//����PWM
	TCCR0B= (0<<WGM02) //����PWM
	| (0b100<<CS00); //256�����@312.5Hz

	//16bit�^�C�}�[�J�E���^1
	TCCR1A= (0b11<<COM0A0)	//����PWM��rA���]�o��
	| (0b00<<COM0B0)		//�W���|�[�g����
	| (0b01<<WGM00);  		//8bit����PWM	
	TCCR1B= (0b00<<WGM12)   //8bit����PWM
	| (0b100<<CS10);		//256�����@312.5Hz

	//8bit�^�C�}�[�J�E���^2
	TCCR2A= (0b0000<<COM2B0)//�W���|�[�g����
	| (0b10<<WGM20);		//��r��v���
	TCCR2B= (0b1<<WGM22)	//��r��v���
	| (0b111<<CS20);		//1024�����@78.125Hz
	OCR2A=30;				//�_�C�i�~�b�N�_������
	OCR2B=0xFF;				//���[�^���[�G���R�[�_�[�Q�[�~���O����
	TIMSK2=1<<OCIE2A;		//�^�C�}�[2A�����݋���
	//PORTB|=0b00000000;
	
	//�O��������
	EICRA= 0b10<<ISC10		//INT1 ���~�[������
	| 0b10<<ISC00;			//INT0 ���~�[������
	EIMSK=0<<INT0			//INT0 �����݋���
	| 0<<INT1;				//INT1 �����݋���
			
		
	//�Q�[�~���O���[�^���[�G���R�[�_�����ݒ�
	OCR0A=0;
	OCR0B=255;
	OCR1A=0;
	
	SetTrackNumber(trackNum);
	unsigned char abStatus=0;
	sei();
    while (1) 
    {
		//countup
		if ((PIND & 0x0C) == 0x08 && abStatus==0)
		{
			abStatus=1;
		}
		if (((PIND & 0x0C) == 0x00) && abStatus==1)
		{
			abStatus=2;
		}
		if (((PIND & 0x0C) == 0x04) && abStatus==2)
		{
			abStatus=3;
		}
		if (((PIND & 0x0C) == 0x0C) && abStatus==3)
		{
			SetTrackNumber(++trackNum);
			if (trackNum>9999)
			{
				SetTrackNumber(1);
			}		
			abStatus=0;
		}
		//countdown
		if ((PIND & 0x0C) == 0x04 && abStatus==0)
		{
			abStatus=4;
		}
		if (((PIND & 0x0C) == 0x00) && abStatus==4)
		{
			abStatus=5;
		}
		if (((PIND & 0x0C) == 0x08) && abStatus==5)
		{
			abStatus=6;
		}
		if (((PIND & 0x0C) == 0x0C) && abStatus==6)
		{
			SetTrackNumber(--trackNum);
			if (trackNum<1)
			{
				SetTrackNumber(9999);
			}
			abStatus=0;
		}
    }
}

void Set7Seg(unsigned char num,unsigned char dot)
{
	const char seg[10]=  //0bGFEDCBAP
	{
		0b10000001,//0
		0b11110011,//1
		0b01001001,//2
		0b01100001,//3
		0b00110011,//4
		0b00100101,//5
		0b00000101,//6
		0b10110001,//7
		0b00000001,//8
		0b00100001,//9
	};
	
	PORTC=(PORTC & 0b11000001) | (seg[num] & 0b00111110);
	PORTB=(PORTB & 0b11110010) | ((seg[num]& 0x80)>>7) | ((seg[num] & 0x40)>>4) | ((~dot & 0x01)<<3); 
}

void SetTrackNumber(uint16_t num)
{
	trackNum=num;
	trackDigit[0]=num/1000;
	num-=1000*trackDigit[0];
	trackDigit[1]=num/100;
	num-=100*trackDigit[1];
	trackDigit[2]=num/10;
	trackDigit[3]=num-10*trackDigit[2];
}



ISR(TIMER2_COMPA_vect)
{
	static unsigned char digit=0;
	//�S���N���A
	cbi(PORTD,PD7);
	cbi(PORTB,PB7);
	cbi(PORTB,PB6);
	cbi(PORTD,PD4);
	//���茅�̕\��
	switch (digit)
	{
		case 0:
		PORTC|=0b00111110;
		PORTB|=0b00001101;
		sbi(PORTD,PD7);
		Set7Seg(trackDigit[0],0);
		break;
		case 1:
		PORTC|=0b00111110;
		PORTB|=0b00001101;		
		sbi(PORTB,PB7);
		Set7Seg(trackDigit[1],0);
		break;
		case 2:
		PORTC|=0b00111110;
		PORTB|=0b00001101;		
		sbi(PORTB,PB6);
		Set7Seg(trackDigit[2],0);
		break;
		case 3:
		PORTC|=0b00111110;
		PORTB|=0b00001101;		
		sbi(PORTD,PD4);
		Set7Seg(trackDigit[3],0);
		break;
		default:
		/* Your code here */
		break;
	}
	roopup(digit,0,3);
	
	//�Q�[�~���O���[�^���[�G���R�[�_
	static unsigned char stage=0;
	if (stage==0)
	{
		OCR0B--;
		OCR1A++;
		if (OCR0B==0) 
		{
			OCR0A=0;
			OCR0B=0;
			OCR1A=255;			
			stage=1;			
		}
	}
	else if (stage==1)
	{
		OCR1A--;
		OCR0A++;
		if (OCR1A==0) 
		{
			OCR0A=255;
			OCR0B=0;
			OCR1A=0;
			stage=2;
		}
	}
	else if (stage==2)
	{
		OCR0A--;
		OCR0B++;
		if (OCR0A==0) 
		{
			OCR0A=0;
			OCR0B=255;
			OCR1A=0;
			stage=0;
		}
	}
}

char rnStatus=0;

//ISR(INT0_vect)
//{
	//EIMSK&=~(1<<INT0);
	//if (rnStatus==0 && (PIND & (1<<PIND3)))
	//{
		//rnStatus=1;//countdown
	//}
	//if (rnStatus==2 && (~PIND & (1<<PIND2)))
	//{
		//SetTrackNumber(++trackNum);
		//if (trackNum>9999)
		//{
			//SetTrackNumber(1);
		//}
		//rnStatus=0;
		//EIMSK|=1<<INT1;
	//}		
//}
//
//ISR(INT1_vect)
//{
	//EIMSK&=~(1<<INT1);
	//if (rnStatus==0 && (PIND & (1<<PIND2)))
	//{
		//rnStatus=2;//countdown
	//}
	//if (rnStatus==1 && (~PIND & (1<<PIND2)))
	//{
		//SetTrackNumber(--trackNum);
		//if (trackNum<1)
		//{
			//SetTrackNumber(9999);
		//}
		//rnStatus=0;
		//EIMSK|=1<<INT0;
	//}
//}
