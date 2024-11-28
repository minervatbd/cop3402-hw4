#include "literal_table.h"

typedef struct literal_table_entry
{
    struct literal_table_entry *next;
    struct literal_table_entry *prev;
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

extern int literal_table_present(const char *target, word_type value)
{
    literal_table_valid();
    return literal_table_find_offset(target, value) >= 0;
}

extern unsigned int literal_table_lookup(const char *target, word_type value)
{
    int res = literal_table_find_offset(target, value);
    if (res != -1)
        return res;
    
    literal *entry = (literal*) malloc(sizeof(literal));

    if (entry == NULL)
        bail_with_error("Couldn't allocate memory for literal table entry!");
    
    entry->next = NULL;
    entry->prev = NULL;
    entry->value = value;
    entry->text = target;
    entry->offset = next_word_offset;

    if (literal_table_empty())
    {
        first = entry;
        last = entry;
    }

    else
    {
        entry->prev = last;
        last->next = entry;
        last = entry;
    }

    literal_table_valid();
    return next_word_offset++;
}

extern void literal_table_output(BOFFILE bof, int backwards)
{
    literal_table_valid();
    literal *entry;
    literal *done_entry;
    
    if (!backwards)
        entry = first;
    else
        entry = last;
    
    while (entry != NULL)
    {
        bof_write_word(bof, entry->value);

        done_entry = entry;

        if (!backwards)
            entry = entry->next;
        else
            entry = entry->prev;
        
        free(done_entry);
    }
}