/*
 * dspw_hw.c
 *
 *  Created on: Mar 18, 2019
 *      Author: Swapna
 */
/*
 * dspw_hw.c
 *
 *  Created on: Jan 29, 2018
 *      Author: Swapna
 */

#include "dspw_hw.h"
#include "dspw_led.h"
#include "hw.h"
#include "console.h"

static void DSPW_RadioFE_Init();
static void DSPW_Unused_GPIO_Init();

void DSPW_HW_Deinit()
{
    DSPW_RadioFE_Off();
}

void DSPW_HW_Init()
{
    DSPW_RadioFE_Init();
    DSPW_Unused_GPIO_Init();

    console_init();
}

void DSPW_RadioFE_Off()
{
    HW_GPIO_Write( RADIO_ANT_SWITCH_POWER_PORT, RADIO_ANT_SWITCH_POWER_PIN, 0);
}

void DSPW_RadioFE_On()
{
    HW_GPIO_Write( RADIO_ANT_SWITCH_POWER_PORT, RADIO_ANT_SWITCH_POWER_PIN, 1);
}

static void DSPW_RadioFE_Init()
{
    GPIO_InitTypeDef initStruct={0};

    initStruct.Mode = GPIO_MODE_OUTPUT_PP;
    initStruct.Pull = GPIO_NOPULL;
    initStruct.Speed = GPIO_SPEED_HIGH;

    HW_GPIO_Init(RADIO_ANT_SWITCH_POWER_PORT, RADIO_ANT_SWITCH_POWER_PIN, &initStruct);
}

static void DSPW_Unused_GPIO_Init()
{
    GPIO_InitTypeDef initStruct={0};

    initStruct.Mode = GPIO_MODE_ANALOG;
    initStruct.Pull = GPIO_NOPULL;

#ifdef UNUSED_PORT_A_PINS
    __GPIOA_CLK_ENABLE();
    HW_GPIO_Init( GPIOA, UNUSED_PORT_A_PINS, &initStruct );
#endif
#ifdef UNUSED_PORT_B_PINS
    __GPIOB_CLK_ENABLE();
    HW_GPIO_Init( GPIOB, UNUSED_PORT_B_PINS, &initStruct );
#endif
}
