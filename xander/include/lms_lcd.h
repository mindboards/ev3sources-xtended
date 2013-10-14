#ifndef LMS_LCD_H_
#define LMS_LCD_H_

#define SCREEN_WIDTH 178
#define SCREEN_HEIGHT 128

#define SMALL_FONT_CELL_WIDTH 8
#define SMALL_FONT_CELL_HEIGHT 8
#define TINY_FONT_CELL_WIDTH 8
#define TINY_FONT_CELL_HEIGHT 8


/**
 * Common raster operations for use with lcdBitBltLCD
 */
#define RASTER_OP_CLEAR         0x00000000L
#define RASTER_OP_AND           0xFF000000L
#define RASTER_OP_ANDREVERSE    0xFF00FF00L
#define RASTER_OP_COPY          0x0000FF00L
#define RASTER_OP_ANDINVERTED   0xFFFF0000L
#define RASTER_OP_NOOP          0x00FF0000L
#define RASTER_OP_XOR           0x00FFFF00L
#define RASTER_OP_OR            0xFFFFFF00L
#define RASTER_OP_NOR           0xFFFFFFFFL
#define RASTER_OP_EQUIV         0x00FFFFFFL
#define RASTER_OP_INVERT        0x00FF00FFL
#define RASTER_OP_ORREVERSE     0xFFFF00FFL
#define RASTER_OP_COPYINVERTED  0x0000FFFFL
#define RASTER_OP_ORINVERTED    0xFF00FFFFL
#define RASTER_OP_NAND          0xFF0000FFL
#define RASTER_OP_SET           0x000000FFL

char displayBuf[(SCREEN_HEIGHT)*(SCREEN_WIDTH+7)/8];

#define LCD_MEM_WIDTH 60 // width of HW Buffer in bytes
#define LCD_BUFFER_LENGTH (LCD_MEM_WIDTH * SCREEN_HEIGHT)
#define FB_DEV "/dev/fb0"

char hwBuffer[LCD_BUFFER_LENGTH];
char *lcd_buff;

void lcdBitBltLCD(char *sourceBuffer, int sourceWidth, int sourceHeight, int sourceX, int sourceY, int destX, int destY, int copyWidth, int copyHeight, int rasterOp);
void lcdDrawCharSmall(char c, int x, int y);
void lcdDrawStringSmall(char *str, int x, int y);
void lcdClearScreen();
void lcdFillScreen();
void lcdSetPixel(int x, int y, int color);
void lcdBitBlt(char *sourceBuffer, int sourceWidth, int sourceHeight, int sourceX, int sourceY, char *destBuffer, int destWidth, int destHeight, int destX, int destY, int copyWidth, int copyHeight, int rasterOp);
char *lcdGetDisplayBufferHandle();
void lcdUpdateScreen();
void lcdInit();

#endif // LMS_LCD_H_
