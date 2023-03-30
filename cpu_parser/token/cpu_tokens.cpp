#include "cpu_tokens.h"
#include "cpu_tokens_macroses.h"
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>


void init_set (struct set_of_tokens* set, size_t size){
    single_token* tokens_from_string = (single_token*)calloc(size, sizeof(single_token));

    set->set_of_tokens = tokens_from_string;
    set->size_of_set   = 0;

    return;
}

void delete_set (struct set_of_tokens* set){
    set->size_of_set = 0;
    free(set->set_of_tokens);

    set->set_of_tokens = NULL;
    return;
}

void skip_spaces(struct parse_ptr* ptr){
    while (*(ptr->offset) == ' ' && *(ptr->offset) != '\0'){ //then we are inside the string
        (ptr->offset) ++;
    }
}

bool add_command_token(struct parse_ptr* ptr, struct set_of_tokens* set);
bool add_register_token(struct parse_ptr* ptr, struct set_of_tokens* set);
bool add_number_token(struct parse_ptr* ptr, struct set_of_tokens* set);
bool add_number_token(struct parse_ptr* ptr, struct set_of_tokens* set);
bool add_mark_token(struct parse_ptr* ptr, struct set_of_tokens* set);
bool add_sign_token(struct parse_ptr* ptr, struct set_of_tokens* set);
bool add_bracket_token(struct parse_ptr* ptr, struct set_of_tokens* set);
bool add_comment_token(struct parse_ptr* ptr, struct set_of_tokens* set);





single_token create_tokens_from_string(struct parse_ptr* ptr, struct set_of_tokens* set) { //in case of error, it returns
                                                                                              //single token with required info to display
    while (*(ptr->offset) != '\0'){
        skip_spaces(ptr);

        // i added 'breaks' to make my code more readable
        if      (add_command_token (ptr, set))
            continue;
        else if (add_register_token(ptr, set))
            continue;
        else if (add_number_token  (ptr, set))
            continue;
        else if (add_mark_token    (ptr, set))
            continue;
        else if (add_bracket_token (ptr, set))
            continue;
        else if (add_sign_token    (ptr, set))
            continue;
        else if (add_comment_token (ptr, set)) //comment can be only after expression
            break;
        else if (*(ptr->offset) != '\0')
            BAD_TOKEN(ptr);  //aka unknown symbol was met
    }
    SUCCESS_TOKEN(); //in case if everything worked out fine
}


bool add_command_token(struct parse_ptr* ptr, struct set_of_tokens* set){
    char command[100] = {};

    int start_command = 0;
    int end_command   = 0;

    sscanf(ptr->offset, "%n%[^# ]%n", &start_command, command, &end_command);
    int command_size = end_command - start_command;

    enum command_list command_code = find_command(command);

    if (command_code == UNKNOWN_COMMAND)
        return false;
    else
    {
        ADD_COMMAND_TOKEN(set, command_code);

        (ptr->offset) += command_size;
        return true;
    }
}

bool add_register_token(struct parse_ptr* ptr, struct set_of_tokens* set){
    char register_name[4] = "";

    for (int i = 0; i < 3; i ++){
         if (*(ptr->offset) != '\0') // check if we inside
            register_name[i] = (ptr->offset)[i];
    }

    //code gen
    enum register_list register_code = find_register(register_name);

    if (register_code == UNKNOWN_REGISTER)
        return false;
    else {
        ADD_REGISTER_TOKEN(set, register_code);

        (ptr->offset) += 3; //move it for register size
        return true;
    }
}

bool add_number_token(struct parse_ptr* ptr, struct set_of_tokens* set){
    char* old_ptr = ptr->offset;
    int val = 0;

    while ('0' <= *(ptr->offset) && *(ptr->offset) <= '9') {
        val = val*10 + *(ptr->offset) - '0';
        (ptr->offset) ++;
    }

    if (old_ptr == (ptr->offset))
        return false;
    else {
        ADD_NUMBER_TOKEN(set, val);

        return true;
    }
}

bool add_mark_token(struct parse_ptr* ptr, struct set_of_tokens* set){
    if ((*(ptr->offset)) == ':'){
        (ptr->offset) ++;
        skip_spaces(ptr);

        char mark[100] = "";
        int start_mark = 0;
        int end_mark   = 0;

        sscanf(ptr->offset, "%n%[^# ]%n", &start_mark, mark, &end_mark);
        int mark_size = end_mark - start_mark;

        if (mark_size > MAX_MARK_SIZE)
            return false;

        if (mark_size == 0)
            return false; //empty mark - syntax error
        else {
            ADD_MARK_TOKEN(set, mark);

            (ptr->offset) += mark_size;
            return true;
        }
    }
    else
        return false; //if there was not found ':' sign at all
}

