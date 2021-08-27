/****************************************************************************
 *
 * Copyright 2020 Samsung Electronics All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/

#ifndef __ARCH_ARM_SRC_S5JS100_CHIP_S5JS100_PINCONFIG_H__
#define __ARCH_ARM_SRC_S5JS100_CHIP_S5JS100_PINCONFIG_H__
/****************************************************************************
 * Included Files
 ****************************************************************************/
#include "gpio.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* GPIO pin definitions *****************************************************/

#define GPIO_DEFAULT_CONFIG (GPIO_GPIO|GPIO_PULLUP|GPIO_INPUT|GPIO_OUTPUT|GPIO_FAST1X)

#define GPIO_GPIO0      (GPIO_DEFAULT_CONFIG | GPIO_PIN0)
#define GPIO_EXT_LNA_ON0        (GPIO_ALT2 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN0)
#define GPIO_MCP_SM_CE      (GPIO_ALT6 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN0)

#define GPIO_GPIO1      (GPIO_DEFAULT_CONFIG | GPIO_PIN1)
#define GPIO_EXT_LNA_ON1        (GPIO_ALT2 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN1)
#define GPIO_MCP_SM_UB      (GPIO_ALT6 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN1)

#define GPIO_GPIO2      (GPIO_DEFAULT_CONFIG | GPIO_PIN2)
#define GPIO_EXT_LNA_GNSS       (GPIO_ALT2 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN2)
#define GPIO_MCP_SM_LB      (GPIO_ALT6 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN2)

#define GPIO_GPIO3      (GPIO_DEFAULT_CONFIG | GPIO_PIN3)
#define GPIO_PA_MODE0       (GPIO_ALT2 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN3)

#define GPIO_GPIO4      (GPIO_DEFAULT_CONFIG | GPIO_PIN4)
#define GPIO_PA_MODE1       (GPIO_ALT2 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN4)

#define GPIO_GPIO5      (GPIO_DEFAULT_CONFIG | GPIO_PIN5)
#define GPIO_PA_ON0         (GPIO_ALT2 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN5)

#define GPIO_GPIO6      (GPIO_DEFAULT_CONFIG | GPIO_PIN6)
#define GPIO_PA_ON1         (GPIO_ALT2 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN6)

#define GPIO_GPIO7      (GPIO_DEFAULT_CONFIG | GPIO_PIN7)
#define GPIO_ACMU_MON_CLK   (GPIO_ALT0 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN7)
#define GPIO_MCMU_MON_CLK   (GPIO_ALT1 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN7)
#define GPIO_MIFCMU_MON_CLK     (GPIO_ALT2 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN7)
#define GPIO_SCMU_MON_CLK   (GPIO_ALT3 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN7)
#define GPIO_MDM_PLL_MON_CLK    (GPIO_ALT4 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN7)
#define GPIO_NBIOT_MON_CLK      (GPIO_ALT5 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN7)


#define GPIO_GPIO8      (GPIO_DEFAULT_CONFIG | GPIO_PIN8)
#define GPIO_USI0_RXD_CLK_SCL   (GPIO_ALT1 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN8)
#define GPIO_MCP_SM_CRE     (GPIO_ALT6 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN8)

#define GPIO_GPIO9      (GPIO_DEFAULT_CONFIG | GPIO_PIN9)
#define GPIO_USI0_TXD_MOSI_SDA  (GPIO_ALT1 | GPIO_FLOAT | GPIO_FAST1X | GPIO_PIN9)
#define GPIO_MCP_FH_RST     (GPIO_ALT6 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN9)


#define GPIO_GPIO10     (GPIO_DEFAULT_CONFIG | GPIO_PIN10)
#define GPIO_USI0_RTSN_MISO_NA  (GPIO_ALT1 | GPIO_FLOAT | GPIO_FAST1X | GPIO_PIN10)
#define GPIO_MCP_RDY_WAIT   (GPIO_ALT6 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN10)

#define GPIO_GPIO11     (GPIO_DEFAULT_CONFIG | GPIO_PIN11)
#define GPIO_USI0_CTSN_CSN_NA   (GPIO_ALT1 | GPIO_FLOAT | GPIO_FAST1X | GPIO_PIN11)
#define GPIO_MCP_A23        (GPIO_ALT6 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN11)

#define GPIO_GPIO12     (GPIO_DEFAULT_CONFIG | GPIO_PIN12)
#define GPIO_USI1_RXD_CLK_SCL   (GPIO_ALT1 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN12)
#define GPIO_USIM0_DETECT   (GPIO_ALT2 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN12)

