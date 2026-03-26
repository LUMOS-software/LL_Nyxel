/*******************************************************************************
 Copyright (c) 2016 Lumen Labs (HK) Limited. All Rights Reserved.
*******************************************************************************/

#include "LL_Platform.h"
#include "LL_Common.h"
void IndicatorOfBrakeFuncOnOff__Start(void);
//
// BLE Service BAS
void battery_level_update(void);
// BLE Service NUS (TX)
unsigned long LL_BLE_TX(unsigned char *p_string, unsigned short length);
// BLE Service NUS (RX)
extern unsigned long sgulNusDataHandleNeeded; // 0: no,  others: length.
extern unsigned char sgaucNusRcvBuf[];
//
extern unsigned long flag_for_relay_remote_msg;
extern unsigned long time_for_relay_remote_msg;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LL_MSG_withDevice(void) {
    if( 1 == gulMsgHandleNeeded     ) { 
        if( (55 > gulRSSI) || (SYS_PAIRING != gtSysState.eOnOFF) ) {
            LL_MsgHandle_Helmet(&sgtMsg); 
            if(1==gulCmdTestingMode) { gulCmdTestingMode = 2; } // ??????
        }
    gulMsgHandleNeeded = 0; }

    LL_MSG_Handle__PublicMessage_parseWhenInMainloop();

//      LL_ESB_AckPayload(); LL_ESB_Tx();
        if(LL_ESB_data_received) {
            nrf_esb_payload_t rx_payload;
            while (nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS) { 
                if(0 == rx_payload.pipe) {
                    if(rx_payload.length == sizeof(T_MSG_toHelmet)) { 
                        gulRSSI = rx_payload.rssi;
                        //
                        memcpy(&sgtMsg, rx_payload.data, sizeof(T_MSG_toHelmet));
                        gulMsgHandleNeeded = 1;
                        #ifdef _LL_2p4GHz_RELAY
                            flag_for_relay_remote_msg = 1; time_for_relay_remote_msg = gulTimerCnt1ms; // relay it to others
                        #endif                        //
                        gtSysState.ulNoConnectionSinceSysOn = 0; // 0: mark that "some remote has connected"
                    } else if(rx_payload.length == sizeof(T_BROADCAST_MSG)) {
                        T_BROADCAST_MSG eBroadcastMsg;
                        memcpy(&eBroadcastMsg, rx_payload.data, sizeof(T_BROADCAST_MSG));
                        memcpy( &(gtPara.tRemote.HardwareID[0]), &(eBroadcastMsg.pairedRemoteID[0]), sizeof(T_LL_HardwareID) ); gulFlashStoreNeeded = 1;
                        memcpy( &(gtPara.tRemote.HardwareID[1]), &(eBroadcastMsg.pairedRemoteID[1]), sizeof(T_LL_HardwareID) ); gulFlashStoreNeeded = 1;
                    } else if(rx_payload.length == sizeof(T_BROADCAST_MSG_POWER_OFF)) { 
                        if(1 == getPara(syncPowerOff)) {    
                            LL_LEDs_OFF();
														LL_Helmet_ChangeStateTo_BeepBeforeOFF();   
                        }
                    } else if( (rx_payload.length == sizeof(T_BROADCAST_MSG_PUBLIC            ))
                           ||  (rx_payload.length == sizeof(T_BROADCAST_MSG_PUBLIC_RE_TRANSMIT)) ) { 
                                geBroadcastState = E_BROADCAST_STATE__OFF;
                                LL_MSG_Handle__PublicMessage_saveWhenInInterrupt(rx_payload.data, rx_payload.length);
                    } else { /* noise */ }
                } else if(1 == rx_payload.pipe) { 
                    if( (rx_payload.length == sizeof(T_BROADCAST_MSG_PUBLIC            ))
                    ||  (rx_payload.length == sizeof(T_BROADCAST_MSG_PUBLIC_RE_TRANSMIT)) ) { 
                        geBroadcastState = E_BROADCAST_STATE__OFF;
                        LL_MSG_Handle__PublicMessage_saveWhenInInterrupt(rx_payload.data, rx_payload.length);
                    } else { /* noise */ }
                }
            }            
        LL_ESB_data_received = 0; }    
}


