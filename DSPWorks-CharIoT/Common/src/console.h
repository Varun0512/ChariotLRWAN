/*
 * console.h
 *
 *  Created on: Mar 30, 2018
 *      Author: Swapna
 */

#ifndef __CONSOLE_H
#define __CONSOLE_H

#include "hw.h"

#include <stdbool.h>
#include <stdint.h>

#define CONSOLE_MAX_CONSOLES        (3)
#define CONSOLE_ID_INVALID          (0xFF)

typedef uint8_t console_id_t;

typedef enum
{
    CONSOLE_ERR_SUCCESS = 0,
    CONSOLE_ERR_UNDERRUN,
    CONSOLE_ERR_OVERFLOW,
    CONSOLE_ERR_INVALID_ARG,
    CONSOLE_ERR_BUSY,
    CONSOLE_ERR_RX_BUSY,
    CONSOLE_ERR_TX_BUSY
}console_error_t;

typedef enum
{
    CONSOLE_STATUS_ACTIVE = 0,
    CONSOLE_STATUS_PAUSED,
    CONSOLE_STATUS_FREE
}console_status_t;

typedef struct
{
    GPIO_TypeDef* port;
    uint16_t pin;
    uint32_t function;
}console_pin_t;

typedef void (*console_rx_callback_t)(uint8_t data);
typedef void (*console_tx_callback_t)();

typedef struct
{
    console_status_t status;
    USART_TypeDef* usart;
    console_pin_t rxPin;
    console_pin_t txPin;
    UART_HandleTypeDef handle;
    uint32_t irq;
    console_rx_callback_t rxCallback;
    console_tx_callback_t txCallback;
    uint8_t rxByte;
}console_t;

console_error_t console_add(USART_TypeDef* usart, uint32_t baudRate,
                               console_pin_t rxPin, console_pin_t txPin, uint32_t irq,
                               console_rx_callback_t rxCallback, console_tx_callback_t txCallback);
extern void console_init();
extern bool console_is_busy(USART_TypeDef* usart);
extern console_error_t console_pause(USART_TypeDef* usart);
extern console_error_t console_resume(USART_TypeDef* usart);
extern console_error_t console_rx(USART_TypeDef* usart);
extern console_error_t console_tx(USART_TypeDef* usart, void* data, uint8_t len);

#endif
