#pragma once

#include <stdint.h>

#define COL_USERNAME_SIZE 32
#define COL_EMAIL_SIZE 255

typedef struct {
    uint32_t id;
    char username[COL_USERNAME_SIZE + 1];
    char email[COL_EMAIL_SIZE + 1];
} row;

void serialize(row *src, void *dest);
void deserialize(void *src, row *dest);

// Size macros

#define sizeof_attr(str, attr) sizeof(((str *)0)->attr)

// Field sizes
#define ID_SIZE sizeof_attr(row, id)
#define USERNAME_SIZE sizeof_attr(row, username)
#define EMAIL_SIZE sizeof_attr(row, email)

// Field offsets
#define ID_OFFSET 0
#define USERNAME_OFFSET (ID_OFFSET + ID_SIZE)
#define EMAIL_OFFSET (USERNAME_OFFSET + EMAIL_SIZE)

// Total size
#define ROW_SIZE (EMAIL_OFFSET + EMAIL_SIZE)

