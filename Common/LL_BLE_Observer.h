/*******************************************************************************
 Copyright (c) 2016 Lumen Labs (HK) Limited. All Rights Reserved.
*******************************************************************************/

#ifndef _LL_BLE_Observer_H
#define _LL_BLE_Observer_H

typedef enum
{
	TYPE_NORMAL = 0,
	TYPE_LOW_POWER_MODE,
}E_SCAN_PARAM_TYPE;

//#include "LL_BLE_Broadcaster.h"

void LL_BLE_Observer__Init(void);
void LL_BLE_Observer(void);
void LL_BLE_Observer__Stop(void);
void LL_BLE_Observer__Modify_Param(E_SCAN_PARAM_TYPE param);

#endif
