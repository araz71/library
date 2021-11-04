/*
 * mqtt.h
 *
 *  Created on: Nov 27, 2019
 *      Author: amin
 */

#ifndef MQTT_H_
#define MQTT_H_

#include <def.h>

typedef enum {
    MQTT_FLAGS_WILL = 0x4,
    MQTT_FLAGS_QoS_AT_MOST_ONCE = 0,
    MQTT_FLAGS_QoS_AT_LEAST_ONCE = 0x8,
    MQTT_FLAGS_QoS_EXACTLY_ONCE = 0x10,
    MQTT_FLAGS_RETAIN = 0x20,
    MQTT_FLAGS_PASSWORD = 0x40,
    MQTT_FLAGS_USERNAME = 0x80,
} mqtt_flags_enu;
typedef enum {
    MQTT_CONACK_ACCEPTED,
    MQTT_CONACK_REFUSED_VERSION,
    MQTT_CONACK_REFUSED_ID,
    MQTT_CONACK_REFUSED_SERVER,
    MQTT_CONACK_REFUSED_USERNAME_PASSWORD,
    MQTT_CONACK_REFUSED_NOT_AUTHORIZED,
} mqtt_conack_enu;

typedef enum {
	MQTT_PUB_TYP_REL,
	MQTT_PUB_TYP_ACK,
	MQTT_PUB_TYP_REC,
	MQTT_PUB_TYP_CMP,
} mqtt_pub_typ_enu;

uint16_t mqtt_connect(char *_id, char *_username, char *_password, uint16_t _flags, uint16_t _keepalive, void (*_conn_cb)(mqtt_conack_enu _resp), uint8_t *_ret) ;
void mqtt_resp_hdl(uint8_t *_resp, uint16_t _len) ;
uint16_t mqtt_publish_exactly_once(char *_topic, uint8_t *_data, uint16_t _len, void (*_pub_cb)(mqtt_pub_typ_enu _typ, uint16_t _resp), uint8_t *_ret) ;
uint16_t mqtt_subscribe(char *_topic, uint8_t _qos, uint16_t _msg_id, void (*_suback_callback_)(uint16_t _mid, uint8_t _resp), uint8_t *_ret);
uint16_t mqtt_puback(uint16_t _msg_id, uint8_t *_res);
void mqtt_publish_rcv_cb( void (*_cb)(uint8_t _qos, uint16_t _msg_id, uint8_t *_topic, uint16_t _topic_len, uint8_t *_data, uint16_t _data_len));
#endif /* MQTT_H_ */
