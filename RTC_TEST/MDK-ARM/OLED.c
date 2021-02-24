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
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);//�˴���ƺ����Ϊ�������ķ�������һ��ֻ��1
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


//���¾��ǻ������ϻ���������OLED����������
void OLED_WR_Cmd( uint8_t cmd )
{
	uint8_t i,k;
	/* ����ƬѡCS��д��������DC */
	OLED_CS(0);
	OLED_DC(0);
	
	for( i=0;i<8;i++ )
	{
		/* ʱ���ߣ���������Ч */
		OLED_SCLK(0);
		k=cmd&(0x80);
		OLED_SDIN(k);
		OLED_SCLK(1);
		cmd<<=1;
	}
	OLED_CS(1);
	OLED_DC(1);	
}


/* дһ������ ��λ���� */
void OLED_WR_Data( uint8_t data )
{
	uint8_t i,k;
	/* ����ƬѡCS��д��������DC */
	OLED_CS(0);
	OLED_DC(1);
	
	for( i=0;i<8;i++ )
	{
		/* ʱ���ߣ���������Ч */
		OLED_SCLK(0);
		k=data&(0x80);
		OLED_SDIN(k);
		OLED_SCLK(1);
		data<<=1;
	}
	OLED_CS(1);
	OLED_DC(1);
}

/* ͼ��ˢ�º��� */
void OLED_Refresh_GRAM( void )
{
	uint8_t i,j;
	for( i=0;i<8;i++ )
	{
		/* ������ʾ����ʼ��ַ */
		OLED_WR_Cmd(0xB0+i);//����ҳ��ַ���У�
		OLED_WR_Cmd(0x00);//�����е�ַ�ĵ���λ
		OLED_WR_Cmd(0x10);//�����е�ַ�ĸ���λ
		for( j=0;j<128;j++ )
		{
			OLED_WR_Data(OLED_GRAM[j][i]);//��GRAM��ͼ����Ϣд����Ļ
		}
	}
}
/* �������� */
void OLED_Clear( void )
{
	uint8_t i,j;
	for( i=0;i<8;i++ )
		for( j=0;j<128;j++ )
			OLED_GRAM[j][i]=0x00;
	OLED_Refresh_GRAM();
}



/* OLED��ʼ������ */
void OLED_Init( void )
{
	/* ���ų�ʼ�� */
	
	OLED_CS(1);
	OLED_DC(1);
	
	/* ��λ */
	OLED_RES(0);
	HAL_Delay(1000);
	OLED_RES(1);
	
	/* ��ʼд���ʼ������ */
	OLED_WR_Cmd(0xAE);//�ر���ʾ
	OLED_WR_Cmd(0xD5);//����ʱ�ӷ�Ƶ����
	OLED_WR_Cmd(80);
	
	OLED_WR_Cmd(0xA8);//��������·��
	OLED_WR_Cmd(0x3F);//·��Ĭ��0x3F��1/64��
	
	OLED_WR_Cmd(0xD3);//������ʾƫ��
	OLED_WR_Cmd(0x00);//ƫ��Ĭ��Ϊ0
	
	OLED_WR_Cmd(0x40);//������ʾ��ʼ��[5:0]
	
	OLED_WR_Cmd(0x8D);//��ɱ�����
	OLED_WR_Cmd(0x14);//bit2������/�ر�
	
	OLED_WR_Cmd(0x20);//�����ڴ��ַģʽ
	OLED_WR_Cmd(0x02);//[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	
	OLED_WR_Cmd(0xA1);//���ض�������,bit0:0,0->0;1,0->127;
	OLED_WR_Cmd(0xC0);//����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	OLED_WR_Cmd(0xDA);//����COMӲ����������
	OLED_WR_Cmd(0x12);//[5:4]����
	
	OLED_WR_Cmd(0x81);//�Աȶ�����
	OLED_WR_Cmd(0xEF);//Ĭ��0x7F����Χ1~255��Խ��Խ����
	
	OLED_WR_Cmd(0xD9);//����Ԥ�������
	OLED_WR_Cmd(0xF1);//[3:0],PHASE 1;[7:4],PHASE 2;
	
	OLED_WR_Cmd(0xDB);//����VCOMH ��ѹ����
	OLED_WR_Cmd(0x30);//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
	
	OLED_WR_Cmd(0xA4);//ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	OLED_WR_Cmd(0xA6);//������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ
	
	OLED_WR_Cmd(0xAF);//������ʾ
	
	/* �������� */
	OLED_Clear();
}







