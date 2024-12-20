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
    strcpy(ret.magic, "BO32"); 
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

//build code sequence recursively
    code_seq_concat(&mainSeq, gen_code_block(prog));
//add exit and tail
    
    if(code_seq_is_empty(mainSeq))
        mainSeq= code_seq_singleton(code_exit(0));
    else
        code_seq_add_to_end(&mainSeq, code_exit(0));
    code_seq_concat(&mainSeq, code_seq_empty());

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
            curr = curr->next;
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

    return code_seq_empty();
}

code_seq gen_code_statement(stmt_t statement){
    code_seq ret;
//based on type of statement, peform the corresponding operations
    switch(statement.stmt_kind){
        case(assign_stmt):{
            assign_stmt_t data = statement.data.assign_stmt;
        //store partial lexical addr in $r5
            ret = code_utils_compute_fp(5, data.idu->levelsOutward);

        //evaultate expression (storing value on top of the stack)
            code_seq_concat(&ret, gen_code_expr(*data.expr));

        //store val in corresponding spot in memory
            code_seq_add_to_end(&ret, code_swr(5, data.idu->attrs->offset_count, 6));

            return ret;
        }
        
        case(call_stmt):{
        //Todo: (extra credit)
        }

        case(if_stmt):{
            if_stmt_t data = statement.data.if_stmt;

            code_seq thenSeq = code_seq_empty(); 
            stmt_t* curr= data.then_stmts->stmt_list.start;
            //iterate through linked list of statements
            while(curr!=NULL){
                code_seq_concat(&thenSeq, gen_code_statement(*curr));
                curr = curr->next;
            }
            if(code_seq_is_empty(thenSeq))
                bail_with_error("No body provided for if statement");
        //push truthy val to top of stack
            code_seq_concat(&ret, gen_code_cond(data.condition));
        //if 0, skip body
            int len = code_seq_size(thenSeq);
            code_seq_concat(&ret, code_utils_allocate_stack_space(1));
            code_seq_add_to_end(&ret, code_addi(SP, 0, 0));
            code_seq_add_to_end(&ret, code_beq(SP, -1, len));
            code_seq_concat(&ret, code_utils_deallocate_stack_space(1));
            code_seq_concat(&ret, thenSeq);

            code_seq elseSeq = code_seq_empty(); 
            curr = data.else_stmts->stmt_list.start;
            while(curr != NULL){
                code_seq_concat(&elseSeq, gen_code_statement(*curr));
            }

            if(!code_seq_is_empty(thenSeq))
                code_seq_concat(&ret, elseSeq);

            return ret;
        }

        case(while_stmt):{
            while_stmt_t data = statement.data.while_stmt;
            stmt_t* curr = data.body->stmt_list.start;
            code_seq whileSeq = code_seq_empty();

            while(curr != NULL){
                code_seq_concat(&whileSeq, gen_code_statement(*curr));
                curr = curr->next;
            }
        //for jrel at end of while block and main condition
            int len = code_seq_size(whileSeq);
            int jumpBack = len * -1;
            code_seq_add_to_end(&whileSeq, code_jrel(jumpBack));

        //push truthy val to top of stack and jump forward if false
            code_seq_concat(&ret, gen_code_cond(data.condition));
            code_seq_concat(&ret, code_utils_allocate_stack_space(1));
            code_seq_add_to_end(&ret, code_addi(SP, 0, 0));
            code_seq_add_to_end(&ret, code_bne(SP, -1, len));
            code_seq_concat(&ret, code_utils_deallocate_stack_space(1));
            code_seq_concat(&ret, whileSeq);
            return ret;
        }
        
        case(read_stmt):{
            read_stmt_t data = statement.data.read_stmt;
            //read number and push onto stack
            code_seq_concat(&ret, code_utils_allocate_stack_space(1));
            code_seq_add_to_end(&ret, code_rch(SP, 0));
            // store partial lexical address in $r5
            assert(data.idu != NULL);
            code_seq_concat(&ret, code_utils_compute_fp(5, data.idu->levelsOutward));

            //store read value in corresponding namespace
            assert(id_use_get_attrs(data.idu) != NULL);
            unsigned int offset_count = id_use_get_attrs(data.idu)->offset_count;
            assert(offset_count <= USHRT_MAX);
            code_seq_add_to_end(&ret, code_lwi(5, offset_count, SP, 0));
            code_seq_concat(&ret, code_utils_deallocate_stack_space(1));
            return ret;
        }
        
        case(print_stmt):{
        //Push expression value to the top of the stack and print
            code_seq_concat(&ret, gen_code_expr(statement.data.print_stmt.expr));
            code_seq_add_to_end(&ret, code_pint(SP, 0));
            return ret;
        }
        
        case(block_stmt):{
            code_seq_concat(&ret, gen_code_block(*statement.data.block_stmt.block));
            return ret;
        }

        default: bail_with_error("Non-statement AST was provided as a statement");
    }

    return code_seq_empty();
}

