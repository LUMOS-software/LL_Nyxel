/*******************************************************************************
 Copyright (c) 2016 Lumen Labs (HK) Limited. All Rights Reserved.
*******************************************************************************/

#include "LL_Platform.h"
#include "LL_Common.h"
#include "LL_Product.h"



// 1. MACRO acc to "Lumos Interface for Partners (BLE Advertising, V1.4)"
#if 1

#define MSG_KeepAlive             0x00
#define MSG_Pairing               0x01
#define MSG_Lumos_Lights_State    0x02
#define MSG_Battery               0x03
#define MSG_Sync_Clock            0x05
#define MSG_Power_ONOFF           0x06
#define MSG_TurningAndBraking     0x07
#define MSG_FlashingPattern       0x08

#define MSGSIZE_KeepAlive           10
#define MSGSIZE_Pairing             10//11
#define MSGSIZE_Lumos_Lights_State  12
#define MSGSIZE_Battery             11
#define MSGSIZE_Sync_Clock          13
#define MSGSIZE_Power_ONOFF         11
#define MSGSIZE_TurningAndBraking   11
#define MSGSIZE_FlashingPattern     17

#endif



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// 2. LL_Thread__step_func functions acc to "Lumos Interface for Partners (BLE Advertising, V1.4)"
#if 1

static unsigned long adv__KeepAlive(void) {
    LL_Radio__spec_data[0] = MSG_KeepAlive;
    LL_Radio__common_stop_init_start(MSGSIZE_KeepAlive, LL_BLE_Broadcaster__interval_and_timeout_of_MSG_KeepAlive);

    return LL_Thread__step_RETURN__Done;
}

static unsigned long adv__Pairing(void) {
    LL_Radio__spec_data[0] = MSG_Pairing;
//  LL_Radio__spec_data[8] = 0x00;
    LL_Radio__common_stop_init_start(MSGSIZE_Pairing, LL_BLE_Broadcaster__interval_and_timeout_of_MSG_Pairing);

    return LL_Thread__step_RETURN__Done;
}

#define LIGHT_ON__front_L       (1<<0)
#define LIGHT_ON__front_R       (1<<1)
#define LIGHT_ON__rear_L        (1<<2)
#define LIGHT_ON__rear_R        (1<<3)
#define LIGHT_ON__turning_L     (1<<4)
#define LIGHT_ON__turning_R     (1<<5)
#define LIGHT_ON__braking_L     (1<<6)
#define LIGHT_ON__braking_R     (1<<7)
#define LIGHT_ON__side_L        (1<<8)
#define LIGHT_ON__side_R        (1<<9)
static unsigned long adv__Lumos_Lights_State(void) {//(unsigned long lights_state) {
    unsigned long lights_state; switch(gtSysState.eTurnState) {
        case TURNING_L:     lights_state = LIGHT_ON__turning_L |                                        LIGHT_ON__front_R | LIGHT_ON__rear_R ; break;
        case TURNING_R:     lights_state = LIGHT_ON__turning_R | LIGHT_ON__front_L | LIGHT_ON__rear_L                                        ; break;
        case TURNING_NONE:  lights_state =                       LIGHT_ON__front_L | LIGHT_ON__rear_L | LIGHT_ON__front_R | LIGHT_ON__rear_R ; break;
        default:            lights_state = 0; break; // should not happen
    }                
    
    LL_Radio__spec_data[0] = MSG_Lumos_Lights_State;
    LL_Radio__spec_data[8] = lights_state;
    LL_Radio__spec_data[9] = lights_state;
    LL_Radio__common_stop_init_start(MSGSIZE_Lumos_Lights_State, LL_BLE_Broadcaster__interval_and_timeout_of_MSG_Lumos_Lights_State);

    return LL_Thread__step_RETURN__Done;
}

static unsigned long adv__Battery(void) {//(unsigned long battery_level) {
    unsigned long battery_level = LL_Battery_Level();

    LL_Radio__spec_data[0] = MSG_Battery;
    LL_Radio__spec_data[8] = (unsigned char)battery_level;
    LL_Radio__common_stop_init_start(MSGSIZE_Battery, LL_BLE_Broadcaster__interval_and_timeout_of_MSG_Battery);

    return LL_Thread__step_RETURN__Done;
}

