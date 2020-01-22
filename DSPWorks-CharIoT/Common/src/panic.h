/*
 * panic.h
 *
 *  Created on: Jan 30, 2018
 *      Author: Swapna
 */

#ifndef _PANIC_H_
#define _PANIC_H_

#include <stdint.h>
#include "dspw_led.h"
#include "app.h"
#include "dspw_hw_conf.h"

extern void panic(uint32_t id, uint32_t location);

#endif /* _PANIC_H_ */
