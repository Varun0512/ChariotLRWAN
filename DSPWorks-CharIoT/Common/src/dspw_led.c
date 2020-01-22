/*
 * dspw_led.c
 *
 *  Created on: Mar 20, 2019
 *      Author: Swapna
 */
/*
 * dspw_led.c
 *
 *  Created on: Jan 30, 2018
 *      Author: Swapna
 */

#include "dspw_led.h"
#include "panic.h"
#include "timeServer.h"
#include "dspw_hw_conf.h"

typedef enum
{
    DSPW_LED_OFF,
    DSPW_LED_ON
}Dspw_Led_State_t;

typedef struct
{
    Dspw_Led_State_t blinkingState;
    TimerEvent_t blinkTimer;
    bool isBlinking;
    uint32_t onTime;
    uint32_t offTime;
    uint16_t pin;
    GPIO_TypeDef* port;
    Dspw_Led_State_t state;
    uint32_t timeElapsed;
    uint32_t timeOut;
}Dspw_Led_Status_t;

#if DSPW_LED_NUM > 0
static Dspw_Led_Status_t _leds[DSPW_LED_NUM];
#endif

#if DSPW_LED_NUM > 0
static void Led1BlinkTimerCallback();
#endif

#if DSPW_LED_NUM > 1
static void Led2BlinkTimerCallback();
#endif

#if DSPW_LED_NUM > 2
static void Led3BlinkTimerCallback();
#endif

#if DSPW_LED_NUM > 3
static void Led4BlinkTimerCallback();
#endif

#if DSPW_LED_NUM > 0
static Dspw_Led_t FindLed(Dspw_Led_t led);
#endif

static void LedBlinkTimerCallback(Dspw_Led_t led);

void DSPW_Led_Init()
{
#if DSPW_LED_NUM > 0
    GPIO_InitTypeDef initStruct={0};

    initStruct.Mode = GPIO_MODE_OUTPUT_PP;
    initStruct.Pull = GPIO_PULLDOWN;

    _leds[DSPW_LED_1].isBlinking = false;
    _leds[DSPW_LED_1].state = DSPW_LED_OFF;
    _leds[DSPW_LED_1].blinkingState = DSPW_LED_OFF;
    _leds[DSPW_LED_1].port = DSPW_LED_1_PORT;
    _leds[DSPW_LED_1].pin = DSPW_LED_1_PIN;

    HW_GPIO_Init(_leds[DSPW_LED_1].port, _leds[DSPW_LED_1].pin, &initStruct);
    HW_GPIO_Write(_leds[DSPW_LED_1].port, _leds[DSPW_LED_1].pin, 0);

    TimerInit(&_leds[DSPW_LED_1].blinkTimer, Led1BlinkTimerCallback);
#endif

#if DSPW_LED_NUM > 1
    _leds[DSPW_LED_2].isBlinking = false;
    _leds[DSPW_LED_2].state = DSPW_LED_OFF;
    _leds[DSPW_LED_2].blinkingState = DSPW_LED_OFF;
    _leds[DSPW_LED_2].port = DSPW_LED_2_PORT;
    _leds[DSPW_LED_2].pin = DSPW_LED_2_PIN;

    HW_GPIO_Init(_leds[DSPW_LED_2].port, _leds[DSPW_LED_2].pin, &initStruct);
    HW_GPIO_Write(_leds[DSPW_LED_2].port, _leds[DSPW_LED_2].pin, 0);

    TimerInit(&_leds[DSPW_LED_2].blinkTimer, Led2BlinkTimerCallback);
#endif

#if DSPW_LED_NUM > 2
    _leds[DSPW_LED_3].isBlinking = false;
    _leds[DSPW_LED_3].state = DSPW_LED_OFF;
    _leds[DSPW_LED_3].blinkingState = DSPW_LED_OFF;
    _leds[DSPW_LED_3].port = DSPW_LED_3_PORT;
    _leds[DSPW_LED_3].pin = DSPW_LED_3_PIN;

    HW_GPIO_Init(_leds[DSPW_LED_3].port, _leds[DSPW_LED_3].pin, &initStruct);
    HW_GPIO_Write(_leds[DSPW_LED_3].port, _leds[DSPW_LED_3].pin, 0);

    TimerInit(&_leds[DSPW_LED_3].blinkTimer, Led3BlinkTimerCallback);
#endif

#if DSPW_LED_NUM > 3
    _leds[DSPW_LED_4].isBlinking = false;
    _leds[DSPW_LED_4].state = DSPW_LED_OFF;
    _leds[DSPW_LED_4].blinkingState = DSPW_LED_OFF;
    _leds[DSPW_LED_4].port = DSPW_LED_4_PORT;
    _leds[DSPW_LED_4].pin = DSPW_LED_4_PIN;

    HW_GPIO_Init(_leds[DSPW_LED_4].port, _leds[DSPW_LED_4].pin, &initStruct);
    HW_GPIO_Write(_leds[DSPW_LED_4].port, _leds[DSPW_LED_4].pin, 0);

    TimerInit(&_leds[DSPW_LED_4].blinkTimer, Led4BlinkTimerCallback);
#endif
}

extern bool DSPW_Led_IsBlinking(Dspw_Led_t led)
{
#if DSPW_LED_NUM > 0
    if(FindLed(led) != DSPW_LED_INVALID)
    {
        return _leds[led].isBlinking;
    }
#endif
    return false;
}

