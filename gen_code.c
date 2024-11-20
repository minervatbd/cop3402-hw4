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
    code_seq ret;
    var_decl_t* curr = varDecls.var_decls;

//iterate through linked list of variable declarations to build code seq
    while(curr!=NULL){
        code_seq_concat(&ret, gen_code_varDecl(curr));
        curr = curr->next;
    }

    return ret;
}

code_seq gen_code_varDecl(var_decl_t* varDecl){
    return gen_code_identList(varDecl->ident_list);
}

code_seq gen_code_identList(ident_list_t idList){
    code_seq ret;
    ident_t* curr = idList.start;

    while(curr!=NULL){
        
        curr->next;
    }

    return ret;
}

code_seq gen_code_proc(proc_decl_t procedures){

}


extern void gen_code_final_output(code mainSeq){

}



