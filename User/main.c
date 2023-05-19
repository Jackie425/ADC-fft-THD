 // ADC 单通道采集

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
// ADC1转换的电压值通过DMA方式传到SRAM
extern __IO uint16_t ADC_ConvertedValue;

// 局部变量，用于保存转换计算后的电压值 	 
int32_t ADC_ConvertedValueLocal[1024]; 
int32_t ADC_ConvertedValueLocal_fft[512];
int32_t Mag[512];//保存幅值计算结果的数组
int32_t Max[5];
char str[16];
unsigned char happy[8]="(*^__^*)";
float THD,TH0=0;

volatile uint8_t flag = 0; //判断是否采样标志位

//延迟函数
void delay()
{
	uint32_t i;
	for(i=0;i<0xfff;i++);
}
/**
  * @brief  主函数
  * @param  无
  * @retval 无
 */
int main(void)
{	
	int i,j,k=0;
	delay_init();	    	 //延时函数初始化	 
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	Lcd_Init();			//初始化  
	LCD_Clear(WHITE);
	LCD_BLK_Set();
	BACK_COLOR=WHITE;
	LED_ON;
	LCD_ShowString(100,160,happy,RED);
//	ADVANCE_TIM_Init();//高级定时器初始化，用于产生PWM波输出
	BASIC_TIM_Init();//基本定时器初始化，用于中断定时，12800hz采样频率，一共采1024个点，1024/12800=0.08秒总采样时间，0.08*1000=80个原信号周期
	// 配置串口
	USART_Config();
	
	// ADC 初始化
	ADCx_Init();

	printf("\r\n ----这是一个ADC单通道DMA读取实验----\r\n");
	int th = 25;
	while(1)
	{
			for(i=0;i<1024;i++)
			{
				if(flag==1){
					flag=0;
					ADC_ConvertedValueLocal[i] = ADC_ConvertedValue; // 读取转换的AD值
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

