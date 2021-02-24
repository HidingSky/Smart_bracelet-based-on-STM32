#include "main.h"
#include "OLED.h"
#include "OLEDFONT.h"
uint8_t OLED_GRAM[128][8];	

void OLED_SCLK( uint8_t x )
{
	if( x==1 )
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET);
}

void OLED_SDIN( uint8_t x )
{
	if( x==0 )
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);//此处设计很妙，因为传过来的非零数不一定只有1
	else
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
}

void OLED_RES( uint8_t x )
{
	if( x==1 )
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);
}

void OLED_DC( uint8_t x )
{
	if( x==1 )
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET);
}

void OLED_CS( uint8_t x )
{
	if( x==1 )
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
}


//往下就是基于以上基础函数的OLED操作命令了
void OLED_WR_Cmd( uint8_t cmd )
{
	uint8_t i,k;
	/* 拉低片选CS，写命令拉低DC */
	OLED_CS(0);
	OLED_DC(0);
	
	for( i=0;i<8;i++ )
	{
		/* 时钟线，上升沿有效 */
		OLED_SCLK(0);
		k=cmd&(0x80);
		OLED_SDIN(k);
		OLED_SCLK(1);
		cmd<<=1;
	}
	OLED_CS(1);
	OLED_DC(1);	
}


/* 写一个数据 高位先行 */
void OLED_WR_Data( uint8_t data )
{
	uint8_t i,k;
	/* 拉低片选CS，写数据拉高DC */
	OLED_CS(0);
	OLED_DC(1);
	
	for( i=0;i<8;i++ )
	{
		/* 时钟线，上升沿有效 */
		OLED_SCLK(0);
		k=data&(0x80);
		OLED_SDIN(k);
		OLED_SCLK(1);
		data<<=1;
	}
	OLED_CS(1);
	OLED_DC(1);
}

/* 图像刷新函数 */
void OLED_Refresh_GRAM( void )
{
	uint8_t i,j;
	for( i=0;i<8;i++ )
	{
		/* 设置显示的起始地址 */
		OLED_WR_Cmd(0xB0+i);//设置页地址（行）
		OLED_WR_Cmd(0x00);//设置列地址的低四位
		OLED_WR_Cmd(0x10);//设置列地址的高四位
		for( j=0;j<128;j++ )
		{
			OLED_WR_Data(OLED_GRAM[j][i]);//将GRAM中图像信息写入屏幕
		}
	}
}
/* 清屏函数 */
void OLED_Clear( void )
{
	uint8_t i,j;
	for( i=0;i<8;i++ )
		for( j=0;j<128;j++ )
			OLED_GRAM[j][i]=0x00;
	OLED_Refresh_GRAM();
}



/* OLED初始化函数 */
void OLED_Init( void )
{
	/* 引脚初始化 */
	
	OLED_CS(1);
	OLED_DC(1);
	
	/* 复位 */
	OLED_RES(0);
	HAL_Delay(1000);
	OLED_RES(1);
	
	/* 开始写入初始化命令 */
	OLED_WR_Cmd(0xAE);//关闭显示
	OLED_WR_Cmd(0xD5);//设置时钟分频因子
	OLED_WR_Cmd(80);
	
	OLED_WR_Cmd(0xA8);//设置驱动路数
	OLED_WR_Cmd(0x3F);//路数默认0x3F（1/64）
	
	OLED_WR_Cmd(0xD3);//设置显示偏移
	OLED_WR_Cmd(0x00);//偏移默认为0
	
	OLED_WR_Cmd(0x40);//设置显示开始行[5:0]
	
	OLED_WR_Cmd(0x8D);//电荷泵设置
	OLED_WR_Cmd(0x14);//bit2，开启/关闭
	
	OLED_WR_Cmd(0x20);//设置内存地址模式
	OLED_WR_Cmd(0x02);//[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	
	OLED_WR_Cmd(0xA1);//段重定义设置,bit0:0,0->0;1,0->127;
	OLED_WR_Cmd(0xC0);//设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	OLED_WR_Cmd(0xDA);//设置COM硬件引脚配置
	OLED_WR_Cmd(0x12);//[5:4]配置
	
	OLED_WR_Cmd(0x81);//对比度设置
	OLED_WR_Cmd(0xEF);//默认0x7F（范围1~255，越大越亮）
	
	OLED_WR_Cmd(0xD9);//设置预充电周期
	OLED_WR_Cmd(0xF1);//[3:0],PHASE 1;[7:4],PHASE 2;
	
	OLED_WR_Cmd(0xDB);//设置VCOMH 电压倍率
	OLED_WR_Cmd(0x30);//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
	
	OLED_WR_Cmd(0xA4);//全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	OLED_WR_Cmd(0xA6);//设置显示方式;bit0:1,反相显示;0,正常显示
	
	OLED_WR_Cmd(0xAF);//开启显示
	
	/* 清屏函数 */
	OLED_Clear();
}







