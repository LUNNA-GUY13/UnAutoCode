#ifndef UNAUTOCODE_H
#define UNAUTOCODE_H

#include <stddef.h>
#include <stdbool.h>

#define MAX_PROGRAM_LINES 1000
#define NUM_REGISTERS 26

typedef enum {
    TOK_REGISTER,
    TOK_NUMBER,
    TOK_OP_ADD,
    TOK_OP_SUB,
    TOK_OP_MUL,
    TOK_OP_DIV,
    TOK_OP_GT,
    TOK_OP_LT,
    TOK_OP_EQ,
    TOK_OP_GTE,
    TOK_OP_LTE,
    TOK_CONTROL,
    TOK_IF,
    TOK_ASSIGN, // =
    TOK_NEWLINE,
    TOK_EOF
} TokenType;

typedef struct {
    TokenType type;
    double value;   // For NUMBER (the actual number) or REGISTER (the index 0-25) or line_number
    int line_num;   // 1-indexed source file line number where this token appears
} Token;

typedef enum {
    OP_NONE,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_GT,
    OP_LT,
    OP_EQ,
    OP_GTE,
    OP_LTE
} OperatorType;

typedef struct {
    bool is_register; // true if register, false if number literal
    double value;     // Register index (0-25) or double literal
} Operand;

typedef enum {
    STMT_EMPTY,
    STMT_ASSIGN,
    STMT_CONTROL
} StatementType;

typedef struct {
    StatementType type;
    int line_num; // The 1-indexed line number in the source file
    union {
        struct {
            int dest_reg; // 0-25
            Operand operand1;
            OperatorType op; // OP_NONE if just simple assignment
            Operand operand2; // Ignored if op == OP_NONE
        } assign;
        struct {
            int target_line;     // The 1-indexed source line number to jump to (as written in source)
            int target_stmt_idx; // Pre-computed statement array index (-1 = halt/end-of-program)
            Operand operand1;
            OperatorType rel_op;
            Operand operand2;
        } control;
    } data;
} Statement;

typedef struct {
    double registers[NUM_REGISTERS];
    int instruction_pointer;
    Statement* program;
    size_t program_size;
} VirtualMachine;

// Prototypes
bool tokenize(const char* source, Token** out_tokens, size_t* out_count);
bool parse(Token* tokens, size_t token_count, Statement** out_program, size_t* out_stmt_count);
void vm_init(VirtualMachine* vm, Statement* program, size_t program_size);
void vm_execute(VirtualMachine* vm);

#endif // UNAUTOCODE_H
