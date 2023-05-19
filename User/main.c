 // ADC ��ͨ���ɼ�

#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_adc.h"
#include "bsp_AdvanceTim.h"  
#include "bsp_TiMbase.h"
#include "stm32_dsp.h"
#include <math.h>
#include "lcd.h"
#include "sys.h"
#include "delay.h"
// ADC1ת���ĵ�ѹֵͨ��DMA��ʽ����SRAM
extern __IO uint16_t ADC_ConvertedValue;

// �ֲ����������ڱ���ת�������ĵ�ѹֵ 	 
int32_t ADC_ConvertedValueLocal[1024]; 
int32_t ADC_ConvertedValueLocal_fft[512];
int32_t Mag[512];//�����ֵ������������
int32_t Max[5];
char str[16];
unsigned char happy[8]="(*^__^*)";
float THD,TH0=0;

volatile uint8_t flag = 0; //�ж��Ƿ������־λ

//�ӳٺ���
void delay()
{
	uint32_t i;
	for(i=0;i<0xfff;i++);
}
/**
  * @brief  ������
  * @param  ��
  * @retval ��
 */
int main(void)
{	
	int i,j,k=0;
	delay_init();	    	 //��ʱ������ʼ��	 
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	Lcd_Init();			//��ʼ��  
	LCD_Clear(WHITE);
	LCD_BLK_Set();
	BACK_COLOR=WHITE;
	LED_ON;
	LCD_ShowString(100,160,happy,RED);
//	ADVANCE_TIM_Init();//�߼���ʱ����ʼ�������ڲ���PWM�����
	BASIC_TIM_Init();//������ʱ����ʼ���������ж϶�ʱ��12800hz����Ƶ�ʣ�һ����1024���㣬1024/12800=0.08���ܲ���ʱ�䣬0.08*1000=80��ԭ�ź�����
	// ���ô���
	USART_Config();
	
	// ADC ��ʼ��
	ADCx_Init();

	printf("\r\n ----����һ��ADC��ͨ��DMA��ȡʵ��----\r\n");
	int th = 25;
	while(1)
	{
			for(i=0;i<1024;i++)
			{
				if(flag==1){
					flag=0;
					ADC_ConvertedValueLocal[i] = ADC_ConvertedValue; // ��ȡת����ADֵ
				}
			  else{
					i = i-1;
				}
			}
			for(i=0;i<1024;i++)
			{
			  //printf("%d %d\n",i,ADC_ConvertedValueLocal[i]); 
				ADC_ConvertedValueLocal[i] =(ADC_ConvertedValueLocal[i])<<16;
			}		
			cr4_fft_1024_stm32(ADC_ConvertedValueLocal_fft, ADC_ConvertedValueLocal, 1024);
			for(i=0;i<512;i++)
			{
				Mag[i] = (ADC_ConvertedValueLocal_fft[i]>>16)*(ADC_ConvertedValueLocal_fft[i]>>16)+((ADC_ConvertedValueLocal_fft[i]<<16)>>16)*((ADC_ConvertedValueLocal_fft[i]<<16)>>16);
				//printf("%d %d\n",i,Mag[i]);
			}
			for(i=1;i<6;i++)
			{
				Max[i-1] = 0;
				for(j=-2;j<3;j++)
				{
					Max[i-1] += ((Mag[80*i+j]>th)?(Mag[80*i+j]):(0));
				}
			}
			THD=sqrt((float)(Max[1]+Max[2]+Max[3]+Max[4])/(float)Max[0]);
			//printf("%f\n",THD);
			delay();
			TH0=THD+TH0;
			k++;
			if(k==3)
			{
				printf("%f\n",TH0/k);
				sprintf(str,"THD = %.4f%%",TH0/k*100);
				LCD_ShowString(60,120,(unsigned char *)str,BLACK);
				TH0=0;
				k=0;

			}

				
	}
}	
/*********************************************END OF FILE**********************/

