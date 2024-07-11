/*
 * gsm.h
 *
 *  Created on: Oct 27, 2019
 *      Author: m
 */

#ifndef GSM_H_
#define GSM_H_

#include<stdio.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <def.h>
#include <delay.h>

#define UNUSED					__attribute__((unused))

#define CTRLZ 0x1A

typedef enum {
	GSM_RESP_TX_ACK,
	GSM_RESP_CMGS,
	GSM_RESP_OK,
	GSM_RESP_SEND_OK,
	GSM_RESP_SIM_STATE,
	GSM_RESP_ERR,
	GSM_RESP_CALL,
	GSM_RESP_USSD,
	_GSM_RESP_MAX_,
} gsm_resp_enu;

uint8_t _gsm_resp_[_GSM_RESP_MAX_];
uint8_t _gprs_sock_state_;
uint8_t _gsm_status_rdy_;
uint8_t gsm_sim_sel;
uint8_t gsm_no_service_cntr;

#define GSM_RESP_IS_OK()	_gsm_resp_[GSM_RESP_OK]
#define GSM_RESP_IS_ERR()	_gsm_resp_[GSM_RESP_ERR]
#define GPRS_IP_STATUS()	_gprs_sock_state_

typedef enum{
	GSM_CALL_ACTIVE,
	GSM_CALL_HELD,
	GSM_CALL_DIALING,
	GSM_CALL_ALERTING,
	GSM_CALL_INCOMMING,
	GSM_CALL_WAITING,
	GSM_CALL_DISCONNECT,
} gsm_call_status_enu;

typedef enum {
	GPRS_IP_STATE_INITIAL,
	GPRS_IP_STATE_IP_START,
	GPRS_IP_STATE_IP_CONFIG,
	GPRS_IP_STATE_IP_GPRSACT,
	GPRS_IP_STATE_IP_STATUS,
	GPRS_IP_STATE_TCP_CONNECTING,
	GPRS_IP_STATE_CONNECT_OK,
	GPRS_IP_STATE_TCP_CLOSING,
	GPRS_IP_STATE_TCP_CLOSED,
	GPRS_IP_STATE_PDP_DEACT,

	GPRS_IP_STATE_WAIT,
} gsm_socket_state_enu;

typedef enum {
	CPIN_NOT_READY = 1,
	CPIN_NOT_INSERTED,
	CPIN_READY,
	CPIN_SIM_PIN,
} cpin_state_enu;

#ifdef GSM_SERVICE_PROVIDER
char GsmOperator[20];
#endif

#ifdef GSM_CHARGE_CHECK
uint32_t GsmSimCharge;
#endif
uint8_t gsm_failed_sms;
uint8_t gsm_failed_calls;

uint8_t gsm_dtmf_code;

#ifdef GSM_CALL_HANDLE
typedef struct {
#ifdef GSM_CALL_IN
	char phone[20];
#endif
	uint8_t dir;
	gsm_call_status_enu status;
} gsm_call_inf_st;

gsm_call_inf_st gsm_call_information;

void gsm_call(char* phone_number);

// Returns GSM calling status
gsm_call_status_enu gsm_get_call_status();

// Sends ATH to GSM and sets status to disconnect.
void gsm_call_finish();
#endif

void gsm_register_sms_handler(void (*handler_cb)(char *number, char *msg));

void gsm_cmd(char *_cmd);
void gsm_resp_clear();
void gsm_putc(char _c);
void gsm_puts(const char *_str);

uint8_t gsm_get_dtmf();

bool_enu gsm_busy();
void gsm_free();
void gsm_alloc(uint32_t _pid);
uint32_t gsm_pid();
bool_enu gsm_isfree();

void gsm_ip_rcv(void (*_rcv_cb)(uint8_t *_pbuf, uint16_t _len));
void sim800_set_data_mode(uint8_t _data_mode, void (*_callback)(uint8_t *_data, uint16_t _len)) ;

bool_enu gsm_service() ;
uint8_t gsm_service_ready();
uint8_t gsm_get_signal_strength();

void task_gsm();
void gsm_init() ;

uint8_t gsm_set_time(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);

bool_enu check_phone(char *_phone);

uint8_t gsm_get_current_simcard();

uint8_t gsm_get_failed_sms();
uint8_t gsm_get_failed_calls();
#endif /* GSM_H_ */
