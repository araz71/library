#include <gsm.h>

static uint8_t _sim800_rx_buffer_[384];
#ifdef GSM_SOCEKT
static uint8_t _ipd_buffer_[128];
#endif
static uint16_t _sim800_rx_cntr_ = 0;
static uint64_t _sim800_rx_ts_;
static uint8_t _sim800_data_mode_ = 0;
static uint8_t _sim800_init_try_ = 0;
static void (*_sim800_data_mode_callback_)(uint8_t *_data, uint16_t _len) = NULL;
static uint8_t _gsm_signal_ = 0;
static uint8_t _gsm_reg_ = 0;
static uint8_t _gsm_sim_ = 0;
static char *_gsm_token_;
void (*gsm_ip_rcv_cb)(uint8_t *_pbuf, uint16_t _len);

extern void usart_putc(USART_TypeDef *_usart, uint8_t _c);

static void (*gsm_sms_handle)(char *number, char *msg) = NULL;

uint8_t gsm_get_failed_sms() {
	return gsm_failed_sms;
}

uint8_t gsm_get_failed_calls() {
	return gsm_failed_calls;
}

void gsm_register_sms_handler(void (*handler_cb)(char *number, char *msg)) {
	gsm_sms_handle = handler_cb;
}

uint8_t gsm_get_current_simcard() {
	return gsm_sim_sel;
}

__attribute__((weak)) bool_enu check_phone(char *_phone) {
	bool_enu phone_valid = TRUE;
	int i = 0;
	for (i = 0; i < 13; i++) {
		if (_phone[i] == '\0') break;
		if (!(_phone[i] >= '0' && _phone[i] <= '9')) {
			phone_valid = FALSE;
		}
	}
	if (i < 5) phone_valid = FALSE;
	return phone_valid;
}

static uint8_t _hex2dec_(char _h) {
	if (_h >= '0' && _h <= '9') {
		return (_h - '0');
	} else if (_h == 'A' || _h == 'a') {
		return 10;
	} else if (_h == 'B' || _h == 'b') {
		return 11;
	} else if (_h == 'C' || _h == 'c') {
		return 12;
	} else if (_h == 'D' || _h == 'd') {
		return 13;
	} else if (_h == 'E' || _h == 'e') {
		return 14;
	} else if (_h == 'F' || _h == 'f') {
		return 15;
	}

	return 0;
}

#ifdef GSM_DEBUG
extern void tuya_putc(char c);
#endif

void gsm_putc(char _c) {
	usart_putc(GSM_USART, _c);
#ifdef GSM_DEBUG
	tuya_putc(_c);
#endif
}
void gsm_resp_clear() {
	_gprs_sock_state_ = GPRS_IP_STATE_INITIAL;
}
void gsm_puts(const char *_str) {
	while (1) {
		if ((*_str) == '\0') {
			return;
		}
		gsm_putc(*_str++);
	}
}
static void _sim800_power_key_(bool_enu _set) {
	if (!_set) {
		GSM_PWR_ON();
	} else {
		GSM_PWR_OFF();
	}
}

#pragma pack(push)
#pragma pack(1)
typedef struct {
	char res[20];
	uint8_t resp_code;
	void (*callback)(uint8_t _resp);
} gsm_resp_st;
#pragma pack(pop)

static void gsm_resp_cb_cpin(uint8_t _resp) {
	_gsm_sim_ = _resp;
}
static void gsm_resp_cb_ok(uint8_t _resp) {
	_gsm_resp_[GSM_RESP_OK] = _resp;
}
static void gsm_resp_cb_err(UNUSED uint8_t _err) {
	_gsm_resp_[GSM_RESP_ERR] = 1;
}
static void gsm_resp_cb_signal(UNUSED uint8_t _p) {
	int signal;
	if (sscanf(_gsm_token_, "+CSQ: %d", &signal) == 1) {
		_gsm_signal_ = (uint8_t) signal;
	} else {
		_gsm_signal_ = 0;
	}
}
static void gsm_resp_cb_reg(UNUSED uint8_t _p) {
	int reg = 0;
	int n = 0;
	if (sscanf(_gsm_token_, "+CREG: %d,%d", &n, &reg) == 2) {
		if (n != 1 && n != 5) {
			_gsm_reg_ = 0;
		} else {
			_gsm_reg_ = 1;
		}
	}

	_gsm_resp_[GSM_RESP_OK] = 1;
}

