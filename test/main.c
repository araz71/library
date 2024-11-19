#include "tools.h"
#include <stdlib.h>
#include <string.h>

#define ERROR   printf("%d: ERROR\r\n", __LINE__)

int extract_and_check(const char* str, int nth, int should_be, int check) {
    int ok = 0;
    int res;
    res = extract_nth_number(str, nth, &ok);
    if (ok && res == should_be) {
        return 1;
    } else {
        if (!check)
            return 0;

        if (!ok) {
            printf("No number found for %d in %d'th position!\r\n", should_be, nth);
        } else {
            printf("Extracted number is mismatch: %d should be %d\r\n", res, should_be);
        }
        return 0;
    }
    return 0;
}

void test_extract_nth_number() {
    const char msg1[] = "+CSTT: 12,34,5,7,8";
    extract_and_check(msg1, 5, 8, 1);
    extract_and_check(msg1, 1, 12, 1);
    extract_and_check(msg1, 3, 5, 1);
    extract_and_check(msg1, 2, 34, 1);
    extract_and_check(msg1, 4, 7, 1);

    const char msg2[] = "+CSQ: 10";
    extract_and_check(msg2, 1, 10, 1);
    if (extract_and_check(msg2, 2, 20, 0))
        ERROR;
    if (extract_and_check(msg2, 2, 10, 0))
        ERROR;

    const char msg3[] = "\0+CSQ: 10";
    if (extract_and_check(msg3, 1, 10, 0) != 0)
        ERROR;
}

#define TOKEN_CHECK(X)     if (next == NULL) { \
    printf("%d: Token is null!\r\n");   \
} else if (strncmp(token, X, strlen(X)) != 0) {  \
    printf("%d: Extracted token mismatch [{%s} - {%s}]!\r\n", __LINE__, next, X);  \
}

int test_separate_string_by() {
    printf("Run %s\r\n", __func__);
    char msg[] = "This is a test. and this: 1234,set,arm,1";
    const char tokens[][40] = {
        "This is a test. and this: 1234",
        "set",
        "arm",
        "1"
    };


    char* next;
    char* token = separate_string_by(msg, ',', &next);
    int i = 0;
    while (next != NULL) {
        if (strcmp(token, tokens[i]) != 0) {
            printf("Extracted token: [%s] is not equal to token: [%s]\r\n",
                    token, tokens[i]);
        }
        i++;
        token = separate_string_by(next, ',', &next);
    }

    char msg2[] = "\r\nThis is a test\r\n";
    token = separate_string_by(msg2, '\n', &next);
    TOKEN_CHECK("\r")
    token = separate_string_by(next, '\n', &next);
    TOKEN_CHECK("This is a test")

    char msg3[] = "\r\nmy name is amin aghakhani\r\n\r\nOK\r\n";
    token = separate_string_by(msg3, '\n', &next);
    TOKEN_CHECK("\r")
    token = separate_string_by(next, '\n', &next);
    TOKEN_CHECK("my name is amin aghakhani\r")
    token = separate_string_by(next, '\n', &next);
    TOKEN_CHECK("\r")
    token = separate_string_by(next, '\n', &next);
    TOKEN_CHECK("OK\r")
}

int main(int argc, char *argv[]) {
	// Test exctract_number
	// Create a file pointer and open the file in read mode
    test_extract_nth_number();
    test_separate_string_by();

    FILE *file = fopen("extract_numbers.txt", "r");
	if (file == NULL) {
        fprintf(stderr, "Unable to open file!\n");
        return 1; // Exit if the file cannot be opened
	}
	// Read each line from the file and store it in 'line' buffer
	char* last_loc;
	int number;
    int ok;
	char line[256]; // Buffer to store each line of the file
	while (fgets(line, sizeof(line), file)) {
        printf("Extract: %s", line);
        last_loc = extract_number(line, &number, &ok);
        while (ok) {
            printf("\t%d\r\n", number);
            if (last_loc == NULL)
                break;

            last_loc = extract_number(last_loc, &number, &ok);
        }
	}
	fclose(file);
	
	return EXIT_SUCCESS;
}
