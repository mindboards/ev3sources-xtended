#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "lms_lcd.h"
#include "tiny_font.xbm"
#include "small_font.xbm"


/**
 * Standard two input BitBlt function with the LCD display as the
 * destination. Supports standard raster ops and
 * overlapping images.
 * @param sourceBuffer byte array containing the source image
 * @param sourceWidth Width of the source image
 * @param sourceHeight Height of the source image
 * @param sourceX X position to start the copy from
 * @param sourceY Y Position to start the copy from
 * @param destX X destination
 * @param destY Y destination
 * @param copyWidth width of the area to copy
 * @param copyHeight height of the area to copy
 * @param rasterOp raster operation.
 */
void lcdBitBltLCD(char *sourceBuffer, int sourceWidth, int sourceHeight, int sourceX, int sourceY, int destX, int destY, int copyWidth, int copyHeight, int rasterOp)
{
  lcdBitBlt(sourceBuffer, sourceWidth, sourceHeight, sourceX, sourceY, displayBuf, SCREEN_WIDTH, SCREEN_HEIGHT, destX, destY, copyWidth, copyHeight, rasterOp);
}

/**
 * Draw a single char on the LCD at specified x,y co-ordinate.
 * @param c Character to display
 * @param x X location
 * @param y Y location
 */
void lcdDrawCharSmall(char c, int x, int y)
{
//  int cell_height = 8;
//  int cell_width = 8;

  lcdBitBltLCD(small_font_bits, small_font_width, small_font_height, SMALL_FONT_CELL_WIDTH * ((c - 32) % 16), SMALL_FONT_CELL_HEIGHT * ((c - 32) / 16), x, y, SMALL_FONT_CELL_WIDTH, TINY_FONT_CELL_HEIGHT, RASTER_OP_COPY);
}


/**
 * Draw a single char on the LCD at specified x,y co-ordinate.
 * @param c Character to display
 * @param x X location
 * @param y Y location
 */
void lcdDrawCharTiny(char c, int x, int y)
{
  lcdBitBltLCD(tiny_font_bits, tiny_font_width, tiny_font_height, TINY_FONT_CELL_WIDTH * ((c - 32) % 16), TINY_FONT_CELL_HEIGHT * ((c - 32) / 16), x, y, TINY_FONT_CELL_WIDTH, TINY_FONT_CELL_HEIGHT, RASTER_OP_COPY);
}


/**
 * Display a string on the LCD at specified x,y co-ordinate.
 *
 * @param str The string to be displayed
 * @param x The x character co-ordinate to display at.
 * @param y The y character co-ordinate to display at.
 */
void lcdDrawStringSmall(char *str, int x, int y)
{
  int i;
  int len = strlen(str);

  // Draw the background rect
  lcdBitBltLCD(NULL, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, x, y, strlen(str) * SMALL_FONT_CELL_WIDTH, SMALL_FONT_CELL_HEIGHT, RASTER_OP_CLEAR);
  // and the characters
  for(i = 0; i < len; i++) {
    lcdDrawCharSmall(str[i], x + (SMALL_FONT_CELL_WIDTH * i), y);
  }
}


/**
 * Display a string on the LCD at specified x,y co-ordinate.
 *
 * @param str The string to be displayed
 * @param x The x character co-ordinate to display at.
 * @param y The y character co-ordinate to display at.
 */
void lcdDrawStringTiny(char *str, int x, int y)
{
  int i;
  int len = strlen(str);

  // Draw the background rect
  lcdBitBltLCD(NULL, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, x, y, strlen(str) * TINY_FONT_CELL_WIDTH, TINY_FONT_CELL_HEIGHT, RASTER_OP_CLEAR);
  // and the characters
  for(i = 0; i < len; i++) {
    lcdDrawCharSmall(str[i], x + (TINY_FONT_CELL_WIDTH * i), y);
  }
}

/**
 * Clear the display.
 */
void lcdClearScreen()
{
  lcdBitBltLCD(NULL, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, RASTER_OP_CLEAR);
}

/**
 * Fill the display.
 */
void lcdFillScreen()
{
  lcdBitBltLCD(NULL, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, RASTER_OP_SET);
}


/**
 * Set a pixel on the screen.
 * @param x the x coordinate
 * @param y the y coordinate
 * @param color the pixel color (0 = white, 1 = black)
 */
void lcdSetPixel(int x, int y, int color)
{
  lcdBitBlt(displayBuf, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, displayBuf, SCREEN_WIDTH, SCREEN_HEIGHT, x, y, 1, 1,(color == 1 ? RASTER_OP_SET : RASTER_OP_CLEAR));
}


