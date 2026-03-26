/*******************************************************************************
 Copyright (c) 2016 Lumen Labs (HK) Limited. All Rights Reserved.
*******************************************************************************/

#include "LL_Platform.h"
#include "LL_Common.h"
#include "LL_Product.h"



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//unsigned long gulRemoteBatteryAutoUpdate = 1;

//static char string[16];
static unsigned long ulTimerCntBatteryUpdate;// = gulTimerCnt1ms;    



#if 1 // Mode and Flashing Pattern
#define LL_PART_BACK_LIGHT    0
#define LL_PART_FRONT_LIGHT_LR  30 // front (L&R)
#define LL_PART_FRONT_LIGHT     31 // front (middle)

#define LL_TYPE_COMMON_ANIMATION 0
#define LL_TYPE_SPECIAL_ANIMATION 1

#define MAX_MODE_NUM    (LL_Mode_Number-1) // -1: start from 0
#define BIT_NUM_OF_FIELD_PART   5   // bit[7..5] is "mode", bit[4..0] is "part"
void packFlashModeParaToApp(unsigned char *buf, unsigned long mode, unsigned long part, unsigned char animation_type, unsigned long state_of_timeslot23to0) {
    buf[0] = 'F';
    buf[1] = (mode << BIT_NUM_OF_FIELD_PART) + part;
    buf[2] = (state_of_timeslot23to0 & 0x000000FF) >>  0;
    buf[3] = (state_of_timeslot23to0 & 0x0000FF00) >>  8;
    buf[4] = (state_of_timeslot23to0 & 0x00FF0000) >> 16; 
    buf[5] = animation_type;    
}
void unpackFlashModeParaFromApp(unsigned char *buf, unsigned long *mode, unsigned long *part, unsigned long *state_of_timeslot23to0) {
    *part                    = buf[1] & (0xFF>>(8-BIT_NUM_OF_FIELD_PART));
    *mode                    = buf[1] >> BIT_NUM_OF_FIELD_PART; if(MAX_MODE_NUM < *mode) { *mode = 0; }
    *state_of_timeslot23to0  = (buf[2]<< 0)
                             + (buf[3]<< 8)
                             + (buf[4]<<16);
}

void packCurrentModeToApp(unsigned char *buf, unsigned long mode) {
    buf[0] = 'f';
    buf[1] = '0' + mode;
}
void unpackCurrentModeFromApp(unsigned char *buf, unsigned long *mode) {
    *mode = buf[1] - '0';
}
#endif



// a function for "send current flash mode to app"
void TxCurrentFlashModeToApp(void) { 
    unsigned char tx_buf[5];
    tx_buf[0] = 'f';
    tx_buf[1] = '0' + gtPara.eModeOfWarningLight;
    tx_buf[2] = 'f';
    tx_buf[3] = 'f';
    tx_buf[4] = 'f';
    LL_BLE_TX(tx_buf, 5);
}



#if 1 // a function for "send flash mode to app"
void TxCurrentFlashModeToApp__Start(void) ;
unsigned long TxFlashModeParaToApp__time_cnt = 0;
unsigned long TxFlashModeParaToApp__step_cnt = 0;
void TxFlashModeParaToApp__Start(void) { 
    LL_Timer_CntStart(TxFlashModeParaToApp__time_cnt);
    TxFlashModeParaToApp__step_cnt = 0; 
}

void TxFlashModeParaToApp__Stop( void) { 
    LL_Timer_CntStop(TxFlashModeParaToApp__time_cnt);
//  TxFlashModeParaToApp__step_cnt = 0; 
}

void TxFlashModeParaToApp(void) { 
    if(0 == TxFlashModeParaToApp__time_cnt) { // this func is OFF
        return; 
    } else { 
        if(50 > LL_Timer_Elapsed_ms(TxFlashModeParaToApp__time_cnt)) { // not time yet
            return;
        } else { // timeout, then reset timer cnt
            LL_Timer_CntStart(TxFlashModeParaToApp__time_cnt);
        }
    }
    unsigned char tx_buf[16]; uint16_t tx_len = 0;
    unsigned long mode, part, e_LED, animationType;
    unsigned char brightness, color;
    // send diff data according to the step
    switch(TxFlashModeParaToApp__step_cnt) {
        case 0: mode = 0; part = LL_PART_FRONT_LIGHT; if(gtPara.ePatternOfFrontLight[mode] <= LED_ANIMATION_FASTFLASH_MODE){ animationType = LL_TYPE_COMMON_ANIMATION;}else{ animationType = gtPara.ePatternOfFrontLight[mode]-2;}  e_LED = E_LL_LED_FRONT;     break;
        case 1: mode = 0; part = LL_PART_BACK_LIGHT;  if(gtPara.ePatternOfRearLight[mode] <= LED_ANIMATION_FASTFLASH_MODE){  animationType = LL_TYPE_COMMON_ANIMATION;}else{ animationType = gtPara.ePatternOfFrontLight[mode]-2;}  e_LED = E_LL_LED_REAR;      break;
        case 2: mode = 1; part = LL_PART_FRONT_LIGHT; if(gtPara.ePatternOfFrontLight[mode] <= LED_ANIMATION_FASTFLASH_MODE){ animationType = LL_TYPE_COMMON_ANIMATION;}else{ animationType = gtPara.ePatternOfFrontLight[mode]-2;}  e_LED = E_LL_LED_FRONT;     break;
        case 3: mode = 1; part = LL_PART_BACK_LIGHT;  if(gtPara.ePatternOfRearLight[mode] <= LED_ANIMATION_FASTFLASH_MODE){  animationType = LL_TYPE_COMMON_ANIMATION;}else{ animationType = gtPara.ePatternOfFrontLight[mode]-2;}  e_LED = E_LL_LED_REAR;      break;
        case 4: mode = 2; part = LL_PART_FRONT_LIGHT; if(gtPara.ePatternOfFrontLight[mode] <= LED_ANIMATION_FASTFLASH_MODE){ animationType = LL_TYPE_COMMON_ANIMATION;}else{ animationType = gtPara.ePatternOfFrontLight[mode]-2;}  e_LED = E_LL_LED_FRONT;     break;
        case 5: mode = 2; part = LL_PART_BACK_LIGHT;  if(gtPara.ePatternOfRearLight[mode] <= LED_ANIMATION_FASTFLASH_MODE){  animationType = LL_TYPE_COMMON_ANIMATION;}else{ animationType = gtPara.ePatternOfFrontLight[mode]-2;}  e_LED = E_LL_LED_REAR;      break;
        case 6: mode = 0; part = 0xFFFFFFFF; break;
        case 7: mode = 1; part = 0xFFFFFFFF; break;
        case 8: mode = 2; part = 0xFFFFFFFF; break;        
        case 9: mode = 0; part = 0xFFFFFFFF; break;
        case 10: mode = 1; part = 0xFFFFFFFF; break;
        case 11: mode = 2; part = 0xFFFFFFFF; break;    
        case 12: part = 0xAAAAAAA; break;
        default: TxFlashModeParaToApp__Stop(); return;
    }

    if(0xFFFFFFFF==part) { // brightness
        tx_buf[0] = 'B';
        tx_buf[1] = 'M';
        tx_buf[2] = mode;
        if(TxFlashModeParaToApp__step_cnt < 9){
            //FRONT
            tx_buf[3] = LL_PART_FRONT_LIGHT;
            LL_Find_Color_Position(gtPara.brightness_individual[mode][E_LL_LED_FRONT], &color, &brightness);  
            tx_buf[4] = color ; 
            tx_buf[5] = brightness;
        }else{
            //REAR
            tx_buf[3] = LL_PART_BACK_LIGHT;
            LL_Find_Color_Position(gtPara.brightness_individual[mode][E_LL_LED_REAR], &color, &brightness); 
            tx_buf[4] = color ; 
            tx_buf[5] = brightness;
        }            
        tx_len = 6;
    } else if(0xAAAAAAA==part) { // animation current mode
        //
        TxCurrentFlashModeToApp();
    } else {       
        packFlashModeParaToApp(tx_buf, mode, part, animationType, LL_Para__get_flashing_pattern(mode, (E_LL_LED)e_LED));//gtPara.customer_flashing_timeslot_states[mode][e_LED]);
        tx_len = 6;
    }
    // wait
        if(NRF_SUCCESS == LL_BLE_TX(tx_buf, tx_len)) { 
        TxFlashModeParaToApp__step_cnt++; 
    } else { /* need send again, so do not go to next step */ }
}
#endif



