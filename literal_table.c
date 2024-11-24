#include <stdlib.h>
#include "literal_table.h"

typedef struct literal_table_entry
{
    struct literal_table_entry *next;
    const char *text;
    word_type value;
    unsigned int offset;
} literal;

static literal *first;
static literal *last;
static unsigned int next_word_offset;

int iteration_state;
literal *iteration_next;

// initialize the literal table
extern void literal_table_initialize()
{
    first = NULL;
    last = NULL;
    next_word_offset = 0;
    iteration_state = 0;
    iteration_next = NULL;
}

extern int literal_table_empty()
{
    return next_word_offset == 0;
}

//extern int literal_table_full()
//{
    
//}

extern int literal_table_size()
{
    return next_word_offset;
}

extern int literal_table_find_offset(const char *target, word_type value)
{
    
}

extern int literal_table_present(const char *sought, word_type value)
{

}

extern unsigned int literal_table_lookup(const char *sought, word_type value)
{

}