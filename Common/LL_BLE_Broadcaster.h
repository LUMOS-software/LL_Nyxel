/*******************************************************************************
 Copyright (c) 2016 Lumen Labs (HK) Limited. All Rights Reserved.
*******************************************************************************/

#ifndef _LL_BLE_Broadcaster_H
#define _LL_BLE_Broadcaster_H // BLE role "Broadcaster" to manage all advertisement (for Lumos APP and other Lumos devices) acc to "Lumos Interface for Partners (BLE Advertising, V1.4)"



// Device Type acc to "Lumos Interface for Partners (BLE Advertising, V1.4)"
#include "LL_Product.h" // to get LL_Device_Type
#define LL_BLE_Broadcaster__Device_Type     LL_Device_Type

// Advertising Parameter: no need modify usually.
#define LL_BLE_Broadcaster__interval_and_timeout_of_MSG_KeepAlive              100, 180 // ms,s
#define LL_BLE_Broadcaster__interval_and_timeout_of_MSG_Pairing                100, 180 // ms,s
#define LL_BLE_Broadcaster__interval_and_timeout_of_MSG_Lumos_Lights_State     100, 180 // ms,s
#define LL_BLE_Broadcaster__interval_and_timeout_of_MSG_Battery                100, 180 // ms,s
#define LL_BLE_Broadcaster__interval_and_timeout_of_MSG_Sync_Clock            1000,   1 // 1000ms and 1s: means ONLY ADV 1 TIME
#define LL_BLE_Broadcaster__interval_and_timeout_of_MSG_Power_ONOFF            100, 180 // ms,s
#define LL_BLE_Broadcaster__interval_and_timeout_of_MSG_TurningAndBraking      100, 180 // ms,s
#define LL_BLE_Broadcaster__interval_and_timeout_of_MSG_FlashingPattern        100, 180 // ms,s



extern T_LL_Thread__var thread_var__BLE_Broadcaster__main;
extern T_LL_Thread__var thread_var__BLE_Broadcaster__sub;
// Pairing Mode (main)
extern const LL_Thread__step_func thread__LL_BLE_Broadcaster__Pairing_APP[];
// Pairing Mode (when Remote L/R pressed)
extern const LL_Thread__step_func thread__LL_BLE_Broadcaster__Pairing_Devices[];
// Normal Mode (main)
extern const LL_Thread__step_func thread__LL_BLE_Broadcaster__Normal_Mode[];
// Normal Mode (when Power ON)
extern const LL_Thread__step_func thread__LL_BLE_Broadcaster__Normal_Mode_Power_ON[];
// Normal Mode (when Turning and Braking)
extern const LL_Thread__step_func thread__LL_BLE_Broadcaster__Normal_Mode_TurningBraking[];
// Normal Mode (when Sync)
extern const LL_Thread__step_func thread__LL_BLE_Broadcaster__Normal_Mode_Sync_FlashingPattern[];
// Normal Mode (when Power OFF)
extern const LL_Thread__step_func thread__LL_BLE_Broadcaster__Normal_Mode_Power_OFF[];



#endif