void LL_MSG_withAPP__TX(void) {
    if(4000 <= LL_Timer_Elapsed_ms(ulTimerCntBatteryUpdate)) { ulTimerCntBatteryUpdate = gulTimerCnt1ms;
        // battery level of helmet
#ifdef _LL_DELETE_BAS // if delete Battery Service
        battery_level_update();
#endif
        // battery level of remote
        #if 0 // 0: no need anymore since Remote can report by itself
        if(    (1)//(1 == gulRemoteBatteryAutoUpdate) 
            && (SYS_PCB_TESTING_MODE != gtSysState.eOnOFF) 
            && (SYS_OFF != gtSysState.eOnOFF) 
            && (0 == gulCmdTestingMode)
            && (SYS_PRODUCTION_TESTING_MODE   != gtSysState.eOnOFF) ) {
                        if(0xCCCCCCCC == gulBatteryOfRemote) { // if Remote Lite
                            sprintf(string, "BL-R:Lite\0");
                        } else { // else, Lumos Remote
                            if(0xFFFFFFFF != gulBatteryOfRemote) { sprintf(string, "BL-R:%03u\0", gulBatteryOfRemote); }
                            else                                 { sprintf(string, "BL-R:NA\0");                    }
                        }
                        unsigned long ulLen = strlen(string); //if(strlen("BL-R:100") > ulLen) { ulLen = strlen("BL-R:100"); }
                        LL_BLE_TX((uint8_t *)string, ulLen);
        }
        #endif
    }
        
    TxFlashModeParaToApp(); //TxCurrentFlashModeToApp();
    
}



