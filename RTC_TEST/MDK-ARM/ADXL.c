#include "main.h"
#include  <math.h>    //Keil library  
#include  <stdio.h>   //Keil library	
#include "ADXL.h"
#include "sys.h"

BYTE BUF[8];                         //接收数据缓存区      	



void delay(unsigned int k)	
{						
unsigned int i,j;				
	for(i=0;i<k;i++)
	{			
	for(j=0;j<121;j++)			
	{;}}						
}

void SCL( uint8_t x )
{
	if( x==1 )
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET);
}

void SDA( uint8_t x )
{
	if( x==0 )
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET);
}
//上边两个对pb6和pb7引脚控制电平的函数也通过测试了，能用
/**************************************
起始信号
**************************************/
void ADXL345_Start()
{
		SDA_OUT();
    SDA(1);                    //拉高数据线
    SCL(1);                    //拉高时钟线
    delay_us(5);                 //延时
    SDA(0);                    //产生下降沿
    delay_us(5);                 //延时
    SCL(0);                    //拉低时钟线
}


/**************************************
停止信号
**************************************/
void ADXL345_Stop()
{
		SDA_OUT();
    SDA(0);                    //拉低数据线
    SCL(1);                    //拉高时钟线
    delay_us(5);                 //延时
    SDA(1);                    //产生上升沿
    delay_us(5);                 //延时
}


/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
void ADXL345_SendACK(uint8_t ack)
{
		SDA_OUT();
    SDA(ack);                  //写应答信号
    SCL(1);                    //拉高时钟线
    delay_us(5);                 //延时
    SCL(0);                    //拉低时钟线
    delay_us(5);                 //延时
}

/**************************************
接收应答信号
**************************************/

uint8_t ADXL345_RecvACK()
{		
    SCL(1);                    //拉高时钟线
    delay_us(5);                 //延时
		uint8_t CY;
		SDA_IN();
    CY = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7);                   //读应答信号
    SCL(0);                    //拉低时钟线
    delay_us(5);                 //延时
		SDA_OUT();
	
    return CY;
}

void ADXL345_SendByte(BYTE dat)
{
    BYTE i;
		uint8_t k;
		SDA_OUT();
    for (i=0; i<8; i++)         //8位计数器
    {
        k = dat & (0x80);            //我糙，终于成了，在这加了个k并上0x80，也就是二进制10000000，再写入，绝了
        SDA(k);               //送数据口
        SCL(1);                //拉高时钟线
        delay_us(5);             //延时
        SCL(0);                //拉低时钟线
        delay_us(5);  //延时
				dat <<= 1;  //移出数据的最高位           
    }
    ADXL345_RecvACK();
}

/**************************************
从IIC总线接收一个字节数据
**************************************/
BYTE ADXL345_RecvByte()
{
    BYTE i;
    BYTE dat = 0;
		uint8_t k;
    SDA(1); 	//使能内部上拉,准备读取数据,
		SDA_IN();
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;             //这个移位必须写在前边
        SCL(1);                //拉高时钟线
        delay_us(5);             //延时
        dat |= HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7);             //读数据               
        SCL(0);                //拉低时钟线
        delay_us(5);              //延时
			
				
    }
		SDA_OUT();
    return dat;
}

//******单字节写入*******************************************

void Single_Write_ADXL345(uchar REG_Address,uchar REG_data)
{
    ADXL345_Start();                  //起始信号
    ADXL345_SendByte(SlaveAddress);   //发送设备地址+写信号
    ADXL345_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf22页 
    ADXL345_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf22页 
    ADXL345_Stop();                   //发送停止信号
}


//********单字节读取*****************************************
uchar Single_Read_ADXL345(uchar REG_Address)
{  	uchar REG_data;
    ADXL345_Start();                          //起始信号
    ADXL345_SendByte(SlaveAddress);           //发送设备地址+写信号
    ADXL345_SendByte(REG_Address);            //发送存储单元地址，从0开始	
    ADXL345_Start();                          //起始信号
    ADXL345_SendByte(SlaveAddress+1);         //发送设备地址+读信号
    REG_data=ADXL345_RecvByte();              //读出寄存器数据
		ADXL345_SendACK(1);   
		ADXL345_Stop();                           //停止信号
    return REG_data; 
}

