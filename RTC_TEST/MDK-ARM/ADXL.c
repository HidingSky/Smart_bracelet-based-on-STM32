#include "main.h"
#include  <math.h>    //Keil library  
#include  <stdio.h>   //Keil library	
#include "ADXL.h"
#include "sys.h"

BYTE BUF[8];                         //�������ݻ�����      	



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
//�ϱ�������pb6��pb7���ſ��Ƶ�ƽ�ĺ���Ҳͨ�������ˣ�����
/**************************************
��ʼ�ź�
**************************************/
void ADXL345_Start()
{
		SDA_OUT();
    SDA(1);                    //����������
    SCL(1);                    //����ʱ����
    delay_us(5);                 //��ʱ
    SDA(0);                    //�����½���
    delay_us(5);                 //��ʱ
    SCL(0);                    //����ʱ����
}


/**************************************
ֹͣ�ź�
**************************************/
void ADXL345_Stop()
{
		SDA_OUT();
    SDA(0);                    //����������
    SCL(1);                    //����ʱ����
    delay_us(5);                 //��ʱ
    SDA(1);                    //����������
    delay_us(5);                 //��ʱ
}


/**************************************
����Ӧ���ź�
��ڲ���:ack (0:ACK 1:NAK)
**************************************/
void ADXL345_SendACK(uint8_t ack)
{
		SDA_OUT();
    SDA(ack);                  //дӦ���ź�
    SCL(1);                    //����ʱ����
    delay_us(5);                 //��ʱ
    SCL(0);                    //����ʱ����
    delay_us(5);                 //��ʱ
}

/**************************************
����Ӧ���ź�
**************************************/

uint8_t ADXL345_RecvACK()
{		
    SCL(1);                    //����ʱ����
    delay_us(5);                 //��ʱ
		uint8_t CY;
		SDA_IN();
    CY = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7);                   //��Ӧ���ź�
    SCL(0);                    //����ʱ����
    delay_us(5);                 //��ʱ
		SDA_OUT();
	
    return CY;
}

void ADXL345_SendByte(BYTE dat)
{
    BYTE i;
		uint8_t k;
		SDA_OUT();
    for (i=0; i<8; i++)         //8λ������
    {
        k = dat & (0x80);            //�Ҳڣ����ڳ��ˣ�������˸�k����0x80��Ҳ���Ƕ�����10000000����д�룬����
        SDA(k);               //�����ݿ�
        SCL(1);                //����ʱ����
        delay_us(5);             //��ʱ
        SCL(0);                //����ʱ����
        delay_us(5);  //��ʱ
				dat <<= 1;  //�Ƴ����ݵ����λ           
    }
    ADXL345_RecvACK();
}

/**************************************
��IIC���߽���һ���ֽ�����
**************************************/
BYTE ADXL345_RecvByte()
{
    BYTE i;
    BYTE dat = 0;
		uint8_t k;
    SDA(1); 	//ʹ���ڲ�����,׼����ȡ����,
		SDA_IN();
    for (i=0; i<8; i++)         //8λ������
    {
        dat <<= 1;             //�����λ����д��ǰ��
        SCL(1);                //����ʱ����
        delay_us(5);             //��ʱ
        dat |= HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7);             //������               
        SCL(0);                //����ʱ����
        delay_us(5);              //��ʱ
			
				
    }
		SDA_OUT();
    return dat;
}

//******���ֽ�д��*******************************************

void Single_Write_ADXL345(uchar REG_Address,uchar REG_data)
{
    ADXL345_Start();                  //��ʼ�ź�
    ADXL345_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�
    ADXL345_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ����ο�����pdf22ҳ 
    ADXL345_SendByte(REG_data);       //�ڲ��Ĵ������ݣ���ο�����pdf22ҳ 
    ADXL345_Stop();                   //����ֹͣ�ź�
}