#define GPIO_GPIO13     (GPIO_DEFAULT_CONFIG | GPIO_PIN13)
#define GPIO_USI1_TXD_MOSI_SDA  (GPIO_ALT1 | GPIO_FLOAT | GPIO_FAST1X | GPIO_PIN13)
#define GPIO_APT_PDM        (GPIO_ALT2 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN13)

#define GPIO_GPIO14     (GPIO_DEFAULT_CONFIG | GPIO_PIN14)
#define GPIO_USI1_RTSN_MISO_NA  (GPIO_ALT1 | GPIO_FLOAT | GPIO_FAST1X | GPIO_PIN14)
#define GPIO_GPS_TIMESTAMP  (GPIO_ALT2 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN14)
#define GPIO_PWM_TOUT0      (GPIO_ALT3 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN14)

#define GPIO_GPIO15     (GPIO_DEFAULT_CONFIG | GPIO_PIN15)
#define GPIO_USI1_CTSN_CSN_NA   (GPIO_ALT1 | GPIO_FLOAT | GPIO_FAST1X | GPIO_PIN15)
#define GPIO_GPS_PAEN       (GPIO_ALT2 | GPIO_PULLDOWN | GPIO_FAST1X | GPIO_PIN15)
#define GPIO_PWM_TOUT1      (GPIO_ALT3 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN15)

#define GPIO_GPIO16     (GPIO_DEFAULT_CONFIG | GPIO_PIN16)
#define GPIO_UART0_CTSB     (GPIO_ALT1 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN16)


#define GPIO_GPIO17     (GPIO_DEFAULT_CONFIG | GPIO_PIN17)
#define GPIO_UART0_RTSB     (GPIO_ALT1 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN17)

#define GPIO_GPIO18     (GPIO_DEFAULT_CONFIG | GPIO_PIN18)
#define GPIO_UART1_CTSB     (GPIO_ALT1 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN18)
#define GPIO_SDIO_DATA0     (GPIO_ALT2 | GPIO_PULLUP | GPIO_OUTPUT| GPIO_FAST3X | GPIO_PIN18)

#define GPIO_GPIO19     (GPIO_DEFAULT_CONFIG | GPIO_PIN19)
#define GPIO_UART1_RTSB     (GPIO_ALT1 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN19)
#define GPIO_SDIO_DATA1     (GPIO_ALT2 | GPIO_PULLUP | GPIO_OUTPUT| GPIO_FAST3X | GPIO_PIN19)
#define GPIO_GNSS_DEBUG0    (GPIO_ALT4 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN19)

#define GPIO_GPIO20     (GPIO_DEFAULT_CONFIG | GPIO_PIN20)
#define GPIO_SDIO_DATA2     (GPIO_ALT2 | GPIO_PULLUP | GPIO_OUTPUT| GPIO_FAST3X | GPIO_PIN20)
#define GPIO_GNSS_DEBUG1    (GPIO_ALT4 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN20)

#define GPIO_GPIO21     (GPIO_DEFAULT_CONFIG | GPIO_PIN21)
#define GPIO_SDIO_DATA3     (GPIO_ALT2 | GPIO_PULLUP | GPIO_OUTPUT| GPIO_FAST3X | GPIO_PIN21)
#define GPIO_GNSS_DEBUG2    (GPIO_ALT4 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN21)

#define GPIO_GPIO22     (GPIO_DEFAULT_CONFIG | GPIO_PIN22)
#define GPIO_PWM_TOUT2      (GPIO_ALT1 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN22)
#define GPIO_SDIO_CLK       (GPIO_ALT2 | GPIO_PULLUP | GPIO_OUTPUT| GPIO_FAST3X | GPIO_PIN22)
#define GPIO_GNSS_DEBUG3    (GPIO_ALT4 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN22)

#define GPIO_GPIO23     (GPIO_DEFAULT_CONFIG | GPIO_PIN23)
#define GPIO_PWM_TOUT3      (GPIO_ALT1 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN23)
#define GPIO_SDIO_CMD       (GPIO_ALT2 | GPIO_PULLUP | GPIO_OUTPUT| GPIO_FAST3X | GPIO_PIN23)
#define GPIO_GNSS_DEBUG4    (GPIO_ALT4 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN23)

#define GPIO_GPIO24     (GPIO_DEFAULT_CONFIG | GPIO_PIN24)
#define GPIO_USIM0_CLK      (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN24)

