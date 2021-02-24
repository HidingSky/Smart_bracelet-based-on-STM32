#include "main.h"
void OLED_SCLK( uint8_t x );
void OLED_SDIN( uint8_t x );
void OLED_RES( uint8_t x );
void OLED_DC( uint8_t x );
void OLED_CS( uint8_t x );
void OLED_WR_Cmd( uint8_t cmd );
void OLED_WR_Data( uint8_t data );
void OLED_Refresh_GRAM( void );
void OLED_Clear( void );
void OLED_Init( void );
void OLED_DrawPoint( uint8_t x,uint8_t y,uint8_t mode );
void OLED_Show_Char( uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode );
void OLED_Show_String( uint8_t x, uint8_t y, const uint8_t *p, uint8_t size, uint8_t mode );