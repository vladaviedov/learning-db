#include "command.h"
#include "input.h"

#include <stdlib.h>
#include <string.h>

#define EXIT ".exit"

status run_meta(input_buf *buffer) {
	if (strcmp(buffer->data, EXIT) == 0) {
		del_input_buf(buffer);
		exit(EXIT_SUCCESS);
	}

	return CMD_UNRECOGNIZED;
}