//*********************************************************
//
//连续读出ADXL345内部加速度数据，地址范围0x32~0x37
//
//*********************************************************
void Multiple_read_ADXL345(void)
{   uchar i;
    ADXL345_Start();                          //起始信号
    ADXL345_SendByte(SlaveAddress);           //发送设备地址+写信号
    ADXL345_SendByte(0x32);                   //发送存储单元地址，从0x32开始	
    ADXL345_Start();                          //起始信号
    ADXL345_SendByte(SlaveAddress+1);         //发送设备地址+读信号
		for (i=0; i<6; i++)                      //连续读取6个地址数据，存储中BUF
    {
        BUF[i] = ADXL345_RecvByte();          //BUF[0]存储0x32地址中的数据
        if (i == 5)
        {
           ADXL345_SendACK(1);                //最后一个数据需要回NOACK
        }
        else
        {
          ADXL345_SendACK(0);                //回应ACK
       }
		}
    ADXL345_Stop();                          //停止信号
    delay_ms(5);
}


//*****************************************************************

//初始化ADXL345，根据需要请参考pdf进行修改************************
void Init_ADXL345()
{
   Single_Write_ADXL345(0x31,0x0B);   //测量范围,正负16g，13位模式
   Single_Write_ADXL345(0x2C,0x08);   //速率设定为12.5 参考pdf13页
   Single_Write_ADXL345(0x2D,0x08);   //选择电源模式   参考pdf24页
   Single_Write_ADXL345(0x2E,0x80);   //使能 DATA_READY 中断
   Single_Write_ADXL345(0x1E,0x00);   //X 偏移量 根据测试传感器的状态写入pdf29页
   Single_Write_ADXL345(0x1F,0x00);   //Y 偏移量 根据测试传感器的状态写入pdf29页
   Single_Write_ADXL345(0x20,0x05);   //Z 偏移量 根据测试传感器的状态写入pdf29页
}
//***********************************************************************

int  dis_data_X;  
int  dis_data_Y;                       //变量
int  dis_data_Z;                       //变量

void process_x()
{		
    dis_data_X=(BUF[1]<<8)+BUF[0];  //合成数据
		if(dis_data_X>10000){
			dis_data_X=-(65535-dis_data_X);
		}	
		
}

void process_y()
{

    dis_data_Y=(BUF[3]<<8)+BUF[2];   //合成数据
		if(dis_data_Y>10000){
			dis_data_Y=-(65535-dis_data_Y);
		}	
		
}

void process_z()
{

    dis_data_Z=(BUF[5]<<8)+BUF[4];   //合成数据
		if(dis_data_Z>10000){
			dis_data_Z=-(65535-dis_data_Z);
		}	
		
		
}

//上边这种写法使得x,y,z三轴数据能够在-255~+255之间随着倾斜的变化而变化

int Steps=0;
int aaa=0;
int bbb=0;
int adxl_display()
{
			Multiple_read_ADXL345(); 
			process_x();
			process_z();
			
			if(dis_data_X>30&&aaa!=2)
			{
						aaa=1;
					
			}
			if(dis_data_X<0&&aaa!=2)
			{
						aaa=0;
			}
			
			if(aaa==1)
			{
						aaa=2;
						Steps+=1;
						
			}
			
			if(aaa==2&&dis_data_X<0)
			{
						aaa=1;
			}
			
			
			
			
			if(dis_data_Z>30&&bbb!=2)
			{
						bbb=1;
					
			}
			if(dis_data_Z<0&&bbb!=2)
			{
						bbb=0;
			}
			
			if(bbb==1)
			{
						bbb=2;
						Steps+=1;
						
			}
			
			if(bbb==2&&dis_data_Z<0)
			{
						bbb=1;
			}
			
			if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13)==0){Steps=0;}//在主界面下，按cancel键可使得步数清零
			
			//写了个很简单的倾角计步算法，通过判断x轴倾角改变aaa值的状态，在通过判断aaa值来计步
			//又加了个bbb，确保在板子竖放，没办法触发x轴倾角大幅度改变的情况下，通过判断z轴数据来完成计步
			return Steps;
			

	
}