//********���ֽڶ�ȡ*****************************************
uchar Single_Read_ADXL345(uchar REG_Address)
{  	uchar REG_data;
    ADXL345_Start();                          //��ʼ�ź�
    ADXL345_SendByte(SlaveAddress);           //�����豸��ַ+д�ź�
    ADXL345_SendByte(REG_Address);            //���ʹ洢��Ԫ��ַ����0��ʼ	
    ADXL345_Start();                          //��ʼ�ź�
    ADXL345_SendByte(SlaveAddress+1);         //�����豸��ַ+���ź�
    REG_data=ADXL345_RecvByte();              //�����Ĵ�������
		ADXL345_SendACK(1);   
		ADXL345_Stop();                           //ֹͣ�ź�
    return REG_data; 
}

//*********************************************************
//
//��������ADXL345�ڲ����ٶ����ݣ���ַ��Χ0x32~0x37
//
//*********************************************************
void Multiple_read_ADXL345(void)
{   uchar i;
    ADXL345_Start();                          //��ʼ�ź�
    ADXL345_SendByte(SlaveAddress);           //�����豸��ַ+д�ź�
    ADXL345_SendByte(0x32);                   //���ʹ洢��Ԫ��ַ����0x32��ʼ	
    ADXL345_Start();                          //��ʼ�ź�
    ADXL345_SendByte(SlaveAddress+1);         //�����豸��ַ+���ź�
		for (i=0; i<6; i++)                      //������ȡ6����ַ���ݣ��洢��BUF
    {
        BUF[i] = ADXL345_RecvByte();          //BUF[0]�洢0x32��ַ�е�����
        if (i == 5)
        {
           ADXL345_SendACK(1);                //���һ��������Ҫ��NOACK
        }
        else
        {
          ADXL345_SendACK(0);                //��ӦACK
       }
		}
    ADXL345_Stop();                          //ֹͣ�ź�
    delay_ms(5);
}


//*****************************************************************

//��ʼ��ADXL345��������Ҫ��ο�pdf�����޸�************************
void Init_ADXL345()
{
   Single_Write_ADXL345(0x31,0x0B);   //������Χ,����16g��13λģʽ
   Single_Write_ADXL345(0x2C,0x08);   //�����趨Ϊ12.5 �ο�pdf13ҳ
   Single_Write_ADXL345(0x2D,0x08);   //ѡ���Դģʽ   �ο�pdf24ҳ
   Single_Write_ADXL345(0x2E,0x80);   //ʹ�� DATA_READY �ж�
   Single_Write_ADXL345(0x1E,0x00);   //X ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
   Single_Write_ADXL345(0x1F,0x00);   //Y ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
   Single_Write_ADXL345(0x20,0x05);   //Z ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
}
//***********************************************************************

int  dis_data_X;  
int  dis_data_Y;                       //����
int  dis_data_Z;                       //����

void process_x()
{		
    dis_data_X=(BUF[1]<<8)+BUF[0];  //�ϳ�����
		if(dis_data_X>10000){
			dis_data_X=-(65535-dis_data_X);
		}	
		
}

void process_y()
{

    dis_data_Y=(BUF[3]<<8)+BUF[2];   //�ϳ�����
		if(dis_data_Y>10000){
			dis_data_Y=-(65535-dis_data_Y);
		}	
		
}

void process_z()
{

    dis_data_Z=(BUF[5]<<8)+BUF[4];   //�ϳ�����
		if(dis_data_Z>10000){
			dis_data_Z=-(65535-dis_data_Z);
		}	
		
		
}

//�ϱ�����д��ʹ��x,y,z���������ܹ���-255~+255֮��������б�ı仯���仯

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
			
			if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13)==0){Steps=0;}//���������£���cancel����ʹ�ò�������
			
			//д�˸��ܼ򵥵���ǼƲ��㷨��ͨ���ж�x����Ǹı�aaaֵ��״̬����ͨ���ж�aaaֵ���Ʋ�
			//�ּ��˸�bbb��ȷ���ڰ������ţ�û�취����x����Ǵ���ȸı������£�ͨ���ж�z����������ɼƲ�
			return Steps;
			

	
}