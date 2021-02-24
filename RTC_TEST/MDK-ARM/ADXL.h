#include "gpio.h"

#define   uchar unsigned char
#define   uint unsigned int	
	
#define	SlaveAddress   0xA6	  //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�
                              //ALT  ADDRESS���Žӵ�ʱ��ַΪ0xA6���ӵ�Դʱ��ַΪ0x3A
typedef unsigned char  BYTE;
typedef unsigned short WORD;

#define SDA_IN() {GPIO_InitTypeDef GPIO_InitStruct = {0};GPIO_InitStruct.Pin = GPIO_PIN_7;GPIO_InitStruct.Mode = GPIO_MODE_INPUT;HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);}//PB7����ģʽ
#define SDA_OUT() {GPIO_InitTypeDef GPIO_InitStruct = {0};GPIO_InitStruct.Pin = GPIO_PIN_7;GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;GPIO_InitStruct.Pull = GPIO_NOPULL;GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);}	//PB7���ģʽ
//�������ԣ��������������IO���Եĸı�궨�����û���⣬������
//�Բۣ����ԣ�SDA_OUT����������
//����ˣ����Է�������
void SCL( uint8_t x );
void SDA( uint8_t x );
void Single_Write_ADXL345(uchar REG_Address,uchar REG_data);
uchar Single_Read_ADXL345(uchar REG_Address);
void Multiple_read_ADXL345(void);
void Init_ADXL345();
int adxl_display();