#define GPIO_GPIO25     (GPIO_DEFAULT_CONFIG | GPIO_PIN25)
#define GPIO_USIM0_DATA     (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN25)

#define GPIO_GPIO26     (GPIO_DEFAULT_CONFIG | GPIO_PIN26)
#define GPIO_USIM0_RST      (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN26)

#define GPIO_GPIO27     (GPIO_DEFAULT_CONFIG | GPIO_PIN27)
#define GPIO_UART0_RXD      (GPIO_ALT0 | GPIO_PULLDOWN | GPIO_FAST3X | GPIO_PIN27)

#define GPIO_GPIO28     (GPIO_DEFAULT_CONFIG | GPIO_PIN28)
#define GPIO_UART0_TXD      (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN28)

#define GPIO_GPIO29     (GPIO_DEFAULT_CONFIG | GPIO_PIN29)
#define GPIO_UART1_RXD      (GPIO_ALT0 | GPIO_PULLDOWN | GPIO_FAST3X | GPIO_PIN29)

#define GPIO_GPIO30     (GPIO_DEFAULT_CONFIG | GPIO_PIN30)
#define GPIO_UART1_TXD      (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN30)

#define GPIO_GPIO31     (GPIO_DEFAULT_CONFIG | GPIO_PIN31)
#define GPIO_SPI0_RXD       (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN31)

#define GPIO_GPIO32     (GPIO_DEFAULT_CONFIG | GPIO_PIN32)
#define GPIO_SPI0_TXD       (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN32)

#define GPIO_GPIO33     (GPIO_DEFAULT_CONFIG | GPIO_PIN33)
#define GPIO_SPI0_CLK       (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN33)

#define GPIO_GPIO34     (GPIO_DEFAULT_CONFIG | GPIO_PIN34)
#define GPIO_SPI0_CS        (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN34)

#define GPIO_GPIO35     (GPIO_DEFAULT_CONFIG | GPIO_PIN35)
#define GPIO_I2C0_SCL       (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN35)

#define GPIO_GPIO36     (GPIO_DEFAULT_CONFIG | GPIO_PIN36)
#define GPIO_I2C0_SDA       (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN36)

#define GPIO_GPIO37     (GPIO_DEFAULT_CONFIG | GPIO_PIN37)
#define GPIO_CS         (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN37)
#define GPIO_MCP_FH_CE      (GPIO_ALT6 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN37)

#define GPIO_GPIO38     (GPIO_DEFAULT_CONFIG | GPIO_PIN38)
#define GPIO_SCK        (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN38)
#define GPIO_MCP_CLK        (GPIO_ALT6 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN38)

#define GPIO_GPIO39     (GPIO_DEFAULT_CONFIG | GPIO_PIN39)
#define GPIO_SI         (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN39)
#define GPIO_MCP_FBCLK      (GPIO_ALT6 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN39)

#define GPIO_GPIO40     (GPIO_DEFAULT_CONFIG | GPIO_PIN40)
#define GPIO_SO         (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN40)
#define GPIO_MCP_OE         (GPIO_ALT6 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN40)

#define GPIO_GPIO41     (GPIO_DEFAULT_CONFIG | GPIO_PIN41)
#define GPIO_WP         (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN41)
#define GPIO_MCP_WE         (GPIO_ALT6 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN41)


#define GPIO_GPIO42     (GPIO_DEFAULT_CONFIG | GPIO_PIN42)
#define GPIO_HLD        (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN42)
#define GPIO_MCP_ADV        (GPIO_ALT6 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN42)

#define GPIO_GPIO43     (GPIO_DEFAULT_CONFIG | GPIO_PIN43)
#define GPIO_ANT_SW0        (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN43)

#define GPIO_GPIO44     (GPIO_DEFAULT_CONFIG | GPIO_PIN44)
#define GPIO_ANT_SW1        (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN44)

#define GPIO_GPIO45     (GPIO_DEFAULT_CONFIG | GPIO_PIN45)
#define GPIO_ANT_SW2        (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN45)

#define GPIO_GPIO46     (GPIO_DEFAULT_CONFIG | GPIO_PIN46)
#define GPIO_ANT_SW3        (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN46)

#define GPIO_GPIO46     (GPIO_DEFAULT_CONFIG | GPIO_PIN46)
#define GPIO_ANT_SW3        (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN46)

#define GPIO_GPIO47     (GPIO_DEFAULT_CONFIG | GPIO_PIN47)
#define GPIO_ANT_SW4        (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN47)

