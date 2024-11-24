#include "gen_code.h"

// Initialize the code generator
void gen_code_initialize()
{
    literal_table_initialize();
}

// Requires: bf is open for writing
// Generate code for prog into bf
void gen_code_program(BOFFILE bf, block_t prog)
{
    code_seq mainSeq = code_seq_empty();
    //Todo: handle constant declarations (needs literal table module)

//build code sequence recursively
    code_seq_concat(&mainSeq, gen_code_block(prog));

//Todo: construct bof header from finished code sequence

//output
    gen_code_final_output(&mainSeq);
}

code_seq gen_code_block(block_t block){
    code_seq ret;
//generate code from var decls
    code_seq_concat(&ret, gen_code_varDecls(block.var_decls));
//same but for proc decls
    code_seq_concat(&ret, gen_code_proc(block.proc_decls));

//if statements exist within block, recursively parse stmts ast into code
    if(block.stmts.stmts_kind != empty_stmts_e){
        stmt_t* curr= block.stmts.stmt_list.start;
    //iterate through linked list of statements
        while(curr!=NULL){
            code_seq_concat(&ret, gen_code_statement(*curr));
        }
    }

    return ret;
}

code_seq gen_code_varDecls(var_decls_t varDecls){
    code_seq ret;
    var_decl_t* curr = varDecls.var_decls;

//iterate through linked list of variable declarations to build code seq
    while(curr!=NULL){
        code_seq_concat(&ret, gen_code_identList(curr->ident_list));
        curr = curr->next;
    }

    return ret;
}
code_seq gen_code_identList(ident_list_t idList){
    code_seq ret;
    ident_t* curr = idList.start;
    
    while(curr!=NULL){
    //for every declared variable, allocate space for it on runtime stack (within current AR)
        code_seq_concat(&ret, code_utils_allocate_stack_space(1));
        curr->next;
    }

    return ret;
}

code_seq gen_code_proc(proc_decls_t procedures){
//Todo
}

code_seq gen_code_statement(stmt_t statement){
    code_seq ret;
//based on type of statement, peform the corresponding operations
    switch(statement.stmt_kind){
        case(assign_stmt):{
        assign_stmt_t data = statement.data.assign_stmt;
        //Todo: copy value to specified register (cpw)
            //code_seq_concat(&ret, code_cpw())
        }
        
        case(call_stmt):{
        //Todo: 
        }

        case(if_stmt):{
        //Todo:
        }

        case(while_stmt):{
        //Todo:
        }
        
        case(read_stmt):{
        //Todo:
        }
        
        case(print_stmt):{
        //Todo:
        }
        
        case(block_stmt):{
        //Todo:
            code_seq_concat(&ret, gen_code_block(*statement.data.block_stmt.block));
        }

        default: bail_with_error("Non-statement AST was provided as a statement");
    }
}


extern void gen_code_final_output(code_seq* mainSeq){
//Todo
}


// call this for literals at the end of the final output
static void gen_code_output_literals(BOFFILE bof)
{
    literal_table_output(bof);
}