#ifdef GSM_SOCEKT
static void gsm_resp_cb_socket(uint8_t _st) {
	char *ptr2state = NULL;
	const char ip_status[][16] = {
			"IP INITIAL",
			"IP START",
			"IP CONFIG",
			"IP GPRSACT",
			"IP STATUS",
			"TCP CONNECTING",
			"CONNECT OK",
			"TCP CLOSING",
			"TCP CLOSED",
			"PDP DEACT"
	};
	for (int i = 0; i < strnlen(_gsm_token_, 100); i++) {
		if (_gsm_token_[i] == ' ') {
			ptr2state = &_gsm_token_[i + 1];
			break;
		}
	}
	if (ptr2state == NULL) {
		return;
	}
	for (int i = 0; i < 10; i++) {
		if (strcmp(ptr2state, ip_status[i]) == 0) {
			_gprs_sock_state_ = i;
			break;
		}
	}
	_gsm_resp_[GSM_RESP_OK] = 1;
}
#endif

static void gsm_resp_cb_wr(UNUSED uint8_t _c) {
	_gsm_resp_[GSM_RESP_TX_ACK] = 1;
}
static void gsm_resp_cb_sms_tx(UNUSED uint8_t _c) {
	_gsm_resp_[GSM_RESP_CMGS] = 1;
}

#ifdef GSM_CALL_HANDLE

gsm_call_status_enu gsm_get_call_status() {
	return gsm_call_information.status;
}

void gsm_call_finish() {
	gsm_call_information.status = GSM_CALL_DISCONNECT;
	gsm_puts("ATH\r\n");
}

void gsm_call(char* phone_number) {
	gsm_call_information.status = GSM_CALL_DIALING;
	gsm_puts("ATD");
	gsm_puts(phone_number);
	gsm_puts(";\r\n");
}

static void gsm_cb_call(UNUSED uint8_t _p) {
	//+CLCC: 1,1,4,0,0,"+989129174769",145,""
	int idx, dir, stat, mode, mpty;

	if (sscanf(_gsm_token_, "+CLCC: %d,%d,%d,%d,%d", &idx, &dir, &stat, &mode, &mpty) == 5) {
		mode = 0;

		for (; _gsm_token_[mode] != '\0'; mode++) {
			if (_gsm_token_[mode] == '\"') {
				break;
			}
		}

		mode++;	//reject "
		mode++;	//reject +
		mode++;	//reject 9
		mode++; //reject 8

		gsm_call_information.dir = dir;
		gsm_call_information.status = stat;
	}
}
#endif

#ifdef GSM_SOCKET
static void gsm_resp_cb_data_send(uint8_t _p) {
	_gsm_resp_[GSM_RESP_SEND_OK] = 1;
}
static void gsm_resp_cb_ipd(uint8_t _p) {
	int len;
	int i;
	for (i = 0; i < 120; i++) {
		if (strncmp((char *)&_ipd_buffer_[i], "+IPD", 4) == 0) {
			memcpy(_ipd_buffer_, &_ipd_buffer_[i], 128 - i);
			break;
		}
	}
	for (i = 0; i < 120; i++) {
		if (_ipd_buffer_[i] == ',') {
			break;
		}
	}
	if (_ipd_buffer_[i] != ',') return;
	i++;
	len = 0;
	for (; i < 120; i++) {
		if (_ipd_buffer_[i] == ':') break;
		len = (len * 10) + (_ipd_buffer_[i] - '0');
	}
	if (_ipd_buffer_[i] != ':') return;
	if (len >= 40) {
		return;
	}
	for (i = 0; i < 128; i++) {
		if (_ipd_buffer_[i] ==':') break;
	}
	i++;
	gsm_ip_rcv_cb(&_ipd_buffer_[i], len);
}
#endif

