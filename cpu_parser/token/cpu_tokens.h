#pragma once
#include <stdio.h>

static const int MAX_MARK_SIZE = 10;

enum command_tags{
    COMMAND              = 0,
    MARK                 = 1,
    REGISTER             = 2,
    NUMBER               = 3,
    OPEN_SQUARE_BRACKET  = 4,
    CLOSE_SQUARE_BRACKET = 5,
    OPEN_ROUND_BRACKET   = 6,
    CLOSE_ROUND_BRACKET  = 7,
    COMMENT              = 8,
    NEW_LINE             = 9, 
    ARITHMETIC_SIGN      = 10,
    NOT_A_COMMAND        = 11,
    PARSE_ERROR          = 12,
    PARSE_SUCCESS        = 13
};

enum command_list {
    HLT     = 0,
    PUSH    = 1,
    ADD     = 2,
    SUB     = 3,
    MUL     = 4,
    DIV     = 5,
    OUT     = 6,
    IN      = 7,
    JUMP    = 8,
    JMP_A   = 9,
    JMP_B   = 10,
    JMP_AE  = 11,
    JMP_BE  = 12,
    CALL    = 13, 
    POP     = 14,
    UNKNOWN_COMMAND = 15,
};

enum arithmetic_sign
{
    PLUS  = 0,
    MINUS = 1,
    MULTIPLY = 2,
    DIVISION = 3
};

enum register_list
{
    RAX = 0,
    RBX = 1,
    RCX = 2,
    RDX = 3,
    UNKNOWN_REGISTER = 4
};

typedef struct _token {
    enum command_tags  tag;
    enum command_list  command_code;
    enum register_list register_code; // rax - 0, rbx - 1, rcx - 2, rdx - 3
    int  arithmetic_code;             // '+' - 0, '-' - 1, '*' - 2, '/' - 3
    int  i_value;       //value if its number
    int  in_line_pos;   // position in the source text line
    char mark[MAX_MARK_SIZE] = ""; //mark name
}single_token;

struct set_of_tokens {
    single_token* set_of_tokens;
    int size_of_set;
};


struct parse_ptr {
    char* offset;
    char* origin;
};

void init_set    (struct set_of_tokens* set, size_t size);
void delete_set  (struct set_of_tokens* set);

single_token create_tokens_from_string(struct parse_ptr* ptr, struct set_of_tokens* set);

enum command_list  find_command(char* command);
enum register_list find_register(const char* reg_name);

void print_tokens(struct set_of_tokens* set);