#define GPIO_GPIO48     (GPIO_DEFAULT_CONFIG | GPIO_PIN48)
#define GPIO_MIPI_RFFE_SCLK (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN48)

#define GPIO_GPIO49     (GPIO_DEFAULT_CONFIG | GPIO_PIN49)
#define GPIO_MIPI_RFFE_DATA (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN49)

#define GPIO_GPIO50     (GPIO_DEFAULT_CONFIG | GPIO_PIN50)
#define GPIO_MCP_ADQ0       (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN50)

#define GPIO_GPIO51     (GPIO_DEFAULT_CONFIG | GPIO_PIN51)
#define GPIO_MCP_ADQ1       (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN51)

#define GPIO_GPIO52     (GPIO_DEFAULT_CONFIG | GPIO_PIN52)
#define GPIO_MCP_ADQ2       (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN52)

#define GPIO_GPIO53     (GPIO_DEFAULT_CONFIG | GPIO_PIN53)
#define GPIO_MCP_ADQ3       (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN53)

#define GPIO_GPIO54     (GPIO_DEFAULT_CONFIG | GPIO_PIN54)
#define GPIO_MCP_ADQ4       (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN54)

#define GPIO_GPIO55     (GPIO_DEFAULT_CONFIG | GPIO_PIN55)
#define GPIO_MCP_ADQ5       (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN55)

#define GPIO_GPIO56     (GPIO_DEFAULT_CONFIG | GPIO_PIN56)
#define GPIO_MCP_ADQ6       (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN56)

#define GPIO_GPIO57     (GPIO_DEFAULT_CONFIG | GPIO_PIN57)
#define GPIO_MCP_ADQ7       (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN57)

#define GPIO_GPIO58     (GPIO_DEFAULT_CONFIG | GPIO_PIN58)
#define GPIO_MCP_ADQ8       (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN58)

#define GPIO_GPIO59     (GPIO_DEFAULT_CONFIG | GPIO_PIN59)
#define GPIO_MCP_ADQ9       (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN59)

#define GPIO_GPIO60     (GPIO_DEFAULT_CONFIG | GPIO_PIN60)
#define GPIO_MCP_ADQ10      (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN60)

#define GPIO_GPIO61     (GPIO_DEFAULT_CONFIG | GPIO_PIN61)
#define GPIO_MCP_ADQ11      (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN61)

#define GPIO_GPIO62     (GPIO_DEFAULT_CONFIG | GPIO_PIN62)
#define GPIO_MCP_ADQ12      (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN62)

#define GPIO_GPIO63     (GPIO_DEFAULT_CONFIG | GPIO_PIN63)
#define GPIO_MCP_ADQ13      (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN63)

#define GPIO_GPIO64     (GPIO_DEFAULT_CONFIG | GPIO_PIN64)
#define GPIO_MCP_ADQ14      (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN64)

#define GPIO_GPIO65     (GPIO_DEFAULT_CONFIG | GPIO_PIN65)
#define GPIO_MCP_ADQ15      (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN65)

#define GPIO_GPIO66     (GPIO_DEFAULT_CONFIG | GPIO_PIN66)
#define GPIO_MCP_ADQ16      (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN66)

#define GPIO_GPIO67     (GPIO_DEFAULT_CONFIG | GPIO_PIN67)
#define GPIO_MCP_ADQ17      (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN67)

#define GPIO_GPIO68     (GPIO_DEFAULT_CONFIG | GPIO_PIN68)
#define GPIO_MCP_ADQ18      (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN68)

#define GPIO_GPIO69     (GPIO_DEFAULT_CONFIG | GPIO_PIN69)
#define GPIO_MCP_ADQ19      (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN69)

#define GPIO_GPIO70     (GPIO_DEFAULT_CONFIG | GPIO_PIN70)
#define GPIO_MCP_ADQ20      (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN70)

#define GPIO_GPIO71     (GPIO_DEFAULT_CONFIG | GPIO_PIN71)
#define GPIO_MCP_ADQ21      (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN71)

#define GPIO_GPIO72     (GPIO_DEFAULT_CONFIG | GPIO_PIN72)
#define GPIO_MCP_ADQ22      (GPIO_ALT0 | GPIO_FLOAT | GPIO_FAST3X | GPIO_PIN72)

#endif /* __ARCH_ARM_SRC_S5JS100_CHIP_S5JS100_PINCONFIG_H__ */