static void gsm_resp_cb_nsms(UNUSED uint8_t _p) {
	char phone[20];
	uint32_t i = 0;
	int l = 0;
	char p1, p2;

	if (gsm_sms_handle == NULL) {
		return;
	}

	GSM_RESP_IS_OK() = TRUE;
	for (i = 0; _gsm_token_[i] != '\"'; i++) {
		if (_gsm_token_[i] == '\0') {
			return;
		}
	}
	i++;
	_gsm_token_ += i;
	if (_gsm_token_[0] != 'R' || _gsm_token_[4] != 'U') {
		return;
	}

	//get number
	for (i = 0; _gsm_token_[i] != ','; i++) {
		if (_gsm_token_[i] == '\0') {
			return;
		}
	}
	i++;		//"
	i += 7;		//+98
	_gsm_token_ += i;
	memset(phone, 0, 20);
	phone[0] = '0';
	for (int l = 0; l < 20; l++) {
		phone[l + 1] = _gsm_token_[(2 * l) + 1];
		if (phone[l + 1] == ',') {
			phone[l + 1] = '\0';
			break;
		}
	}

	_gsm_token_ = strtok(NULL, "\r\n");

	if (_gsm_token_ == NULL) {
		return;
	}
	uint8_t rcv_b = 0;
	for (i = 0; i < strlen(_gsm_token_); i += 2) {
		p1 = _hex2dec_(_gsm_token_[i]);
		p2 = _hex2dec_(_gsm_token_[i + 1]);
		rcv_b = (p1 << 4) | p2;
		if (rcv_b == 0) {	// DAMN ON IRANCELL
			continue;
		}
		_gsm_token_[l] = (p1 << 4) | p2;
		l++;
	}
	_gsm_token_[l] = 0;

	gsm_sms_handle(phone, _gsm_token_);
}

#ifdef GSM_USSD_CHARGE
static void gsm_cb_cusd_resp(uint8_t _p) {
	const char rial_utf8[] = "063106CC06270644";
	const char space_utf8[] = "0020";
	int i = 0;
	// "Your balance is 50128 Rial .Your account will expire after 2020/11/1

	if (gsm_pid() != 0xAABBCC) {
		return;
	}
	_gsm_resp_[GSM_RESP_USSD] = 2;
	GsmSimCharge = 0;

	for (i = 0; i < strlen(_gsm_token_); i++) {
		if ((i + 4) < strlen(_gsm_token_) && (strncmp(_gsm_token_ + i, "Rial", 4) == 0 || strncmp(_gsm_token_ + i, "IRR", 3) == 0)) {
			i--;
			i--;
			for (; _gsm_token_[i] != ' '; i--) {
				if (i <= 1) {
					return;
				}
			}
			i++;
			for (; _gsm_token_[i] != ' '; i++) {
				GsmSimCharge = (GsmSimCharge * 10) + (_gsm_token_[i] - '0');
				if (i >= strlen(_gsm_token_)) {
					GsmSimCharge = 0;
					return;
				}
			}

			_gsm_resp_[GSM_RESP_USSD] = TRUE;
			break;
		} else if ((i + strlen(rial_utf8)) < strlen(_gsm_token_) && strncmp(_gsm_token_ + i, rial_utf8, strlen(rial_utf8)) == 0) {
			i -= 4;
			i -= 4;
			for (; i > 0; i -= 4) {
				if (i < 0) {
					return;
				}
				if (strncmp(_gsm_token_ + i, space_utf8, 4) == 0
						|| strncmp(_gsm_token_ + i, "003A", 4) == 0)
				{
					i += 4;
					char temp[5];
					temp[4] = '\0';
					int d_cntr = 0;
					for (; i < strlen(_gsm_token_); i += 4) {
						temp[0] = _gsm_token_[i];
						temp[1] = _gsm_token_[i + 1];
						temp[2] = _gsm_token_[i + 2];
						temp[3] = _gsm_token_[i + 3];

						d_cntr++;
						if (strcmp(temp, "0020") == 0
								|| strcmp(temp, "0631") == 0
								|| d_cntr >= 15
								|| (!(temp[3] >= '0' && temp[3] <= '9'))
								|| temp[2] != '3'
								|| temp[0] != '0'
								|| temp[1] != '0')
						{
							_gsm_resp_[GSM_RESP_USSD] = TRUE;
							return;
						} else {
							uint8_t num;
							num = (temp[2] - '0') << 4;
							num |= (temp[3] - '0');
							num -= '0';
							GsmSimCharge = (GsmSimCharge * 10) + num;
						}
					}
				}
			}
		}
	}
}
#endif

