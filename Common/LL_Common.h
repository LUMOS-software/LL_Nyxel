/*******************************************************************************
 Copyright (c) 2016 Lumen Labs (HK) Limited. All Rights Reserved.
*******************************************************************************/

#ifndef _LL_Common_H
#define _LL_Common_H



// Thread
#include "LL_Thread.h"
//
// Battery
#include "LL_Battery.h"
#include "LL_BatteryIndicator.h"
#include "LL_Battery_Charge.h"
//
// Button & LED
#include "LL_Key.h"
#include "LL_LED.h"
//
// BLE (Common)
#include "LL_BLE.h"
//
// BLE (with Lumos APP)
#include "LL_MSG_withAPP.h"
//
// BLE (with Lumos devices)
#include "LL_MSG_withRemote.h" // old one: Nordic ESB
#include "LL_MSG_withDevice.h" // new one: LUMOS BLE Sync
#include "LL_Sync.h"
#include "LL_BLE_Broadcaster.h"

// BLE (Observer)
#include "LL_BLE_Observer.h"

// ws2812
#include "LL_LED_Panel_WS2812.h"
//
// BLE Service BAS
void battery_level_update(void);
// BLE Service NUS (TX)
unsigned long LL_BLE_TX(unsigned char *p_string, unsigned short length);
// BLE Service NUS (RX)
extern unsigned long sgulNusDataHandleNeeded; // 0: no,  others: length.
extern unsigned char sgaucNusRcvBuf[];
//
// States
#include "LL_SysMode.h"
//
// Persistent
#include "LL_Para.h"
//
// Other functions
void IndicatorOfBrakeFuncOnOff__Start(void);
extern unsigned long gulDisconnectRountine;



#endif


