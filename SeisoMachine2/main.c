/*
 * SeisoMachine2.c
 *
 * Created: 2021/08/04 0:32:36
 * Author : SIMPSONMAN
 */ 

#define	F_CPU	8000000UL		// CKSEL[1:0]=10, CKDIV8=0

#define sbi(a,b) a|=1<<b;
#define cbi(a,b) a&=~(1<<b);

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>



#define PlaySound(x) dfCommand[6]=x;SendCommand(&dfCommand[0],(unsigned char)0x08);


int main(void)
{

	//�|�[�gB�@2 3 GP F �� G�@
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
	
	PORTD|=0b10000000;
	
	//8bit�^�C�}�[�J�E���^�O//PWM
	TCCR0A= (0b11<<COM1A0)	//����PWM��rA���]�o��
	| (0b11<<COM0B0)		//����PWM��rB���]�o��
	| (0b11<<WGM00);  		//����PWM
	TCCR0B= (0<<WGM02) //����PWM
	| (0b100<<CS00); //256�����@312.5Hz
	OCR0A=0;
	OCR0B=255;
	//16bit�^�C�}�[�J�E���^1
	TCCR1A= (0b11<<COM0A0)	//����PWM��rA���]�o��
	| (0b00<<COM0B0)		//�W���|�[�g����
	| (0b01<<WGM00);  		//8bit����PWM	
	TCCR1B= (0b00<<WGM12)   //8bit����PWM
	| (0b100<<CS10);		//256�����@312.5Hz
	OCR1A=0;
	//8bit�^�C�}�[�J�E���^2
	TCCR2A= (0b0000<<COM2B0)//�W���|�[�g����
	| (0b10<<WGM20);		//��r��v���
	TCCR2B= (0b1<<WGM22)	//��r��v���
	| (0b111<<CS20);		//1024�����@78.125Hz
	OCR2A=40;
	TIMSK2=1<<OCIE2A;		//�^�C�}�[2A�����݋���
	
	//PORTB|=0b00000000;
	
	sei();
    while (1) 
    {
    }
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
		sbi(PORTD,PD7);
		break;
		case 1:
		sbi(PORTB,PB7);
		break;
		case 2:
		sbi(PORTB,PB6);
		break;
		case 3:
		sbi(PORTD,PD4);
		break;
		default:
		/* Your code here */
		break;
	}
	digit++;
	if (digit>3)
	{
		digit=0;
	}
}
