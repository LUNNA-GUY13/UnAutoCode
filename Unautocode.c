/*
*This program is a interpreter of the original 1952 Autocode language for Alick Glennie's the Ferranti Mark 1 computer.
*The 1952 Autocode was a assembly language for the Ferranti Mark 1 computer.
*
*It was invented by Alick Glennie in 1952 and was the first autocode to be used on a stored-program computer.
*It was a "minimal" autocode, meaning that it was designed to be as simple as possible to use.
*This version is made by LunnaGuy13(me) in 2026 because i was bored and C is """fun"""
*/


#include "unautocode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

// ---------------------------------------------------------
// Lexer
// ---------------------------------------------------------

static bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

static void add_token(Token** tokens, size_t* count, size_t* capacity, Token t) {
    if (*count >= *capacity) {
        *capacity = (*capacity == 0) ? 64 : (*capacity * 2);
        Token* tmp = realloc(*tokens, *capacity * sizeof(Token));
        if (!tmp) {
            fprintf(stderr, "Fatal: out of memory during tokenization\n");
            exit(1);
        }
        *tokens = tmp;
    }
    (*tokens)[*count] = t;
    (*count)++;
}

bool tokenize(const char* source, Token** out_tokens, size_t* out_count) {
    size_t capacity = 0;
    *out_tokens = NULL;
    *out_count = 0;
    
    int line_num = 1;
    const char* p = source;
    
    while (*p != '\0') {
        // Only spaces and tabs are horizontal whitespace — \r is NOT generic whitespace.
        if (*p == ' ' || *p == '\t') {
            p++;
            continue;
        }
        
        // Windows CRLF: consume \r\n as one atomic newline.
        if (*p == '\r' && *(p + 1) == '\n') {
            Token t = {TOK_NEWLINE, 0, line_num};
            add_token(out_tokens, out_count, &capacity, t);
            line_num++;
            p += 2;
            continue;
        }
        
        // Unix LF or bare CR (old Mac) — both count as a newline.
        if (*p == '\n' || *p == '\r') {
            Token t = {TOK_NEWLINE, 0, line_num};
            add_token(out_tokens, out_count, &capacity, t);
            line_num++;
            p++;
            continue;
        }
        
        // Handle comments: skip everything until any line terminator.
        if (*p == ';') {
            while (*p != '\n' && *p != '\r' && *p != '\0') {
                p++;
            }
            continue;
        }
        
        if (strncmp(p, "CONTROL", 7) == 0) {
            Token t = {TOK_CONTROL, 0, line_num};
            add_token(out_tokens, out_count, &capacity, t);
            p += 7;
            continue;
        }
        
        if (strncmp(p, "IF", 2) == 0) {
            Token t = {TOK_IF, 0, line_num};
            add_token(out_tokens, out_count, &capacity, t);
            p += 2;
            continue;
        }
        
        if (*p == '=' && *(p+1) == '=') {
            Token t = {TOK_OP_EQ, 0, line_num};
            add_token(out_tokens, out_count, &capacity, t);
            p += 2;
            continue;
        }
        if (*p == '>') {
            if (*(p+1) == '=') {
                Token t = {TOK_OP_GTE, 0, line_num};
                add_token(out_tokens, out_count, &capacity, t);
                p += 2;
            } else {
                Token t = {TOK_OP_GT, 0, line_num};
                add_token(out_tokens, out_count, &capacity, t);
                p++;
            }
            continue;
        }
        if (*p == '<') {
            if (*(p+1) == '=') {
                Token t = {TOK_OP_LTE, 0, line_num};
                add_token(out_tokens, out_count, &capacity, t);
                p += 2;
            } else {
                Token t = {TOK_OP_LT, 0, line_num};
                add_token(out_tokens, out_count, &capacity, t);
                p++;
            }
            continue;
        }
        if (*p == '=') {
            Token t = {TOK_ASSIGN, 0, line_num};
            add_token(out_tokens, out_count, &capacity, t);
            p++;
            continue;
        }
        if (*p == '+') { Token t = {TOK_OP_ADD, 0, line_num}; add_token(out_tokens, out_count, &capacity, t); p++; continue; }
        if (*p == '*') { Token t = {TOK_OP_MUL, 0, line_num}; add_token(out_tokens, out_count, &capacity, t); p++; continue; }
        if (*p == '/') { Token t = {TOK_OP_DIV, 0, line_num}; add_token(out_tokens, out_count, &capacity, t); p++; continue; }
        
        // Handle '-' which can be minus operator or part of a negative number
        if (*p == '-') {
            // It's a negative number if the next char is a digit AND previous token wasn't a register or number.
            // Actually, in the grammar, `operator` has `-`, and `number` has `["-"] digit`.
            // If it's preceded by ASSIGN, OP_EQ, IF, etc., it's a number. If preceded by operand, it's an operator.
            bool is_operator = false;
            if (*out_count > 0) {
                TokenType prev = (*out_tokens)[*out_count - 1].type;
                if (prev == TOK_REGISTER || prev == TOK_NUMBER) {
                    is_operator = true;
                }
            }
            if (is_operator) {
                Token t = {TOK_OP_SUB, 0, line_num};
                add_token(out_tokens, out_count, &capacity, t);
                p++;
                continue;
            }
        }
        
        // Number literal (starts with digit or minus sign followed by digit)
        if (is_digit(*p) || (*p == '-' && is_digit(*(p+1)))) {
            char* endptr;
            double val = strtod(p, &endptr);
            Token t = {TOK_NUMBER, val, line_num};
            add_token(out_tokens, out_count, &capacity, t);
            p = endptr;
            continue;
        }
        
        // Register (c0 - c25)
        if (*p == 'c' || *p == 'C') {
            if (is_digit(*(p+1))) {
                char* endptr;
                long reg_idx = strtol(p+1, &endptr, 10);
                Token t = {TOK_REGISTER, (double)reg_idx, line_num};
                add_token(out_tokens, out_count, &capacity, t);
                p = endptr;
                continue;
            }
        }
        
        fprintf(stderr, "Lexer error at line %d: Unknown character '%c'\n", line_num, *p);
        return false;
    }
    
    Token t = {TOK_EOF, 0, line_num};
    add_token(out_tokens, out_count, &capacity, t);
    return true;
}

