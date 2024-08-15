/*
 * gsm_socket.c
 *
 *  Created on: Oct 27, 2019
 *      Author: amin
 */
#ifdef GSM_SOCKET

#include <gsm_socket.h>

#pragma pack(push)
#pragma pack(1)
static struct gsm_socket_st {
	void (*connect_cb)(int8_t _st);
	void (*transmitt_cb)(uint8_t _st);
	void (*rcv_cb)(uint8_t *_buf, uint16_t _len);

	uint8_t flag;
	uint8_t close_req;

	uint8_t *pTxBuf;
	uint16_t pTxLen;
	uint8_t tx_req;
	uint8_t free_req;
} gsm_socket_inf;
#pragma pack(pop)

bool_enu gsm_socket_req(
		void (*_connect_cb)(int8_t _st), void (*_transmit_cb)(uint8_t _st),
		void (*_rcv_cb)(uint8_t *_buf, uint16_t _len)
		)
{
	if (gsm_socket_inf.flag || !gsm_service()) {
		return 0;
	}
	gsm_socket_inf.connect_cb = _connect_cb;
	gsm_socket_inf.rcv_cb = _rcv_cb;
	gsm_socket_inf.transmitt_cb = _transmit_cb;
	gsm_socket_inf.flag = 1;
	gsm_socket_inf.close_req = 0;
	return 1;
}

void gsm_socket_free() {
	gsm_socket_inf.free_req = 1;
}

bool_enu gsm_socket_close() {
	gsm_socket_inf.close_req = 1;
	return 1;
}

void gsm_socket_write(uint8_t *_pBuf, uint16_t _pLen) {
	gsm_socket_inf.pTxBuf = _pBuf;
	gsm_socket_inf.pTxLen = _pLen;
	gsm_socket_inf.tx_req = 1;
}

bool_enu gsm_socket_busy() {
	if (gsm_socket_inf.flag) {
		return 1;
	}
	return 0;
}

void cipshut() {
	gsm_cmd("CIPCLOSE");
	gsm_socket_inf.connect_cb(-1);
}

void task_gsm_socket() {
	static uint8_t st = 0;
	static uint64_t ts, wts;
	static bool_enu  cmd;
	static uint8_t last_st;
	char buffer[10];


	if (gsm_socket_inf.close_req && (gsm_pid() == (uint32_t)task_gsm_socket || !gsm_busy())) {
		gsm_alloc((uint32_t)task_gsm_socket);
		gsm_socket_inf.close_req = 0;
		st = 100;
	}

	if (last_st != st) {
		last_st = st;
		ts = get_timestamp();
		if (st == 100) {
			cipshut();
		}
	}

	if (st == 0) {
		if (gsm_socket_inf.flag && !gsm_busy()) {
			gsm_socket_inf.flag = 0;
			gsm_alloc((uint32_t) task_gsm_socket);
			gsm_ip_rcv(gsm_socket_inf.rcv_cb);
			gsm_cmd("CSTT=\"mcinet\"");
			st = 1;
		}
	} else if (st == 1) {
		if (GSM_RESP_IS_OK()) {
			gsm_cmd("CGATT=1");
			st = 2;
		} else if (delay_s(ts, 1) || GSM_RESP_IS_ERR()) {
			st = 100;
		}
	} else if (st == 2) {
		if (GSM_RESP_IS_OK()) {
			cmd = 0;
			gsm_cmd("CIICR");
			st = 3;
		} else if (delay_s(ts, 15) || GSM_RESP_IS_ERR()) {
			st = 100;
		}
	} else if (st == 3) {
		if (delay_s(wts, 10)) {
			wts = get_timestamp();
			gsm_cmd("CIPSTATUS");
			cmd = 1;
		}
		if (GPRS_IP_STATUS() == GPRS_IP_STATE_IP_GPRSACT) {
			gsm_cmd("CIFSR");
			st = 4;
		} else if (delay_s(ts, 85) || GSM_RESP_IS_ERR() || (cmd && delay_s(wts, 2) &&
				(GPRS_IP_STATUS() == GPRS_IP_STATE_INITIAL ||
						GPRS_IP_STATUS() == GPRS_IP_STATE_PDP_DEACT)))
		{
			gsm_socket_inf.connect_cb(-1);
			st = 100;
		}
	} else if (st == 4) {
		if (delay_s(ts, 1)) {
			gsm_cmd("");
			gsm_free();
			st = 6;
		}
	} else if (st == 6) {
		if (_gsm_status_rdy_) {
			if (GPRS_IP_STATUS() == GPRS_IP_STATE_CONNECT_OK) {
				gsm_socket_inf.connect_cb(1);
				st = 7;
			} else if (GPRS_IP_STATUS() != GPRS_IP_STATE_TCP_CONNECTING) {
				gsm_socket_inf.connect_cb(-1);
				cipshut();
				gsm_free();
				st = 0;
			}
		}
	} else if (st == 7) {
		if (GPRS_IP_STATUS() == GPRS_IP_STATE_CONNECT_OK) {
			if (gsm_socket_inf.tx_req) {
				gsm_socket_inf.tx_req = 0;
				_gsm_resp_[GSM_RESP_TX_ACK] = 0;
				gsm_puts("AT+CIPSEND=");
				sprintf(buffer, "%d\r\n", gsm_socket_inf.pTxLen);
				gsm_puts(buffer);
				st = 8;
			}
		} else {
			if (!gsm_busy() || gsm_pid() == (uint32_t) task_gsm_socket) {
				gsm_socket_inf.connect_cb(-1);
				cipshut();
				gsm_free();
				st = 0;
			}
		}
	} else if (st == 8) {
		if (_gsm_resp_[GSM_RESP_TX_ACK]) {
			_gsm_resp_[GSM_RESP_SEND_OK] = 0;
			for (int i = 0; i < gsm_socket_inf.pTxLen; i++) {
				gsm_putc(gsm_socket_inf.pTxBuf[i]);
			}
			st = 9;
		} else if (delay_s(ts, 15)) {
			gsm_socket_inf.transmitt_cb(0);
			st = 10;
		}
	} else if (st == 9) {
		if (_gsm_resp_[GSM_RESP_SEND_OK]) {
			gsm_socket_inf.transmitt_cb(1);
			st = 7;
		} else if (delay_s(ts, 120) || GSM_RESP_IS_ERR()) {
			gsm_socket_inf.transmitt_cb(0);
			st = 10;
		}
	} else if (st == 100) {
		if (_gsm_resp_[GSM_RESP_OK] || GSM_RESP_IS_ERR() || delay_s(ts, 1)) {
			gsm_cmd("CIPSHUT");
			st = 101;
		}
	} else if (st == 101) {
		if (_gsm_resp_[GSM_RESP_OK] || GSM_RESP_IS_ERR() || delay_s(ts, 1)) {
			gsm_cmd("CGATT=0");
			st = 102;
		}
	} else if (st == 102) {
		if (_gsm_resp_[GSM_RESP_OK] || GSM_RESP_IS_ERR() || delay_s(ts, 1)) {
			gsm_free();
			st = 0;
		}
	}
}
#endif