bool add_sign_token(struct parse_ptr* ptr, struct set_of_tokens* set){
    switch (*(ptr->offset)){
        case '+': {
            ADD_SIGN_TOKEN(set, PLUS);

            (ptr->offset) ++;
            return true;
        }
        break;

        case '-': {
            ADD_SIGN_TOKEN(set, MINUS);

            (ptr->offset) ++;
            return true;
        }

        break;

        case '*': {
            ADD_SIGN_TOKEN(set, MULTIPLY);

            (ptr->offset) ++;
            return true;
        }
        break;

        case '/': {
            ADD_SIGN_TOKEN(set, DIVISION);

            (ptr->offset) ++;
            return true;
        }

        case '\n': {
            ADD_SIGN_TOKEN(set, NEW_LINE);

            (ptr->offset) ++;
            return true;
        }
        break;

        default:
            return false;
        break;
    }

    return false;
}

bool add_bracket_token(struct parse_ptr* ptr, struct set_of_tokens* set){
    switch (*(ptr->offset)){
        case '(': {
            ADD_BRAKET_TOKEN(set, OPEN_ROUND_BRACKET);

            (ptr->offset) ++;
            return true;
        }
        break;

        case ')': {
            ADD_BRAKET_TOKEN(set, CLOSE_ROUND_BRACKET);

            (ptr->offset) ++;
            return true;
        }
        break;

        case '[': {
            ADD_BRAKET_TOKEN(set, OPEN_SQUARE_BRACKET);

            (ptr->offset) ++;
            return true;
        }
        break;

        case ']': {
            ADD_BRAKET_TOKEN(set, CLOSE_SQUARE_BRACKET);

            (ptr->offset) ++;
            return true;
        }
        break;

        default:
            return false;
        break;
    }

    return false;
}

bool add_comment_token(struct parse_ptr* ptr, struct set_of_tokens* set){
    if (*(ptr->offset) == '#') {
        ADD_COMMENT_TOKEN(set);

        (ptr->offset) ++;
        return true;
    }
    return false;
}

void print_tokens(struct set_of_tokens* set){
    for (int i = 0; i < set->size_of_set; i ++){
        switch (set->set_of_tokens[i].tag){
            case COMMAND:
                printf("COMMAND %d\n", set->set_of_tokens[i].command_code);
            break;
            case MARK:
                printf("MARK: [%s]\n", set->set_of_tokens[i].mark);
            break;
            case REGISTER:
                printf("REGISTER: %d\n", set->set_of_tokens[i].register_code);
            break;
            case NUMBER:
                printf("NUMBER: %d\n", set->set_of_tokens[i].i_value);
            break;
            case OPEN_SQUARE_BRACKET:
                printf("OPEN_SQUARE_BRAKET\n");
            break;
            case CLOSE_SQUARE_BRACKET:
                printf("CLOSE_SQUARE_BRACKET\n");
            break;
            case OPEN_ROUND_BRACKET:
                printf("OPEN_ROUND_BRACKET\n");
            break;
            case CLOSE_ROUND_BRACKET:
                printf("CLOSE_ROUND_BRACKET\n");
            break;
            case COMMENT:
                printf("COMMENT\n");
            break;
            case NEW_LINE:
                printf ("NEW_LINE\n");
            break;
            case ARITHMETIC_SIGN:
                printf ("ARITHMETIC_SIGN\n");
            break;
        }
    }
}

//i'll come up  with code generating inserts that will ease new commands addition
enum command_list find_command(char* command){
    if (!strcmp(command, "push"))
        return PUSH;
    else
        return UNKNOWN_COMMAND;
}

//same as above
enum register_list find_register(const char* reg_name){
    if (!strcmp (reg_name, "rax"))
        return RAX;
    else if (!strcmp(reg_name, "rbx"))
        return RBX;
    else if (!strcmp(reg_name, "rcx"))
        return RCX;
    else if (!strcmp(reg_name, "rdx"))
        return RDX;
    else
        return UNKNOWN_REGISTER;
}