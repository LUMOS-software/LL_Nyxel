/*******************************************************************************
 Copyright (c) 2016 Lumen Labs (HK) Limited. All Rights Reserved.
*******************************************************************************/

//#include <stdio.h>  // sprintf
//#include <string.h>
//#include "PCA10040.h"
//#include "nrf_delay.h"
//#include "app_error.h"

//#include "ble_hci.h"
//#include "ble_advertising.h"

//#include "LL_Clock.h" // get clock for sync
//#include "LL_Timer.h"
//#include "LL_Para.h" // netID
//#include "LL_SysMode.h"
//#include "LL_Para.h"
//#include "LL_Battery.h"
//#include "LL_BatteryIndicator.h"
//#include "LL_Battery_Charge.h"
//#include "LL_MSG_withRemote.h"
//#include "LL_BLE.h" // BLE Company ID
//#include "LL_Sync.h"
//#include "LL_BLE_Broadcaster.h"

#include "LL_Platform.h"
#include "LL_Common.h"
#include "LL_Product.h"



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1 // we need change the BLE name between "Ultra" (for Lumos APP) and "LMM" (for other Lumos devices)
#include "ble_advdata.h"
void LL_Radio__update_device_name(const char *new_name) { // similar to "gap_params_init"
    uint32_t err_code;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)new_name, strlen(new_name));
    APP_ERROR_CHECK(err_code);
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ble_advertising_init_t          init;
static ble_advdata_manuf_data_t advdata_manuf_data;
unsigned char            LL_Radio__spec_data[20];      // 20: max of Lumos BLE Advertisement

void on_adv_evt(ble_adv_evt_t ble_adv_evt);
void LL_Radio__common_stop_init_start(unsigned long size_including_2_bytes_Company_ID, unsigned long adv_interval_ms, unsigned long adv_timeout_s) {
    uint32_t      err_code;

    {
        LL_BLE_Adv_stop();
    }
    { // BLE name
        ble_gap_conn_sec_mode_t sec_mode;
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
        err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)"LMM", strlen("LMM"));
        APP_ERROR_CHECK(err_code);        
    }    
    { // copy from advertising_init()
        memset(&init, 0, sizeof(init)); {
            init.advdata.name_type          = BLE_ADVDATA_FULL_NAME;
            init.advdata.include_appearance = true;
            init.advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
            {
                advdata_manuf_data.company_identifier = LL_BLE_COMPANY_ID_OF_LUMOS; // LUMOS Company ID
                {
                    LL_Radio__spec_data[1] = 0xCC; // Signature
                    LL_Radio__spec_data[2] = gtPara.netID[0];
                    LL_Radio__spec_data[3] = gtPara.netID[1];
                    LL_Radio__spec_data[4] = gtPara.netID[2];
                    LL_Radio__spec_data[5] = gtPara.netID[3];
                    LL_Radio__spec_data[6] = 0x00; // Message Count
                    LL_Radio__spec_data[7] = LL_BLE_Broadcaster__Device_Type;
                }
                advdata_manuf_data.data.size    = size_including_2_bytes_Company_ID - 2; // -2: the 2 bytes data "Company ID"
                advdata_manuf_data.data.p_data  = LL_Radio__spec_data;
            }; init.advdata.p_manuf_specific_data = &advdata_manuf_data;
            {
                init.config.ble_adv_fast_enabled  = true;
                init.config.ble_adv_fast_interval = MSEC_TO_UNITS(adv_interval_ms, UNIT_0_625_MS); // adv_interval_ms: ms;
                init.config.ble_adv_fast_timeout  = adv_timeout_s; // adv_timeout_s: s;
            }
            {
                init.evt_handler = on_adv_evt;                
            }
        }
        {
            LL_BLE_Adv_init(&init);
        }
    }
    {   
        lumos_connectable = false;
        LL_BLE_Adv_start();        
        lumos_connectable = true; 
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LL_BLE_SetAdManu(unsigned short manu);
void advertising_init(void);
extern uint16_t m_conn_handle;
extern void battery_level_update(void);

unsigned long LL_Radio__START_ADV_FOR_APP_PAIRING(void) {
   LL_BLE_Adv_stop(); // stop the previous adv anyway
    if(m_conn_handle == BLE_CONN_HANDLE_INVALID) { // if not connected by APP
//      LL_BLE_Adv_stop();
        LL_Radio__update_device_name(LL_BLE_ADV_NAME);
        advertising_init();
        LL_BLE_SetAdManu(LL_BLE_ADV_FOR_APP_PAIRING);
        #ifdef _LL_DELETE_BAS // if delete Battery Service
            battery_level_update();
        #endif
        lumos_connectable = true; LL_BLE_Adv_start();
    }
    return LL_Thread__step_RETURN__Done;//LL_Thread__step_RETURN__NotDone; // WHY "NotDone" ?
}

unsigned long LL_Radio__START_ADV_FOR_APP_CONNECTING(void) {
    if(m_conn_handle == BLE_CONN_HANDLE_INVALID) { // if not connected by APP
        LL_BLE_Adv_stop();
        LL_Radio__update_device_name(LL_BLE_ADV_NAME);
        advertising_init();
        LL_BLE_SetAdManu(LL_BLE_ADV_FOR_APP_CONNECTING);
        #ifdef _LL_DELETE_BAS // if delete Battery Service
            battery_level_update();
        #endif
        lumos_connectable = true; LL_BLE_Adv_start(); //timeslot_sd_start();
    }
    return LL_Thread__step_RETURN__Done;//LL_Thread__step_RETURN__NotDone; // WHY "NotDone" ?
}