#ifdef GSM_SERVICE_PROVIDER
static void gsm_cb_service_provider(UNUSED uint8_t _p) {
	uint32_t i = 0;

	for (; i < strlen(_gsm_token_); i++) {
		if (_gsm_token_[i] == '\"')
			break;
	}
	i++;

	int l = 0;
	for (; _gsm_token_[i] != '\"'; i++) {
		if (i >= strlen(_gsm_token_) || l >= 20) {
			memset(GsmOperator, 0, 19);
			return;
		}
		GsmOperator[l++] = _gsm_token_[i];
	}
	GsmOperator[l] = '\0';
}
#endif

#ifdef GSM_TIME
extern void rtc_set(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);
static void gsm_resp_cclk(UNUSED uint8_t _p) {
	int y, m, d, h, M, s;
	if (sscanf(_gsm_token_, "+CCLK: \"%d/%d/%d,%d:%d:%d", &y, &m, &d, &h, &M, &s) == 6) {
		rtc_set(y, m, d, h, M, s);
	}
}
uint8_t gsm_set_time(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
	if (!gsm_busy()) {
		char buffer[48];
		sprintf(buffer, "AT+CCLK=\"%02d/%02d/%02d,%02d:%02d:%02d-32\"\r\n",
				year, month, day, hour, min, sec);
		gsm_puts(buffer);
		return 1;
	}
	return 0;
}
#endif

void gsm_cb_dtmf(UNUSED uint8_t _p) {
	if (sscanf(_gsm_token_, "+DTMF: %c", &gsm_dtmf_code) == 0) {
		gsm_dtmf_code = 0xFF;
	}
}

uint8_t gsm_get_dtmf() {
	uint8_t temp = 0xFF;
	if (gsm_dtmf_code != 0xFF) {
		temp = gsm_dtmf_code;
		gsm_dtmf_code = 0xFF;
	}

	return temp;
}

static const gsm_resp_st _gsm_resp_str[] = {

	{ "OK", 				1, 							&gsm_resp_cb_ok 		},
	{"SHUT OK",				1,							&gsm_resp_cb_ok			},
	{ "> ",					0,							&gsm_resp_cb_wr 		},

#ifdef GSM_USSD_HANDLE
	{"+CUSD:",				0,							&gsm_cb_cusd_resp		},
#endif

	{ "ERROR", 				0, 							&gsm_resp_cb_err 		},

#ifdef GSM_CALL_HANDLE
	{"+CLCC:",				0,							&gsm_cb_call			},
#endif

#ifdef GSM_SERVICE_PROVIDER
	{"+CSPN",				0,							&gsm_cb_service_provider},
#endif

	{ "+CSQ:", 				0, 							&gsm_resp_cb_signal		},
	{ "+CREG:", 			0, 							&gsm_resp_cb_reg 		},
	{ "+CMGR:",				0,							&gsm_resp_cb_nsms		},
	{ "+CMGS:",				0,							&gsm_resp_cb_sms_tx 	},
	{ "+CPIN: READY", 		CPIN_READY, 				&gsm_resp_cb_cpin 		},
	{ "+CPIN: NOT READY", 	CPIN_NOT_READY, 			&gsm_resp_cb_cpin 		},
	{ "+CPIN: NOT INSERTED",CPIN_NOT_INSERTED, 			&gsm_resp_cb_cpin 		},

	{ "+DTMF"				,0,							&gsm_cb_dtmf			},

#ifdef GSM_TIME
	{ "+CCLK",				0,							&gsm_resp_cclk			},
#endif

#ifdef GSM_SOCKET
	{"+IPD,",				0,							&gsm_resp_cb_ipd		},
	{ "STATE:",				0,							&gsm_resp_cb_socket		},
	{ "SEND OK",			0,							&gsm_resp_cb_data_send  },
#endif

	{ "\0",					0,							NULL					},
};

