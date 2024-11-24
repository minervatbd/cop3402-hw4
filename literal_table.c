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

static void literal_table_valid()
{
    int empty = literal_table_empty();
    if (empty != (first == NULL) || empty != (last == NULL))
        bail_with_error("Literal table invalid");
}

// initialize the literal table
extern void literal_table_initialize()
{
    first = NULL;
    last = NULL;
    next_word_offset = 0;
    iteration_state = 0;
    iteration_next = NULL;
    literal_table_valid();
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
    literal_table_valid();
    literal *entry = first;

    while (entry != NULL)
    {
        if (strcmp(entry->text, target) == 0)
            return entry->offset;
        entry = entry->next;
    }

    return -1;
}

extern int literal_table_present(const char *sought, word_type value)
{
    literal_table_valid();
    return literal_table_find_offset(sought, value) >= 0;
}

extern unsigned int literal_table_lookup(const char *sought, word_type value)
{

}