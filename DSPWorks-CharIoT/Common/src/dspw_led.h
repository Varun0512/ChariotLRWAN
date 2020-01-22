/*
 * dspw_led.h
 *
 *  Created on: Mar 20, 2019
 *      Author: Swapna
 */

#ifndef _DSPW_LED_H_
#define _DSPW_LED_H_

#include "hw.h"
#include <stdbool.h>

typedef enum
{
#if DSPW_LED_NUM > 0
    DSPW_LED_1,
#endif
#if DSPW_LED_NUM > 1
    DSPW_LED_2,
#endif
#if DSPW_LED_NUM > 2
    DSPW_LED_3,
#endif
#if DSPW_LED_NUM > 3
    DSPW_LED_4,
#endif
    DSPW_LED_INVALID
}Dspw_Led_t;

extern void DSPW_Led_Init();
extern bool DSPW_Led_IsBlinking(Dspw_Led_t led);
extern void DSPW_Led_Off(Dspw_Led_t led);
extern void DSPW_Led_On(Dspw_Led_t led);
extern void DSPW_Led_StartBlinking(Dspw_Led_t led, uint32_t onTime, uint32_t offTime, uint32_t timeOut);
extern void DSPW_Led_StopBlinking(Dspw_Led_t led);

#endif /* _DSPW_LED_H_ */
