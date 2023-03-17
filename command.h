#pragma once

#include "input.h"

typedef enum {
	CMD_SUCCESS,
	CMD_FAILED,
	CMD_UNRECOGNIZED
} status;

status run_meta(input_buf *buffer);
