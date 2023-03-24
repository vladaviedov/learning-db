#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "input.h"
#include "command.h"
#include "table.h"

void cleanup(input_buf *i, table *t);

int main() {
	input_buf *input = new_input_buf();
	table *table = create_table();

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
				case RES_EXIT:
					cleanup(input, table);
				default:
					continue;
			}
		}

        statement st;
        result parse_status = parse_statement(input, &st);
        switch (parse_status) {
            case RES_UNRECOGNIZED:
                printf("unrecognized keyword\n");
                continue;
            case RES_FAILED:
                printf("syntax error\n");
                continue;
            default:
                break;
        }

        execute(&st, table);
        printf("executed\n");
	}
}

void cleanup(input_buf *i, table *t) {
	del_input_buf(i);
	destroy_table(t);
	exit(EXIT_SUCCESS);
}
