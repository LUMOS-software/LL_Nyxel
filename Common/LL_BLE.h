/*******************************************************************************
 Copyright (c) 2016 Lumen Labs (HK) Limited. All Rights Reserved.
*******************************************************************************/

#ifndef _LL_BLE_H
#define _LL_BLE_H

#include "ble_advertising.h"

// "Company ID" of "Manufacturer Data" in BLE advertisement
#define LL_BLE_COMPANY_ID_OF_LUMOS  0x0F04 // READ ONLY!
#define LL_BLE_COMPANY_ID_OF_LUMOS__BYTE_L  ((LL_BLE_COMPANY_ID_OF_LUMOS & 0x00FF) >> 0)
#define LL_BLE_COMPANY_ID_OF_LUMOS__BYTE_H  ((LL_BLE_COMPANY_ID_OF_LUMOS & 0xFF00) >> 8)


void LL_BLE_Adv_init(ble_advertising_init_t const * const p_init);
void LL_BLE_Adv_start(void);
void LL_BLE_Adv_stop(void);


#endif
