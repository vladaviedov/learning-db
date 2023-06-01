#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "input.h"
#include "command.h"
#include "table.h"

void cleanup(input_buf *i, table *t);

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "invalid arguments\n");
		exit(EXIT_FAILURE);
	}

	input_buf *input = new_input_buf();
	table *table = open_table(argv[1]);

	while (1) {
		prompt(input);

		if (input->data[0] == '.') {
			result status = run_meta(input, table);
			switch (status) {
				case RES_UNRECOGNIZED:
					fprintf(stderr, "unrecognized command\n");
					continue;
				case RES_FAILED:
					fprintf(stderr, "an error occured\n");
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
                fprintf(stderr, "unrecognized keyword\n");
                continue;
            case RES_FAILED:
                fprintf(stderr, "syntax error\n");
                continue;
            default:
                break;
        }

        execute(&st, table);
        printf("executed\n");
	}
}

void cleanup(input_buf *i, table *t) {
	close_table(t);
	del_input_buf(i);
	exit(EXIT_SUCCESS);
}
