/*
 * mqtt.c
 *
 *  Created on: Nov 27, 2019
 *      Author: amin
 */


#include <mqtt.h>
#define MQTT_PROTOCOL_NAME					"MQTT"

typedef enum {
    MQTT_MSG_TYPE_RESERVED_1,
    MQTT_MSG_TYPE_CONNECT,
    MQTT_MSG_TYPE_CONNACK,
    MQTT_MSG_TYPE_PUBLISH,
    MQTT_MSG_TYPE_PUBACK,
    MQTT_MSG_TYPE_PUBREC,
    MQTT_MSG_TYPE_PUBREL,
    MQTT_MSG_TYPE_PUBCOMP,
    MQTT_MSG_TYPE_SUBSCRIBE,
    MQTT_MSG_TYPE_SUBACK,
    MQTT_MSG_TYPE_UNSUBSCRIBE,
    MQTT_MSG_TYPE_UNSUBACK,
    MQTT_MSG_TYPE_PINGREQ,
    MQTT_MSG_TYPE_PINGRESP,
    MQTT_MSG_TYPE_DISCONNECT,
    MQTT_MSG_TYPE_RESERVED_2,
} mqtt_msg_type_enu;

void (*_mqtt_subscribed_callback_)(uint16_t _mid, uint8_t _resp);
void (*_mqtt_connect_callback_)(mqtt_conack_enu _resp);
void (*_mqtt_published_callback_)(mqtt_pub_typ_enu _typ, uint16_t _mid);

static void (*_mqtt_publish_callback_)(uint8_t _qos, uint16_t _msg_id, uint8_t *_topic, uint16_t _topic_len, uint8_t *_data, uint16_t _data_len);

uint16_t mqtt_connect(char *_id, char *_username, char *_password, uint16_t _flags, uint16_t _keepalive, void (*_conn_cb)(mqtt_conack_enu _resp), uint8_t *_ret) {
	uint16_t mqtt_bcntr = 0;
	uint8_t flag = 2;

	_ret[0] = 0;
	_ret[mqtt_bcntr++] = MQTT_MSG_TYPE_CONNECT << 4;
	mqtt_bcntr++; //reject length

	_ret[mqtt_bcntr++] = 0;
	_ret[mqtt_bcntr++] = strnlen(MQTT_PROTOCOL_NAME, 10);
    	memcpy(_ret + mqtt_bcntr, MQTT_PROTOCOL_NAME, 4);
	mqtt_bcntr += 4;

	_ret[mqtt_bcntr++] = 4;		//version

	if (_username != NULL) {
		flag |= 0x80;
	}
	if (_password != NULL) {
		flag |= 0x40;
	}
	_ret[mqtt_bcntr++] = flag;

	_ret[mqtt_bcntr++] = 0x0;		//keep alive
	_ret[mqtt_bcntr++] = 120;

	_ret[mqtt_bcntr++] = 0;		//len
	_ret[mqtt_bcntr++] = strnlen(_id, 20);
	memcpy(_ret + mqtt_bcntr, (uint8_t *) _id, strnlen(_id, 20));
	mqtt_bcntr += strnlen(_id, 20);

	if (_username != NULL) {
		_ret[mqtt_bcntr++] = 0;		//len
		_ret[mqtt_bcntr++] = strnlen(_username, 20);
		memcpy(_ret + mqtt_bcntr, (uint8_t *) _username, _ret[mqtt_bcntr - 1]);
		mqtt_bcntr += _ret[mqtt_bcntr - 1];
	}

	if (_password != NULL) {
		_ret[mqtt_bcntr++] = 0;		//len
		_ret[mqtt_bcntr++] = strnlen(_password, 20);
		memcpy(_ret + mqtt_bcntr, (uint8_t *) _password, _ret[mqtt_bcntr - 1]);
		mqtt_bcntr += _ret[mqtt_bcntr - 1];
	}

	_ret[1] = mqtt_bcntr - 2;		//msg length

	_mqtt_connect_callback_ = _conn_cb;
	return mqtt_bcntr;
}

uint16_t mqtt_puback(uint16_t _msg_id, uint8_t *_res) {
	uint16_t mqtt_bcntr = 0;
	_res[mqtt_bcntr++] = MQTT_MSG_TYPE_PUBACK << 4;
	_res[mqtt_bcntr++] = 0x02;
	_res[mqtt_bcntr++] = (_msg_id >> 8) & 0xFF;
	_res[mqtt_bcntr++] = (_msg_id) & 0xFF;

	return mqtt_bcntr;
}