static const char _sim800_init_commands_[][20] = {
		"CMGF=1",
		"CMGDA=\"DEL ALL\"",
		"CNMI=2,1,0,0",
		"CSMP=17,167,0,16",
		"DDET=1,1,1,1",
#ifdef GSM_SOCKET
		"CIPSHUT",
		"CIPMODE=0",
		"CIPHEAD=1",
		"CIPMUX=0",
#endif

#ifdef GSM_CALL_HANDLE
		"CLCC=1",
#endif

#ifdef GSM_USSD_HANDLE
		"CUSD=1",
#endif

		"CMIC=0,15",
		"\0",
};

static void _sim800_cmd_(char *_cmd) {
	_gsm_resp_[GSM_RESP_OK] = FALSE;
	_gsm_resp_[GSM_RESP_ERR] = FALSE;
	gsm_puts(_cmd);
	gsm_puts("\r\n");
}

void gsm_cmd(char *_cmd) {
	_gsm_resp_[GSM_RESP_OK] = 0;
	_gsm_resp_[GSM_RESP_ERR] = 0;
	gsm_puts("AT+");
	gsm_puts(_cmd);
	gsm_puts("\r\n");
}
void _sim800_ch_(char _c) {
	gsm_putc(_c);
}

static uint32_t _pid_;

void gsm_ip_rcv(void (*_rcv_cb)(uint8_t *_pbuf, uint16_t _len)) {
	gsm_ip_rcv_cb = _rcv_cb;
}

void gsm_alloc(uint32_t _pid) {
	_pid_ = _pid;
}

void gsm_free() {
	_pid_ = 0;
}

bool_enu gsm_busy() {
	if (_pid_) return TRUE;
	return FALSE;
}

bool_enu gsm_isfree() {
	return !gsm_busy();
}

uint32_t gsm_pid() {
	return _pid_;
}

void _task_sim800_rcv_hdl_() {
	if (_sim800_rx_cntr_) {
		if (delay_ms(_sim800_rx_ts_, 50)) {
#ifdef GSM_SOCEKT
			memcpy(_ipd_buffer_, _sim800_rx_buffer_, 128);
#endif
			if (_sim800_data_mode_) {
				if (_sim800_data_mode_callback_ != NULL) {
					_sim800_data_mode_callback_(_sim800_rx_buffer_, _sim800_rx_cntr_);
					_sim800_rx_cntr_ = 0;
				}
			} else {
				_gsm_token_ = strtok((char *) _sim800_rx_buffer_, "\r\n");
				while (_gsm_token_ != NULL) {
					for (uint8_t resp_cntr = 0; resp_cntr < 100; resp_cntr++) {
						if (_gsm_resp_str[resp_cntr].res[0] == '\0') {
							break;
						} else if (strncmp(_gsm_resp_str[resp_cntr].res, _gsm_token_, strlen(_gsm_resp_str[resp_cntr].res)) == 0) {
							if (_gsm_resp_str[resp_cntr].callback != NULL) {
								_gsm_resp_str[resp_cntr].callback(_gsm_resp_str[resp_cntr].resp_code);
							}
							break;
						}
					}
					_gsm_token_ = strtok(NULL, "\r\n");
				}
				_sim800_rx_cntr_ = 0;
				memset(_sim800_rx_buffer_, 0, 256);
			}
		}
	}
}

typedef enum {
	gsm_init_power_on,
	gsm_init_power_down,
	gsm_init_loop_back_off,
	gsm_init_sim_off,
	gsm_init_wait_to_sim_off,
	gsm_init_sim_on,
	gsm_init_wait_to_sim_on,
	gsm_init_wait_to_startup,
	gsm_init_run,
	gsm_init_run_ack,
	gsm_init_wait_to_bring_up,
	gsm_init_idle,
	gsm_no_serv_timer,
	gsm_init_err,
	gsm_gps_init,
	gsm_gps_init_ack,
} gsm_init_enu;

