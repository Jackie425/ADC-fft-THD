//////////////////////////////////////////////////////////////////////////////////	 
//  深圳市沃乐康科技有限公司  WWW.WLKLCD.COM  0755-32882855   专业液晶显示供应商   //
//  主程序 （硬件SPI2 ST7789+154ips-240*240 @STM32F103C8T6 MDK5.14）             //
//  https://wlklcd.1688.com/     https://shop341012592.taobao.com/                 //
//  例程仅供学习参考，不做量产使用，不得用于商业用途                              //
//******************************************************************************/
// --------------接口连线-------------------------------------------------------
//              屏幕           STM32F103
//          1、 GND -------------  GND
//          2、 VCC -------------  3.3v电源
//          3、 SCL -------------  PB13  SPI时钟信号
//          4、 SDA -------------  PB15  SPI数据信号 
//          5、 RES -------------- PB11
//          6、 DC  -------------- PB14
//          7、 CS  -------------- PB12 
//					8、	BLC -------------- PB10    控制背光 高电平亮 低电平灭
// -----------------------------------------------------------------------------
//******************************************************************************/
#include "lcd.h"
#include "lcdfont.h"
#include "delay.h"
#include "bmp.h"
#include "stm32f10x_spi.h"
u16 BACK_COLOR;   //背景色


/******************************************************************************
      函数说明：LCD串行数据写入函数
      入口数据：dat  要写入的串行数据
      返回值：  无
******************************************************************************/
void LCD_Writ_Bus(u8 dat) 
{	
	u8 i;
	LCD_CS_Clr();
	for(i=0;i<8;i++)
	{			  
		LCD_SCLK_Clr();
		if(dat&0x80)
		   LCD_SDIN_Set();
		else
		   LCD_SDIN_Clr();
		LCD_SCLK_Set();
		dat<<=1;
	}	
  LCD_CS_Set();	
}

/******************************************************************************
      函数说明：硬件SPI
      入口数据：dat  要写入的串行数据
      返回值：  无
******************************************************************************/
void LCD_SPIWrit_Bus(u8 dat) 
{	
	 //while(!SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2,dat);
	while(!SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE));

}

/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	LCD_CS_Clr();
	LCD_DC_Clr();//写命令
	//LCD_Writ_Bus(dat);
	LCD_SPIWrit_Bus(dat);
	
	LCD_CS_Set();
}

/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	LCD_CS_Clr();
	LCD_DC_Set();//写数据
	//LCD_Writ_Bus(dat);
	LCD_SPIWrit_Bus(dat);
	//delay_ms(1);
	LCD_CS_Set();
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
	LCD_CS_Clr();
	LCD_DC_Set();//写数据
	LCD_SPIWrit_Bus(dat>>8);
	LCD_SPIWrit_Bus(dat);
	LCD_CS_Set();
}



/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
	
	
	
	if(USE_HORIZONTAL==0)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==1)
	{
		y1=y1+40;
	y2=y2+40;
		
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);//储存器写
	}
	else
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);//储存器写
	}
}


void Hardware_SPI_Init(void)
{
///////初始化SPI端口////////////////
	SPI_InitTypeDef SPI_InitStructure;
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);	 //使能SPI2端口时钟
	
	
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	//SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	
	SPI_Init(SPI2,&SPI_InitStructure);
	//SPI_Init(SPI2,&amp;SPI_InitStructure);
	
	SPI_Cmd(SPI2,ENABLE);
}

