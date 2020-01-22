/*
 * app.c
 *
 *  Created on: Mar 16, 2019
 *      Author: Swapna
 * app.c
 *
 *  Created on: Jan 30, 2018
 *      Author: Swapna
 */

#include "app.h"
#include "eeprom.h"
#include "dspw_hw.h"
#include "dspw_led.h"
#include "hw_msp.h"
#include "lora.h"
#include "timeServer.h"
#include <stdbool.h>

// App specific defines
#define APP_SW_VERSION                (0x01)
#define APP_HW_VERSION                (0x10)
#define APP_PORT                      (0x3)

//App Events
#define APP_EVT_JOINED                (1 << 0)
#define APP_EVT_DUTY_CYCLE            (1 << 1)

// TX data events
#define APP_TX_EVT_INFO               (1 << 0)
#define APP_TX_EVT_DUTY_CYCLE         (1 << 1)
//#define APP_TX_SOS_ALERT_EVT          (1 << 2)

// Miscellaneous
#define APP_LOW_BAT_THRESHOLD         (10) // ~30% battery remaining
#define APP_TX_BACKOFF_TIMEOUT        (10000)
#define APP_DATA_BUFF_SIZE            (64)

// Static variables
static uint32_t _appEvents;
static uint32_t _appTxEvents;
static bool _isAssociated;
static bool _isFlashDirty;
static bool _isTxBusy;
static TimerEvent_t _txBackOffTimer;
static TimerEvent_t _txTimer;
static uint8_t AppDataBuff[APP_DATA_BUFF_SIZE];
static lora_AppData_t AppData={ AppDataBuff,  0 ,0 };
static LoraFlagStatus LoraMacProcessRequest=LORA_RESET;

// Constants
static const uint8_t _oui[IEEE_OUI_LEN] = {IEEE_OUI};

// Static functions

static void App_LoraConfirmClass( DeviceClass_t Class );
static void App_LoraDutyCycleEvent();
static void App_LoraRxData(lora_AppData_t* AppData);
static void App_LoraTxNeeded ( void );
static void App_LoraTxTimer(TxEventType_t EventType);
static void App_OnLoraJoined( void );
static void App_Send( void* context );
static void TxBackOffTimerCallback();
static void UpdateLeds();

/* !
 *Initialises the Lora Parameters
 */
static LoRaParam_t LoRaParamInit= { CONFIG_LORAWAN_ADR_STATE,
                                    CONFIG_LORAWAN_DEFAULT_DATA_RATE,
                                    CONFIG_LORAWAN_PUBLIC_NETWORK};

static LoRaMainCallback_t LoRaMainCallbacks = { HW_GetBatteryLevel,
                                                NULL,
                                                NULL,
                                                NULL,
                                                App_LoraRxData,
                                                App_OnLoraJoined,
                                                App_LoraConfirmClass,
                                                App_LoraTxNeeded,
                                                App_LoraMacProcessNotify};

bool App_CanPause()
{
    if(LoraMacProcessRequest!=LORA_SET && !_appTxEvents)
        return true;
    return false;
}

void App_Init()
{
    // Initialize the hardware
    DSPW_HW_Init();


    // Start with a clean slate
    _appEvents = 0;
    _appTxEvents = APP_TX_EVT_INFO;
    _isAssociated = false;
    _isFlashDirty = false;
    _isTxBusy = false;

    TimerInit(&_txBackOffTimer, TxBackOffTimerCallback);
    TimerSetValue(&_txBackOffTimer, APP_TX_BACKOFF_TIMEOUT);


    /* Configure the Lora Stack*/
    LORA_Init( &LoRaMainCallbacks, &LoRaParamInit);

    /* RED LED is turned on to indicate device is powered up */
    DSPW_Led_On(DSPW_LED_RED);
    DelayMs(APP_LED_DURATION_POWER_ON);             // RED LED is on for 10 seconds
    DSPW_Led_Off(DSPW_LED_RED);

    LORA_Join();

    App_LoraTxTimer( TX_ON_TIMER);
}

static void App_LoraDutyCycleEvent()
{
    TimerStart( &_txTimer);

    /* Send the duty cycle packet */
    if(_isAssociated){
        _appEvents |= APP_EVT_DUTY_CYCLE;
    }
}

