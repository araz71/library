/*
 * gsm_charge.c
 *
 *  Created on: Feb 23, 2019
 *      Author: amin
 */


#include <gsm_charge.h>
#include <gsm.h>

#ifdef GSM_CHARGE_CHECK
const char UssdChargeCodeReq[][10] = { "*140*11#", "*141*1#", "*140#" };

void task_gsm_charge_check() {
	static uint64_t ts;
	static uint8_t st = 0, last_st = 0;
	static uint8_t idx = 0;
	static uint32_t delay_time;

	if (last_st != st) {
		last_st = st;
		if (st == 200 || st == 100 || st == 4) {
			gsm_cmd("CSCS=\"HEX\"");
			gsm_free();
		}
		ts = get_timestamp();
	}

	if (st == 0) {
		if (!gsm_busy() && gsm_service()) {
			gsm_alloc(0xAABBCC);
			gsm_cmd("CSCS=\"GSM\"");
			idx = 0xFF;
			st = 1;
		}

	} else if (st == 1) {
		if (delay_ms(ts, 500)) {
			if (strncmp(GsmOperator, "IR-MCI", 5) == 0) {
				idx = 0;
			} else if (strncmp(GsmOperator, "Irancell", 8) == 0) {
				idx = 1;
			} else if (strncmp(GsmOperator, "RighTel", 7) == 0) {
				idx = 2;
			}
			if (idx != 0xFF) {
				_gsm_resp_[GSM_RESP_OK] = 0;
				gsm_puts("AT+CUSD=1,\"");
				gsm_puts((char *)UssdChargeCodeReq[idx]);
				gsm_puts("\"\r\n");
				st = 2;
			} else {
				delay_time = 30;
				st = 4;
			}
		}
	} else if (st == 2) {
		if (_gsm_resp_[GSM_RESP_OK]) {
			_gsm_resp_[GSM_RESP_USSD] = 0;
			st = 3;
		} else if (_gsm_resp_[GSM_RESP_ERR] || delay_s(ts, 2)) {
			delay_time = 21600;
			st = 4;
		}
	} else if (st == 3) {
		if (_gsm_resp_[GSM_RESP_USSD] == 1) {
			delay_time = 86400;
			st = 4;
		}
		if (delay_s(ts, 20) || _gsm_resp_[GSM_RESP_USSD] == 2) {
			delay_time = 21600;
			st = 4;
		}
	} else if (st == 4) {
		if (delay_s(ts, delay_time)) {		//every 12h
			st = 0;
		}
	}
}
#endif
