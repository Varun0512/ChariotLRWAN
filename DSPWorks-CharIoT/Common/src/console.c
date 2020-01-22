/*
 * console.c
 *
 *  Created on: Mar 30, 2018
 *      Author: Swapna
 */

#include "console.h"
#include "panic.h"

// Static functions
static void configure_usart(UART_HandleTypeDef* usartHandle, USART_TypeDef* usart, uint32_t baudRate);
static void configure_usart_gpio_pin(console_pin_t pin);
static void disable_usart_irq(uint32_t irq);
static void enable_usart_clock(USART_TypeDef* usart);
static void enable_usart_gpio_clock(GPIO_TypeDef* gpio);
static void enable_usart_irq(uint32_t irq);
static console_id_t find_console(USART_TypeDef* usart);

// Console DB
static console_t _consoles[CONSOLE_MAX_CONSOLES];

console_error_t console_add(USART_TypeDef* usart, uint32_t baudRate,
                               console_pin_t rxPin, console_pin_t txPin, uint32_t irq,
                               console_rx_callback_t rxCallback, console_tx_callback_t txCallback)
{
    if(!usart)
        return CONSOLE_ERR_INVALID_ARG;

    uint8_t i;
    for(i=0; i<CONSOLE_MAX_CONSOLES; i++)
    {
        if(_consoles[i].usart == usart)
            return CONSOLE_ERR_BUSY;

        if(_consoles[i].status == CONSOLE_STATUS_FREE)
            break;
    }

    if(i >= CONSOLE_MAX_CONSOLES)
        return CONSOLE_ERR_BUSY;

    // Mark the console as active
    _consoles[i].status = CONSOLE_STATUS_ACTIVE;

    // Configure RX gpio
    enable_usart_gpio_clock(rxPin.port);
    configure_usart_gpio_pin(rxPin);
    _consoles[i].rxPin = rxPin;

    // Configure TX gpio
    enable_usart_gpio_clock(txPin.port);
    configure_usart_gpio_pin(txPin);
    _consoles[i].txPin = txPin;

    // Configure USART
    enable_usart_clock(usart);
    configure_usart(&_consoles[i].handle, usart, baudRate);
    _consoles[i].usart = usart;

    // Configure IRQ
    _consoles[i].rxCallback = rxCallback;
    _consoles[i].txCallback = txCallback;
    _consoles[i].irq = irq;
    enable_usart_irq(irq);

    return CONSOLE_ERR_SUCCESS;
}

void console_init()
{
    for(uint8_t i=0; i<CONSOLE_MAX_CONSOLES; i++)
    {
        _consoles[i].usart = NULL;
        _consoles[i].status = CONSOLE_STATUS_FREE;
    }
}

bool console_is_busy(USART_TypeDef* usart)
{
    console_id_t id = find_console(usart);

    if(id != CONSOLE_ID_INVALID)
    {
#ifdef USARTX_HAS_TX_RX_STATES
    	return (_consoles[id].handle.gState != HAL_UART_STATE_READY);
#else
        return ((_consoles[id].handle.State == HAL_UART_STATE_BUSY_TX) ||
        		(_consoles[id].handle.State == HAL_UART_STATE_BUSY_TX_RX));
#endif
    }

    panic(0, (uint32_t)console_is_busy);
    return false;  // Never reached
}