// ---------------------------------------------------------
// Parser
// ---------------------------------------------------------

static Token* current_token;
static bool has_error = false;

static void advance() {
    if (current_token->type != TOK_EOF) {
        current_token++;
    }
}

static bool match(TokenType type) {
    if (current_token->type == type) {
        advance();
        return true;
    }
    return false;
}

static void error(const char* msg) {
    fprintf(stderr, "Parse error at line %d: %s\n", current_token->line_num, msg);
    has_error = true;
}


static bool is_operator(TokenType type) {
    return type == TOK_OP_ADD || type == TOK_OP_SUB || type == TOK_OP_MUL || type == TOK_OP_DIV;
}

static bool is_rel_operator(TokenType type) {
    return type == TOK_OP_GT || type == TOK_OP_LT || type == TOK_OP_EQ || type == TOK_OP_GTE || type == TOK_OP_LTE;
}

static OperatorType token_to_op(TokenType type) {
    switch(type) {
        case TOK_OP_ADD: return OP_ADD;
        case TOK_OP_SUB: return OP_SUB;
        case TOK_OP_MUL: return OP_MUL;
        case TOK_OP_DIV: return OP_DIV;
        case TOK_OP_GT:  return OP_GT;
        case TOK_OP_LT:  return OP_LT;
        case TOK_OP_EQ:  return OP_EQ;
        case TOK_OP_GTE: return OP_GTE;
        case TOK_OP_LTE: return OP_LTE;
        default: return OP_NONE;
    }
}

static Operand parse_operand() {
    Operand op = {0};
    if (current_token->type == TOK_REGISTER) {
        if (current_token->value < 0 || current_token->value > 25) {
            error("Array out of bounds: Register index must be between c0 and c25");
        }
        op.is_register = true;
        op.value = current_token->value;
        advance();
    } else if (current_token->type == TOK_NUMBER) {
        op.is_register = false;
        op.value = current_token->value;
        advance();
    } else {
        error("Expected operand (register or number)");
    }
    return op;
}

