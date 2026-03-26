/*******************************************************************************
 Copyright (c) 2016 Lumen Labs (HK) Limited. All Rights Reserved.
*******************************************************************************/

#ifndef _LL_BATTERYINDICATOR_H
#define _LL_BATTERYINDICATOR_H

extern T_LL_Thread__var thread_var__battery_indicator__main;
extern T_LL_Thread__var thread_var__battery_indicator__sub;

extern const LL_Thread__step_func steps_battery_indicator_when_power_on[];
extern const LL_Thread__step_func steps_battery_indicator_when_running[];

    
    
#endif
