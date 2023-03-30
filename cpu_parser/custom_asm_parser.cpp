#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

//TO DO: 
// write tokenizator
// put all errors in enum
// write simple descriptor for them


//!!!ADDITION!!!
//IN ADDITION - SHOW IN CONSOLE PARTICULAR PLACE, WHERE ERROR OCCURED
//COME UP WITH AN IDEA HOW TO DISPLAY ALL ERRORS OCCURED

const int REGISTER_FOUND = -1;

typedef struct _argument{
    int ram_flag;
    int register_flag;
    int register_num;
    int i_value;
}command_arg;

typedef struct _parsing {
    command_arg argument;
    int         command_code;
} parse_result;

void get_register_name(char* source, char* register_name){
    for (int i = 0; i < 3; i ++) {
        if (source[i] != '\0')
            register_name[i] = source[i];
        else
            break;
    }
}

int is_register_valid(char* register_name){

    if (!strncmp(register_name, "rax", 3))
        return 0;

    else if (!strncmp(register_name, "rbx", 3))
        return 1;
    
    else if (!strncmp(register_name, "rcx", 3))
        return 2;
    
    else if (!strncmp(register_name, "rdx", 3))
        return 3;

    else 
        return -1; //register name is not valid
}



int get_file_size (FILE* file) {
    fseek(file, SEEK_SET, SEEK_END);
    int file_size = ftell(file);
    printf ("file size is %d\n", file_size);
    fseek(file, SEEK_SET, SEEK_SET);

    return file_size;
}

//this will be replaced with tokenizator implementation
void remove_spaces (char* expression) {
    assert (expression != NULL);

    int pos = 0;
    int pos_to_move = 0;

    while (expression[pos] != '\0')
    {
        if (expression[pos] == ' ')
        {
            while (expression[pos] == ' ')
                pos++;
        }
        else
        {
            expression[pos_to_move] = expression[pos];
            pos_to_move ++;
            pos ++;
        }
    }
    expression[pos_to_move] = '\0';
    
    return;
}

void parse_the_string(char*  string, parse_result* result);
void get_command_code(char** command_ptr, parse_result* result);

int get_E(char** instr_ptr, parse_result* result);
int get_T(char** instr_ptr, parse_result* result);
int get_P (char** instr_ptr, parse_result* result);
int get_N(char** instr_ptr, parse_result* result);

int main () {
    FILE* source_file = fopen("source_file.txt", "r");
    assert (source_file != NULL);

    int file_size = get_file_size(source_file);

    char test_string[100] = "";
    fread(test_string, sizeof(char), file_size, source_file);

    parse_result result = {};
    result.argument.i_value      = 0;
    result.argument.ram_flag     = 0;
    result.argument.register_num = -1;

    parse_the_string(test_string, &result);

    fclose(source_file);
}

void parse_the_string(char* string, parse_result* result){
    assert(string != NULL);
    assert(result != NULL);

    get_command_code(&string, result);

    remove_spaces(string);
    printf ("!DEBUG! line to parse: [˜˜˜%s˜˜˜]\n", string);

    int result_to_add = 0;

    char* instr_ptr = string;
    if (*instr_ptr == '[') {
        result->argument.ram_flag = 1;
        instr_ptr ++;

        result_to_add = get_E(&instr_ptr, result);

        assert (*instr_ptr == ']');
        instr_ptr ++;
    }
    else {
        result_to_add = get_E(&instr_ptr, result);
    }

    assert(*instr_ptr == '\0');

    result->argument.i_value += result_to_add;

    printf("reg_num:  %d\n", result->argument.register_num);
    printf("i_value:  %d\n", result->argument.i_value);
    printf("ram_flag: %d\n", result->argument.ram_flag);
}

void get_command_code(char** command_ptr, parse_result* result){ 
    assert(command_ptr != NULL);
    assert(result      != NULL);

    char command[10] = "";

    int start = 0;
    int end = 0;

    sscanf(*command_ptr, "%n%s%n", &start, command, &end);
    int command_size = end - start;

    printf ("!DEBUG! command: %s\n", command);

    result->command_code = 777;
    (*command_ptr) += command_size;

    return;
}

int get_E(char** instr_ptr, parse_result* result){
    //(rax + 10)*2,  regiter can not be inside brackets
    //rax*2 or rax/2 register can not be an argument of devision/multiplication
    //they can only stay apart and be only one in the line
    int val = 0;
    val = get_T(instr_ptr, result);

    if (val == REGISTER_FOUND) //it means that register can be wraped with expressions around
        val = 0;

    while (**instr_ptr == '+' || **instr_ptr == '-') { 
        char operation = **instr_ptr;
        (*instr_ptr) ++;

    
        int val2 = get_T(instr_ptr, result);

        if (operation == '-' && val2 == REGISTER_FOUND)
            assert (false && "'-' SIGN BEFORE REGISTER NAME");

        if (operation == '+')
            val += val2;
        else 
            val -= val2;
    }

    assert(result->argument.register_flag <=1 && "MULTIPLE REGISTERS ARE USED");

    return val;
}

int get_T(char** instr_ptr, parse_result* result){
    assert (instr_ptr != NULL);
    int val = get_P(instr_ptr, result);


    while (**instr_ptr == '*' || **instr_ptr == '/')
    {
        if (val == REGISTER_FOUND)
            assert(false && "REGISTERS CAN NOT BE MULTIPLIED ON ANYTHING");

        char op = **instr_ptr;
        (*instr_ptr) ++;

        int val2 = get_P(instr_ptr, result);

        if (val2 == REGISTER_FOUND)
            assert(false && "REGISTERS CAN NOT BE MULTIPLIED ON ANYTHING");

        if (op == '*')
            val *= val2;
        else 
            val /= val2;
    }

    return val;
}


int get_P (char** instr_ptr, parse_result* result) {
    int val = 0;
    if (**instr_ptr == '(')
    {
        (*instr_ptr) ++;

        int register_flag_before = result->argument.register_flag;
            val = get_E(instr_ptr, result);
        int register_flag_after = result->argument.register_flag;


        if (register_flag_after - register_flag_before)
            assert(false && "REGISTER INSIDE ROUND BRACKETS");

        assert (**instr_ptr == ')');
        (*instr_ptr) ++;
    }
    else {
        val = get_N(instr_ptr, result);
    }

    return val;
}

int get_N(char** instr_ptr, parse_result* result){
    char* old_ptr = *instr_ptr;

    if (*old_ptr == 'r') {
        char register_name[3] = "";

        get_register_name(old_ptr, register_name); //while getting a register, i does not iterate further
        int register_num = is_register_valid(register_name);

        if (register_num >= 0){
            result->argument.register_flag += 1; //only one register in expression is possible
            result->argument.register_num  = register_num;

            (*instr_ptr) += 3; // moving iterator for register name length

            return REGISTER_FOUND;
        }

        else 
            assert (false && "SYNTAX ERROR, INCORRECT REGISTER NAME"); //it is only for now
    }

    //taking a num from the string
    int val = 0;
    while ('0' <= **instr_ptr && **instr_ptr <= '9')
    {
        val = val*10 + **instr_ptr - '0';
        (*instr_ptr) ++;
    }

    assert (old_ptr != *instr_ptr);

    return val;
}