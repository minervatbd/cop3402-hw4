#ifndef LITERAL_TABLE_H
#define LITERAL_TABLE_H
#include "machine_types.h"

extern void literal_table_initialize();

extern int literal_table_empty();

//extern int literal_table_full();

extern int literal_table_size();

extern int literal_table_find_offset(const char *target, word_type value);

extern int literal_table_present(const char *sought, word_type value);

extern unsigned int literal_table_lookup(const char *sought, word_type value);

#endif