/**
 * Standard two input BitBlt function. Supports standard raster ops and
 * overlapping images.
 * @param sourceBuffer byte array containing the source image
 * @param sourceWidth Width of the source image
 * @param sourceHeight Height of the source image
 * @param sourceX X position to start the copy from
 * @param sourceY Y Position to start the copy from
 * @param destBuffer byte array containing the destination image
 * @param destWidth Width of the destination image
 * @param destHeight Height of the destination image
 * @param destX X destination
 * @param destY Y destination
 * @param copyWidth width of the area to copy
 * @param copyHeight height of the area to copy
 * @param rasterOp raster operation.
 */
void lcdBitBlt(char *sourceBuffer, int sourceWidth, int sourceHeight, int sourceX, int sourceY, char *destBuffer, int destWidth, int destHeight, int destX, int destY, int copyWidth, int copyHeight, int rasterOp)
{
  // This is a partial implementation of the BitBlt algorithm.

  int trim;
  int xinc;
  int yinc;
  char firstBit;

  int sourceWidthBytes;
  int destWidthBytes;
  int inStart;
  int outStart;
  char inStartBit;
  char outStartBit;

  char ca1;
  char cx1;
  char ca2;
  char cx2;
  char noDst;
  int ycnt;

  int ix;
  int ox;
  int byteCnt;

  int inIndex;
  int outIndex;
  int cnt;

  char inBit;
  char inVal;
  char inAnd;
  char inXor;
  char outBit;
  char outPixels;

  char resBit;

  if (destX < 0)
  {
    trim = -destX;
    destX = 0;
    sourceX += trim;
    copyWidth -= trim;
  }

  if (destY < 0)
  {
    trim = -destY;
    destY = 0;
    sourceY += trim;
    copyHeight -= trim;
  }

  if (sourceX < 0 || sourceY < 0)
    return;

  if (destX + copyWidth > destWidth)
    copyWidth = destWidth - destX;

  if (sourceX + copyWidth > sourceWidth)
    copyWidth = sourceWidth - sourceX;

  if (copyWidth <= 0)
    return;
  if (destY + copyHeight > destHeight)
    copyHeight = destHeight - destY;

  if (sourceY + copyHeight > sourceHeight)
    copyHeight = sourceHeight - sourceY;

  if (copyHeight <= 0)
    return;

  // Setup initial parameters and check for overlapping copy
  xinc = 1;
  yinc = 1;
  firstBit = 1;

  if (sourceBuffer == destBuffer)
  {
    // If copy overlaps we use reverse direction
    if (destY > sourceY)
    {
      sourceY = sourceY + copyHeight - 1;
      destY = destY + copyHeight - 1;
      yinc = -1;
    }
    if (destX > sourceX)
    {
      firstBit = 0x80;
      xinc = -1;
      sourceX = sourceX + copyWidth - 1;
      destX = destX + copyWidth - 1;
    }
  }

  if (sourceBuffer == NULL)
      sourceBuffer = destBuffer;

  sourceWidthBytes = (sourceWidth + 7) / 8;
  destWidthBytes = (destWidth + 7) / 8;
  inStart = sourceY * sourceWidthBytes;
  outStart = destY * destWidthBytes;
  inStartBit = (1 << (sourceX & 0x7));
  outStartBit = (1 << (destX & 0x7));
  destWidthBytes *= yinc;
  sourceWidthBytes *= yinc;

  // Extract rop sub-fields
  ca1 = (rasterOp >> 24);
  cx1 = (rasterOp >> 16);
  ca2 = (rasterOp >>  8);
  cx2 = rasterOp;
  noDst = (ca1 == 0) && (cx1 == 0);

  // Check for byte aligned case and optimise for it
  if (copyWidth >= 8 && inStartBit == firstBit && outStartBit == firstBit)
  {
    ix = sourceX/8;
    ox = destX/8;
    byteCnt = copyWidth/8;
    ycnt = copyHeight;

    while (ycnt-- > 0)
    {
      inIndex = inStart + ix;
      outIndex = outStart + ox;
      cnt = byteCnt;
      while(cnt-- > 0)
      {
        if (noDst)
          destBuffer[outIndex] = ((sourceBuffer[inIndex] & ca2)^cx2);
        else
        {
          inVal = sourceBuffer[inIndex];
          destBuffer[outIndex] = ((destBuffer[outIndex] & ((inVal & ca1)^cx1)) ^ ((inVal & ca2)^cx2));
        }
        outIndex += xinc;
        inIndex += xinc;
      }
      ix += sourceWidthBytes;
      ox += destWidthBytes;
    }
    // Do we have a final non byte multiple to do?
    copyWidth &= 0x7;
    if (copyWidth == 0)
        return;

    sourceX += byteCnt * 8;
    destX += byteCnt * 8;
  }
  // General non byte aligned case
  ix = sourceX / 8;
  ox = destX / 8;
  ycnt = copyHeight;

  while(ycnt-- > 0)
  {
    inIndex = inStart + ix;
    inBit = inStartBit;
    inVal = sourceBuffer[inIndex];
    inAnd = (inVal & ca1)^cx1;
    inXor = (inVal & ca2)^cx2;
    outIndex = outStart + ox;
    outBit = outStartBit;
    outPixels = destBuffer[outIndex];
    cnt = copyWidth;

    while(1 == 1)
    {
      if (noDst)
      {
        if ((inXor & inBit) != 0)
          outPixels |= outBit;
        else
          outPixels &= ~outBit;
      }
      else
      {
        resBit = (outPixels & ((inAnd & inBit) != 0 ? outBit : 0))^((inXor & inBit) != 0 ? outBit : 0);
        outPixels = (outPixels & ~outBit) | resBit;
      }

      if (--cnt <= 0)
        break;

      if (xinc > 0)
      {
        inBit <<= 1;
        outBit <<= 1;
      }
      else
      {
        inBit >>= 1;
        outBit >>= 1;
      }
      if (inBit == 0)
      {
        inBit = firstBit;
        inIndex += xinc;
        inVal = sourceBuffer[inIndex];
        inAnd = (inVal & ca1)^cx1;
        inXor = (inVal & ca2)^cx2;
      }
      if (outBit == 0)
      {
        destBuffer[outIndex] = outPixels;
        outBit = firstBit;
        outIndex += xinc;
        outPixels = destBuffer[outIndex];
      }
    }
    destBuffer[outIndex] = outPixels;
    inStart += sourceWidthBytes;
    outStart += destWidthBytes;
  }
}

