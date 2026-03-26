/*******************************************************************************
 Copyright (c) 2016 Lumen Labs (HK) Limited. All Rights Reserved.
*******************************************************************************/

#include "nrf_soc.h"
#include "LL_Power.h"

void LL_Power_Init(void)
{
    // nothing!
}

void LL_Power_Sleep(void)
{
		sd_power_system_off();
}