void mqtt_resp_hdl(uint8_t *_resp, uint16_t _len) {
	uint8_t msg_type;

	uint16_t msg_id = 0;
	uint8_t QoS = _resp[0] & 0x6;
	uint8_t topic_len;
	uint8_t topic[15];
	uint8_t msg[10];
	uint16_t msg_len;

	uint16_t length = 0;

	uint16_t multiplier = 1;
	uint8_t digit;
	uint8_t cntr = 1;


	if (_len < 4) {
		return;
	}

	msg_type = _resp[0];
	msg_type = (msg_type >> 4) & 0xFF;
	do {
		digit = _resp[cntr];
		length += (digit & 127) * multiplier;
		multiplier *= 128;
		cntr++;
	} while ((digit & 128) != 0);

	if (msg_type == MQTT_MSG_TYPE_CONNACK) {
		uint8_t conack = _resp[cntr + 1];
		_mqtt_connect_callback_((mqtt_conack_enu) conack);
	} else if (msg_type == MQTT_MSG_TYPE_PUBACK) {
		msg_id = (_resp[cntr] << 8) | (_resp[cntr + 1]);
		_mqtt_published_callback_(MQTT_PUB_TYP_ACK, msg_id);
	} else if (msg_type == MQTT_MSG_TYPE_SUBACK) {
		msg_id = (_resp[cntr] << 8) | (_resp[cntr + 1]);
		_mqtt_subscribed_callback_(msg_id, _resp[4]);
	} else if (msg_type == MQTT_MSG_TYPE_PUBLISH) {

			topic_len = _resp[cntr + 1];
			cntr++;
			cntr++;

			if (topic_len > 10) {
				return;
			}
			memcpy(topic, &_resp[cntr], topic_len);
			topic[topic_len] = 0;
			cntr += topic_len;

			if (QoS > 0) {
				msg_id = _resp[cntr++] << 8;
				msg_id |= _resp[cntr++];
			}

			msg_len = length - 2 - topic_len;
			memcpy(msg, &_resp[cntr], 3);
			msg[2] = 0;

			_mqtt_publish_callback_(QoS, msg_id, topic, topic_len, msg, msg_len);
	}
}

uint16_t mqtt_publish_exactly_once(char *_topic, uint8_t *_data, uint16_t _len, void (*_pub_cb)(mqtt_pub_typ_enu _typ, uint16_t _resp), uint8_t *_ret) {
	uint16_t len = 0;
	uint16_t mqtt_bcntr = 0;
	_ret[mqtt_bcntr++] = (MQTT_MSG_TYPE_PUBLISH << 4) | 0x2;

	len = (strnlen(_topic, 20) + _len);
	len = len + 4;

	uint8_t digit = 0;
	do {
		digit = len % 128;
		len = len / 128;
		if (len > 0) {
			digit |= 0x80;
		}
		_ret[mqtt_bcntr++] = digit;
	} while (len > 0);

	_ret[mqtt_bcntr++] = 0;
	_ret[mqtt_bcntr++] = strlen(_topic);

	memcpy(_ret + mqtt_bcntr, _topic, _ret[mqtt_bcntr - 1]);
	mqtt_bcntr += strlen(_topic);

	//msgID
	uint16_t rnd = get_timestamp();
	_ret[mqtt_bcntr++] = rnd >> 8;
	_ret[mqtt_bcntr++] = rnd;
	memcpy(_ret + mqtt_bcntr, _data, _len);
	mqtt_bcntr += _len;

	_mqtt_published_callback_ = _pub_cb;
	return mqtt_bcntr;
}

uint16_t mqtt_subscribe(char *_topic, uint8_t _qos, uint16_t _msg_id, void (*_suback_callback_)(uint16_t _mid, uint8_t _resp), uint8_t *_ret) {
	uint16_t mqtt_bcntr = 0;
	_ret[mqtt_bcntr++] = (MQTT_MSG_TYPE_SUBSCRIBE << 4) | 0x02;
	_ret[mqtt_bcntr++] = 4 + strlen(_topic) + 1;

	_ret[mqtt_bcntr++] = (_msg_id >> 8) & 0xFF;
	_ret[mqtt_bcntr++] = (_msg_id) & 0xFF;

	_ret[mqtt_bcntr++] = 0;
	_ret[mqtt_bcntr++] = strlen(_topic);
	memcpy(_ret + mqtt_bcntr, _topic, _ret[mqtt_bcntr - 1]);
	mqtt_bcntr += _ret[mqtt_bcntr - 1];

	_ret[mqtt_bcntr++] = _qos;

	_mqtt_subscribed_callback_ = _suback_callback_;
	return mqtt_bcntr;
}

void mqtt_publish_rcv_cb( void (*_cb)(uint8_t _qos, uint16_t _msg_id, uint8_t *_topic, uint16_t _topic_len, uint8_t *_data, uint16_t _data_len)) {
	_mqtt_publish_callback_ = _cb;
}