static void App_LoraRxData( lora_AppData_t *AppData )
{
    if(!AppData->BuffSize)
        return;

    if(AppData->Port != APP_PORT)
        return;

    switch(AppData->Buff[0])
       {
           // TODO: Packet parsing code goes here
       }
}

static void App_LoraConfirmClass( DeviceClass_t Class )
{
    AppData.BuffSize = 0;
    AppData.Port = APP_PORT;

    LORA_send( &AppData, LORAWAN_UNCONFIRMED_MSG);
}

static void App_LoraTxTimer(TxEventType_t EventType)
{
    if (EventType == TX_ON_TIMER)
    {
        /* send everytime timer elapses */
        TimerInit( &_txTimer, App_LoraDutyCycleEvent );
        TimerSetValue( &_txTimer, APP_TX_DUTYCYCLE);
        App_LoraDutyCycleEvent();
    }
}

static void App_LoraTxNeeded( void )
{
    AppData.BuffSize = 0;
    AppData.Port = APP_PORT;

    LORA_send( &AppData, LORAWAN_UNCONFIRMED_MSG);
}

static void App_OnLoraJoined( void )
{
    _isAssociated = true;
    _appEvents |= APP_EVT_JOINED;

    _appTxEvents = 0;
    LORA_RequestClass( CONFIG_LORAWAN_DEFAULT_CLASS );
}

void App_LoraMacProcessNotify(void)
{
    LoraMacProcessRequest=LORA_SET;
}

void App_Pause()
{
}

void App_Process()
{
    /* If there are one or more packets to send,
     * re-try after a back-off */
    if((_appTxEvents != 0) && !_isTxBusy)
    {
        // There are one or more pending packets
        // to be sent. Send the next one now!
        _isTxBusy = true;
        App_Send(NULL);
        TimerStart(&_txBackOffTimer);
    }

    if(_appEvents & APP_EVT_JOINED)
    {
        _appTxEvents |= APP_TX_EVT_DUTY_CYCLE;
        _appEvents &= ~APP_EVT_JOINED;
    }

    if(_appEvents & APP_EVT_DUTY_CYCLE)
    {
         _appEvents &= ~APP_EVT_DUTY_CYCLE;
    }

    if (LoraMacProcessRequest==LORA_SET)
    {
        /*reset notification flag*/
        LoraMacProcessRequest=LORA_RESET;
        LoRaMacProcess( );
    }

    UpdateLeds();
}

void App_Resume()
{
}

static void App_Send( void* context )
{
    if ( LORA_JoinStatus () != LORA_SET)
    {
        /*Not joined, try again later*/
        LORA_Join();
        return;
    }

    // Check if the info message needs to be sent
    if(_appTxEvents & APP_TX_EVT_INFO)
    {
        App_Info_Packet_t info;

        info.header = 0x00;
        memcpy(&info.oui, _oui, IEEE_OUI_LEN);
        info.deviceType = App_Device_Type_Humiture;
        info.swVersion = APP_SW_VERSION;
        info.hwVersion = APP_HW_VERSION;
        info.batteryLevel = HW_GetBatteryLevel();
        info.dutyCycle = 2; //hard coded value

        memcpy(AppData.Buff, &info, sizeof(info));
        AppData.BuffSize = sizeof(info);
        AppData.Port = APP_PORT;

        // Mark the event as handled
        _appTxEvents &= ~APP_TX_EVT_INFO;
    }
    // If none of the App related TX events are set,
    // this must be the duty cycle!
    else if(_appTxEvents & APP_TX_EVT_DUTY_CYCLE)
    {
    	// TODO: Fill in the default duty cycle data here
    	App_DutyCycle_t duty;
    	duty.header = 0x02;
    	duty.dutyCycle = 0x11;
    	memcpy(AppData.Buff, &duty, sizeof(duty));
    	AppData.BuffSize = sizeof(duty);
    	AppData.Port = APP_PORT;
        _appTxEvents &= ~APP_TX_EVT_DUTY_CYCLE;

    }

    LORA_send( &AppData, CONFIG_LORAWAN_CONFIRMED_MSG);
}




static void TxBackOffTimerCallback()
{
    _isTxBusy = false;
}

static void UpdateLeds()
{

}