static void gsm_check_for_sim_sel() {
#ifdef GSM_2_SIMCARD_SUPPORTED
	gsm_no_service_cntr++;
	if (gsm_no_service_cntr >= 3) {
		gsm_failed_sms = 0;
		gsm_failed_calls = 0;
		gsm_no_service_cntr = 0;
		if (gsm_sim_sel == 0) {
			mlog("gsm switched to sim-2.");
			gsm_sim_sel = 1;
			SIM_2_SEL();
		} else {
			mlog("gsm switched to sim-1.");
			gsm_sim_sel = 0;
			SIM_1_SEL();
		}
	}
#endif
}

static void _task_sim800_init_() {
	static uint8_t st = gsm_init_power_on, last_st = 0xFF;
	static uint64_t ts;
	static uint8_t cmd_cntr = 0;
	static uint8_t try_cmd = 0;

	if (last_st != st) {
		last_st = st;
		ts = get_timestamp();
	}

	if (st == gsm_init_power_on) {
		_sim800_power_key_(TRUE);
		st = gsm_init_power_down;
		gsm_alloc(0xFFFFFFFF);

	} else if (st == gsm_init_power_down) {
		if (delay_s(ts, 2)) {
			_sim800_power_key_(FALSE);
			sim800_set_data_mode(FALSE, NULL);
			st = gsm_init_loop_back_off;
		}

	} else if (st == gsm_init_loop_back_off) {
		if (delay_s(ts, 2)) {
			_sim800_cmd_("ATE0");
			st = gsm_init_sim_off;
		}

	} else if (st == gsm_init_sim_off) {
		if (_gsm_resp_[GSM_RESP_OK]) {
			_sim800_init_try_ = 0;
			gsm_cmd("CFUN=0");
			st = gsm_init_wait_to_sim_off;
		} else if (delay_ms(ts, 500)) {
			st = gsm_init_power_on;
		}

	} else if (st == gsm_init_wait_to_sim_off) {
		if (_gsm_resp_[GSM_RESP_OK]) {
			_sim800_init_try_ = 0;
			st = gsm_init_sim_on;
		} else if (delay_s(ts, 15) || _gsm_resp_[GSM_RESP_ERR]) {
			st  = gsm_init_power_on;
		}

	} else if (st == gsm_init_sim_on) {
		if (delay_s(ts, 2)) {
			gsm_cmd("CFUN=1");
			st = gsm_init_wait_to_sim_on;
		}

	} else if (st == gsm_init_wait_to_sim_on) {
		if (_gsm_resp_[GSM_RESP_OK]) {
			_sim800_init_try_ = 0;
			st = gsm_init_wait_to_startup;

		} else if (delay_s(ts, 15) || _gsm_resp_[GSM_RESP_ERR]) {
			st = gsm_init_power_on;

		} else if (_gsm_sim_ == CPIN_NOT_INSERTED) {
#ifdef GSM_2_SIMCARD_SUPPORTED
			if (gsm_sim_sel == 0) {
				gsm_sim_sel = 1;
				SIM_2_SEL();
				mlog("gsm swithced to sim-2.");
			} else {
				gsm_sim_sel = 0;
				SIM_1_SEL();
				mlog("gsm swithced to sim-1.");
			}
#endif
			st = gsm_init_power_on;
		}

	} else if (st == gsm_init_wait_to_startup) {
		if (delay_s(ts, 10)) {
			cmd_cntr = 0;
			st = gsm_init_run;
		}

	} else if (st == gsm_init_run) {
		if (_sim800_init_commands_[cmd_cntr][0] != '\0') {
			gsm_cmd((char *) _sim800_init_commands_[cmd_cntr]);
			st = gsm_init_run_ack;
		} else {
			st = gsm_init_wait_to_bring_up;
		}

	} else if (st == gsm_init_run_ack) {
		if (_gsm_resp_[GSM_RESP_OK]) {
			try_cmd = 0;
			cmd_cntr++;
			st = gsm_init_run;
		} else if (delay_ms(ts, 500) || _gsm_resp_[GSM_RESP_ERR]) {
			try_cmd++;
			if (try_cmd >= 3) {
				try_cmd = 0;
				gsm_check_for_sim_sel();
				st = gsm_init_power_on;
			} else {
				st = gsm_init_run;
			}
		}

	} else if (st == gsm_init_wait_to_bring_up) {
		gsm_free();
		st = gsm_init_idle;
	}

	if (st == gsm_init_idle) {
		if (gsm_failed_sms > 5 ||
				(!gsm_service() && delay_s(ts, 120) && !gsm_busy()))
		{
			gsm_alloc((uint32_t)_task_sim800_init_);
			gsm_check_for_sim_sel();
			st = gsm_init_power_on;
		}
	}
}

