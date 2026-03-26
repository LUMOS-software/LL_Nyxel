/*******************************************************************************
 Copyright (c) 2016 Lumen Labs (HK) Limited. All Rights Reserved.
*******************************************************************************/

#ifndef _LL_Radio_H
#define _LL_Radio_H



void LL_Radio__update_device_name(const char *new_name);

extern unsigned char LL_Radio__spec_data[20];
void LL_Radio__common_stop_init_start(unsigned long size_including_2_bytes_Company_ID, unsigned long adv_interval_ms, unsigned long adv_timeout_s);

unsigned long LL_Radio__START_ADV_FOR_APP_PAIRING(void);
unsigned long LL_Radio__START_ADV_FOR_APP_CONNECTING(void);



#endif
