/*
 * gsm_socket.h
 *
 *  Created on: Oct 27, 2019
 *      Author: amin
 */

#ifndef GSM_SOCKET_H_
#define GSM_SOCKET_H_
#include <gsm.h>

#ifdef GSM_SOCKET
void task_gsm_socket();
void gsm_socket_write(uint8_t *_pBuf, uint16_t _pLen) ;
bool_enu gsm_socket_busy();
bool_enu gsm_isconn();
bool_enu gsm_socket_close();
void gsm_socket_free();
//gsm socket
bool_enu gsm_socket_req(
		void (*_connect_cb)(int8_t _st),
		void (*_transmit_cb)(uint8_t _st),
		void (*_rcv_cb)(uint8_t *_buf, uint16_t _len));
#endif

#endif /* GSM_SOCKET_H_ */
