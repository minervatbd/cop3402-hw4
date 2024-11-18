#ifndef GEN_CODE_H
#define GEN_CODE_H

#include "bof.h"
#include "ast.h"
#include "literal_table.h"

// Initialize the code generator
extern void gen_code_initialize();

// Requires: bf is open for writing
// Generate code for prog into bf
extern void gen_code_program(BOFFILE bf, block_t prog);

#endif