void STM_Port_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能B端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_11|GPIO_Pin_14|GPIO_Pin_10;	 //PB13-SCK PB15-SDA PB11-RESET PB12-CS PB14-RS  PB10-BLC
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能B端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	//GPIO_Init(GPIOB,&GPIO_InitStructure);
	
  LCD_CS_Set();
}
/******************************************************************************
      函数说明：LCD初始化函数
      入口数据：无
      返回值：  无
******************************************************************************/
void Lcd_Init(void)
{
	

	STM_Port_Init();
	Hardware_SPI_Init();
	LCD_CS_Set();
	LCD_RST_Set();
	LCD_DC_Set();
		
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	 //使能D端口时钟
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	 
 	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	//GPIO_Init(GPIOD, &GPIO_InitStructure);	 
 	//GPIO_SetBits(GPIOD,GPIO_Pin_2);

	LCD_RST_Clr();
	delay_ms(20);
	LCD_RST_Set();
	delay_ms(100);
	LCD_BLK_Set();
	
LCD_WR_REG(0x11);              
delay_ms(120); 

LCD_WR_REG(0x36); //MX, MY, RGB mode 
if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
else LCD_WR_DATA8(0xA0);

LCD_WR_REG(0x3A);			
LCD_WR_DATA8(0x55);   //   5 565  6 666

LCD_WR_REG(0xB2);			
LCD_WR_DATA8(0x0C);
LCD_WR_DATA8(0x0C); 
LCD_WR_DATA8(0x00); 
LCD_WR_DATA8(0x33); 
LCD_WR_DATA8(0x33); 			

LCD_WR_REG(0xB7);			
LCD_WR_DATA8(0x35);

LCD_WR_REG(0xBB);			
LCD_WR_DATA8(0x32); //Vcom=1.35V
												  	
LCD_WR_REG(0xC2);			
LCD_WR_DATA8(0x01);

LCD_WR_REG(0xC3);			
LCD_WR_DATA8(0x19); //GVDD=4.8V 
													  
LCD_WR_REG(0xC4);			
LCD_WR_DATA8(0x20); //VDV, 0x20:0v

LCD_WR_REG(0xC6);			
LCD_WR_DATA8(0x0F); //0x0F:60Hz        	

LCD_WR_REG(0xD0);			
LCD_WR_DATA8(0xA4);
LCD_WR_DATA8(0xA1); 											  												  																								
	    
LCD_WR_REG(0xE0);     
LCD_WR_DATA8(0xD0);   
LCD_WR_DATA8(0x08);   
LCD_WR_DATA8(0x0E);   
LCD_WR_DATA8(0x09);   
LCD_WR_DATA8(0x09);   
LCD_WR_DATA8(0x05);   
LCD_WR_DATA8(0x31);   
LCD_WR_DATA8(0x33);   
LCD_WR_DATA8(0x48);   
LCD_WR_DATA8(0x17);   
LCD_WR_DATA8(0x14);   
LCD_WR_DATA8(0x15);   
LCD_WR_DATA8(0x31);   
LCD_WR_DATA8(0x34);   

LCD_WR_REG(0xE1);     
LCD_WR_DATA8(0xD0);   
LCD_WR_DATA8(0x08);   
LCD_WR_DATA8(0x0E);   
LCD_WR_DATA8(0x09);   
LCD_WR_DATA8(0x09);   
LCD_WR_DATA8(0x15);   
LCD_WR_DATA8(0x31);   
LCD_WR_DATA8(0x33);   
LCD_WR_DATA8(0x48);   
LCD_WR_DATA8(0x17);   
LCD_WR_DATA8(0x14);   
LCD_WR_DATA8(0x15);   
LCD_WR_DATA8(0x31);   
LCD_WR_DATA8(0x34);   


LCD_WR_REG(0x21);

LCD_WR_REG(0x2A); //Column Address Set
LCD_WR_DATA8(0x00);
LCD_WR_DATA8(0x00); //0
LCD_WR_DATA8(0x00);
LCD_WR_DATA8(0xEF); //239

LCD_WR_REG(0x2B); //Row Address Set
LCD_WR_DATA8(0x00);
LCD_WR_DATA8(0x00); //0
LCD_WR_DATA8(0x00);
LCD_WR_DATA8(0xEF); //239

LCD_WR_REG(0x29);


} 