code_seq gen_code_cond(condition_t cond){
    code_seq ret;
//evaultate condition expression (storing value on top of the stack)
        if(cond.cond_kind == ck_db){
            db_condition_t condData = cond.data.db_cond;
                
        //evaluate two sub-expressions and push them both to the top of the stack
            code_seq_concat(&ret, gen_code_expr(condData.dividend));
            code_seq_concat(&ret, gen_code_expr(condData.divisor));

        //move second expression (divisor) to $r4
            code_seq_add_to_end(&ret, code_lwr(4, SP, 0));
        //readjust SP
            code_seq_concat(&ret, code_utils_deallocate_stack_space(1));
        
        //compute dividend % divisor
            code_seq_add_to_end(&ret, code_div(SP, 0));
            code_seq_add_to_end(&ret, code_cfhi(SP, 0));
            code_seq_add_to_end(&ret, code_nori(SP, 0, 0));
                
        } else{
            rel_op_condition_t condData = cond.data.rel_op_cond;
            code_seq_concat(&ret, gen_code_rel_op(condData.rel_op, condData));
        }
    return ret;
}

code_seq gen_code_expr(expr_t expression){
    switch(expression.expr_kind){
        case(expr_bin):
            return gen_code_binary_op_expr(expression.data.binary);
            break;
        case(expr_negated):
            return gen_code_ident(expression.data.ident, 0);
	        break; 
        
        case(expr_ident):
            return gen_code_ident(expression.data.ident, 1);
	        break;           
        
        case(expr_number):
            return gen_code_number(expression.data.number);
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
    //Todo: construct bof header from finished code sequence
    bof_write_header(bf, bfh);
    gen_code_output_seq(bf, *mainSeq);
    gen_code_output_literals(bf);
    bof_close(bf);
}

code_seq gen_code_number(number_t number){
//store literal on top of stack
    code_seq ret = code_utils_allocate_stack_space(1);
    unsigned int offset = literal_table_lookup(number.text, number.value);

    code_seq_concat(&ret, code_seq_singleton(code_lwi(SP, 0, GP, offset)));
    return ret;
}

code_seq gen_code_ident(ident_t var, int sw){
//store partial lexical addr (secondary) in $r7
    code_seq ret = code_utils_compute_fp(7, var.idu->levelsOutward);
    id_attrs* attrs;
//check var exists
    assert((attrs = id_use_get_attrs(var.idu)) != NULL);

//based on negative switch, push corresponding (+/-)value to the top of the stack
    code_seq_concat(&ret, code_seq_singleton(code_lwi(SP, 0, 7, attrs->offset_count)));
    if(!sw)
        code_seq_concat(&ret, code_seq_singleton(code_neg(SP, 0, SP, 0)));

    return ret;
}

code_seq gen_code_binary_op_expr(binary_op_expr_t exp){
    // put the values of the two subexpressions on the stack
    code_seq ret = gen_code_expr(*(exp.expr1));
    code_seq_concat(&ret, gen_code_expr(*(exp.expr2)));

    // do the operation, putting the result on the stack
    code_seq_concat(&ret, gen_code_op(exp.arith_op, exp));
    return ret;
}

code_seq gen_code_op(token_t operation, binary_op_expr_t exp){
    switch(operation.code){
        //case(eqsym | neqsym | ltsym | leqsym | gtsym | geqsym):
            //return gen_code_rel_op(operation, exp); 
            //break;
        case(plussym | minussym | multsym | divsym):
            return gen_code_arith_op(operation);
	        break;

        default:
	        bail_with_error("Unknown token code (%d) was provided.", operation.code);
	        break;
    }

    return code_seq_empty();
}



code_seq gen_code_rel_op(token_t operation, rel_op_condition_t exp){
    code_seq ret;  
    //push both expression values to top of stack
    code_seq_concat(&ret, gen_code_expr(exp.expr1));
    code_seq_concat(&ret, gen_code_expr(exp.expr2));

    //move first expression to $r4
    code_seq_add_to_end(&ret, code_lwr(4, SP, 0));
    //readjust SP
    code_seq_concat(&ret, code_utils_deallocate_stack_space(1));

//store truthy value (1/0) on top of stack based on op code
    switch (operation.code) {
        case eqsym: {
            code_seq_add_to_end(&ret, code_xor(SP, 0, 4, 0));
            code_seq_add_to_end(&ret, code_addi(7, 0, 1));
            code_seq_add_to_end(&ret, code_xor(SP, 0, 7, 0));
	        break;
        }
	
        case neqsym:{
            code_seq_add_to_end(&ret, code_xor(SP,0, 4, 0));
            break;
        }

        case ltsym:{
        //todo
            code_seq_concat(&ret, code_seq_empty());
            break;
        }
	
        case leqsym:{
        //todo
            code_seq_concat(&ret, code_seq_empty());
            break;
        }
        
        case gtsym:{
        //todo
            code_seq_concat(&ret, code_seq_empty());
            break;
        }
        
        case geqsym:{
        //todo
            code_seq_concat(&ret, code_seq_empty());
            break;
        }

        default:
            bail_with_error("Unknown token code (%d) provided in gen_code_rel_op",
                operation.code);
            break;
    }
    return ret;
}

code_seq gen_code_arith_op(token_t operation){
    code_seq ret = gen_code_save_operands();

//consume operands and store result on top of the stack
    switch (operation.code) {
        case plussym:
            code_seq_concat(&ret, code_utils_allocate_stack_space(1));
	        code_seq_add_to_end(&ret, code_add(SP, 0, 4, 0));
            code_seq_add_to_end(&ret, code_add(SP, 0, 5, 0));
	        break;
        
        case minussym:
	        code_seq_concat(&ret, code_utils_allocate_stack_space(1));
	        code_seq_add_to_end(&ret, code_sub(SP, 0, 4, 0));
            code_seq_add_to_end(&ret, code_sub(SP, 0, 5, 0));
	        break;

        case multsym:
	        code_seq_concat(&ret, code_utils_allocate_stack_space(1));
	        code_seq_add_to_end(&ret, code_add(SP, 0, 4, 0));
            code_seq_add_to_end(&ret, code_mul(5, 0));
            break;

        case divsym:
	        code_seq_concat(&ret, code_utils_allocate_stack_space(1));
	        code_seq_add_to_end(&ret, code_add(SP, 0, 4, 0));
            code_seq_add_to_end(&ret, code_div(5, 0));
            break;

        default:
	    bail_with_error("Unexpected arithOp (%d) was provided", operation.code);
	    break;
    }
    return ret;
}

code_seq gen_code_save_operands(){
    code_seq ret;
    //store operands (first two spots in stack) in $r4/r5
    code_seq_add_to_end(&ret, code_lwr(4, SP, -1));
    code_seq_add_to_end(&ret, code_lwr(5, SP, 0));
    //readjust SP
    code_seq_concat(&ret, code_utils_deallocate_stack_space(2));

    return ret;
}

// call this for literals at the end of the final output
void gen_code_output_literals(BOFFILE bof)
{
    // flip this bool for forwards/backwards output
    int backwards = 0;
    literal_table_output(bof, backwards);
}
