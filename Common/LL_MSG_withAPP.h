/*******************************************************************************
 Copyright (c) 2016 Lumen Labs (HK) Limited. All Rights Reserved.
*******************************************************************************/

#ifndef _LL_MSG_withAPP_H
#define _LL_MSG_withAPP_H



// Indicate the Normal/Pairing Mode by the first 2 bytes of "BLE ADV Manufacturer Data", which is "Company ID" actually.
#define LL_BLE_ADV_FOR_APP_CONNECTING   0x0000
#define LL_BLE_ADV_FOR_APP_PAIRING      0x0101

void TxCurrentFlashModeToApp(void);

void TxFlashModeParaToApp__Start(void);
void TxFlashModeParaToApp__Stop( void);

void LL_MSG_withAPP__TX(void);
void LL_MSG_withAPP__RX(void);



#endif


