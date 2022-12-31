/**
 * @file ssh_1106.c
 * @author your name (you@domain.com)
 * @brief 代码框架整理, 与芯片相关的分别放到不同的文件夹下, OLED 都放在 oled.c/oled.h 中
 *        供应商提供
 * @version 0.1
 * @date 2022-12-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//中景园电子
//店铺地址：http://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  文 件 名   : main.c
//  版 本 号   : v2.0
//  作    者   : HuangKai
//  生成日期   : 2014-0101
//  最近修改   : 
//  功能描述   : OLED 4接口演示例程(51系列)
//              说明: 
//              ----------------------------------------------------------------
//              GND    电源地
//              VCC  接5V或3.3v电源
//              D0   接PD6（SCL）
//              D1   接PD7（SDA）
//              RES  接PD4
//              DC   接PD5
//              CS   接PD3               
//              ----------------------------------------------------------------
// 修改历史   :
// 日    期   : 
// 作    者   : HuangKai
// 修改内容   : 创建文件
//版权所有，盗版必究。
//Copyright(C) 中景园电子2014/3/16
//All rights reserved
//******************************************************************************/

// VCC_IN:
// GND
// D0:SPI 接口是为 SPI 时钟线, IIC 时钟线;
// D1:SPI 接口时为 SPI 数据线, IIC 数据线;
// RES:OLED 复位, OLED 上电后需要做一个复位
// DC:SPI 数据/命令选择引脚, IIC 时用来设置 IIC 地址

#include "stdlib.h"
#include "oledfont.h"  	 
#include "delay.h"
#include "oled.h"
#include "ssh_1106.h"


//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   

#if OLED_MODE == OLED_8080
//向SSD1106写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 cmd)
{
    DATAOUT(dat);	    
    if(cmd)
      OLED_DC_Set();
    else 
      OLED_DC_Clr();		   
    OLED_CS_Clr();
    OLED_WR_Clr();	 
    OLED_WR_Set();
    OLED_CS_Set();	  
    OLED_DC_Set();	 
} 	    	    
#else
/**
 * @brief 通过 SPI 向 SSD1106 写入一个字节。
 * 
 * @param dat 要写入的数据/命令
 * @param cmd 数据/命令标志 0,表示命令;1,表示数据;
 */
void OLED_WR_Byte(u8 dat,u8 cmd)
{
    u8 i;

    // command/data
    if(cmd)
        OLED_DC_Set();
    else 
        OLED_DC_Clr();
		  
    OLED_CS_Clr(); // 片选

    for(i = 0; i < 8; i++){ // 从高位到低位依次写入
        OLED_SCLK_Clr(); // 拉低时钟, SPI 仅仅在时钟的边沿通信, 所以这里拉低拉高时钟
        if(dat & 0x80) 
           OLED_SDIN_Set(); // 1
        else 
           OLED_SDIN_Clr(); // 0
        OLED_SCLK_Set(); // 拉高时钟
        dat <<= 1;   
    }				 		  
    OLED_CS_Set(); // 恢复片选
    OLED_DC_Set(); // 恢复 DC

    return;
} 
#endif

void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
    OLED_WR_Byte(0xb0+y,OLED_CMD);
    OLED_WR_Byte((((x+2)&0xf0)>>4)|0x10,OLED_CMD);
    OLED_WR_Byte(((x+2)&0x0f),OLED_CMD); 
}

//开启OLED显示    
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
    OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
    OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

//关闭OLED显示     
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
    OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
    OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		

//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{
    u8 i,n;		

    // 0 - 7 页依次清除    
    for(i=0;i<8;i++)  
    {  
        OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
        OLED_WR_Byte (0x02,OLED_CMD);      //设置显示位置—列低地址
        OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
        for(n = 0; n < 128; n++)
            OLED_WR_Byte(0, OLED_DATA); 
    } //更新显示
}

/**
 * @brief 在指定位置显示字符
 * 
 * @param x 0~127 在指定位置显示一个字符,包括部分字符
 * @param y 0~63
 * @param chr 
 * @param mode 0,反白显示;1,正常显示	
 * @param SIZE 选择字体 16/12
 */
void OLED_ShowChar(u8 x, u8 y, u8 chr)
{      	
    unsigned char c = 0, i = 0;	
    c = chr - ' ';//得到偏移后的值

    if(x > Max_Column - 1){
        x = 0; 
        y = y + 2; // 换行, y + 2??
    }

    if(SIZE == 16){
        OLED_Set_Pos(x, y);	
        for(i = 0; i < 8; i++)
            OLED_WR_Byte(F8X16[c * 16 + i], OLED_DATA);
        OLED_Set_Pos(x, y + 1);
        for(i = 0; i < 8; i++)
            OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA);
    }
    else {
        OLED_Set_Pos(x, y + 1);
        for(i = 0; i < 6; i++)
            OLED_WR_Byte(F6x8[c][i], OLED_DATA);
    }

    return;
}

//m^n函数
u32 oled_pow(u8 m,u8 n)
{
    u32 result=1;	 
    while(n--)result*=m;    
    return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{
    u8 t,temp;
    u8 enshow=0;						   
    for(t=0;t<len;t++)
    {
        temp=(num/oled_pow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                OLED_ShowChar(x+(size/2)*t,y,' ');
                continue;
            }
            else 
                enshow=1; 
              
        }
        OLED_ShowChar(x + (size / 2) * t, y, (temp + '0')); 
    }
} 