console_error_t console_pause(USART_TypeDef* usart)
{
    console_id_t id = find_console(usart);
    if(id == CONSOLE_ID_INVALID)
        panic(0, (uint32_t)console_pause);

    // For an already paused console, do nothing
    if(_consoles[id].status == CONSOLE_STATUS_PAUSED)
        return CONSOLE_ERR_SUCCESS;

    // If console is busy, let the caller know
#ifdef USARTX_HAS_TX_RX_STATES
    if(_consoles[id].handle.gState != HAL_UART_STATE_READY)
#else
    if(_consoles[id].handle.State != HAL_UART_STATE_READY)
#endif
    {
        return CONSOLE_ERR_BUSY;
    }

#ifdef USARTX_HAS_ABORT_FUNCTIONS
    HAL_UART_AbortReceive_IT(&_consoles[id].handle);
    HAL_UART_AbortTransmit_IT(&_consoles[id].handle);
#else
    /* Check if a transmit process is ongoing or not */
    if(_consoles[id].handle.State == HAL_UART_STATE_BUSY_TX)
    {
    	_consoles[id].handle.State = HAL_UART_STATE_BUSY_TX_RX;
    }
    else
    {
    	_consoles[id].handle.State = HAL_UART_STATE_BUSY_RX;
    }

    /* Enable the UART Parity Error Interrupt */
    __HAL_UART_ENABLE_IT(&_consoles[id].handle, UART_IT_PE);

    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    __HAL_UART_ENABLE_IT(&_consoles[id].handle, UART_IT_ERR);

    /* Enable the UART Data Register not empty Interrupt */
    __HAL_UART_ENABLE_IT(&_consoles[id].handle, UART_IT_RXNE);
#endif

    disable_usart_irq(_consoles[id].irq);

    _consoles[id].status = CONSOLE_STATUS_PAUSED;

    return CONSOLE_ERR_SUCCESS;
}

console_error_t console_resume(USART_TypeDef* usart)
{
    console_id_t id = find_console(usart);
    if(id == CONSOLE_ID_INVALID)
        panic(0, (uint32_t)console_resume);

    // For an already active console, do nothing
    if(_consoles[id].status == CONSOLE_STATUS_ACTIVE)
        return CONSOLE_ERR_SUCCESS;

    _consoles[id].status = CONSOLE_STATUS_ACTIVE;

    enable_usart_irq(_consoles[id].irq);
    return CONSOLE_ERR_SUCCESS;
}

console_error_t console_rx(USART_TypeDef* usart)
{
    console_id_t id = find_console(usart);
    if(id == CONSOLE_ID_INVALID)
        return CONSOLE_ERR_INVALID_ARG;

    if(_consoles[id].status != CONSOLE_STATUS_ACTIVE)
        panic(0,(uint32_t)console_rx);

#ifdef USARTX_HAS_TX_RX_STATES
    if(_consoles[id].handle.RxState != HAL_UART_STATE_READY)
#else
    if((_consoles[id].handle.State != HAL_UART_STATE_READY) &&
          (_consoles[id].handle.State != HAL_UART_STATE_BUSY_TX))
#endif
    {
        return CONSOLE_ERR_RX_BUSY;
    }

    if(HAL_UART_Receive_IT(&_consoles[id].handle, &_consoles[id].rxByte, 1) != HAL_OK)
        return CONSOLE_ERR_BUSY;

    return CONSOLE_ERR_SUCCESS;
}

console_error_t console_tx(USART_TypeDef* usart, void* data, uint8_t len)
{
    if(!data || !len)
        return CONSOLE_ERR_INVALID_ARG;

    console_id_t id = find_console(usart);
    if(id == CONSOLE_ID_INVALID)
        return CONSOLE_ERR_INVALID_ARG;

    if(_consoles[id].status != CONSOLE_STATUS_ACTIVE)
        panic(0,(uint32_t)console_tx);

#ifdef USARTX_HAS_TX_RX_STATES
    if(_consoles[id].handle.gState != HAL_UART_STATE_READY)
#else
    if((_consoles[id].handle.State != HAL_UART_STATE_READY) &&
          (_consoles[id].handle.State != HAL_UART_STATE_BUSY_RX))
#endif
    {
        return CONSOLE_ERR_TX_BUSY;
    }

    if(HAL_UART_Transmit_IT(&_consoles[id].handle, data, len) != HAL_OK)
        return CONSOLE_ERR_BUSY;

    return CONSOLE_ERR_SUCCESS;
}

// Static functions
static void configure_usart(UART_HandleTypeDef* usartHandle, USART_TypeDef* usart, uint32_t baudRate)
{
    usartHandle->Instance = usart;
    usartHandle->Init.BaudRate = baudRate;
    usartHandle->Init.WordLength = UART_WORDLENGTH_8B;
    usartHandle->Init.Parity= UART_PARITY_NONE;
    usartHandle->Init.StopBits = UART_STOPBITS_1;
    usartHandle->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    usartHandle->Init.Mode = UART_MODE_RX | UART_MODE_TX;
    usartHandle->Init.OverSampling = UART_OVERSAMPLING_16;

    if(HAL_UART_Init(usartHandle) != HAL_OK)
        panic(0, (uint32_t)configure_usart);
}

