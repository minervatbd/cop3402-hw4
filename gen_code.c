#include "gen_code.h"

// Initialize the code generator
void gen_code_initialize()
{
    //initalize literal table (call module function)
}

// Requires: bf is open for writing
// Generate code for prog into bf
void gen_code_program(BOFFILE bf, block_t prog)
{
    code_seq mainSeq = code_seq_empty();
    code_seq_concat(&mainSeq, gen_code_varDecls(prog.var_decls));
    
}


code_seq gen_code_statement(stmt_t statement){

}

code_seq gen_code_varDecls(var_decls_t varDecls){

}

code_seq gen_code_ident(ident_t identifier){

}

code_seq gen_code_proc(proc_decl_t procedures){

}


extern void gen_code_final_output(code mainSeq){

}