//OLED_GRAM[128][8]
/* ���㺯��������Ļ���ص�Ϊ��λ�������Ͻ�Ϊԭ�� x:0~127 y:0~63 
��x,y�����껻��:OLED_GRAM[x][7-y/8]|=1<<(7-y%8)��

modeȡ1���ԣ�ȡ0����
*/
void OLED_DrawPoint( uint8_t x,uint8_t y,uint8_t mode )
{
	/* ��Ҫ�����y���꣬����y������λ����OLED_GRAM����Ӧ��λ */
	uint8_t i,j,temp;
	/* �ж������Ƿ񳬳���Χ */
	if(x>127||y>63)return;
	
	i=7-y/8;//����ڼ�ҳ
	j=y%8;
	temp=0x01<<(7-j);//��λ���㾫ȷ�ҳ��������ص�
	if(mode==0)
		OLED_GRAM[x][i]&=~temp;
	else
		OLED_GRAM[x][i]|=temp;
}



/* �ڣ�x,y����������/����ָ����С�ַ�chr
mode��0�Ƿ��ԣ�1��������ʾ
size��12/16/24
ASCII�ַ���: !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
*/
void OLED_Show_Char( uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode )
{
	/* temp�ǵ�ǰ��Ӧ��һ���ֽڵĵ㼯��y0�ǳ�ʼ���� */
	uint8_t temp,t,t1;
	uint8_t y0=y;
	/* csize�ǵ����ַ���ռ�ֽڵĶ��� */
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);
	/* ���ƫ�Ƶĵ�ַ����õ����ַ����ֿ��е���ţ����㿪ʼ�� */
	/* chr�������ַ����ֿ��еĵ���� */
	chr=chr-' ';
	
	/* ��Ӧ�ַ��㼯�ж��ٸ��ֽ� */
	for( t=0;t<csize;t++ )
	{
		/* �����ַ��Ĵ�Сѡ����Ӧ�ֿ⣬����chr�õ�������ַ���ַ */
		switch( size )
		{
			case 12:temp=asc2_1206[chr][t];break;//12x6(��x��)
			case 16:temp=asc2_1608[chr][t];break;//16x8
			case 24:temp=asc2_2412[chr][t];break;//24x12
			default:return;//û����Ӧ�ֿ�
		}
		
		/* ����ÿһ�еĵ� */
		for( t1=0;t1<8;t1++ )
		{
			if( temp&0x80 )
				OLED_DrawPoint( x,y,mode );
			else
				OLED_DrawPoint( x,y,!mode );
			
			temp<<=1;
			y++;
			
			/* ���ݸ���ÿһ�е����ص�Ķ��٣��ж����Ƿ�����
			���˾ͻ��У����������ڴ��л��� �����ֿ⻭��ԭ����أ�			
			*/
			if( ( y-y0 )==size )
			{
				y=y0;
				x++;
				/* һ����������ѭ����ֱ��ת����һ���ֽڵ㼯�Ĵ�ӡ */
				break;
			}
		}
	}
}




/* ��x,y������ʾ�����꣬*p���ַ������׵�ַ��size���ַ��㼯��С */
void OLED_Show_String( uint8_t x, uint8_t y, const uint8_t *p, uint8_t size, uint8_t mode )
{
		/* �ж��Ƿ�Ϸ��ַ���ͬʱҲ�޶��˷�Χ */
	while( (*p<='~')&&(*p>=' ') )
	{
		/* �����ʼ�зŲ��£��ƶ�����һ�� */
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
		/* �ƶ�����һ���ַ�λ�ã�size/2����Ϊ���㼯ʱ���ǣ���X�У����� ��=2X�У�����size�������� */
		x=x+size/2;
		p++;
	}
}