void GSM_USART_ISR() {
	uint8_t c = GSM_USART->RDR;
	GSM_USART->ISR;
	GSM_USART->ICR = 0xFFFF;
	_sim800_rx_ts_ = get_timestamp();
	_sim800_rx_buffer_[_sim800_rx_cntr_++] = c;
	_sim800_rx_buffer_[_sim800_rx_cntr_] = '\0';
	if (_sim800_rx_cntr_ >= 380) {
		_sim800_rx_cntr_ = 0;
	}
#ifdef GSM_DEBUG
	tuya_putc(c);
#endif
}

void sim800_set_data_mode(uint8_t _data_mode, void (*_callback)(uint8_t *_data, uint16_t _len)) {
	_sim800_rx_cntr_ = 0;
	_sim800_rx_buffer_[_sim800_rx_cntr_] = '\0';
	_sim800_data_mode_ = _data_mode;
	_sim800_data_mode_callback_ = _callback;
}
void gsm_init() {
	_gsm_sim_ = 0;
	_gsm_signal_ = 0;
	_gsm_reg_ = 0;
	gsm_alloc((uint32_t)_task_sim800_init_);
	_sim800_init_try_ = 0;
	_sim800_data_mode_ = FALSE;
	_sim800_data_mode_callback_ = NULL;
	gsm_sim_sel = 0;
	gsm_no_service_cntr = 0;
	gsm_failed_sms = 0;
	gsm_failed_calls = 0;
}

uint8_t gsm_service_ready() {
	return _gsm_status_rdy_;
}

bool_enu gsm_service() {
	if (_gsm_reg_ == 0
			|| _gsm_signal_ < 12
			|| _gsm_sim_ != CPIN_READY
			|| _gsm_reg_ != 1)
	{
		return FALSE;
	}
	return TRUE;
}

const char _v_gsm_serv_cmds_[][16] = {
		"CREG=1",
		"CSQ",
		"CREG?",
		"CPIN?",

#ifdef GSM_SERVICE_PROVIDER
		"CSPN?",
#endif

#ifdef GSM_TIME
		"CCLK?",
#endif

#ifdef GSM_SOCKET
		"CIPSTATUS",
#endif

		"CSCS=\"HEX\"",
		"CSMP=49,167,0,8",
		"CMGR=1",
		"CMGD=1",
		"CMGR=2",
		"CMGD=2",
		"CMGR=3",
		"CMGD=3",

#ifdef GSM_CALL_HANDLE
		"CLCC",
#endif
		"-",
};

static void _task_gsm_service_() {
	static uint8_t st = 0;
	static uint8_t cmd_cntr = 0;
	static uint64_t ts;

	if (st == 0) {
		if (!gsm_busy()) {
			_gsm_reg_ = 0;
			_gsm_signal_ = 0;
			_gsm_sim_ = 0;
			cmd_cntr = 0;
			_gsm_status_rdy_ = FALSE;
			gsm_alloc((uint32_t)_task_gsm_service_);
			st = 1;
		}
	} else if (st == 1) {
		if (_v_gsm_serv_cmds_[cmd_cntr][0] != '-') {
			if (delay_ms(ts, 500) || GSM_RESP_IS_OK() || GSM_RESP_IS_ERR()) {
				gsm_cmd((char *)_v_gsm_serv_cmds_[cmd_cntr++]);
				ts = get_timestamp();
			}
		} else {
			st = 2;
		}
	} else if (st == 2) {
		_gsm_status_rdy_ = TRUE;
		gsm_free();
		st = 3;
	} else {
		if (delay_s(ts, 5)) {
			st = 0;
		}
	}
}

uint8_t gsm_get_signal_strength() {
	return _gsm_signal_;
}

void task_gsm() {
	_task_sim800_init_();
	_task_sim800_rcv_hdl_();
	_task_gsm_service_();
}