/******************************************************************************
      函数说明：LCD清屏函数
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_Clear(u16 Color)
{
	u16 i,j;  	
	LCD_Address_Set(0,0,LCD_W-1,LCD_H-1);
    for(i=0;i<LCD_W;i++)
	  {
			for (j=0;j<LCD_H;j++)
				{
					LCD_WR_DATA(Color);
				}
	  }
}

/******************************************************************************
      函数说明：LCD显示彩条函数
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_DispBand(void)
{
	u16 i,j,k;  	
	unsigned int color[8]={0x001f,0x07e0,0xf800,0x07ff,0xf81f,0xffe0,0x0000,0xffff};
	
	LCD_Address_Set(0,0,LCD_W-1,LCD_H-1);
	
	for(i=0;i<8;i++)
	{
	   for(j=0;j<LCD_H/8;j++)
		 {
		   for(k=0;k<LCD_W;k++)
			 {
			  LCD_WR_DATA(color[i]);
			 }
		 }
	}
	for(j=0;j<LCD_H%8;j++)
	{
	 for(k=0;k<LCD_W;k++)
		{LCD_WR_DATA(color[7]);}
	}
	
    
}

/******************************************************************************
      函数说明：LCD显示灰阶函数
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_DispGrayHor16(void)
{
	u16 i,j,k;  	
	
	LCD_Address_Set(0,0,LCD_W-1,LCD_H-1);
	
	for(i=0;i<LCD_H;i++)
	{
	  for(j=0;j<LCD_W%8;j++)
		{LCD_WR_DATA(0);}
		
		for(j=0;j<16;j++)
		{
		  for(k=0;k<LCD_W/16;k++)
			{
			  LCD_WR_DATA8(((j*2)<<3)|((j*4)>>3));
				LCD_WR_DATA8(((j*4)<<5)|(j*2));
			}
		}
	}
	
    
}

/******************************************************************************
      函数说明：LCD显示雪花函数
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_DispSnow(void)
{
	u16 i,j;  
  u16 dat;	
	
	LCD_Address_Set(0,0,LCD_W-1,LCD_H-1);
	dat=0;
	
	for(i=0;i<LCD_H;i++)
	{
	  for(j=0;j<LCD_W/2;j++)
		{LCD_WR_DATA(dat);
		 LCD_WR_DATA(~dat);}
		dat=~dat;
	}  
}

/******************************************************************************
      函数说明：LCD显示方块函数
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_DispBlock(void)
{
	u16 i,j,k;  
  k=LCD_H/4;
	
	LCD_Address_Set(0,0,LCD_W-1,LCD_H-1);
	
	for(i=0;i<k;i++)
	{
	  for(j=0;j<LCD_W;j++)
		{LCD_WR_DATA(0x7bef);}
	}  
	for(i=0;i<k*2;i++)
	 {
	  for(j=0;j<LCD_W/4;j++)
		 {LCD_WR_DATA(0x7bef);}
		for(j=0;j<LCD_W/2;j++)
		 {LCD_WR_DATA(0);}
		for(j=0;j<LCD_W/4;j++)
		 {LCD_WR_DATA(0x7bef);}
	 }
	 for(i=0;i<k;i++)
	 {
	   for(j=0;j<LCD_W;j++)
		{LCD_WR_DATA(0x7bef);}
	 }
}


/******************************************************************************
      函数说明：LCD显示汉字
      入口数据：x,y   起始坐标
                index 汉字的序号
                size  字号
      返回值：  无
******************************************************************************/
void LCD_ShowChinese(u16 x,u16 y,u8 index,u8 size,u16 color)	
{  
	u8 i,j;
	u8 *temp,size1;
	if(size==16){temp=Hzk16;}//选择字号
	if(size==32){temp=Hzk32;}
  LCD_Address_Set(x,y,x+size-1,y+size-1); //设置一个汉字的区域
  size1=size*size/8;//一个汉字所占的字节
	temp+=index*size1;//写入的起始位置
	for(j=0;j<size1;j++)
	{
		for(i=0;i<8;i++)
		{
		 	if((*temp&(1<<i))!=0)//从数据的低位开始读
			{
				LCD_WR_DATA(color);//点亮
			}
			else
			{
				LCD_WR_DATA(BACK_COLOR);//不点亮
			}
		}
		temp++;
	 }
}
/******************************************************************************
      函数说明：LCD显示汉字
      入口数据：x,y   起始坐标
      返回值：  无
******************************************************************************/
void LCD_DrawPoint(u16 x,u16 y,u16 color)
{
	LCD_Address_Set(x,y,x,y);//设置光标位置 
	LCD_WR_DATA(color);
} 
/******************************************************************************
      函数说明：LCD画一个大的点
      入口数据：x,y   起始坐标
      返回值：  无
******************************************************************************/
void LCD_DrawPoint_big(u16 x,u16 y,u16 color)
{
	LCD_Fill(x-1,y-1,x+1,y+1,color);
} 
/******************************************************************************
      函数说明：在指定区域填充颜色
      入口数据：xsta,ysta   起始坐标
                xend,yend   终止坐标
      返回值：  无
******************************************************************************/
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{          
	u16 i,j; 
	LCD_Address_Set(xsta,ysta,xend,yend);      //设置光标位置 
	for(i=ysta;i<=yend;i++)
	{													   	 	
		for(j=xsta;j<=xend;j++)LCD_WR_DATA(color);//设置光标位置 	    
	} 					  	    
}
/******************************************************************************
      函数说明：画线
      入口数据：x1,y1   起始坐标
                x2,y2   终止坐标
      返回值：  无
******************************************************************************/
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1;
	uRow=x1;//画线起点坐标
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向 
	else if (delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//水平线 
	else {incy=-1;delta_y=-delta_x;}
	if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		LCD_DrawPoint(uRow,uCol,color);//画点
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}
/******************************************************************************
      函数说明：画矩形
      入口数据：x1,y1   起始坐标
                x2,y2   终止坐标
      返回值：  无
******************************************************************************/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}
/******************************************************************************
      函数说明：画圆
      入口数据：x0,y0   圆心坐标
                r       半径
      返回值：  无
******************************************************************************/
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color)
{
	int a,b;
	int di;
	a=0;b=r;	  
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a,color);             //3           
		LCD_DrawPoint(x0+b,y0-a,color);             //0           
		LCD_DrawPoint(x0-a,y0+b,color);             //1                
		LCD_DrawPoint(x0-a,y0-b,color);             //2             
		LCD_DrawPoint(x0+b,y0+a,color);             //4               
		LCD_DrawPoint(x0+a,y0-b,color);             //5
		LCD_DrawPoint(x0+a,y0+b,color);             //6 
		LCD_DrawPoint(x0-b,y0+a,color);             //7
		a++;
		if((a*a+b*b)>(r*r))//判断要画的点是否过远
		{
			b--;
		}
	}
}

