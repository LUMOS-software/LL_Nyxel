/*******************************************************************************
 Copyright (c) 2016 Lumen Labs (HK) Limited. All Rights Reserved.
 *******************************************************************************/

#ifndef _LL_Platform_H
#define _LL_Platform_H


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Toolchain Keil
//
#include <stdint.h>
#include <stdio.h>  // sprintf
#include <string.h>
#include <stdlib.h>
//#include "LL_BuildTime.h"   // add building time into BLE advertisement



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Chipset Nordic
//

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "nordic_common.h"
#include "nrf.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "app_timer.h"
#include "ble_nus.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "bsp_btn_ble.h"
#include "nrf_pwr_mgmt.h"
#include "ble_dis.h"
#include "ble_bas.h"
#include "ble_dfu.h"
#include "nrf_dfu_ble_svci_bond_sharing.h"
#include "nrf_svci_async_function.h"
#include "nrf_svci_async_handler.h"
#include "nrf_power.h"
#include "nrf_bootloader_info.h"
#include "nrf_ble_scan.h"
#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
//
#include "nrf_drv_gpiote.h"
#include "app_pwm.h"
#include "nrf_drv_pwm.h"
#include "nrfx_pwm.h"
#include "nrf_delay.h"
#define LL_2BYTE_TO_LONG(byte0, byte1)              \
            ( (((unsigned long)(byte0)) <<  0)      \
            | (((unsigned long)(byte1)) <<  8) )
#define LL_3BYTE_TO_LONG(byte0, byte1, byte2)       \
            ( (((unsigned long)(byte0)) <<  0)      \
            | (((unsigned long)(byte1)) <<  8)      \
            | (((unsigned long)(byte2)) << 16) )
#define LL_4BYTE_TO_LONG(byte0, byte1, byte2, byte3)    \
            ( (((unsigned long)(byte0)) <<  0)          \
            | (((unsigned long)(byte1)) <<  8)          \
            | (((unsigned long)(byte2)) << 16)          \
            | (((unsigned long)(byte3)) << 24) )
//#define LL_5BYTE_TO_LONGLONG(byte0, byte1, byte2, byte3, byte4)                     \
//            ( (((unsigned long long)(byte0)) <<  0)                                      \
//            | (((unsigned long long)(byte1)) <<  8)                                      \
//            | (((unsigned long long)(byte2)) << 16)                                      \
//            | (((unsigned long long)(byte3)) << 24)                                      \
//            | (((unsigned long long)(byte4)) << 32) )
//#define LL_6BYTE_TO_LONGLONG(byte0, byte1, byte2, byte3, byte4, byte5)              \
//            ( (((unsigned long long)(byte0)) <<  0)                                      \
//            | (((unsigned long long)(byte1)) <<  8)                                      \
//            | (((unsigned long long)(byte2)) << 16)                                      \
//            | (((unsigned long long)(byte3)) << 24)                                      \
//            | (((unsigned long long)(byte4)) << 32)                                      \
//            | (((unsigned long long)(byte5)) << 40) )
//#define LL_7BYTE_TO_LONGLONG(byte0, byte1, byte2, byte3, byte4, byte5, byte6)       \
//            ( (((unsigned long long)(byte0)) <<  0)                                      \
//            | (((unsigned long long)(byte1)) <<  8)                                      \
//            | (((unsigned long long)(byte2)) << 16)                                      \
//            | (((unsigned long long)(byte3)) << 24)                                      \
//            | (((unsigned long long)(byte4)) << 32)                                      \
//            | (((unsigned long long)(byte5)) << 40)                                      \
//            | (((unsigned long long)(byte6)) << 48) )
//#define LL_8BYTE_TO_LONGLONG(byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7)    \
//            ( (((unsigned long long)(byte0)) <<  0)                                          \
//            | (((unsigned long long)(byte1)) <<  8)                                          \
//            | (((unsigned long long)(byte2)) << 16)                                          \
//            | (((unsigned long long)(byte3)) << 24)                                          \
//            | (((unsigned long long)(byte4)) << 32)                                          \
//            | (((unsigned long long)(byte5)) << 40)                                          \
//            | (((unsigned long long)(byte6)) << 48)                                          \
//            | (((unsigned long long)(byte7)) << 56) )



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Lumos Drivers
//
// Board
#include "LL_Board.h"
//
// Core
#include "LL_HardwareID.h"
#include "LL_Debug.h"
#include "LL_Power.h"
#include "LL_Reset.h"
#include "LL_Clock.h"
#include "LL_Timer.h"
//
// IO
#include "LL_GPIO.h"
#include "LL_ADC.h"
#include "LL_PWM.h"
//
// Radio
#include "esb_timeslot.h"
#include "LL_ESB.h"
#include "LL_Radio.h"
//
// Persistent
#include "LL_Flash.h"

//flash
#include "fds.h"


#endif


