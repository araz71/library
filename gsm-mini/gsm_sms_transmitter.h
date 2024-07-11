/*
 * gsm_sms_transmitter.h
 *
 *  Created on: Oct 29, 2019
 *      Author: amin
 */

#ifndef GSM_SMS_TRANSMITTER_H_
#define GSM_SMS_TRANSMITTER_H_

#include <gsm.h>

typedef enum {
	SMS_SUCC_FAILED,
	SMS_SUCC_OK,
	SMS_SUCC_ERR_PHONE,
	SMS_SUCC_BUSY,
	SMS_SUCC_REQ_OK,
} sms_succ_enu;

void sms_req(char *_phonenumber, char *_msg, bool_enu _en);
void task_sms_tx();
sms_succ_enu sms_is_transmitted();

#endif /* GSM_SMS_TRANSMITTER_H_ */
