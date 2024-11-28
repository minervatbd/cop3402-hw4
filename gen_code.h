#ifndef GEN_CODE_H
#define GEN_CODE_H

#include "bof.h"
#include "ast.h"
#include "spl.tab.h"
#include "utilities.h"
#include "regname.h"
#include "code.h"
#include "code_seq.h"
#include "code_utils.h"
#include "literal_table.h"

// Initialize the code generator
extern void gen_code_initialize();

// Requires: bf is open for writing
// Generate code for prog into bf
extern void gen_code_program(BOFFILE bf, block_t prog);
extern void gen_code_output_seq(BOFFILE bf, code_seq cs);

//code generation
extern code_seq gen_code_block(block_t block);
extern code_seq gen_code_statement(stmt_t statement);
extern code_seq gen_code_varDecls(var_decls_t varDecls);
extern code_seq gen_code_identList(ident_list_t idList);
extern code_seq gen_code_proc(proc_decls_t procedures);
extern code_seq gen_code_expr(expr_t expression);

//expressions
extern code_seq gen_code_binary_op_expr(binary_op_expr_t exp);
extern code_seq gen_code_op(token_t operation, binary_op_expr_t exp);
extern code_seq gen_code_save_operands();
extern code_seq gen_code_cond(condition_t cond);
extern code_seq gen_code_rel_op(token_t operation, rel_op_condition_t exp);
extern code_seq gen_code_arith_op(token_t operation);
extern code_seq gen_code_ident(ident_t var, int sw);
extern code_seq gen_code_number(number_t number);


//ending functions
extern BOFHeader gen_code_makeHeader(code_seq* mainSeq);
extern void gen_code_output_literals(BOFFILE bof);
extern void gen_code_final_output(BOFFILE bf, code_seq* mainSeq);

#endif
