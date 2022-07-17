/*
 * gsm_sms_transmitter.c
 *
 *  Created on: Oct 29, 2019
 *      Author: amin
 */

#include <gsm_sms_transmitter.h>

static char *msg = NULL;
static char *phone_number;
static uint8_t dil;
static sms_succ_enu sms_succ;

void sms_req(char *_phonenumber, char *_msg, bool_enu _en) {
	if (check_phone(_phonenumber)) {
		msg = _msg;
		dil = _en;
		phone_number = _phonenumber;
		sms_succ = SMS_SUCC_BUSY;
	} else {
		sms_succ = SMS_SUCC_ERR_PHONE;
	}
}

uint8_t sms_is_transmitted() {
	return sms_succ;
}

const char _dec2hex_[] = {
		'0',
		'1',
		'2',
		'3',
		'4',
		'5',
		'6',
		'7',
		'8',
		'9',
		'A',
		'B',
		'C',
		'D',
		'E',
		'F',
};

void task_sms_tx() {
	static uint8_t st = 0;
	static uint64_t ts, wts;
	uint size = 0;
	if (st == 0) {
		if (msg != NULL && !gsm_busy() && gsm_service()) {
			gsm_alloc((uint32_t)task_sms_tx);
			_gsm_resp_[GSM_RESP_TX_ACK] = 0;
			gsm_puts("AT+CMGS=\"");
			gsm_puts(phone_number);
			gsm_puts("\"\r\n");
			ts = get_timestamp();
			st = 1;
		}
	} else if (st == 1) {
		if (delay_ms(ts, 10000) || _gsm_resp_[GSM_RESP_TX_ACK]) {
			if (dil) {
				size = strlen(msg);
				for (int i = 0; i < size; i++) {
					if (msg[i] == '~') {
						i++;
						while (msg[i] != '~' && i < size) {
							gsm_putc(msg[i]);
							i++;
						}
						i++;
					} else {
						gsm_putc('0');
						gsm_putc('0');
						gsm_putc(_dec2hex_[(msg[i] >> 4) & 0x0F]);
						gsm_putc(_dec2hex_[msg[i] & 0x0F]);
					}
				}
			} else {
				gsm_puts(msg);
			}
			gsm_putc(CTRLZ);
			_gsm_resp_[GSM_RESP_OK] = 0;
			wts = get_timestamp();
			ts = get_timestamp();
			st = 2;
		}
	} else if (st == 2) {
		if (GSM_RESP_IS_OK() || delay_s(wts, 240) || GSM_RESP_IS_ERR()) {
			if (GSM_RESP_IS_OK()) sms_succ = SMS_SUCC_OK;
			else sms_succ = SMS_SUCC_FAILED;
			msg = NULL;
			gsm_free();
			st = 0;
		}
	}
}