static void configure_usart_gpio_pin(console_pin_t pin)
{
    GPIO_InitTypeDef gpio;

    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_HIGH;
    gpio.Pin = pin.pin;
    gpio.Alternate = pin.function;

    HAL_GPIO_Init(pin.port, &gpio);
}

static void disable_usart_irq(uint32_t irq)
{
    HAL_NVIC_DisableIRQ(irq);
}

static void enable_usart_clock(USART_TypeDef* usart)
{
    if(usart == USART1)
        __HAL_RCC_USART1_CLK_ENABLE();
    else if(usart == USART2)
        __HAL_RCC_USART2_CLK_ENABLE();
#ifdef USART3
    else if(usart == USART3)
        __HAL_RCC_USART3_CLK_ENABLE();
#endif
    else
        panic(0, (uint32_t)enable_usart_clock);
}

static void enable_usart_gpio_clock(GPIO_TypeDef* gpio)
{
    if(gpio == GPIOA)
        __HAL_RCC_GPIOA_CLK_ENABLE();
    else if(gpio == GPIOB)
        __HAL_RCC_GPIOB_CLK_ENABLE();
    else if(gpio == GPIOC)
        __HAL_RCC_GPIOC_CLK_ENABLE();
    else if(gpio == GPIOD)
        __HAL_RCC_GPIOD_CLK_ENABLE();
    else if(gpio == GPIOE)
        __HAL_RCC_GPIOE_CLK_ENABLE();
#ifdef GPIOF
    else if(gpio == GPIOF)
        __HAL_RCC_GPIOF_CLK_ENABLE();
#endif
#ifdef GPIOG
    else if(gpio == GPIOG)
        __HAL_RCC_GPIOG_CLK_ENABLE();
#endif
#ifdef GPIOH
    else if(gpio == GPIOH)
        __HAL_RCC_GPIOH_CLK_ENABLE();
#endif
    else
        panic(0, (uint32_t)enable_usart_clock);
}

static void enable_usart_irq(uint32_t irq)
{
    HAL_NVIC_SetPriority(irq, 0xF, 0);
    HAL_NVIC_EnableIRQ(irq);
}

static console_id_t find_console(USART_TypeDef* usart)
{
    if(!usart)
        return CONSOLE_ID_INVALID;

    for(uint8_t i=0; i<CONSOLE_MAX_CONSOLES; i++)
    {
        if(_consoles[i].usart == usart)
            return i;
    }

    return CONSOLE_ID_INVALID;
}

static void IRQHandler(console_id_t id)
{
    HAL_UART_IRQHandler(&_consoles[id].handle);
}

// USART Callbacks
void USART1_IRQHandler()
{
    console_id_t id = find_console(USART1);

    if(id == CONSOLE_ID_INVALID)
        panic(0, (uint32_t)USART1_IRQHandler);

    IRQHandler(id);
}

void USART2_IRQHandler()
{
    console_id_t id = find_console(USART2);

    if(id == CONSOLE_ID_INVALID)
        panic(0, (uint32_t)USART2_IRQHandler);

    IRQHandler(id);
}

#ifdef USART3
void USART3_IRQHandler()
{
    console_id_t id = find_console(USART3);

    if(id == CONSOLE_ID_INVALID)
        panic(0, (uint32_t)USART3_IRQHandler);

    IRQHandler(id);
}
#endif

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* handle)
{
    console_id_t id = find_console(handle->Instance);

    if(id != CONSOLE_ID_INVALID)
    {
        if(_consoles[id].rxCallback != NULL)
        {
            _consoles[id].rxCallback(_consoles[id].rxByte);
        }
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *handle)
{
    console_id_t id = find_console(handle->Instance);

    if(id != CONSOLE_ID_INVALID)
    {
        if(_consoles[id].txCallback != NULL)
        {
            _consoles[id].txCallback();
        }
    }
}