/******************************************************************************
      函数说明：显示字符
      入口数据：x,y    起点坐标
                num    要显示的字符
                mode   1叠加方式  0非叠加方式
      返回值：  无
******************************************************************************/
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 mode,u16 color)
{
    u8 temp;
    u8 pos,t;
	  u16 x0=x;    
    if(x>LCD_W-16||y>LCD_H-16)return;	    //设置窗口		   
	num=num-' ';//得到偏移后的值
	LCD_Address_Set(x,y,x+8-1,y+16-1);      //设置光标位置 
	if(!mode) //非叠加方式
	{
		for(pos=0;pos<16;pos++)
		{ 
			temp=asc2_1608[(u16)num*16+pos];		 //调用1608字体
			for(t=0;t<8;t++)
		    {                 
		        if(temp&0x01)LCD_WR_DATA(color);
				else LCD_WR_DATA(BACK_COLOR);
				temp>>=1;
				x++;
		    }
			x=x0;
			y++;
		}	
	}else//叠加方式
	{
		for(pos=0;pos<16;pos++)
		{
		    temp=asc2_1608[(u16)num*16+pos];		 //调用1608字体
			for(t=0;t<8;t++)
		    {                 
		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos,color);//画一个点     
		        temp>>=1; 
		    }
		}
	}   	   	 	  
}


/******************************************************************************
      函数说明：显示字符串
      入口数据：x,y    起点坐标
                *p     字符串起始地址
      返回值：  无
******************************************************************************/
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 color)
{         
    while(*p!='\0')
    {       
        if(x>LCD_W-16){x=0;y+=16;}
        if(y>LCD_H-16){y=x=0;LCD_Clear(RED);}
        LCD_ShowChar(x,y,*p,0,color);
        x+=8;
        p++;
    }  
}


/******************************************************************************
      函数说明：显示数字
      入口数据：m底数，n指数
      返回值：  无
******************************************************************************/
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}


/******************************************************************************
      函数说明：显示数字
      入口数据：x,y    起点坐标
                num    要显示的数字
                len    要显示的数字个数
      返回值：  无
******************************************************************************/
void LCD_ShowNum(u16 x,u16 y,u16 num,u8 len,u16 color)
{         	
	u8 t,temp;
	u8 enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+8*t,y,' ',0,color);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+8*t,y,temp+48,0,color); 
	}
} 


/******************************************************************************
      函数说明：显示小数
      入口数据：x,y    起点坐标
                num    要显示的小数
                len    要显示的数字个数
      返回值：  无
******************************************************************************/
void LCD_ShowNum1(u16 x,u16 y,float num,u8 len,u16 color)
{         	
	u8 t,temp;
	u8 enshow=0;
	u16 num1;
	num1=num*100;
	for(t=0;t<len;t++)
	{
		temp=(num1/mypow(10,len-t-1))%10;
		if(t==(len-2))
		{
			LCD_ShowChar(x+8*(len-2),y,'.',0,color);
			t++;
			len+=1;
		}
	 	LCD_ShowChar(x+8*t,y,temp+48,0,color);
	}
}


/******************************************************************************
      函数说明：显示40x40图片
      入口数据：x,y    起点坐标
      返回值：  无
******************************************************************************/
void LCD_ShowPicture(u16 x1,u16 y1,u16 x2,u16 y2)
{
	int i;
	  LCD_Address_Set(x1,y1,x2,y2);
		for(i=0;i<1600;i++)
	  { 	
			LCD_WR_DATA8(image[i*2+1]);	 
			LCD_WR_DATA8(image[i*2]);			
	  }			
}




