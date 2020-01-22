/*
 * config.h
 *
 *  Created on: Mar 16, 2019
 *      Author: Swapna
 */

/*
 * config.h
 *
 *  Created on: Mar 9, 2018
 *      Author: Swapna
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

/*!
 * Default LoRa Configuration
 */
#define CONFIG_LORAWAN_ADR_STATE                    CONFIG_LORAWAN_ADR_ON
#define CONFIG_LORAWAN_ADR_ON                       1
#define CONFIG_LORAWAN_CONFIRMED_MSG                DISABLE
#define CONFIG_LORAWAN_PUBLIC_NETWORK               true
#define CONFIG_TX_EVT                               TX_ON_TIMER
#define CONFIG_LORAWAN_DEFAULT_DATA_RATE            DR_0
#define CONFIG_LORAWAN_DEFAULT_CLASS                CLASS_A

#define DSPW_GATEWAY								0      /* set 1 to activate DSPWorks Gateway*/
#define CONFIG_OTA                                  1
#define CONFIG_IEEE_OUI                             PREFIX_HERE
#define CONFIG_DEVICE_EUI                           { CONFIG_IEEE_OUI, 0x01, 0x00, 0x00, 0x00, 0x0F }
#define CONFIG_APP_KEY                              {PUT APPLICATION KEY HERE}
/*!
 * Default Channel Configuration
 */
#if (DSPW_GATEWAY == 1)
#define CONFIG_NUMB_DEFAULT_CHANNELS                1
#define CONFIG_LC1_FREQ                             (865200000)
#else
#define CONFIG_NUMB_DEFAULT_CHANNELS                3
#define CONFIG_LC1_FREQ                             (865162500) //SET this frequency
#define CONFIG_LC2_FREQ                             (865502500) // SET this frequency
#define CONFIG_LC3_FREQ                             (866085000) // SET this frequency
#endif // DSPW_GATEWAY == 1

#if (CONFIG_OTA == 0)
#define CONFIG_DEVICE_ADDR                          (uint32_t)0x22000422
#define CONFIG_NET_SESSION_KEY                      {  }
#define CONFIG_APP_SESSION_KEY                      {  }
#endif

#endif /* _CONFIG_H_ */
