#ifndef GEN_CODE_H
#define GEN_CODE_H

#include "bof.h"
#include "ast.h"
#include "code.h"
#include "code_seq.h"

// Initialize the code generator
extern void gen_code_initialize();

// Requires: bf is open for writing
// Generate code for prog into bf
extern void gen_code_program(BOFFILE bf, block_t prog);

//code generation
code_seq gen_code_statement(stmt_t statement);
code_seq gen_code_varDecls(var_decl_t varDecls);
code_seq gen_code_varDecl(var_decl_t* varDecl);
code_seq gen_code_identList(ident_list_t idList);
code_seq gen_code_proc(proc_decl_t procedures);

//ending functions
extern void gen_code_makeHeader(code mainSeq);
extern void gen_code_final_output(code mainSeq);

#endif