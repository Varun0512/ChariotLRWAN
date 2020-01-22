/*
 * dspw_hw_conf.h
 *
 *  Created on: Mar 18, 2019
 *      Author: Swapna
 */

#ifndef DSPW_HW_CONF_H_
#define DSPW_HW_CONF_H_

//#define RADIO_DIO_4
//#define RADIO_DIO_5

/* LORA I/O definition */

#ifdef USE_SX126X_DVK

#define USE_INTERNAL_TRIMMING_CAP
#define TRIMMING_CAP_XTA                          0x1B
#define TRIMMING_CAP_XTB                          0x1B

#define RADIO_RESET_PORT                          GPIOC
#define RADIO_RESET_PIN                           GPIO_PIN_13

#define RADIO_MOSI_PORT                           GPIOB
#define RADIO_MOSI_PIN                            GPIO_PIN_5

#define RADIO_MISO_PORT                           GPIOB
#define RADIO_MISO_PIN                            GPIO_PIN_4

#define RADIO_SCLK_PORT                           GPIOB
#define RADIO_SCLK_PIN                            GPIO_PIN_3

#define RADIO_NSS_PORT                            GPIOA
#define RADIO_NSS_PIN                             GPIO_PIN_15

#define RADIO_BUSY_PORT                           GPIOA
#define RADIO_BUSY_PIN                            GPIO_PIN_8

#define RADIO_DIO_1_PORT                          GPIOA
#define RADIO_DIO_1_PIN                           GPIO_PIN_7

#define RADIO_DIO_2_PORT                          GPIOB
#define RADIO_DIO_2_PIN                           GPIO_PIN_0

#define RADIO_DIO_3_PORT                          GPIOB
#define RADIO_DIO_3_PIN                           GPIO_PIN_2

#define RADIO_ANT_SWITCH_POWER_PORT               GPIOA
#define RADIO_ANT_SWITCH_POWER_PIN                GPIO_PIN_12

#define DEVICE_SEL_PORT                           GPIOA
#define DEVICE_SEL_PIN                            GPIO_PIN_4

/*#define RADIO_LEDTX_PORT                        GPIOA
#define RADIO_LEDTX_PIN                           GPIO_PIN_1*/

/*#define RADIO_LEDRX_PORT                        GPIOA
#define RADIO_LEDRX_PIN                           GPIO_PIN_0*/

#else


#define RADIO_RESET_PORT                          GPIOA
#define RADIO_RESET_PIN                           GPIO_PIN_0

#define RADIO_MOSI_PORT                           GPIOA
#define RADIO_MOSI_PIN                            GPIO_PIN_7

#define RADIO_MISO_PORT                           GPIOA
#define RADIO_MISO_PIN                            GPIO_PIN_6

#define RADIO_SCLK_PORT                           GPIOA
#define RADIO_SCLK_PIN                            GPIO_PIN_5

#define RADIO_NSS_PORT                            GPIOB
#define RADIO_NSS_PIN                             GPIO_PIN_6

#define RADIO_DIO_0_PORT                          GPIOA
#define RADIO_DIO_0_PIN                           GPIO_PIN_10

#define RADIO_DIO_1_PORT                          GPIOB
#define RADIO_DIO_1_PIN                           GPIO_PIN_3

#define RADIO_DIO_2_PORT                          GPIOB
#define RADIO_DIO_2_PIN                           GPIO_PIN_5

#define RADIO_DIO_3_PORT                          GPIOB
#define RADIO_DIO_3_PIN                           GPIO_PIN_4

#ifdef RADIO_DIO_4
#define RADIO_DIO_4_PORT                          GPIOA
#define RADIO_DIO_4_PIN                           GPIO_PIN_9
#endif

#ifdef RADIO_DIO_5
#define RADIO_DIO_5_PORT                          GPIOC
#define RADIO_DIO_5_PIN                           GPIO_PIN_7
#endif

#define RADIO_ANT_SWITCH_PORT                     GPIOC
#define RADIO_ANT_SWITCH_PIN                      GPIO_PIN_1

