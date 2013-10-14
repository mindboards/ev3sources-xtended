/*
 * lms_buttons.h
 *
 *  Created on: Sep 16, 2013
 *      Author: xander
 */

#ifndef LMS_BUTTONS_H_
#define LMS_BUTTONS_H_

typedef   enum
{
  LED_BLACK                     = '0',
  LED_GREEN                     = '1',
  LED_RED                       = '2',
  LED_ORANGE                    = '3',
  LED_GREEN_FLASH               = '4',
  LED_RED_FLASH                 = '5',
  LED_ORANGE_FLASH              = '6',
  LED_GREEN_PULSE               = '7',
  LED_RED_PULSE                 = '8',
  LED_ORANGE_PULSE              = '9'
} LEDPATTERN;

void buttonsInit();
void ledSet(LEDPATTERN pattern);
void buttonsRead(char *buttonState);
void buttonsReadDebounced(char *buttonState);

#endif /* LMS_BUTTONS_H_ */
