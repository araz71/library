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

char GsmOperator[20];
uint32_t GsmSimCharge;
#define CTRLZ 0x1A

void gsm_register_sms_handler(void (*handler_cb)(char *number, char *msg));

void gsm_cmd(char *_cmd);
void gsm_resp_clear();
void gsm_putc(char _c);
void gsm_puts(char *_str);

bool_enu gsm_busy();
void gsm_free();
void gsm_alloc(uint32_t _pid);
uint32_t gsm_pid();
void gsm_ip_rcv(void (*_rcv_cb)(uint8_t *_pbuf, uint16_t _len));
void sim800_set_data_mode(uint8_t _data_mode, void (*_callback)(uint8_t *_data, uint16_t _len)) ;
bool_enu gsm_service() ;
void task_gsm();
void gsm_init() ;
uint8_t gsm_get_signal_strength();
uint8_t gsm_service_ready();
bool_enu check_phone(char *_phone);
#endif /* GSM_H_ */
