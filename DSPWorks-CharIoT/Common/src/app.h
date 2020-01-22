/*
 * app.h
 *
 *  Created on: Mar 16, 2019
 *      Author: Swapna
 */

/*
 * app.h
 *
 *  Created on: Jan 30, 2018
 *      Author: Swapna
 */

#ifndef _APP_H_
#define _APP_H_

#include "hw.h"
#include <stdint.h>
#include <stdbool.h>
#include "lora.h"

/*!
 * Defines the application data transmission duty cycle. 5s, value in [ms].
 */
#define APP_TX_DUTYCYCLE                             (2 * 60 * 1000)// 2 minutes


// LED durations in milliseconds
#define APP_LED_DURATION_POWER_ON                 (5 * 1000)
#define APP_LED_DURATION_JOINING_ON               (500)
#define APP_LED_DURATION_JOINING_OFF              (500)
#define APP_LED_DURATION_JOINED                   (3 * 1000)
#define APP_LED_DURATION_JOINING_MAX              (20 * 60 * 1000)
#define BATTERY_CHARGING_DURATION_ON              (500)
#define BATTERY_CHARGING_DURATION_OFF             (500)
#define BATTERY_CHARGING_DURATION_MAX             (240 * 60 * 1000) // 4 hours



typedef enum
{
    App_Device_Type_Humiture = 1,
    App_Device_Type_Door = 2,
    App_Device_Type_Temperature = 3,
    App_Device_Type_Current = 4,
    App_Device_Type_Water_Meter = 5,
    App_Device_Type_Energy_Meter = 6,
    App_Device_Type_Gas_Meter = 7,
    App_Device_Type_Smart_Switch = 8
}App_Device_Type_t;

// Data storage structs
typedef struct
{
    uint8_t header;
    uint16_t dutyCycle;
}__attribute__((packed)) App_DutyCycle_t;

// Protocol structs
typedef struct
{
	uint8_t header;
	uint8_t oui[IEEE_OUI_LEN];
	App_Device_Type_t deviceType;
	uint8_t swVersion;
	uint8_t hwVersion;
	uint8_t batteryLevel;
	uint8_t dutyCycle;
}__attribute__((packed)) App_Info_Packet_t;


typedef enum
{
    /* Rx Packet Types */
    App_Packet_Type_Config = 0x01,
}App_Packet_Type;

typedef struct
{
    uint8_t type;
    uint8_t apiVersion;
    uint8_t data[0];
}__attribute__((packed)) App_Packet_Rx_t;

typedef struct
{
    uint8_t dutyCycle;
}__attribute__((packed)) App_Packet_Config_t;

extern bool App_CanPause();
extern void App_Init();
extern void App_LoraTxData(lora_AppData_t *AppData, LoraConfirm_t* IsTxConfirmed);
extern void App_Pause();
extern void App_Process();
extern void App_Resume();
extern void App_LoraMacProcessNotify(void);


#endif /* _APP_H_ */