char *lcdGetDisplayBufferHandle()
{
  return displayBuf;
}

/**
 * Update the hardware display contents from the internal display bitmap.
 */
void lcdUpdateScreen()
{
  char convert[] = {
          0x00, // 000 00000000
          0xE0, // 001 11100000
          0x1C, // 010 00011100
          0xFC, // 011 11111100
          0x03, // 100 00000011
          0xE3, // 101 11100011
          0x1F, // 110 00011111
          0xFF  // 111 11111111
  };

  int inOffset = 0;
  int outOffset = 0;
  int row = 0;
  int i;
  int pixels;
  for(row = 0; row < SCREEN_HEIGHT; row++)
  {
    for(i = 0; i < 7; i++)
    {
      pixels = displayBuf[inOffset++] & 0xff;
      pixels |= (displayBuf[inOffset++] & 0xff) << 8;
      pixels |= (displayBuf[inOffset++] & 0xff) << 16;

      hwBuffer[outOffset++] = convert[pixels & 0x7];
      pixels >>= 3;
      hwBuffer[outOffset++] = convert[pixels & 0x7];
      pixels >>= 3;
      hwBuffer[outOffset++] = convert[pixels & 0x7];
      pixels >>= 3;
      hwBuffer[outOffset++] = convert[pixels & 0x7];
      pixels >>= 3;
      hwBuffer[outOffset++] = convert[pixels & 0x7];
      pixels >>= 3;
      hwBuffer[outOffset++] = convert[pixels & 0x7];
      pixels >>= 3;
      hwBuffer[outOffset++] = convert[pixels & 0x7];
      pixels >>= 3;
      hwBuffer[outOffset++] = convert[pixels & 0x7];
    }
    pixels = displayBuf[inOffset++] & 0xff;
    pixels |= (displayBuf[inOffset++] & 0xff) << 8;
    hwBuffer[outOffset++] = convert[pixels & 0x7];
    pixels >>= 3;
    hwBuffer[outOffset++] = convert[pixels & 0x7];
    pixels >>= 3;
    hwBuffer[outOffset++] = convert[pixels & 0x7];
    pixels >>= 3;
    hwBuffer[outOffset++] = convert[pixels & 0x7];
  }
  memcpy(lcd_buff, hwBuffer, LCD_BUFFER_LENGTH);
}


void      lcdInit()
{
  int fb_dev = open(FB_DEV, O_RDWR);
  if (fb_dev < 0)
    fprintf(stderr, "LCD_DEVICE_FILE_NOT_FOUND\n");

  lcd_buff =  mmap(0, LCD_BUFFER_LENGTH, PROT_WRITE | PROT_READ, MAP_SHARED, fb_dev, 0);
  if (lcd_buff == MAP_FAILED)
    fprintf(stderr, "LCD_DEVICE_FILE_NOT_FOUND\n");
}