//OLED_GRAM[128][8]
/* 画点函数，以屏幕像素点为单位，以左上角为原点 x:0~127 y:0~63 
（x,y）坐标换算:OLED_GRAM[x][7-y/8]|=1<<(7-y%8)；

mode取1正显，取0反显
*/
void OLED_DrawPoint( uint8_t x,uint8_t y,uint8_t mode )
{
	/* 主要是求出y坐标，根据y坐标来位运算OLED_GRAM中相应的位 */
	uint8_t i,j,temp;
	/* 判断坐标是否超出范围 */
	if(x>127||y>63)return;
	
	i=7-y/8;//算出第几页
	j=y%8;
	temp=0x01<<(7-j);//由位运算精确找出坐标像素点
	if(mode==0)
		OLED_GRAM[x][i]&=~temp;
	else
		OLED_GRAM[x][i]|=temp;
}



/* 在（x,y）坐标正显/反显指定大小字符chr
mode：0是反显，1是正常显示
size：12/16/24
ASCII字符集: !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
*/
void OLED_Show_Char( uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode )
{
	/* temp是当前对应的一个字节的点集，y0是初始坐标 */
	uint8_t temp,t,t1;
	uint8_t y0=y;
	/* csize是单个字符所占字节的多少 */
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);
	/* 求出偏移的地址差，即得到该字符在字库中的序号（从零开始） */
	/* chr会锁定字符在字库中的的序号 */
	chr=chr-' ';
	
	/* 相应字符点集有多少个字节 */
	for( t=0;t<csize;t++ )
	{
		/* 根据字符的大小选择相应字库，根据chr得到具体的字符地址 */
		switch( size )
		{
			case 12:temp=asc2_1206[chr][t];break;//12x6(行x列)
			case 16:temp=asc2_1608[chr][t];break;//16x8
			case 24:temp=asc2_2412[chr][t];break;//24x12
			default:return;//没有相应字库
		}
		
		/* 画出每一列的点 */
		for( t1=0;t1<8;t1++ )
		{
			if( temp&0x80 )
				OLED_DrawPoint( x,y,mode );
			else
				OLED_DrawPoint( x,y,!mode );
			
			temp<<=1;
			y++;
			
			/* 根据给出每一列的像素点的多少，判断列是否满：
			满了就换列，不满继续在此列画点 （与字库画点原理相关）			
			*/
			if( ( y-y0 )==size )
			{
				y=y0;
				x++;
				/* 一列满，跳出循环，直接转到下一个字节点集的打印 */
				break;
			}
		}
	}
}




/* （x,y）是显示的坐标，*p是字符串的首地址，size是字符点集大小 */
void OLED_Show_String( uint8_t x, uint8_t y, const uint8_t *p, uint8_t size, uint8_t mode )
{
		/* 判断是否合法字符，同时也限定了范围 */
	while( (*p<='~')&&(*p>=' ') )
	{
		/* 如果初始行放不下，移动到下一行 */
		if( x>(128-(size/2)) )
		{
			x=0;
			y=y+size;
		}
		if( y>(64-size) )
		{
			x=y=0;
			OLED_Clear();
		}
		
		OLED_Show_Char( x,y,*p,size,mode );
		/* 移动到下一个字符位置，size/2是因为做点集时就是：行X列，而且 行=2X列，所以size就是行数 */
		x=x+size/2;
		p++;
	}
}

