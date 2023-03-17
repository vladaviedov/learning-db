#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "input.h"
#include "command.h"

int main() {
	input_buf *input = new_input_buf();
	while (1) {
		prompt(input);

		if (input->data[0] == '.') {
			result status = run_meta(input);
			switch (status) {
				case RES_UNRECOGNIZED:
					printf("unrecognized command\n");
					continue;
				case RES_FAILED:
					printf("an error occured\n");
					continue;
				case RES_SUCCESS:
					continue;
			}
		}

        statement st;
        result parse_status = parse_statement(input, &st);
        switch (parse_status) {
            case RES_UNRECOGNIZED:
                printf("unrecognized keyword\n");
                continue;
            default:
                break;
        }

        execute(&st);
        printf("executed\n");
	}
}
