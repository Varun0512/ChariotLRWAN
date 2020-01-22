/*
 * panic.c
 *
 *  Created on: Mar 20, 2019
 *      Author: Swapna
 */
/*
 * panic.c
 *
 *  Created on: Jan 30, 2018
 *      Author: Swapna
 */

#include "panic.h"

void panic(uint32_t id, uint32_t location)
{
    DSPW_Led_On(DSPW_LED_RED);
    DSPW_Led_On(DSPW_LED_BLUE);
    while(1);
}