static unsigned long adv__Sync_Clock(void) {
    unsigned long clock_now = LL_Clock__LED_CLOCK_get();
    LL_Radio__spec_data[ 0] = MSG_Sync_Clock;
    LL_Radio__spec_data[ 8] = (unsigned char)(clock_now >>  0);
    LL_Radio__spec_data[ 9] = (unsigned char)(clock_now >>  8);
    LL_Radio__spec_data[10] = (unsigned char)(clock_now >> 16);
    LL_Radio__common_stop_init_start(MSGSIZE_Sync_Clock, LL_BLE_Broadcaster__interval_and_timeout_of_MSG_Sync_Clock);

    return LL_Thread__step_RETURN__Done;
}

#define POWER_STATE__OFF    0x00
#define POWER_STATE__ON     0x01
static void adv__Power_ONOFF(unsigned char power_state) {
    LL_Radio__spec_data[0] = MSG_Power_ONOFF;
    LL_Radio__spec_data[8] = power_state;
    LL_Radio__common_stop_init_start(MSGSIZE_Power_ONOFF, LL_BLE_Broadcaster__interval_and_timeout_of_MSG_Power_ONOFF);
}
static unsigned long adv__Power_ON(void) {
    adv__Power_ONOFF(POWER_STATE__ON);
    return LL_Thread__step_RETURN__Done;
}
static unsigned long adv__Power_OFF(void) {
    adv__Power_ONOFF(POWER_STATE__OFF);
    return LL_Thread__step_RETURN__Done;
}

#include "LL_SysMode.h" // E_TURN_STATE and E_BRAKE_STATE
#define TURNING_CMD_NONE      0
#define TURNING_CMD_L         1
#define TURNING_CMD_R         2
#define BRAKING_CMD_NONE      0
#define BRAKING_CMD_BRAKING   1
static unsigned long adv__TurningAndBraking(void) {//(E_TURN_STATE eTurnState, E_BRAKE_STATE eBrakeState) {
    unsigned char turning_cmd; switch(gtSysState.eTurnState) {
        case TURNING_L:     turning_cmd = TURNING_CMD_L;    break;
        case TURNING_R:     turning_cmd = TURNING_CMD_R;    break;
        case TURNING_NONE:  turning_cmd = TURNING_CMD_NONE; break;
        default:            turning_cmd = TURNING_CMD_NONE; break; // should not happen
    }
    unsigned char braking_cmd; switch(gtSysState.eBrakeState) {
        case BRAKE_ON:  braking_cmd = BRAKING_CMD_BRAKING;  break;
        case BRAKE_OFF: braking_cmd = BRAKING_CMD_NONE;     break;
        default:        braking_cmd = BRAKING_CMD_NONE;     break; // should not happen
    }        
    LL_Radio__spec_data[0] = MSG_TurningAndBraking;
    LL_Radio__spec_data[8] = ((turning_cmd & 0x0F) << 4)
                 | ((braking_cmd & 0x0F) << 0);
    LL_Radio__common_stop_init_start(MSGSIZE_TurningAndBraking, LL_BLE_Broadcaster__interval_and_timeout_of_MSG_TurningAndBraking);

    return LL_Thread__step_RETURN__Done;
}

static unsigned long adv__FlashingPattern(void) {//(unsigned char sender, unsigned long pattern_front, unsigned long pattern_rear) {
    unsigned char sender = (unsigned char)E_SYNC_MSG__FROM_HELMET;
    unsigned long pattern_front = LL_Para__get_flashing_pattern(gtSysState.eModeOfWarningLight, E_LL_LED_FRONT  );
    unsigned long pattern_rear  = LL_Para__get_flashing_pattern(gtSysState.eModeOfWarningLight, E_LL_LED_REAR );

    LL_Radio__spec_data[ 0] = MSG_FlashingPattern;
    LL_Radio__spec_data[ 8] = sender;
    LL_Radio__spec_data[ 9] = (pattern_front & 0x000000FF) >>  0;
    LL_Radio__spec_data[10] = (pattern_front & 0x0000FF00) >>  8;
    LL_Radio__spec_data[11] = (pattern_front & 0x00FF0000) >> 16;
    LL_Radio__spec_data[12] = (pattern_rear & 0x000000FF) >>  0;
    LL_Radio__spec_data[13] = (pattern_rear & 0x0000FF00) >>  8;
    LL_Radio__spec_data[14] = (pattern_rear & 0x00FF0000) >> 16;
    LL_Radio__common_stop_init_start(MSGSIZE_FlashingPattern, LL_BLE_Broadcaster__interval_and_timeout_of_MSG_FlashingPattern);

    return LL_Thread__step_RETURN__Done;
}

#endif



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// 3. Threads

