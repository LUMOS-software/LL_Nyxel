/*******************************************************************************
 Copyright (c) 2016 Lumen Labs (HK) Limited. All Rights Reserved.
*******************************************************************************/

#include "LL_Platform.h"
#include "LL_Common.h"



T_LL_Thread__var thread_var__battery_indicator__sub;
//
// step functions
//
static unsigned long red_on(  void) {
    #if defined(LL_Charging_Lights_Controlled_By_Software)
        LL_LED_ON(E_LL_LED_CHARGE_RED);
    #elif defined(LL_Charging_Lights_Controlled_By_Hardware)
        //LL_LED_ON(E_LL_LED_PCB_Red);
    #endif
    return LL_Thread__step_RETURN__Done;
}
static unsigned long red_off( void) {
    #if defined(LL_Charging_Lights_Controlled_By_Software)
        LL_LED_OFF(E_LL_LED_CHARGE_RED);
    #elif defined(LL_Charging_Lights_Controlled_By_Hardware)
        //LL_LED_OFF(E_LL_LED_PCB_Red);
    #endif
    return LL_Thread__step_RETURN__Done;
}
//
static unsigned long green_on(  void) { 
    #if defined(LL_Charging_Lights_Controlled_By_Software)
        LL_LED_ON(E_LL_LED_CHARGE_GREEN);
    #endif
    return LL_Thread__step_RETURN__Done;
}
static unsigned long green_off( void) { 
    #if defined(LL_Charging_Lights_Controlled_By_Software)
        LL_LED_OFF(E_LL_LED_CHARGE_GREEN);
    #endif
    return LL_Thread__step_RETURN__Done;
}
//
static E_LL_LED all_leds[5] = { 
    #if defined(LL_Charging_Lights_Controlled_By_Software)
        E_LL_LED_CHARGE_RED,
    #elif defined(LL_Charging_Lights_Controlled_By_Hardware)
        //E_LL_LED_PCB_Red,
    #endif
    E_LL_LED_FRONT,// E_LL_LED_BACK_L, E_LL_LED_BACK_R, E_LL_LED_BUZZER 
	};
static unsigned long all_on(  void) { for(int i = 0; i < 5; i++) { LL_LED_ON(  all_leds[i]); } return LL_Thread__step_RETURN__Done; }
static unsigned long all_off( void) { for(int i = 0; i < 5; i++) { LL_LED_OFF( all_leds[i]); } return LL_Thread__step_RETURN__Done; }
//
// threads
const LL_Thread__step_func steps_green_flash[] = {
    green_on,
    LL_Thread__delay_ms(500),
    green_off,
    LL_Thread__delay_ms(500),
    LL_Thread__step__END_OR_LOOP_THIS_THREAD
};
const LL_Thread__step_func steps_red_flash[] = {
    red_on,
    LL_Thread__delay_ms(500),
    red_off,
    LL_Thread__delay_ms(500),
    LL_Thread__step__END_OR_LOOP_THIS_THREAD
};
const LL_Thread__step_func steps_red_alert[] = {
    red_on,
    LL_Thread__delay_ms(300),
    red_off,
    LL_Thread__delay_ms(200),
    LL_Thread__step__END_OR_LOOP_THIS_THREAD
};
const LL_Thread__step_func steps_all_alert[] = {
    all_on,
    LL_Thread__delay_ms(140),
    all_off,
    LL_Thread__delay_ms(60),
    LL_Thread__step__END_OR_LOOP_THIS_THREAD
};



T_LL_Thread__var thread_var__battery_indicator__main;
//
// step functions
//
static unsigned long battery_indicator_when_power_on(void) {
    if(1 == gtPara.ulNeedCharge) { // if the last sleep is due to low-battery
        LL_Thread__start(&thread_var__battery_indicator__sub, steps_red_alert, 4);
    } else { // else, no need charge now
        unsigned long battery_level = LL_Battery_Level(); if(LL_BATTERY_LEVEL_NONE == battery_level) return LL_Thread__step_RETURN__NotDone; // keep waiting if battery not measured yet
        if(LL_VALUE_OF_BATT_LVL_ALERT < battery_level) { 
            LL_Thread__start(&thread_var__battery_indicator__sub, steps_green_flash, 0);
        } else if(LL_VALUE_OF_BATT_LVL_OFF < battery_level) { 
            LL_Thread__start(&thread_var__battery_indicator__sub, steps_red_flash, 0);
        } else {
            gtPara.ulNeedCharge = 1; gulFlashStoreNeeded = 1; // !!!!!!
            LL_Thread__start(&thread_var__battery_indicator__sub, steps_red_alert, 4);
        }
    }
    // for all the "LL_Thread__start" above:
    return LL_Thread__step_RETURN__Done;//_andThisThreadHasBeenRestartedOrUpdated;
}
static unsigned long battery_indicator_when_running(void) {
    unsigned long battery_level = LL_Battery_Level(); if(LL_BATTERY_LEVEL_NONE == battery_level) return LL_Thread__step_RETURN__NotDone; // keep waiting if battery not measured yet
    if(1 < battery_level) { 
        return LL_Thread__step_RETURN__NotDone; // keep monitoring
    } else {
        gtPara.ulNeedCharge = 1; gulFlashStoreNeeded = 1; 
        { // some deinit at first
            // off the timeslot to make beep stable:
            sd_radio_session_close();
            // switch off other routine of LED/buzzer:
            LL_BeepForTurning__OFF();        // LL_Helmet_BeepWhenTurning()
            gtSysState.eBrakeState = BRAKE_OFF;     // LL_Helmet_TurnOffBrakeLightAuto()
            gtSysState.eTurnState = TURNING_NONE;   // LL_Helmet_TurnOffTurningLightAuto()                        
        }
        LL_Thread__start(&thread_var__battery_indicator__sub, steps_all_alert, 4);
        return LL_Thread__step_RETURN__Done;//_andThisThreadHasBeenRestartedOrUpdated;
    }
}
static unsigned long battery_indicator_wait_for_the_sub_done(void) {
    if(LL_Thread__is_stopped((&thread_var__battery_indicator__sub))) {
        return LL_Thread__step_RETURN__Done;
    } else {
        return LL_Thread__step_RETURN__NotDone;        
    }
}
//
// threads
const LL_Thread__step_func steps_battery_indicator_when_power_on[] = {
    battery_indicator_when_power_on,
    battery_indicator_wait_for_the_sub_done,
    LL_Thread__step__END_OR_LOOP_THIS_THREAD
};
const LL_Thread__step_func steps_battery_indicator_when_running[] = {
    battery_indicator_when_running,
    battery_indicator_wait_for_the_sub_done,
    LL_Thread__step__END_OR_LOOP_THIS_THREAD
};



