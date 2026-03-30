/*******************************************************************************
 Copyright (c) 2016 Lumen Labs (HK) Limited. All Rights Reserved.
*******************************************************************************/

#ifndef _LL_BATTERY_CHARGE_H
#define _LL_BATTERY_CHARGE_H

extern unsigned char gulCharging;

unsigned long LL_Battery_Charging__NotOnlyChargingMode_ButAlsoChargingIndeed(void);
unsigned long LL_Battery__isCharging(void);

void LL_Battery_Charging__Init(void);
void LL_Battery_Charging__Loop(void);

void LL_Battery_Charging_Animation_Init(void);
void LL_Battery_Charging_Animation_Loop(void);

#endif