// 3.1 sub ones: adv for app, keep alive, sync clock ...
#if 1
T_LL_Thread__var thread_var__BLE_Broadcaster__sub;
const static LL_Thread__step_func thread_AdvForApp_andKeepAlive[] = {
    LL_Radio__START_ADV_FOR_APP_CONNECTING,
    LL_Thread__delay_ms(12*1000),
    adv__KeepAlive,
    LL_Thread__delay_ms(3*1000),
    LL_Thread__step__END_OR_LOOP_THIS_THREAD
};
const static LL_Thread__step_func thread_SyncClock[] = {
    adv__Sync_Clock,
    LL_Thread__delay_ms(130), // 130ms is easier got by other 100ms/200ms scanning 
    LL_Thread__step__END_OR_LOOP_THIS_THREAD
};
#endif



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// 3.2 main ones: Pairing Mode, Normal Mode, power ON/OFF, turning/braking ...
#if 1



T_LL_Thread__var thread_var__BLE_Broadcaster__main;



// Pairing Mode (main)
const LL_Thread__step_func thread__LL_BLE_Broadcaster__Pairing_APP[] = {
    LL_Radio__START_ADV_FOR_APP_PAIRING,
    LL_Thread__delay_ms(3*60*1000), // why 3min: the timeout of Nordic BLE advertisement is often 3min
    LL_Thread__step__END_OR_LOOP_THIS_THREAD
};
// Pairing Mode (when Remote L/R pressed)
const LL_Thread__step_func thread__LL_BLE_Broadcaster__Pairing_Devices[] = { // usually for Remote, trigger by button event
    adv__Pairing,
    LL_Thread__delay_ms(5*1000), // 5s is enough for other devices to get the pairing message by scanning
    LL_Thread__update_the_current_thread(thread__LL_BLE_Broadcaster__Pairing_APP, LL_Thread__LOOP_FOREVER)
};



// Normal Mode (main)
const LL_Thread__step_func thread__LL_BLE_Broadcaster__Normal_Mode[] = {
    adv__Battery,
    LL_Thread__delay_ms(3*1000),
    //
    LL_Thread__start_a_sub_thread(          &thread_var__BLE_Broadcaster__sub, thread_AdvForApp_andKeepAlive, 4),
    LL_Thread__wait_for_a_sub_thread_done(  &thread_var__BLE_Broadcaster__sub                                  ),
    //
    LL_Thread__start_a_sub_thread(          &thread_var__BLE_Broadcaster__sub, thread_SyncClock, 5),
    LL_Thread__wait_for_a_sub_thread_done(  &thread_var__BLE_Broadcaster__sub                     ),
    //
    LL_Thread__step__END_OR_LOOP_THIS_THREAD
};
// Normal Mode (when Power ON)
const LL_Thread__step_func thread__LL_BLE_Broadcaster__Normal_Mode_Power_ON[] = {
    adv__Power_ON,
    LL_Thread__delay_ms(2*1000),//5*1000,//10*1000,//20*1000,
    LL_Thread__update_the_current_thread(thread__LL_BLE_Broadcaster__Normal_Mode, LL_Thread__LOOP_FOREVER)
};
// Normal Mode (when Turning and Braking)
const LL_Thread__step_func thread__LL_BLE_Broadcaster__Normal_Mode_TurningBraking[] = {
    adv__Lumos_Lights_State, // for Cyber
    LL_Thread__delay_ms(5*1000),
    adv__TurningAndBraking,
    LL_Thread__delay_ms(5*1000),
    LL_Thread__update_the_current_thread(thread__LL_BLE_Broadcaster__Normal_Mode, LL_Thread__LOOP_FOREVER)
};
// Normal Mode (when Sync)
const LL_Thread__step_func thread__LL_BLE_Broadcaster__Normal_Mode_Sync_FlashingPattern[] = {
    adv__FlashingPattern,
    LL_Thread__delay_ms(5*1000),
    //
    LL_Thread__start_a_sub_thread(          &thread_var__BLE_Broadcaster__sub, thread_SyncClock, 5),
    LL_Thread__wait_for_a_sub_thread_done(  &thread_var__BLE_Broadcaster__sub                     ),
    //
    LL_Thread__update_the_current_thread(thread__LL_BLE_Broadcaster__Normal_Mode, LL_Thread__LOOP_FOREVER)
};
// Normal Mode (when Power OFF)
const LL_Thread__step_func thread__LL_BLE_Broadcaster__Normal_Mode_Power_OFF[] = {
    adv__Power_OFF,
    LL_Thread__delay_ms(20*1000),
    LL_Thread__step__END_OR_LOOP_THIS_THREAD
};



#endif



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


