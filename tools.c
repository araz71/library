#include "tools.h"

char* separate_string_by(char* msg, const char delim, char** next) {
	char* start = msg;
	if (msg == NULL) {
		*next = NULL;
		return NULL;
	}

	for (uint16_t i = 0; i < strlen(msg); i++) {
		if (msg[i] == delim) {
			msg[i] = '\0';
			*next = &msg[i + 1];
			return start;
		}
	}

    *next = NULL;
    return start;
}

int extract_nth_number(const char* str, int nth, int* ok) {
	char number[10];
	uint8_t i = 0;
	uint8_t fnd_cntr = 0;
	uint8_t fnd = 0;
	uint8_t ncntr = 0;
	*ok = 0;
	memset(number, 0, sizeof(number));
	for (i = 0; i < strlen(str); i++) {
		if (ncntr >= 8) {
			fnd = 0;
			break;
		}
		if (str[i] >= '0' && str[i] <= '9') {
			fnd = 1;
			number[ncntr++] = str[i];
		} else {
			if (fnd == 1) {
				fnd_cntr++;
				if (fnd_cntr == nth) {
					break;
				}
                memset(number, 0, sizeof(number));
				fnd = 0;
				ncntr = 0;
			}
		}
	}

    if (i == strlen(str)) {
        if (ncntr != 0) {
            fnd_cntr++;
        }
    }

	if (fnd_cntr == nth) {
		*ok = 1;
	}

	if (*ok)
		return atoi(number);
	else
		return 0;
}

char* extract_number(const char* str, int* result, int* ok) {
	char number[10];
	int8_t find = 0;
	uint8_t loc = 0;
	char* ret_loc = NULL;
	uint8_t i;

    *ok = 1;
	memset(number, 0, sizeof(number));
	for (i = 0; i < strlen(str); i++) {
		ret_loc = (char *)&str[i];
		if (str[i] >= '0' && str[i] <='9') {
			find = 1;
			number[loc++] = str[i];
			if (loc >= 8) {
				break;
			}
		} else {
			if (find) {
				break;
			}
		}
	}

    if (find == 0) {
        *ok = 0;
    } else {
        *result = atoi(number);
    }

	if (i == strlen(str)) {
		ret_loc = NULL;
	}

	return (char *)ret_loc;
}
