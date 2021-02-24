#include "gpio.h"

#define   uchar unsigned char
#define   uint unsigned int	
	
#define	SlaveAddress   0xA6	  //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
                              //ALT  ADDRESS引脚接地时地址为0xA6，接电源时地址为0x3A
typedef unsigned char  BYTE;
typedef unsigned short WORD;

#define SDA_IN() {GPIO_InitTypeDef GPIO_InitStruct = {0};GPIO_InitStruct.Pin = GPIO_PIN_7;GPIO_InitStruct.Mode = GPIO_MODE_INPUT;HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);}//PB7输入模式
#define SDA_OUT() {GPIO_InitTypeDef GPIO_InitStruct = {0};GPIO_InitStruct.Pin = GPIO_PIN_7;GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;GPIO_InitStruct.Pull = GPIO_NOPULL;GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);}	//PB7输出模式
//经过测试，以上两句对引脚IO属性的改变宏定义绝对没问题，放心用
//卧槽，不对，SDA_OUT好像有问题
//解决了，可以方向用了
void SCL( uint8_t x );
void SDA( uint8_t x );
void Single_Write_ADXL345(uchar REG_Address,uchar REG_data);
uchar Single_Read_ADXL345(uchar REG_Address);
void Multiple_read_ADXL345(void);
void Init_ADXL345();
int adxl_display();