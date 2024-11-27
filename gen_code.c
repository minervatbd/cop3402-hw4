#include <string.h>
#include "gen_code.h"

#define STACK_SPACE 4096

// Initialize the code generator
void gen_code_initialize()
{
    literal_table_initialize();
}

// Requires: bf if open for writing in binary
// and prior to this scope checking and type checking have been done.
// Write all the instructions in cs to bf in order
void gen_code_output_seq(BOFFILE bf, code_seq cs)
{
    while (!code_seq_is_empty(cs)) {
	bin_instr_t inst = code_seq_first(cs)->instr;
	instruction_write_bin_instr(bf, inst);
	cs = code_seq_rest(cs);
    }
}

// Return a header appropriate for the give code
BOFHeader gen_code_makeHeader(code_seq* mainSeq)
{
    BOFHeader ret;
    strncpy(ret.magic, "FBF", 4); // idk
    ret.text_start_address = 0;
    ret.text_length = code_seq_size(*mainSeq) * BYTES_PER_WORD;
    int dsa = MAX(ret.text_length, 1024) + BYTES_PER_WORD;
    ret.data_start_address = dsa;
    ret.data_length = literal_table_size() * BYTES_PER_WORD;
    int sba = dsa
	+ ret.data_start_address
	+ ret.data_length + STACK_SPACE;
    ret.stack_bottom_addr = sba;
    return ret;
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
    gen_code_final_output(bf, &mainSeq);
}

code_seq gen_code_block(block_t block){

//Initalize code sequence for each scope
    code_seq ret;
    ret = code_utils_save_registers_for_AR();
//generate code from var decls
    code_seq_concat(&ret, gen_code_varDecls(block.var_decls));
//same but for proc decls
    //code_seq_concat(&ret, gen_code_proc(block.proc_decls));

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
    //for every declared variable, allocate space for it on runtime stack and initalize
        code_seq temp = code_utils_allocate_stack_space(1);
        code_seq_add_to_end(&temp, code_addi(SP, 0, 0));

    //add to end and continue
        code_seq_concat(&ret, temp);
        curr= curr->next;
    }

    return ret;
}

code_seq gen_code_proc(proc_decls_t procedures){
//Todo: (extra credit)
}

code_seq gen_code_statement(stmt_t statement){
    code_seq ret;
//based on type of statement, peform the corresponding operations
    switch(statement.stmt_kind){
        case(assign_stmt):{
        assign_stmt_t data = statement.data.assign_stmt;
        //store partial lexical addr in $r3
        ret = code_utils_compute_fp(3, data.idu->levelsOutward);
        //evaultate expression and add to return seq
        code_seq_concat(&ret, gen_code_expr(*data.expr));
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

code_seq gen_code_expr(expr_t expression){
    code_seq ret;

    switch(expression.expr_kind){
        case(expr_bin):
            //return gen_code_binary_op_expr(expression.data.binary);
            break;
        case(expr_negated):
            //return gen_code_negated(expression.data.ident);
	        break; 
        
        case(expr_ident):
            //return gen_code_ident(expression.data.ident);
	        break;           
        
        case(expr_number):
            //return gen_code_number(exp.data.number);
	        break;
        
        default:
            bail_with_error("Unexpected expr_kind_e (%d) in gen_code_expr", expression.expr_kind);
	        break;
    }
    return code_seq_empty();
}


void gen_code_final_output(BOFFILE bf, code_seq* mainSeq)
{
    BOFHeader bfh = gen_code_makeHeader(mainSeq);
    bof_write_header(bf, bfh);
    gen_code_output_seq(bf, *mainSeq);
    gen_code_output_literals(bf);
    bof_close(bf);
}


// call this for literals at the end of the final output
void gen_code_output_literals(BOFFILE bof)
{
    // flip this bool for forwards/backwards output
    int backwards = 0;
    literal_table_output(bof, backwards);
}
