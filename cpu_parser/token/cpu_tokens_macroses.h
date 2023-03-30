#include <stdio.h>
#include "cpu_tokens.h"


#define POSITION(ptr) (int)((ptr->offset) - (ptr->origin))

#define ADD_REGISTER_TOKEN(set, code)                            \
do{                                                              \
    single_token token  = {};                                    \
    token.tag           = REGISTER;                              \
    token.register_code = code;                                  \
    token.in_line_pos   = POSITION(ptr);                         \
                                                                 \
    set->set_of_tokens[set->size_of_set] = token;                \
    set->size_of_set ++;                                         \
}while(0)                                                        \

#define ADD_COMMAND_TOKEN(set, code)                             \
do{                                                              \
    single_token token = {};                                     \
    token.tag          = COMMAND;                                \
    token.command_code = code;                                   \
    token.in_line_pos  = POSITION(ptr);                          \
                                                                 \
    set->set_of_tokens[set->size_of_set] = token;                \
    set->size_of_set ++;                                         \
}while(0)                                                        \

#define ADD_COMMENT_TOKEN(set)                                   \
do{                                                              \
    single_token token = {};                                     \
    token.tag          = COMMENT;                                \
    token.in_line_pos  = POSITION(ptr);                          \
                                                                 \
    set->set_of_tokens[set->size_of_set] = token;                \
    set->size_of_set ++;                                         \
}while(0)                                                        \

#define ADD_NUMBER_TOKEN(set, val)                               \
do{                                                              \
    single_token token = {};                                     \
    token.tag          = NUMBER;                                 \
    token.i_value      = val;                                    \
    token.in_line_pos  = POSITION(ptr);                          \
                                                                 \
    set->set_of_tokens[set->size_of_set] = token;                \
    set->size_of_set ++;                                         \
}while(0)                                                        \

#define ADD_BRAKET_TOKEN(set, code)                              \
do{                                                              \
    single_token token    = {};                                  \
    token.tag             = code;                                \
    token.in_line_pos     = POSITION(ptr);                       \
                                                                 \
    set->set_of_tokens[set->size_of_set] = token;                \
    set->size_of_set ++;                                         \
}while(0)                                                        \

#define ADD_MARK_TOKEN(set, mark_name)                           \
do{                                                              \
    single_token token = {};                                     \
    token.tag          = MARK;                                   \
    strncpy(token.mark, mark_name, MAX_MARK_SIZE);               \
    token.in_line_pos = POSITION(ptr);                           \
                                                                 \
    set->set_of_tokens[set->size_of_set] = token;                \
    set->size_of_set ++;                                         \
}while(0)                                                        \

#define ADD_SIGN_TOKEN(set, code)                                \
do{                                                              \
    single_token token    = {};                                  \
    token.tag             = ARITHMETIC_SIGN;                     \
    token.arithmetic_code = code;                                \
    token.in_line_pos     = POSITION(ptr);                       \
                                                                 \
    set->set_of_tokens[set->size_of_set] = token;                \
    set->size_of_set ++;                                         \
}while(0)                                                        \


#define BAD_TOKEN(ptr)                                           \
do{                                                              \
single_token bad_token = {};                                     \
                                                                 \
bad_token.tag         = PARSE_ERROR;                             \
bad_token.in_line_pos = (int)((ptr->offset) - (ptr->origin));    \
                                                                 \
return bad_token;                                                \
}                                                                \
while(0)

#define SUCCESS_TOKEN()                          \
do {                                             \
single_token success_token = {};                 \
success_token.tag = PARSE_SUCCESS;               \
                                                 \
return success_token;                            \
}                                                \
while(0)