#endif

#define SPI_CLK_ENABLE()                          __HAL_RCC_SPI1_CLK_ENABLE()

#define SPI1_AF                                   GPIO_AF0_SPI1

#ifdef USE_STM32L0XX_NUCLEO
#define ADC_READ_CHANNEL                          ADC_CHANNEL_4
#define ADCCLK_ENABLE()                           __HAL_RCC_ADC1_CLK_ENABLE() ;
#define ADCCLK_DISABLE()                          __HAL_RCC_ADC1_CLK_DISABLE() ;

#elif defined(STM32L071xx)
#define ADC_READ_CHANNEL                          ADC_CHANNEL_4
#define ADCCLK_ENABLE()                           __HAL_RCC_ADC1_CLK_ENABLE() ;
#define ADCCLK_DISABLE()                          __HAL_RCC_ADC1_CLK_DISABLE() ;
#endif

#define USARTx                                    USART1
#define USARTx_CLK_ENABLE()                       __USART1_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()               __GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()               __GPIOA_CLK_ENABLE()
#define DMAx_CLK_ENABLE()                         __HAL_RCC_DMA1_CLK_ENABLE()

#define USARTx_FORCE_RESET()                      __USART1_FORCE_RESET()
#define USARTx_RELEASE_RESET()                    __USART1_RELEASE_RESET()

#define USARTx_TX_PIN                             GPIO_PIN_9
#define USARTx_TX_GPIO_PORT                       GPIOA
#define USARTx_TX_AF                              GPIO_AF4_USART1
#define USARTx_RX_PIN                             GPIO_PIN_10
#define USARTx_RX_GPIO_PORT                       GPIOA
#define USARTx_RX_AF                              GPIO_AF4_USART1

#define USARTx_IRQn                               USART1_IRQn
#define USARTx_IRQHandler                         USART1_IRQHandler

#define USARTx_TX_DMA_CHANNEL                     DMA1_Channel7

#define USARTx_TX_DMA_REQUEST                     DMA_REQUEST_4

#define USARTx_DMA_TX_IRQn                        DMA1_Channel4_5_6_7_IRQn
#define USARTx_DMA_TX_IRQHandler                  DMA1_Channel4_5_6_7_IRQHandler

#define USARTx_Priority                           0
#define USARTx_DMA_Priority                       0

// Uncomment this line if the USART_TypeDef struct has separate gState and RxState
#define USARTX_HAS_TX_RX_STATES
// Uncomment this line to specify if the AbortTransmit/AbortReceive
// functions are supported by this projects HAL_UART code
#define USARTX_HAS_ABORT_FUNCTIONS

// Data EEPROM start and end address
#define DATA_EEPROM_START_ADDR                    0x08080000
#define DATA_EEPROM_END_ADDR                      0x08080FFF
#define ERASED_BIT                                FLASH_PECR_FIX

// Timeout value(cycles) for EEPROM operations
#define EEPROM_PRG_TIMEOUT                        (uint32_t)0x8000

/* LED definition */
#define DSPW_LED_NUM                              3
#define DSPW_LED_1_PORT                           GPIOA
#define DSPW_LED_1_PIN                            GPIO_PIN_1
#define DSPW_LED_2_PORT                           GPIOA
#define DSPW_LED_2_PIN                            GPIO_PIN_2
#define DSPW_LED_3_PORT                           GPIOA
#define DSPW_LED_3_PIN                            GPIO_PIN_3

#define DSPW_LED_RED                              DSPW_LED_1//DSPW_LED_INVALID
#define DSPW_LED_GREEN                            DSPW_LED_2
#define DSPW_LED_BLUE                             DSPW_LED_3

/* Unused Pins definition */
#define UNUSED_PORT_A_PINS                        GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_11 | GPIO_PIN_13 | GPIO_PIN_14
#define UNUSED_PORT_B_PINS                        GPIO_PIN_1 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15

#endif /* DSPW_HW_CONF_H_ */
