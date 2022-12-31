/**
 * @file i2c_soft.c
 * @author your name (you@domain.com)
 * @brief 两个 GPIO 模拟软件 I2C
 *        I2C 是低速总线, 速度 400K bps 左右, 使用 GPIO 模拟影响不大, 
 *        SPI 速度较快, 使用 GPIO 模拟速度较慢
 *      
 *        注意必须严格按照 I2C 通讯时序来, 一点点不一样都可能会导致通信异常。。。
 * @version 0.1
 * @date 2022-12-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "i2c_soft.h"
#include "delay.h" 	

// 串行总线
// 开始信号, 结束信号, 应答信号
// 软件 I2C:
// 使用 GPIO 模拟 I2C 信号实现, 而不是真正的 I2C
// PC12 - SCL; 
// PC11 - SDA
// 硬件上都有 3.3V 上拉
                                     


/**
 * @brief 初始化IIC, 配置 GPIO
 *        PC12 - SCL; PC11 - SDA
 * 
 */
void IIC_Init(void)
{
    RCC->APB2ENR |= 1 << 4;     // 先使能外设IO PORTC时钟 							 
    GPIOC->CRH &= 0XFFF00FFF;   // clear
    GPIOC->CRH |= 0X00033000;	// PC11/12 推挽输出，输出模式, 最大 50MHz, 复用功能, 推挽输出
    GPIOC->ODR |= 3 << 11;      // PC11,12 输出高
}

//产生IIC起始信号
// SDA 由高电平向低电平跳变(产生下降沿)表示开始传送数据
void IIC_Start(void)
{
    SDA_OUT();     // sda 线输出
    IIC_SDA = 1;	  	  
    IIC_SCL = 1;
    delay_us(4);
    IIC_SDA = 0;    // START:when CLK is high, DATA change form high to low(falling edge)
    delay_us(4);
    IIC_SCL = 0;    // 钳住I2C总线，准备发送或接收数据(外部带上拉)

    return;
}

//产生IIC停止信号
// SDA 由低电平向高电平跳变, 表示产生停止信号
void IIC_Stop(void)
{
    SDA_OUT();//sda线输出
    IIC_SCL = 0;
    IIC_SDA = 0;
    delay_us(4); 
    IIC_SCL = 1;	//STOP:when CLK is high DATA change form low to high(rolling edge)
     delay_us(4); 
    IIC_SDA = 1;//发送I2C总线结束信号 						   	
}
  
/**
 * @brief 等待应答信号到来
 *        I2C 发完一个信号后要等待对方恢复一个 ACK，根据情况判断是否继续发送
 * 
 * @return u8 1，接收应答失败; 0，接收应答成功
 */
u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime=0;
    SDA_IN();      // SDA设置为输入  
    IIC_SDA = 1;	// 设置为输入还可以写吗?
    delay_us(1);	   
    IIC_SCL = 1;
    delay_us(1);	 
    while(READ_SDA) // 对方拉低一段时间 DA 作为应答
    {
        ucErrTime++;
        if(ucErrTime > 250) // I2C 异常
        {
            IIC_Stop();
            return I2C_ERR;
        }
    }
    IIC_SCL = 0; // 时钟拉低

    return I2C_SUCCESS;  
} 

//产生ACK应答
// SDA 产生特定的低电平脉冲, 表示应答
void IIC_Ack(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 0;
    delay_us(2);
    IIC_SCL = 1;
    delay_us(2);
    IIC_SCL = 0;
}

//不产生ACK应答		 
// SDA 拉高就不产生应答   
void IIC_NAck(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 1; // SDA 拉高
    delay_us(2);
    IIC_SCL = 1;
    delay_us(2);
    IIC_SCL = 0;
}	


/**
 * @brief IIC发送一个字节
 *        返回从机有无应答
 * 
 * @param txd 
 * @return * void 1，有应答; 0，无应答
 */
void IIC_Send_Byte(u8 txd)
{
    u8 t;   
    SDA_OUT(); 	    
    IIC_SCL = 0;        // 钳住低电平, 拉低时钟开始数据传输

    // 一位一位的写, 从高位开始发送
    for(t = 0; t < 8; t++)
    {
        IIC_SDA = (txd & 0x80) >> 7;
        txd <<= 1; 	  
        delay_us(2);   // 对TEA5767这三个延时都是必须的
        IIC_SCL = 1;
        delay_us(2); 
        IIC_SCL = 0;	// CLK 下降沿
        delay_us(2);
    }

    return;
} 

/**
 * @brief 读1个字节
 *        每读取一个字节都发送 ACK
 *        
 * @param ack ack=1时，发送ACK; ack=0，发送nACK
 * @return u8 返回接收到的数据(1个字节)
 */
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;
    SDA_IN(); // SDA设置为输入
    for(i = 0; i < 8; i++ )
    {
        IIC_SCL = 0; 
        delay_us(2);
        IIC_SCL = 1;	// CLK 上升沿
        receive <<= 1;
        if(READ_SDA)
            receive++;   
        delay_us(1); 
    }

    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK 

    return receive;
}

/**
 * @brief 
 * 
 * @param daddr 
 * @param addr 
 * @param data 
 */
void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data)
{

}

/**
 * @brief 
 * 
 * @param daddr 
 * @param addr 
 * @return u8 
 */
u8 IIC_Read_One_Byte(u8 daddr,u8 addr)
{

}