void DSPW_Led_Off(Dspw_Led_t led)
{
#if DSPW_LED_NUM > 0
    if(FindLed(led) != DSPW_LED_INVALID)
    {
        HW_GPIO_Write(_leds[led].port, _leds[led].pin, 0);
        _leds[led].state = DSPW_LED_OFF;
    }
    else
    {
        panic(0, (uint32_t)DSPW_Led_Off);
    }
#endif
}

void DSPW_Led_On(Dspw_Led_t led)
{
#if DSPW_LED_NUM > 0
    if(FindLed(led) != DSPW_LED_INVALID)
    {
        HW_GPIO_Write(_leds[led].port, _leds[led].pin, 1);
        _leds[led].state = DSPW_LED_ON;
    }
    else
    {
        panic(0, (uint32_t)DSPW_Led_On);
    }
#endif
}

void DSPW_Led_StartBlinking(Dspw_Led_t led, uint32_t onTime, uint32_t offTime, uint32_t timeOut)
{
#if DSPW_LED_NUM > 0
    if(FindLed(led) != DSPW_LED_INVALID)
    {
        /* Set up the blinking state */
        _leds[led].blinkingState = _leds[led].state;
        _leds[led].isBlinking = true;
        _leds[led].offTime = offTime;
        _leds[led].onTime = onTime;
        _leds[led].timeElapsed = 0;
        _leds[led].timeOut = timeOut;

        /* Start the blink timer */
        if(_leds[led].blinkingState == DSPW_LED_ON)
            TimerSetValue(&_leds[led].blinkTimer, _leds[led].onTime);
        else
            TimerSetValue(&_leds[led].blinkTimer, _leds[led].offTime);

        TimerStart(&_leds[led].blinkTimer);
    }
    else
    {
        panic(0, (uint32_t)DSPW_Led_StartBlinking);
    }
#endif
}

void DSPW_Led_StopBlinking(Dspw_Led_t led)
{
#if DSPW_LED_NUM > 0
    if(FindLed(led) != DSPW_LED_INVALID)
    {
        /* Set up the blinking state */
        _leds[led].isBlinking = false;
        _leds[led].timeElapsed = 0;
        _leds[led].timeOut = 0;
        TimerStop(&_leds[led].blinkTimer);

        /* Set the led to its previous state */
        if(_leds[led].state == DSPW_LED_ON)
            HW_GPIO_Write(_leds[led].port, _leds[led].pin, 1);
        else
            HW_GPIO_Write(_leds[led].port, _leds[led].pin, 0);
    }
    else
    {
        panic(0, (uint32_t)DSPW_Led_StopBlinking);
    }
#endif
}

// Static functions
#if DSPW_LED_NUM > 0
static Dspw_Led_t FindLed(Dspw_Led_t led)
{
    for(uint8_t i=0; i<DSPW_LED_INVALID; i++)
    {
        if(i == (uint8_t)led)
            return (Dspw_Led_t)i;
    }

    return (Dspw_Led_t)DSPW_LED_INVALID;
}
#endif

static void LedBlinkTimerCallback(Dspw_Led_t led)
{
#if DSPW_LED_NUM > 0
    if(_leds[led].isBlinking)
    {
        uint32_t interval = 0;

        if(_leds[led].blinkingState == DSPW_LED_ON)
        {
            // Turn off the LED and save the new blinking state
            HW_GPIO_Write(_leds[led].port, _leds[led].pin, 0);
            _leds[led].blinkingState = DSPW_LED_OFF;

            // Re-calculate the time elapsed
            _leds[led].timeElapsed += _leds[led].onTime;

            // Start the "Off" timer
            interval = _leds[led].offTime;
        }
        else
        {
            // Turn on the LED and save the new blinking state
            HW_GPIO_Write(_leds[led].port, _leds[led].pin, 1);
            _leds[led].blinkingState = DSPW_LED_ON;

            // Re-calculate the time elapsed
            _leds[led].timeElapsed += _leds[led].offTime;

            // Start the "On" timer
            interval = _leds[led].onTime;
        }

        // Restart the blink timer, if required
        if(_leds[led].timeOut == 0)
        {
            // Keep blinking forever
            TimerSetValue(&_leds[led].blinkTimer, interval);
            TimerStart(&_leds[led].blinkTimer);
        }
        else if(_leds[led].timeElapsed >= _leds[led].timeOut)
        {
            DSPW_Led_StopBlinking(led);
        }
        else
        {
            if(interval > (_leds[led].timeOut - _leds[led].timeElapsed))
            {
                interval = _leds[led].timeOut - _leds[led].timeElapsed;
            }

            TimerSetValue(&_leds[led].blinkTimer, interval);
            TimerStart(&_leds[led].blinkTimer);
        }
    }
#endif
}

#if DSPW_LED_NUM > 0
static void Led1BlinkTimerCallback()
{
    LedBlinkTimerCallback(DSPW_LED_1);
}
#endif

#if DSPW_LED_NUM > 1
static void Led2BlinkTimerCallback()
{
    LedBlinkTimerCallback(DSPW_LED_2);
}
#endif

#if DSPW_LED_NUM > 2
static void Led3BlinkTimerCallback()
{
    LedBlinkTimerCallback(DSPW_LED_3);
}
#endif

#if DSPW_LED_NUM > 3
static void Led4BlinkTimerCallback()
{
    LedBlinkTimerCallback(DSPW_LED_4);
}
#endif