//显示一个字符号串
void OLED_ShowString(u8 x,u8 y,u8 *chr)
{
    unsigned char j=0;
    while (chr[j]!='\0'){		
        OLED_ShowChar(x,y,chr[j]);
        x += 8;
        if(x > 120){
            x=0;
            y+=2;
        }
        j++;
    }
}

/**
 * @brief 显示汉字
 *        这种方式只能显示指定的汉字
 * 
 * @param x position
 * @param y 
 * @param no 汉字在字库中的位置索引 index
 */
void OLED_ShowCHinese(u8 x,u8 y,u8 no)
{      			    
    u8 t, adder=0;

    OLED_Set_Pos(x, y);	
    for(t = 0; t < 16; t++){
        OLED_WR_Byte(Hzk[2 * no][t], OLED_DATA);
        adder += 1;
    }
    OLED_Set_Pos(x, y+1);	
    for(t = 0; t < 16; t++){	
        OLED_WR_Byte(Hzk[2 * no + 1][t], OLED_DATA);
        adder += 1;
    }
    return;		
}

/**
 * @brief 显示显示BMP图片 128 × 64
 * 
 * @param x0 起始点坐标(x,y), x的范围0～127
 * @param y0 
 * @param x1 ，y为页的范围0～7*
 * @param y1 
 * @param BMP 
 */
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{
    unsigned int j=0;
    unsigned char x,y;

    if(y1 % 8 == 0) 
        y = y1 / 8;      
    else
        y = y1 / 8 + 1;
    for(y = y0; y < y1; y++){
        OLED_Set_Pos(x0,y);
        for(x = x0; x < x1; x++){      
            OLED_WR_Byte(BMP[j++], OLED_DATA);	    	
        }
    }

    return;
}


//初始化SSD1306					    
void OLED_Init(void)
{ 	
// GPIO initialization with HAL lib
//  	GPIO_InitTypeDef  GPIO_InitStructure;

//  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOG, ENABLE);	 //使能PC,D,G端口时钟
// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_3|GPIO_Pin_8;	 //PD3,PD6推挽输出  
//  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
//  	GPIO_Init(GPIOD, &GPIO_InitStructure);	  //初始化GPIOD3,6
//  	GPIO_SetBits(GPIOD,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_3|GPIO_Pin_8);	//PD3,PD6 输出高

//  #if OLED_MODE == OLED_8080
//  	GPIO_InitStructure.GPIO_Pin =0xFF; //PC0~7 OUT推挽输出
//  	GPIO_Init(GPIOC, &GPIO_InitStructure);
//  	GPIO_SetBits(GPIOC,0xFF); //PC0~7输出高
//  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				 //PG13,14,15 OUT推挽输出
//  	GPIO_Init(GPIOG, &GPIO_InitStructure);
//  	GPIO_SetBits(GPIOG,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);						 //PG13,14,15 OUT  输出高
//  #else
//  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;				 //PC0,1 OUT推挽输出
//  	GPIO_Init(GPIOC, &GPIO_InitStructure);
//  	GPIO_SetBits(GPIOC,GPIO_Pin_0|GPIO_Pin_1);						 //PC0,1 OUT  输出高

// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 //PG15 OUT推挽输出	  RST
//  	GPIO_Init(GPIOG, &GPIO_InitStructure);
//  	GPIO_SetBits(GPIOG,GPIO_Pin_15);						 //PG15 OUT  输出高
//  #endif

    RCC->APB2ENR |= 1 << 3; //使能PORTB时钟 
    RCC->APB2ENR |= 1 << 4; //使能PORTC时钟 	  
#if OLED_MODE == OLED_8080		//使用8080并口模式				 
    JTAG_Set(SWD_ENABLE);
    GPIOB->CRL=0X33333333;
    GPIOB->ODR|=0XFFFF;								    	 
 
    GPIOC->CRH&=0XFFFFFF00;
    GPIOC->CRL&=0X00FFFFFF;
    GPIOC->CRH|=0X00000033;
    GPIOC->CRL|=0X33000000;
    GPIOC->ODR|=0X03C0;
#else //使用4线 SPI 串口模式
    GPIOB->CRL &= 0XFFFFFF00; // PB1, PB2clear
    GPIOB->CRL |= 0XF0000033; // output push pull
    GPIOB->ODR |= (3 << 0);   // 0X03 set output 1
#if OLED_RESET_GPIO_CTR
    GPIOB->CRH &= 0xF0FFFFFF; // PB14 clear
    GPIOB->CRH |= 0x03000000; // output push pull
    GPIOB->ODR |= (1 << 14);  // output 1
#endif
    GPIOC->CRH &= 0XFFFFFF00; // PC8, PC9 clear  
    GPIOC->CRH |= 0X00000033; // output push pull
    GPIOC->ODR |= (3 << 8);     // output 1
#endif
 
    OLED_RST_Set();
    delay_ms(100);
    OLED_RST_Clr();
    delay_ms(100);
    OLED_RST_Set(); 
                      
    OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
    OLED_WR_Byte(0x02,OLED_CMD);//---set low column address
    OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
    OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
    OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
    OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
    OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
    OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
    OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WR_Byte(0x00,OLED_CMD);//-not offset
    OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
    OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
    OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
    OLED_WR_Byte(0x12,OLED_CMD);
    OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
    OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
    OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_WR_Byte(0x02,OLED_CMD);//
    OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
    OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
    OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
    OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
    OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
    
    OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 
    OLED_Clear();
    OLED_Set_Pos(0,0); 	

    return;
}  
