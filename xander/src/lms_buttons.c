/*
 * lms_buttons.c
 *
 *  Created on: Sep 16, 2013
 *      Author: xander
 */

#include <stdlib.h>
#include <sys/mman.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#include "lms2012.h"
#include "lms_buttons.h"

UI *pButtonState;
int uiFile;

void buttonsInit()
{
  uiFile = open(UI_DEVICE_NAME,O_RDWR | O_SYNC);
  UI *pTmp;

  if (uiFile >= MIN_HANDLE)
  {
    pTmp = (UI*)mmap(0, sizeof(UI), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, uiFile, 0);

    if (pTmp != MAP_FAILED)
      pButtonState =  pTmp;
  }
}

void ledSet(LEDPATTERN pattern)
{
  char buff[2] = {0, 0};
  buff[0] = pattern;
  if (uiFile >= MIN_HANDLE)
  {
    write(uiFile, buff, 2);
  }
}

void buttonsRead(char *buttonState)
{
  if (buttonState != NULL)
    memcpy(buttonState, pButtonState, sizeof(UI));
}

// To be called every 10 ms, will use a 40 ms debounce time.
void buttonsReadDebounced(char *buttonState)
{
  int counter;
  static uint8_t state[BUTTONS] = {0, 0, 0, 0, 0, 0};

  if (buttonState == NULL)
    return;

  for (counter = 0; counter < BUTTONS; counter++)
  {
    state[counter] = (state[counter] << 1) | 0xF0 | (~pButtonState->Pressed[counter] & 0x01);
    // 0xF8 = 1111 1000
    buttonState[counter] = (state[counter] == 0xF8) ? 1 : 0;
  }
}