static Statement parse_statement() {
    Statement stmt = {0};
    stmt.line_num = current_token->line_num;

    if (current_token->type == TOK_CONTROL) {
        advance(); // skip CONTROL
        if (current_token->type != TOK_NUMBER) {
            error("Expected line number after CONTROL");
            return stmt;
        }
        stmt.type = STMT_CONTROL;
        stmt.data.control.target_line = (int)current_token->value;
        advance();
        
        if (!match(TOK_IF)) {
            error("Expected IF after CONTROL <line>");
            return stmt;
        }
        
        stmt.data.control.operand1 = parse_operand();
        
        if (!is_rel_operator(current_token->type)) {
            error("Expected relational operator in condition");
            return stmt;
        }
        stmt.data.control.rel_op = token_to_op(current_token->type);
        advance();
        
        stmt.data.control.operand2 = parse_operand();
        
    } else if (current_token->type == TOK_REGISTER) {
        stmt.type = STMT_ASSIGN;
        if (current_token->value < 0 || current_token->value > 25) {
            error("Array out of bounds: Register index must be between c0 and c25");
        }
        stmt.data.assign.dest_reg = (int)current_token->value;
        advance();
        
        if (!match(TOK_ASSIGN)) {
            error("Expected '=' after register in assignment");
            return stmt;
        }
        
        stmt.data.assign.operand1 = parse_operand();
        
        if (is_operator(current_token->type)) {
            stmt.data.assign.op = token_to_op(current_token->type);
            advance();
            stmt.data.assign.operand2 = parse_operand();
            
            // Strictly reject nested expressions (e.g. c1 = c2 + c3 + c4)
            if (is_operator(current_token->type)) {
                error("Nested expressions are strictly prohibited (max two operands, one operator)");
            }
        } else {
            stmt.data.assign.op = OP_NONE;
        }
    } else {
        error("Expected statement (CONTROL or Register Assignment)");
    }
    
    return stmt;
}

bool parse(Token* tokens, size_t token_count, Statement** out_program, size_t* out_stmt_count) {
    (void)token_count;
    current_token = tokens;
    has_error = false;
    
    size_t capacity = 64;
    *out_program = malloc(capacity * sizeof(Statement));
    if (!*out_program) {
        fprintf(stderr, "Fatal: out of memory during parsing\n");
        return false;
    }
    *out_stmt_count = 0;
    
    while (current_token->type != TOK_EOF && !has_error) {
        while (current_token->type == TOK_NEWLINE) {
            advance();
        }
        if (current_token->type == TOK_EOF) break;
        
        Statement stmt = parse_statement();
        if (has_error) break;
        
        if (*out_stmt_count >= capacity) {
            capacity *= 2;
            *out_program = realloc(*out_program, capacity * sizeof(Statement));
        }
        
        (*out_program)[*out_stmt_count] = stmt;
        (*out_stmt_count)++;
        
        if (current_token->type != TOK_NEWLINE && current_token->type != TOK_EOF) {
            error("Expected newline after statement");
            break;
        }
    }
    
    if (has_error) {
        free(*out_program);
        *out_program = NULL;
        *out_stmt_count = 0;
        return false;
    }
    
    // Pre-compute CONTROL jump targets: resolve source line numbers -> statement indices (O(1) at runtime).
    for (size_t i = 0; i < *out_stmt_count; i++) {
        if ((*out_program)[i].type != STMT_CONTROL) continue;
        int target_line = (*out_program)[i].data.control.target_line;
        int resolved = -1; // -1 means halt (target beyond end of program)
        for (size_t j = 0; j < *out_stmt_count; j++) {
            if ((*out_program)[j].line_num >= target_line) {
                resolved = (int)j;
                break;
            }
        }
        (*out_program)[i].data.control.target_stmt_idx = resolved;
    }
    
    return true;
}

// ---------------------------------------------------------
// Virtual Machine
// ---------------------------------------------------------

void vm_init(VirtualMachine* vm, Statement* program, size_t program_size) {
    memset(vm->registers, 0, sizeof(vm->registers));
    vm->instruction_pointer = 0;
    vm->program = program;
    vm->program_size = program_size;
}

