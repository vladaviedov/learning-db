#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "input.h"
#include "command.h"

int main(int argc, char **argv) {
	input_buf *input = new_input_buf();
	while (1) {
		prompt(input);

		if (input->data[0] == '.') {
			status result = run_meta(input);
			switch (result) {
				case CMD_UNRECOGNIZED:
					printf("unrecognized command\n");
					continue;
				case CMD_FAILED:
					printf("an error occured\n");
					continue;
				case CMD_SUCCESS:
					continue;
			}
		}
	}
}
