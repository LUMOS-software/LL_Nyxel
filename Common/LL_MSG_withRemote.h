/*******************************************************************************
 Copyright (c) 2016 Lumen Labs (HK) Limited. All Rights Reserved.
*******************************************************************************/

#ifndef _LL_MSG_withRemote_H
#define _LL_MSG_withRemote_H



#include "LL_HardwareID.h"



extern unsigned long gulBatteryOfRemote;

#define LL_DEFAULT_OPTION_SLEEPTIME_OF_REMOTE   1 // 1: add 1 "10s" to the base 20s, so it's 30s

typedef struct
{
    T_LL_HardwareID tHardwareID;    // ID of remote.
    char cStateTurn; // as a remote: 'O' means no turning;  'L' means turning left;  'R' means turning right.
    char cStateBrake;  // as a brake: 'K' means braking;  'N' means no braking; "C" means "Cheap Remote".
    char cBattery;
    signed short ssAcc[3];
    char cBrakeFuncON;  // [0]: 0/1 means OFF/ON. [3..1]: ver_H.  [7..4]: ver_L.
                        // Only used by test DK now, not used by helmet yet.
    char cKxyz[3];
    signed short ssThreshhold;
    unsigned short usVerDate;   // LL_VER_DATE
}T_MSG_toHelmet;
extern T_MSG_toHelmet gtMsgToHelmet;

typedef struct
{
    T_LL_HardwareID tHardwareID;    // ID of remote.
    char cBrakeFuncON;              // 1£ºON; 0: OFF.
    signed short ssKxyz[3];
    signed short ssThreshhold;
}T_MSG_toRemote;
void LL_MSG_toRemote_pack(void);



void LL_HelmetActionWhenStateChanged(void);
extern unsigned long gulMsgHandleNeeded;
extern unsigned long    gulRSSI;
extern T_MSG_toHelmet   sgtMsg;
void LL_MsgHandle_Helmet(T_MSG_toHelmet *ptMsg);



void LL_MSG_Handle__PublicMessage_saveWhenInInterrupt(unsigned char *pMessage, unsigned long length);
void LL_MSG_Handle__PublicMessage_parseWhenInMainloop(void);

    

#endif


