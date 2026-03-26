/*******************************************************************************
 Copyright (c) 2016 Lumen Labs (HK) Limited. All Rights Reserved.
*******************************************************************************/

#include "nrf52.h"

#include "LL_HardwareID.h"



T_LL_HardwareID gtHwID;
unsigned long gulHashOfHwID;

void LL_HardwareID_Get(T_LL_HardwareID *ptID, unsigned long *pulHash) {
    ptID->Vender = LL_HARDWARE_VENDER_NORDIC;
    // The data alignment in nRF51 implementation is Little Endian. (see nRF51_RM_v3.0 Page14.)
    ptID->auc[0] = NRF_FICR->DEVICEID[0]      ;
    ptID->auc[1] = NRF_FICR->DEVICEID[0] >>  8;
    ptID->auc[2] = NRF_FICR->DEVICEID[0] >> 16;
    ptID->auc[3] = NRF_FICR->DEVICEID[0] >> 24;
    ptID->auc[4] = NRF_FICR->DEVICEID[1]      ;
    ptID->auc[5] = NRF_FICR->DEVICEID[1] >>  8;
    ptID->auc[6] = NRF_FICR->DEVICEID[1] >> 16;
    ptID->auc[7] = NRF_FICR->DEVICEID[1] >> 24;
    
    // calculate 32-bit hash of HardwareID
    unsigned long hash_val = 0; {
        unsigned long prime = 31; // an often-used prime number
        unsigned char *pucID = (unsigned char *)ptID; for(int i = 0; i < sizeof(T_LL_HardwareID); i++) {
            hash_val = (hash_val * prime + pucID[i]);
        }
    }; *pulHash = hash_val;
}

unsigned long LL_HardwareID_IsEqual(T_LL_HardwareID *ptIDa, T_LL_HardwareID *ptIDb)
{
    // compare .Vender
    if(ptIDa->Vender != ptIDb->Vender) return 0;
    // compare .auc[0~7]
    for(int i = 0; i < LL_HARDWARE_ID_LEN; i++) { if( ptIDa->auc[i] != ptIDb->auc[i] ) return 0; }
    // compare finish
    return 1;
}