static double eval_operand(VirtualMachine* vm, Operand op) {
    if (op.is_register) {
        if (op.value < 0 || op.value >= NUM_REGISTERS) {
            fprintf(stderr, "VM Error: Register out of bounds\n");
            exit(1);
        }
        return vm->registers[(int)op.value];
    } else {
        return op.value;
    }
}

void vm_execute(VirtualMachine* vm) {
    while (vm->instruction_pointer >= 0 && (size_t)vm->instruction_pointer < vm->program_size) {
        Statement* stmt = &vm->program[vm->instruction_pointer];
        
        if (stmt->type == STMT_ASSIGN) {
            double val1 = eval_operand(vm, stmt->data.assign.operand1);
            double result = val1;
            
            if (stmt->data.assign.op != OP_NONE) {
                double val2 = eval_operand(vm, stmt->data.assign.operand2);
                switch(stmt->data.assign.op) {
                    case OP_ADD: result = val1 + val2; break;
                    case OP_SUB: result = val1 - val2; break;
                    case OP_MUL: result = val1 * val2; break;
                    case OP_DIV: 
                        if (val2 == 0) {
                            fprintf(stderr, "VM Runtime Error at line %d: Division by zero\n", stmt->line_num);
                            return;
                        }
                        result = val1 / val2; 
                        break;
                    default: break;
                }
            }
            
            vm->registers[stmt->data.assign.dest_reg] = result;
            vm->instruction_pointer++;
            
        } else if (stmt->type == STMT_CONTROL) {
            double val1 = eval_operand(vm, stmt->data.control.operand1);
            double val2 = eval_operand(vm, stmt->data.control.operand2);
            bool condition = false;
            
            // Use an epsilon tolerance for floating-point equality.
            const double EPSILON = 1e-9;
            switch (stmt->data.control.rel_op) {
                case OP_GT:  condition = val1 > val2; break;
                case OP_LT:  condition = val1 < val2; break;
                case OP_EQ:  condition = (val1 - val2) < EPSILON && (val2 - val1) < EPSILON; break;
                case OP_GTE: condition = val1 >= val2; break;
                case OP_LTE: condition = val1 <= val2; break;
                default: break;
            }
            
            if (condition) {
                // Use the pre-computed jump target (O(1) lookup, set at parse time).
                int target_ip = stmt->data.control.target_stmt_idx;
                if (target_ip < 0) {
                    // Target line beyond program — treat as halt.
                    vm->instruction_pointer = (int)vm->program_size;
                } else {
                    vm->instruction_pointer = target_ip;
                }
            } else {
                vm->instruction_pointer++;
            }
        } else {
            vm->instruction_pointer++;
        }
    }
}

// ---------------------------------------------------------
// Main Entry
// ---------------------------------------------------------
#ifndef TEST_BUILD
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <program.uac>\n", argv[0]);
        return 1;
    }
    
    FILE* file = fopen(argv[1], "rb");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }
    
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* source = malloc(length + 1);
    if (source) {
        fread(source, 1, length, file);
        source[length] = '\0';
    }
    fclose(file);
    
    if (!source) {
        fprintf(stderr, "Failed to allocate memory for source\n");
        return 1;
    }
    
    Token* tokens = NULL;
    size_t token_count = 0;
    if (!tokenize(source, &tokens, &token_count)) {
        free(source);
        if (tokens) free(tokens);
        return 1;
    }
    
    Statement* program = NULL;
    size_t stmt_count = 0;
    if (!parse(tokens, token_count, &program, &stmt_count)) {
        free(source);
        free(tokens);
        return 1;
    }
    
    printf("--- UnAutoCode Engine v2.0 Booted ---\n");
    printf("Executing script: %s\n\n", argv[1]);
    
    VirtualMachine vm;
    vm_init(&vm, program, stmt_count);
    
    // vm_execute runs its own internal loop until the program terminates.
    vm_execute(&vm);
    
    printf("--- Execution Finished. Active Registers ---\n");
    for (int i = 0; i < NUM_REGISTERS; i++) {
        if (vm.registers[i] != 0.0) {
            printf("c%d = %.2f\n", i, vm.registers[i]);
        }
    }
    
    free(program);
    free(tokens);
    free(source);
    
    return 0;
}
#endif
