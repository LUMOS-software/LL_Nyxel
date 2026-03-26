/*******************************************************************************
 Copyright (c) 2016 Lumen Labs (HK) Limited. All Rights Reserved.
*******************************************************************************/

#include "LL_Platform.h"
#include "LL_Common.h"
//#include "LL_SysMode.h"

unsigned long gulCharging = 0;
unsigned long LL_Battery_Charging__NotOnlyChargingMode_ButAlsoChargingIndeed(void) {
    return LL_Battery__isCharging();
}

unsigned long LL_Battery__isCharging(void) {
    unsigned long lumos5V         = LL_GPIO_InputRead(0, LL_PIN_CHARGING_5V);
    return (LL_PIN_Y__CHARGING_5V == lumos5V);
}

void LL_Battery_Charging__Init(void) {
    LL_GPIO_InputCfg(0, LL_PIN_CHARGING_5V,     LL_PULLMODE_CHARGING_5V,    LL_GPIO_TRIGGER_NONE);
}