static void handle_turning_braking(E_TURN_STATE turning, E_BRAKE_STATE braking) { gtSysState.eTurnState = turning; gtSysState.eBrakeState = braking; LL_HelmetActionWhenStateChanged(); }
//static void handle_auto_braking_onoff(unsigned long onoff) { gtPara.ulBrakeFunction = onoff; gulFlashStoreNeeded = 1; IndicatorOfBrakeFuncOnOff__Start(); }
void LL_MSG_withAPP__RX(void) {
    if(0 == sgulNusDataHandleNeeded) return;
////if(1 == gulDisconnectRountine)   return; // Do not accept command if current connetion not confirmed!
    
    uint8_t * p_data = sgaucNusRcvBuf;
    uint16_t length  = sgulNusDataHandleNeeded;
    
    unsigned long      cmd_long;
//  unsigned long long cmd_longlong;

    char string[24]; 

    if(2 == length) {
        cmd_long = LL_2BYTE_TO_LONG(p_data[0], p_data[1]); switch(cmd_long) {
            //
            // turning & braking
            case LL_2BYTE_TO_LONG('O', 'N'): handle_turning_braking(TURNING_NONE, BRAKE_OFF); break;//gtSysState.eTurnState = TURNING_NONE; gtSysState.eBrakeState = BRAKE_OFF; LL_HelmetActionWhenStateChanged(); break;
            case LL_2BYTE_TO_LONG('O', 'K'): handle_turning_braking(TURNING_NONE, BRAKE_ON ); break;//gtSysState.eTurnState = TURNING_NONE; gtSysState.eBrakeState = BRAKE_ON ; LL_HelmetActionWhenStateChanged(); break;
            case LL_2BYTE_TO_LONG('L', 'N'): handle_turning_braking(TURNING_L   , BRAKE_OFF); break;//gtSysState.eTurnState = TURNING_L   ; gtSysState.eBrakeState = BRAKE_OFF; LL_HelmetActionWhenStateChanged(); break;
            case LL_2BYTE_TO_LONG('L', 'K'): handle_turning_braking(TURNING_L   , BRAKE_ON ); break;//gtSysState.eTurnState = TURNING_L   ; gtSysState.eBrakeState = BRAKE_ON ; LL_HelmetActionWhenStateChanged(); break;
            case LL_2BYTE_TO_LONG('R', 'N'): handle_turning_braking(TURNING_R   , BRAKE_OFF); break;//gtSysState.eTurnState = TURNING_R   ; gtSysState.eBrakeState = BRAKE_OFF; LL_HelmetActionWhenStateChanged(); break;
            case LL_2BYTE_TO_LONG('R', 'K'): handle_turning_braking(TURNING_R   , BRAKE_ON ); break;//gtSysState.eTurnState = TURNING_R   ; gtSysState.eBrakeState = BRAKE_ON ; LL_HelmetActionWhenStateChanged(); break;
            //
            // Auto-Braking ON/OFF
            case LL_2BYTE_TO_LONG('B', 'O'): gtPara.ulBrakeFunction = 1; gulFlashStoreNeeded = 1; IndicatorOfBrakeFuncOnOff__Start(); break;
            case LL_2BYTE_TO_LONG('B', 'C'): gtPara.ulBrakeFunction = 0; gulFlashStoreNeeded = 1; IndicatorOfBrakeFuncOnOff__Start(); break;
            case LL_2BYTE_TO_LONG('B', 'S'): sprintf(string, "B?:%+06d", gtPara.ssThreshhold);
                if(0 == gtPara.ulBrakeFunction) { string[1] = 'C'; }//sprintf(string, "BC:%+06d", gtPara.ssThreshhold); }
                else                            { string[1] = 'O'; }//sprintf(string, "BO:%+06d", gtPara.ssThreshhold); }
                LL_BLE_TX((uint8_t *)string, 9);
            break;
            //
            // Battery Level
            case LL_2BYTE_TO_LONG('B', 'L'): {
                unsigned long battery_level = LL_Battery_Level();
                //char string[8]; 
                if(LL_BATTERY_LEVEL_NONE != battery_level) { sprintf(string, "BL:%03u\0", battery_level); }
                else                                       { sprintf(string, "BL:N/A\0");               }
                LL_BLE_TX((uint8_t *)string, strlen(string));
            }; break;
            //
            // Brightness
            #if 0 // 0: cancelled after each mode has its own brightness
            case LL_2BYTE_TO_LONG('B', 'B'): {
                sprintf(string, "BB%d", gtPara.ucBrightnessOfBackLight);
                LL_BLE_TX((uint8_t *)string, 3);
            }; break;
            #endif
            //
            // Buzzer
            case LL_2BYTE_TO_LONG('B', '0'): //gtPara.beep_mode_of_power_on_off = E_BEEP_MODE_OF_POWER_ON_OFF__MUTE; gtPara.eBeepMode = BEEP_NONE;                  gulFlashStoreNeeded = 1; LL_BeepForTurning__ON(); break;
            case LL_2BYTE_TO_LONG('B', '1'): //gtPara.beep_mode_of_power_on_off = E_BEEP_MODE_OF_POWER_ON_OFF__BEEP; gtPara.eBeepMode = BEEP_NONE;                  gulFlashStoreNeeded = 1; LL_BeepForTurning__ON(); break;
            case LL_2BYTE_TO_LONG('B', '2'): //gtPara.beep_mode_of_power_on_off = E_BEEP_MODE_OF_POWER_ON_OFF__BEEP; gtPara.eBeepMode = BEEP_WHEN_TURNING_ONOFF;    gulFlashStoreNeeded = 1; LL_BeepForTurning__ON(); break;
            case LL_2BYTE_TO_LONG('B', '3'): //gtPara.beep_mode_of_power_on_off = E_BEEP_MODE_OF_POWER_ON_OFF__BEEP; gtPara.eBeepMode = BEEP_EVERY_1_TURNING_FLASH; gulFlashStoreNeeded = 1; LL_BeepForTurning__ON(); break;
            {
                E_BEEP_MODE_OF_POWER_ON_OFF beep_when_power_onoff[ ] = { E_BEEP_MODE_OF_POWER_ON_OFF__MUTE, E_BEEP_MODE_OF_POWER_ON_OFF__BEEP, E_BEEP_MODE_OF_POWER_ON_OFF__BEEP, E_BEEP_MODE_OF_POWER_ON_OFF__BEEP };
                E_BEEP_MODE beep_when_turning[                     ] = { BEEP_NONE                        , BEEP_NONE                        , BEEP_WHEN_TURNING_ONOFF          , BEEP_EVERY_1_TURNING_FLASH        };
                unsigned long index = p_data[1] - '0';
                gtPara.beep_mode_of_power_on_off = beep_when_power_onoff[ index]; 
                gtPara.eBeepMode                 = beep_when_turning[     index]; 
                gulFlashStoreNeeded = 1;
                LL_BeepForTurning__ON(); 
            }; break;
        }
				#ifdef Buzzer_Preset
        else if( ('S' == p_data[0]) && ('N' == p_data[1]) ) { gtPara.eBeepMode = BEEP_NONE;
            T_BUZZER_BEEP beep_type = BUZZER_BEEP__NONE;
            memcpy(&(gtPara.beep_of_turning[E_BEEP_OF_TURNING__L_BEGIN]), &beep_type, sizeof(T_BUZZER_BEEP));
            memcpy(&(gtPara.beep_of_turning[E_BEEP_OF_TURNING__R_BEGIN]), &beep_type, sizeof(T_BUZZER_BEEP));
            memcpy(&(gtPara.beep_of_turning[E_BEEP_OF_TURNING__L_END]),   &beep_type, sizeof(T_BUZZER_BEEP));
            memcpy(&(gtPara.beep_of_turning[E_BEEP_OF_TURNING__R_END]),   &beep_type, sizeof(T_BUZZER_BEEP));
            //
            gulFlashStoreNeeded = 1; } 
        else if( ('S' == p_data[0]) && ('O' == p_data[1]) ) { gtPara.eBeepMode = BEEP_WHEN_TURNING_ONOFF;    
            T_BUZZER_BEEP beep_type = BUZZER_BEEP__ONCE;
            memcpy(&(gtPara.beep_of_turning[E_BEEP_OF_TURNING__L_BEGIN]), &beep_type, sizeof(T_BUZZER_BEEP));
            memcpy(&(gtPara.beep_of_turning[E_BEEP_OF_TURNING__R_BEGIN]), &beep_type, sizeof(T_BUZZER_BEEP));
            memcpy(&(gtPara.beep_of_turning[E_BEEP_OF_TURNING__L_END]),   &beep_type, sizeof(T_BUZZER_BEEP));
            memcpy(&(gtPara.beep_of_turning[E_BEEP_OF_TURNING__R_END]),   &beep_type, sizeof(T_BUZZER_BEEP));
            //
            gulFlashStoreNeeded = 1; } 
        else if( ('S' == p_data[0]) && ('C' == p_data[1]) ) { gtPara.eBeepMode = BEEP_EVERY_1_TURNING_FLASH; 
            T_BUZZER_BEEP beep_type_begin = BUZZER_BEEP__PER_0p5S;
            T_BUZZER_BEEP beep_type_end   = BUZZER_BEEP__NONE;
            memcpy(&(gtPara.beep_of_turning[E_BEEP_OF_TURNING__L_BEGIN]), &beep_type_begin, sizeof(T_BUZZER_BEEP));
            memcpy(&(gtPara.beep_of_turning[E_BEEP_OF_TURNING__R_BEGIN]), &beep_type_begin, sizeof(T_BUZZER_BEEP));
            memcpy(&(gtPara.beep_of_turning[E_BEEP_OF_TURNING__L_END]),   &beep_type_end,   sizeof(T_BUZZER_BEEP));
            memcpy(&(gtPara.beep_of_turning[E_BEEP_OF_TURNING__R_END]),   &beep_type_end,   sizeof(T_BUZZER_BEEP));
            //
            gulFlashStoreNeeded = 1; } 
        else if( ('S' == p_data[0]) && ('H' == p_data[1]) ) { gtPara.eBeepMode = BEEP_EVERY_4_TURNING_FLASH; 
            T_BUZZER_BEEP beep_type_begin = BUZZER_BEEP__PER_2S;
            T_BUZZER_BEEP beep_type_end   = BUZZER_BEEP__NONE;
            memcpy(&(gtPara.beep_of_turning[E_BEEP_OF_TURNING__L_BEGIN]), &beep_type_begin, sizeof(T_BUZZER_BEEP));
            memcpy(&(gtPara.beep_of_turning[E_BEEP_OF_TURNING__R_BEGIN]), &beep_type_begin, sizeof(T_BUZZER_BEEP));
            memcpy(&(gtPara.beep_of_turning[E_BEEP_OF_TURNING__L_END]),   &beep_type_end,   sizeof(T_BUZZER_BEEP));
            memcpy(&(gtPara.beep_of_turning[E_BEEP_OF_TURNING__R_END]),   &beep_type_end,   sizeof(T_BUZZER_BEEP));
            //
            gulFlashStoreNeeded = 1; } 
        else if( ('S' == p_data[0]) && ('L' == p_data[1]) ) { gtPara.eBeepMode = BEEP_EVERY_8_TURNING_FLASH; 
            T_BUZZER_BEEP beep_type_begin = BUZZER_BEEP__PER_2S;
            T_BUZZER_BEEP beep_type_end   = BUZZER_BEEP__NONE;
            memcpy(&(gtPara.beep_of_turning[E_BEEP_OF_TURNING__L_BEGIN]), &beep_type_begin, sizeof(T_BUZZER_BEEP));
            memcpy(&(gtPara.beep_of_turning[E_BEEP_OF_TURNING__R_BEGIN]), &beep_type_begin, sizeof(T_BUZZER_BEEP));
            memcpy(&(gtPara.beep_of_turning[E_BEEP_OF_TURNING__L_END]),   &beep_type_end,   sizeof(T_BUZZER_BEEP));
            memcpy(&(gtPara.beep_of_turning[E_BEEP_OF_TURNING__R_END]),   &beep_type_end,   sizeof(T_BUZZER_BEEP));
            //
            gulFlashStoreNeeded = 1; }
				#endif
    } else if(3 == length) {
        if( ('B' == p_data[0]) && ('B' == p_data[1]) ) {
            #if 0 // 0: cancelled after each mode has its own brightness
            if( ('0' <= p_data[2]) && ('9' >= p_data[2]) ) {
                unsigned long ulBrightness = (p_data[2]-'0'+1) * 10;
                LL_PWM_DutySet(E_LL_LED_FRONT, ulBrightness); 
                gtPara.ucBrightnessOfBackLight = ulBrightness; gulFlashStoreNeeded = 1; 
            }
            #endif
        } else if('Z' == p_data[0]) { // buzzer setting, "Zmn", while "m" is 0/1/2/3 menas L(begin)/R(begin)/L(end)/R(end), n = 0~5, means None/Once/0.5s/2s/Car-Blinker/Woodcock
            unsigned long m = p_data[1] - '0';
            unsigned long n = p_data[2] - '0'; {
                T_BUZZER_BEEP beep_types[] = {
                    BUZZER_BEEP__NONE, 
                    BUZZER_BEEP__ONCE, 
                    BUZZER_BEEP__PER_0p5S, 
                    BUZZER_BEEP__PER_2S, 
                    BUZZER_BEEP__1HZ, 
                    BUZZER_BEEP__WOODCOCK
                };
                memcpy(&(gtPara.beep_of_turning[m]), &(beep_types[n]), sizeof(T_BUZZER_BEEP));
            gulFlashStoreNeeded = 1; }
        }
    } else if(4 == length) {
        if( ('B' == p_data[0]) && ('M' == p_data[1]) ) { 
            unsigned char mode       = p_data[2]; if(5 < mode) return;
            unsigned char brightness = p_data[3]; //if(100 < brightness) return;
            
            LL_Get_Color_Value_Via_Brightness(brightness, &gtPara.brightness_individual[mode][E_LL_LED_FRONT], &gtPara.brightness_individual[mode][E_LL_LED_REAR]);
            gulFlashStoreNeeded = 1; // !!!!!!
            LL_HelmetActionWhenStateChanged();
        }else if(0 == memcmp("PUB", p_data, 3)) { char command = p_data[3]; // turn ON/OFF public network
            if('1' == command) { // if command "ON"
                gtPara.publicNetworkEnabled = 1; // 1: enabled
                gulFlashStoreNeeded = 1;
            } else if('0' == command) { // else if command "OFF"
                gtPara.publicNetworkEnabled = 0; // 0: disabled
                gulFlashStoreNeeded = 1;
            } else if('?' == command) { // else if command "read"
                if(1 == gtPara.publicNetworkEnabled) {
                    sprintf(string, "PUB1\0");
                } else {
                    sprintf(string, "PUB0\0");
                }
                LL_BLE_TX((uint8_t *)string, strlen(string));
            }
				}else if( 
            ('T' == p_data[0]) && 
            ('F' == p_data[1]) &&
            ('M' == p_data[2])  ) {// factory mode
							
            if(p_data[3] == '0'){
                gtPara.glFactoryMode = false;
            }else if(p_data[3] == '1'){
                gtPara.glFactoryMode = true;
            }
            sprintf(string, "TFM-%u",gtPara.glFactoryMode);
						LL_BLE_TX((uint8_t *)string, strlen(string));         
            gulFlashStoreNeeded = 1; // !!!!!!  
        } else {
            cmd_long = LL_4BYTE_TO_LONG(p_data[0], p_data[1], p_data[2], p_data[3]); switch(cmd_long) {
                case LL_4BYTE_TO_LONG('B', 'L', '-', 'R'): { // BL-R
                    if(0xCCCCCCCC == gulBatteryOfRemote) { // if Remote Lite
                        sprintf(string, "BL-R:Lite\0");
                    } else { // else, Lumos Remote
                        if(0xFFFFFFFF != gulBatteryOfRemote) { sprintf(string, "BL-R:%03u\0", gulBatteryOfRemote); }
                        else                                 { sprintf(string, "BL-R:NA\0");                    }
                    }
                    LL_BLE_TX((uint8_t *)string, strlen(string));
                }; break;
                case LL_4BYTE_TO_LONG('V', 'E', 'R', 'H'): { // VERH
                    sprintf(string, "%04d-%02d-%02d"     ,
                        ((LL_VER_DATE&0xF000)>>12) + 2016,
                        ((LL_VER_DATE&0x0F00)>> 8)       ,
                        ((LL_VER_DATE&0x00FF)    )       );
                    LL_BLE_TX((uint8_t *)string, 10);
                }; break;
                case LL_4BYTE_TO_LONG('V', 'E', 'R', 'R'): { // VERR
                    unsigned short first_byte_high_4bit;
                    unsigned short first_byte_low_4bit;
                    unsigned short second_byte_high_4bit;
//		    		unsigned short second_byte_low_4bit;
		
						if(sgtMsg.cStateBrake == 'C') {
								sprintf(string, "RL-V0.5");							
						}else{				
							if(sgtMsg.usVerDate != 0x0000){
								uint8_t year,fw_version_h,fw_version_l,model_h;
								first_byte_high_4bit = ((sgtMsg.usVerDate)&0xF000)>>12;	
								first_byte_low_4bit = ((sgtMsg.usVerDate)&0x0F00)>>8;	
								second_byte_high_4bit = ((sgtMsg.usVerDate)&0x00F0)>>4;	
//								second_byte_low_4bit = (sgtMsg.usVerDate)&0x000F;
								
								if(second_byte_high_4bit > 0x01){ // >0x01 indicates not the date but the model
									fw_version_h = first_byte_high_4bit; 
									fw_version_l = first_byte_low_4bit;
									model_h = second_byte_high_4bit;
									//model_l = second_byte_low_4bit;
									if(model_h == 0x0A)//model_h = LL_MODEL_H*10 in order to distinguish from the date
									{ //remote1.0
										sprintf(string, "RN51-V%d.%d",fw_version_h, fw_version_l);
									}
									else if(model_h == 0x02)
									{ //remote2.0
										sprintf(string, "RN52-V%d.%d",fw_version_h, fw_version_l);
									}
									else if(model_h == 0x04)
									{ //remote4.0 remote_USBC										
										sprintf(string, "RU-V%d.%d",fw_version_h, fw_version_l);
									}
								}else{
									year = first_byte_high_4bit; //year:0~F
									//month = first_byte_low_4bit;	//month:1-C
									if(year <= 0x01){	//2016-2017
										sprintf(string,"RN51-V2.5");
									}else{
										sprintf(string,"RN52-V1.3");
									}
								}
							}else{
									sprintf(string,"UNKRMT");
							}
						}
                    LL_BLE_TX((uint8_t *)string, strlen(string));
                }; break;
                case LL_4BYTE_TO_LONG('T', 'R', 'S', 'N'): { // TRSN
                    #ifndef _LL_DELETE_PRODUCTION_CODE
                                    if(SYS_PRODUCTION_TESTING_MODE == gtSysState.eOnOFF) {                
                                        sprintf(string, "FRSN");
                                        memcpy(&string[4], &(gtPara.aucSN[0]), 14);
                                        LL_BLE_TX((uint8_t *)string, 18);
                                        //LL_Helmet_CmdTestingMode_CMD(LL_CMD_TESTING_RSNH);
                                    }
                    #endif                
                }; break;
            }
        }
    } else if(5 == length) {
        if('F' == p_data[0]) { // customer flashing mode cmd
                    // parse the cmd
                    unsigned long mode            ;//= p_data[1] >> 6; if(2 < mode) { mode = 0; }
                    unsigned long part            ;//= p_data[1] & 0x3F;
                    unsigned long timeslot_states ;//= (p_data[4]<<16) + (p_data[3]<<8) + p_data[2];
                    unpackFlashModeParaFromApp(p_data, &mode, &part, &timeslot_states);
                    if(LL_PART_BACK_LIGHT == part) {
                        LL_Para__set_flashing_pattern_fromApp(timeslot_states, mode, E_LL_LED_REAR);//gtPara.customer_flashing_timeslot_states[mode][E_LL_LED_BACK_L] = timeslot_states;
                        //gulFlashStoreNeeded = 1;
                    } else if(LL_PART_FRONT_LIGHT == part) {
                        LL_Para__set_flashing_pattern_fromApp(timeslot_states, mode, E_LL_LED_FRONT);//gtPara.customer_flashing_timeslot_states[mode][E_LL_LED_FRONT] = timeslot_states;
                        //gulFlashStoreNeeded = 1;
                    }

                    // switch to this mode immediately
                    switch(mode) {
                        case 0:  gtSysState.eModeOfWarningLight = E_MODE_OF_WARNING_LIGHT__Mode3; break;
                        case 1:  gtSysState.eModeOfWarningLight = E_MODE_OF_WARNING_LIGHT__Mode1; break;
                        case 2:  gtSysState.eModeOfWarningLight = E_MODE_OF_WARNING_LIGHT__Mode2; break;
                        default: gtSysState.eModeOfWarningLight = E_MODE_OF_WARNING_LIGHT__Mode3; break; // shd not happen
                    }
                    LL_Helmet_ChangeStateTo_NextMode();
        } else if('f' == p_data[0]) {
                    unsigned long mode ;//= p_data[1] - '0';
                    unpackCurrentModeFromApp(p_data, &mode);
                    // switch to this mode immediately
                    switch(mode) {
                        case 0:  gtSysState.eModeOfWarningLight = E_MODE_OF_WARNING_LIGHT__Mode3; break;
                        case 1:  gtSysState.eModeOfWarningLight = E_MODE_OF_WARNING_LIGHT__Mode1; break;
                        case 2:  gtSysState.eModeOfWarningLight = E_MODE_OF_WARNING_LIGHT__Mode2; break;
                        default: gtSysState.eModeOfWarningLight = E_MODE_OF_WARNING_LIGHT__Mode3; break; // shd not happen
                    }
                    LL_Helmet_ChangeStateTo_NextMode();
        } else {
            cmd_long = LL_4BYTE_TO_LONG(p_data[0], p_data[1], p_data[2], p_data[3]); switch(cmd_long) {
                case LL_4BYTE_TO_LONG('N', 'E', 'T', 'I'): { if('D' == p_data[4]){// NETID: read "paired app ID"
                    //            sprintf(string, "NETID ");
                    //            memcpy(&string[6], &(gtPara.netID), LL_ESB_BASE_ADDR_LEN);
                    //            string[6+LL_ESB_BASE_ADDR_LEN] = '\0';
                    //            LL_BLE_TX((uint8_t *)string, strlen(string));
                }}; break;
                case LL_4BYTE_TO_LONG('T', 'P', 'A', 'I'): { if('R' == p_data[4]){// TPAIR
                    #ifndef _LL_DELETE_PRODUCTION_CODE
                                    if( (SYS_PRODUCTION_TESTING_MODE == gtSysState.eOnOFF) || (4 == gulCmdTestingMode) ) {
                                        sprintf(string, "FPAIR\0");
                                        LL_BLE_TX((uint8_t *)string, strlen(string));
                                        // turn off the LED, then enter pairing mode:
                                        LL_LEDs_OFF(); //LL_Helmet_RedGreenFlashWhenTestMode_OFF();
                                        gulCmdTestingMode = 1; LL_Helmet_ChangeStateTo_Pairing();
                                    }
                    #endif                
                }}; break;
                case LL_4BYTE_TO_LONG('T', 'E', 'S', 'T'): { if('T' == p_data[4]){// TESTT
                    #ifndef _LL_DELETE_PRODUCTION_CODE
                                    if(SYS_PAIRING == gtSysState.eOnOFF) {
                                        gulDisconnectRountine = 0; // !!!!!!
                                        // Entering test-mode
                                        sprintf(string, "FESTT\0");
                                        LL_BLE_TX((uint8_t *)string, 5);
                                        LL_Helmet_ChangeStateTo_ProductionTestingMode();
                                    }
                    #endif                
                }}; break;
                case LL_4BYTE_TO_LONG('T', 'R', 'S', 'N'): { 
                    if('H' == p_data[4]){// TRSNH
                        #ifndef _LL_DELETE_PRODUCTION_CODE
                                        if(SYS_PRODUCTION_TESTING_MODE == gtSysState.eOnOFF) {                
                                            sprintf(string, "FSNH");
                                            memcpy(&string[4], &(gtPara.aucSN[0]), 10);
                                            string[14] = '\0';
                                            LL_BLE_TX((uint8_t *)string, strlen(string));
                                            //LL_Helmet_CmdTestingMode_CMD(LL_CMD_TESTING_RSNH);
                                        }
                        #endif                
                    }
                    if('L' == p_data[4]){// TRSNL
                        #ifndef _LL_DELETE_PRODUCTION_CODE
                                        if(SYS_PRODUCTION_TESTING_MODE == gtSysState.eOnOFF) {                
                                            sprintf(string, "FSNL");
                                            memcpy(&string[4], &(gtPara.aucSN[10]), 10);
                                            string[14] = '\0';
                                            LL_BLE_TX((uint8_t *)string, strlen(string));
                                            //LL_Helmet_CmdTestingMode_CMD(LL_CMD_TESTING_RSNH);
                                        }
                        #endif                
                    }
                }; break;
                case LL_4BYTE_TO_LONG('T', 'S', 'H', 'U'): { if('T' == p_data[4]){// TSHUT
                    #ifndef _LL_DELETE_PRODUCTION_CODE
                                    if( (SYS_PRODUCTION_TESTING_MODE == gtSysState.eOnOFF) || (4 == gulCmdTestingMode) ) {
                                        sprintf(string, "FSHUT");
                                        LL_BLE_TX((uint8_t *)string, strlen(string));
                                        nrf_delay_ms(100); LL_Helmet_ChangeStateTo_OFF();
                                    }
                    #endif                
                }}; break;
                case LL_4BYTE_TO_LONG('T', 'B', 'R', 'E'): { 
                    if('1' == p_data[4]){// TBRE1
                        #ifndef _LL_DELETE_PRODUCTION_CODE
                                        if(SYS_PRODUCTION_TESTING_MODE == gtSysState.eOnOFF) {                
                                            sprintf(string, "FBRE1");
                                            LL_BLE_TX((uint8_t *)string, strlen(string));
                                            gtPara.ulBrakeFunction = 1; gulFlashStoreNeeded = 1;
                                        }
                        #endif                
                    }
                    if('0' == p_data[4]){// TBRE0
                        #ifndef _LL_DELETE_PRODUCTION_CODE
                                        if(SYS_PRODUCTION_TESTING_MODE == gtSysState.eOnOFF) {                
                                            sprintf(string, "FBRE0");
                                            LL_BLE_TX((uint8_t *)string, strlen(string));
                                            gtPara.ulBrakeFunction = 0; gulFlashStoreNeeded = 1;
                                        }
                        #endif                
                    }
                }; break;
                case LL_4BYTE_TO_LONG('T', 'R', 'X', 'Y'): { if('Z' == p_data[4]){// TRXYZ
                    #ifndef _LL_DELETE_PRODUCTION_CODE
                                    if(SYS_PRODUCTION_TESTING_MODE == gtSysState.eOnOFF) {                
                                        sprintf(string, "FRXYZ%+04d%+04d%+04d", gtPara.ssKxyz[0], gtPara.ssKxyz[1], gtPara.ssKxyz[2]);
                                        LL_BLE_TX((uint8_t *)string, strlen(string));
                                    }
                    #endif                
                }}; break;
                case LL_4BYTE_TO_LONG('T', 'G', 'R', 'T'): { if('H' == p_data[4]){// TGRTH
                    #ifndef _LL_DELETE_PRODUCTION_CODE
                                    if(SYS_PRODUCTION_TESTING_MODE == gtSysState.eOnOFF) {                
                                        sprintf(string, "FGRTH%+06d", gtPara.ssThreshhold);
                                        LL_BLE_TX((uint8_t *)string, strlen(string));
                                    }
                    #endif                
                }}; break;
                case LL_4BYTE_TO_LONG('T', 'R', 'E', 'S'): { if('T' == p_data[4]){// TREST
                    #ifndef _LL_DELETE_PRODUCTION_CODE
                                    if(SYS_PRODUCTION_TESTING_MODE == gtSysState.eOnOFF) {                
                                        sprintf(string, "FREST\0");
                                        LL_BLE_TX((uint8_t *)string, strlen(string));
                                        gtPara.ulBrakeFunction = 0;
                                        gtPara.ssKxyz[0] = 100;
                                        gtPara.ssKxyz[1] =   0;
                                        gtPara.ssKxyz[2] =   0;
                                        gulFlashStoreNeeded = 1;
                                    }
                    #endif                
                }}; break;
                case LL_4BYTE_TO_LONG('T', 'R', 'U', 'F'): { if('C' == p_data[4]){// TRUFC
							//sprintf(string, "URevCnt:%d", HelmetRevCnt);
                }}; break;
            }
        }
    } else if(6 == length) {
        if(SYS_PAIRING == gtSysState.eOnOFF) { 
            if(0 == strncmp((const char *)(gtPara.scBleAddr), (const char *)(p_data), BLE_DEVICE_ADDR_LEN) ) {
                    // app ID match, not exit pairing mode.
                    TxFlashModeParaToApp__Start();
            } else {
                    memcpy(gtPara.scBleAddr, p_data, BLE_DEVICE_ADDR_LEN); 
                    //SEGGER_RTT_WriteString(0, "pairing: receive ID\n");
                    // exit pairing mode at once:
                    gulFlashStoreNeeded = 1;                            
                    LL_Helmet_ChangeStateTo_ON();
                    TxFlashModeParaToApp__Start();
            }
        } else {
            if(0 == strncmp((const char *)(gtPara.scBleAddr), (const char *)(p_data), BLE_DEVICE_ADDR_LEN) ) {
                gulDisconnectRountine = 0;
                //SEGGER_RTT_WriteString(0, "normal: ID match\n");
                TxFlashModeParaToApp__Start();
            } else {
                //SEGGER_RTT_WriteString(0, "normal: ID not match\n");
            }
        }
    } else if(8 == length) {
        if('X' == p_data[0]) { gtPara.ssKxyz[0]    = atoi((const char *)(&(p_data[1]))); gulFlashStoreNeeded = 1; }
        if('Y' == p_data[0]) { gtPara.ssKxyz[1]    = atoi((const char *)(&(p_data[1]))); gulFlashStoreNeeded = 1; }
        if('Z' == p_data[0]) { gtPara.ssKxyz[2]    = atoi((const char *)(&(p_data[1]))); gulFlashStoreNeeded = 1; }
        if('T' == p_data[0]) { gtPara.ssThreshhold = atoi((const char *)(&(p_data[1]))); gulFlashStoreNeeded = 1; }
				if(0 == memcmp("POWEROFF", p_data, 8)) { // if message is "POWEROFF"
                            { // Power Off: copied from the one of "Long Press Power Off"
//                                if(1 == getPara(syncPowerOff)) {
//                                    geBroadcastState = E_BROADCAST_STATE__NEED_PTX_PowerOffMessage; // sync "power off"
//                                }
                                //
                                LL_LEDs_OFF();
                                LL_Helmet_ChangeStateTo_BeepBeforeOFF();   
                            }                                
                        }
    } else if(9 == length) {
        if(      0 == memcmp("PAIRING-Y", p_data, 9) ) { if(SYS_ON      == gtSysState.eOnOFF) { LL_Helmet_ChangeStateTo_Pairing(); } }
        else if( 0 == memcmp("PAIRING-N", p_data, 9) ) { if(SYS_PAIRING == gtSysState.eOnOFF) { LL_Helmet_ChangeStateTo_ON();      } }
				else if(0 == memcmp("POWEROFF", p_data, 8)) { char command = p_data[8]; // turn ON/OFF sync POWER off
						if('1' == command) { // if command "ON"
								setPara(syncPowerOff, 1); // 1: enabled
								gulFlashStoreNeeded = 1;
						} else if('0' == command) { // else if command "OFF"
								setPara(syncPowerOff, 0); // 0: disabled
								gulFlashStoreNeeded = 1;
						} else if('?' == command) { // else if command "read"
								if(1 == getPara(syncPowerOff)) {
										sprintf(string, "POWEROFF1\0");
								} else {
										sprintf(string, "POWEROFF0\0");
								}
								LL_BLE_TX((uint8_t *)string, strlen(string));
						}
				}
		} else if((10) == length) { 
        if( 0 == memcmp("NETID ", p_data, 6) ) { // change the "paired app ID"
            memcpy(gtPara.netID, &(p_data[6]), LL_ESB_BASE_ADDR_LEN); gtPara.netID_exist = 1; gulFlashStoreNeeded = 1;
						geBroadcastState = E_BROADCAST_STATE__NEED_PTX_OnlyClock;//E_BROADCAST_STATE__NEED_PTX; // sync
            sprintf(string, "NETID changed\0");
						LL_BLE_TX((uint8_t *)string, strlen(string));
        }else if('A' == p_data[0] && 'N' == p_data[1] && 'C' == p_data[2]) { // customer flashing mode cmd
            // parse the cmd
            T_Animation_Data AnimationData;
            AnimationData.eMode = p_data[3];
            AnimationData.part = p_data[4];
            
            AnimationData.color_code = p_data[5];
            AnimationData.type = (E_Ws2812_Animation_Type)p_data[6];
            if(AnimationData.type == LL_TYPE_COMMON_ANIMATION){  //need flash_pattern
                AnimationData.flash_pattern  = (p_data[7]<< 16) + (p_data[8]<< 8) + (p_data[9]<< 0);
                if(LL_PART_BACK_LIGHT == AnimationData.part) {
                    LL_Para__set_flashing_pattern_fromApp(AnimationData.flash_pattern, AnimationData.eMode, E_LL_LED_REAR);
                    LL_Get_Color_Value_Via_Color(AnimationData.color_code, &gtPara.brightness_individual[AnimationData.eMode][E_LL_LED_REAR]);
                    gtPara.ePatternOfRearLight[AnimationData.eMode] = AnimationData.type;
                    //gtPara.brightness_individual[AnimationData.eMode][E_LL_LED_REAR] = AnimationData.color_code;
                    gulFlashStoreNeeded = 1;
                } else if(LL_PART_FRONT_LIGHT == AnimationData.part) {
                    LL_Para__set_flashing_pattern_fromApp(AnimationData.flash_pattern, AnimationData.eMode, E_LL_LED_FRONT);
                    LL_Get_Color_Value_Via_Color(AnimationData.color_code, &gtPara.brightness_individual[AnimationData.eMode][E_LL_LED_FRONT]);
                    gtPara.ePatternOfFrontLight[AnimationData.eMode] = AnimationData.type;
                    //gtPara.brightness_individual[AnimationData.eMode][E_LL_LED_FRONT] = AnimationData.color_code;
                    gulFlashStoreNeeded = 1;
                }                
            }else{
                #define Animation_FlashPattern 0x00FFFFFF
                AnimationData.type += 2;//ignore solid+slow+flash 
                if(AnimationData.type < LED_ANIMATION_COMMON_MODE_NUM){
                    if(LL_PART_BACK_LIGHT == AnimationData.part) {
                        LL_Para__set_flashing_pattern_fromApp(Animation_FlashPattern, AnimationData.eMode, E_LL_LED_REAR);//gtPara.customer_flashing_timeslot_states[mode][E_LL_LED_BACK_L] = timeslot_states;
                        gtPara.ePatternOfRearLight[AnimationData.eMode] = AnimationData.type;
                        gulFlashStoreNeeded = 1;
                    } else if(LL_PART_FRONT_LIGHT == AnimationData.part) {
                        LL_Para__set_flashing_pattern_fromApp(Animation_FlashPattern, AnimationData.eMode, E_LL_LED_FRONT);//gtPara.customer_flashing_timeslot_states[mode][E_LL_LED_FRONT] = timeslot_states;
                        gtPara.ePatternOfFrontLight[AnimationData.eMode] = AnimationData.type;
                        gulFlashStoreNeeded = 1;
                    } 
                }
            }                
                
            // switch to this mode immediately
            switch(AnimationData.eMode) {
                case 0:  gtSysState.eModeOfWarningLight = E_MODE_OF_WARNING_LIGHT__Mode3; break;
                case 1:  gtSysState.eModeOfWarningLight = E_MODE_OF_WARNING_LIGHT__Mode1; break;
                case 2:  gtSysState.eModeOfWarningLight = E_MODE_OF_WARNING_LIGHT__Mode2; break;
                default: gtSysState.eModeOfWarningLight = E_MODE_OF_WARNING_LIGHT__Mode3; break; // shd not happen
            }
            LL_Helmet_ChangeStateTo_NextMode();
        }
    } else if((11) == length) { 
        if( 0 == memcmp("NETID RESET", p_data, 11) ) { // reset network ID
            extern T_LL_HardwareID gtHwID;
            memcpy(&(gtPara.netID), &(gtHwID.auc[0]), LL_ESB_BASE_ADDR_LEN); gtPara.netID_exist = 0; gulFlashStoreNeeded = 1;
            sprintf(string, "NETID reset\0");
            LL_BLE_TX((uint8_t *)string, strlen(string));
        } else if (0 == memcmp("DEL REMOTES", p_data, 11)) { 
            gtPara.tRemote.bits_isCheapRemote = 0;
            for(int indexDevice = 0; indexDevice < DEVICE_NUM_OF_XXX; indexDevice++) {
                memset(gtPara.tRemote.HardwareID[indexDevice].auc, 0xFF, LL_HARDWARE_ID_LEN);
            }
            sprintf(string, "Remotes deleted.\0");
            LL_BLE_TX((uint8_t *)string, strlen(string));                                                        
        }
    } else if(12 == length) {
        if(
            ('T' == p_data[0]) && 
            ('G' == p_data[1]) &&
            ('W' == p_data[2]) &&
            ('T' == p_data[3]) &&
            ('H' == p_data[4])    ) {
#ifndef _LL_DELETE_PRODUCTION_CODE
                if(SYS_PRODUCTION_TESTING_MODE == gtSysState.eOnOFF) { 
                    sprintf(string, "FGWTH\0");
                    LL_BLE_TX((uint8_t *)string, strlen(string));
                    gtPara.ssThreshhold = atoi((const char *)(&(p_data[5]))); 
                    gulFlashStoreNeeded = 1;
                }
#endif                
        }        
    } else if(15 == length) { // buzzer: "Bn-XYXYXYXYXYXY" while "n" is 0/1/2/3 menas L(begin)/R(begin)/L(end)/R(end), "B" can also be "b", loop if "B" not if "b".
        unsigned long need_loop = (p_data[0] == 'B') ? 1 : 0;
        unsigned long index     = p_data[1] - '0';
        unsigned long X, Y, XY; for(int i = 0; i < 6; i++) { // 6 "XY"
            // get XY
            X = (p_data[2*i+3] - '0');
            Y = (p_data[2*i+4] - '0');
            XY = X * 10 + Y; //if(0 == XY) { error = 1; break; }
            // save
            gtPara.beep_of_turning[index].slots_duration[i] = XY; gulFlashStoreNeeded = 1;
        }
        gtPara.beep_of_turning[index].need_loop = need_loop;
        // respond
        sprintf(string, "Buzzer time changed.\0"); LL_BLE_TX((uint8_t *)string, strlen(string));
    } else if(16 == length) {            
        if(
            ('T' == p_data[0]) && 
            ('G' == p_data[1]) &&
            ('W' == p_data[2])    ) {
                if(SYS_PRODUCTION_TESTING_MODE == gtSysState.eOnOFF) { 
                    sprintf(string, "FGW\0");
                    LL_BLE_TX((uint8_t *)string, strlen(string));
                    gtPara.ssKxyz[0] = atoi((const char *)(&(p_data[ 3]))); 
                    gtPara.ssKxyz[1] = atoi((const char *)(&(p_data[ 7]))); 
                    gtPara.ssKxyz[2] = atoi((const char *)(&(p_data[11]))); 
                    gulFlashStoreNeeded = 1;
                }
        }
    } else if(18 == length) {
        if( ('T' == p_data[0]) && 
            ('W' == p_data[1]) &&
            ('S' == p_data[2]) &&
            ('N' == p_data[3])    ) {
#ifndef _LL_DELETE_PRODUCTION_CODE
                if(SYS_PRODUCTION_TESTING_MODE == gtSysState.eOnOFF) { 
#if 0                    
                    if('H' == p_data[4]) {
                        sprintf(string, "FWSN-H\0");
                        LL_BLE_TX((uint8_t *)string, strlen(string));
                        memcpy(&(gtPara.aucSN[0]), &(p_data[5]), 10);
                        gulFlashStoreNeeded = 1;
                    }
                    if('L' == p_data[4]) {
                        sprintf(string, "FWSN-L\0");
                        LL_BLE_TX((uint8_t *)string, strlen(string));
                        memcpy(&(gtPara.aucSN[10]), &(p_data[5]), 10);
                        gulFlashStoreNeeded = 1;
                    }
#else
                    sprintf(string, "FWSN\0");
                    LL_BLE_TX((uint8_t *)string, strlen(string));
                    memcpy(&(gtPara.aucSN[0]), &(p_data[4]), 14);
                    gulFlashStoreNeeded = 1;                    
                    // turn off the LED, then enter pairing mode:
                    LL_LEDs_OFF(); //LL_Helmet_RedGreenFlashWhenTestMode_OFF();
                    gulCmdTestingMode = 1; LL_Helmet_ChangeStateTo_Pairing();
#endif                    
                }
#endif                
        } 
//    } else if(19 == length) { // buzzer: XYZ-XYZ-XYZ-XYZ-XYZ
//        gtPara.customer_flashing_timeslot_states[gtPara.eMode][E_LL_LED_BUZZER] = 0;
//        unsigned long X, Y, Z, XYZ, states = 0, error = 0; for(int i = 0; i < 5; i++) { // 5 "XYZ"
//            // get XYZ
//            X = (p_data[4*i+0] - '0');
//            Y = (p_data[4*i+1] - '0');
//            Z = (p_data[4*i+2] - '0');
//            XYZ = X * 100 + Y * 10 + Z; if(0 == XYZ) { error = 1; break; }
//            // add XYZ into XYZ series
//            XYZ = XYZ << (6*i);
//            states += XYZ;
//        }
//        // respond
//        if(0 == error) {
//            gtPara.customer_flashing_timeslot_states[gtPara.eMode][E_LL_LED_BUZZER] = states; gulFlashStoreNeeded = 1;
//            sprintf(string, "Buzzer time changed.\0"); LL_BLE_TX((uint8_t *)string, strlen(string));
//        } else {
//            sprintf(string, "Buzzer time error.\0"); LL_BLE_TX((uint8_t *)string, strlen(string));
//        }
    }
    
    sgulNusDataHandleNeeded = 0;
}
