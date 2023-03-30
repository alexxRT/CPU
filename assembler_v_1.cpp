#include <stdio.h>
#include <stdlib.h>
#include "TEXT.h"
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "Memory.h"


#define RESET   "\033[0m"
#define BLACK   "\033[30m"                 /* Black */
#define RED     "\033[31m"                 /* Red */
#define GREEN   "\033[32m"                 /* Green */
#define YELLOW  "\033[33m"                 /* Yellow */
#define BLUE    "\033[34m"                 /* Blue */
#define MAGENTA "\033[35m"                 /* Magenta */
#define CYAN    "\033[36m"                 /* Cyan */
#define WHITE   "\033[37m"                 /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

const char* SIGN = "LP";
const int RG_LENGTH = 3;
const int VERSION = 1;
const int HEADER_SIZE = 4;
const int MARKS_SIZE = 50;
const int SKIP_COMMAND = -1;
const int INVALID_JUMP = -1;

typedef struct _invariants
{
    short registr_counter;
    short arithmetic_flag;
    short argument_before;
    short open_bracket;
    short close_bracket;
    short open_bracket_flag;
} syntax_invariants;

typedef struct arg
{
    int args[10];
    int args_num;
}args_set;

// args size by default equales 10, first position goes for register # only, next 9 positions for variables 
// no JUMP or CALL on metka
// no metka for JUMP or CALL

void PrintErrorInfo           (int ErrCode, int code_line);
int CheckBracketsInvariants   (syntax_invariants* flags, args_set* argument);
int CheckArgumentInvariants   (syntax_invariants* flags);
int CheckArithmeticInvariants (syntax_invariants* flags);

#define ARE_BRACKETS_GOOD( flags, argumet, code_line )     \
int ErrCode =  CheckBracketsInvariants (flags, argument);  \
do                                                         \
{                                                          \
if (ErrCode)                                               \
{                                                          \
    PrintErrorInfo (ErrCode, code_line);                   \
    return ErrCode;                                        \
}                                                          \
}                                                          \
while (0)

#define IS_ARGUMENT_GOOD( flags, code_line )              \
int ErrCode = CheckArgumentInvariants (flags);            \
do                                                        \
{                                                         \
if (ErrCode)                                              \
{                                                         \
    PrintErrorInfo (ErrCode, code_line);                  \
    return ErrCode;                                       \
}                                                         \
}                                                         \
while (0)

#define IS_ARITHMETIC_GOOD( flags, code_line )            \
int ErrCode = CheckArithmeticInvariants (flags);          \
do                                                        \
{                                                         \
if (ErrCode)                                              \
{                                                         \
    PrintErrorInfo (ErrCode, code_line);                  \
    return ErrCode;                                       \
}                                                         \
}                                                         \
while (0)

#define ARE_MARKS_GOOD( jump_marks, instr )               \
int ErrCode = CheckJumpsInvariants (jump_marks, instr);   \
do                                                        \
{                                                         \
if (ErrCode)                                              \
{                                                         \
    PrintErrorInfo (ErrCode, code_line);                  \
    return ErrCode;                                       \
}                                                         \
}                                                         \
while (0)


typedef struct my_mark
{
    char* string_mark;
    int jump_pos; 
    int code_line;  // for PrintErrorInfo
} mark;

typedef struct _marks
{
    mark* marks_array;
    int position;
}marks;


enum COMMAND_LIST
{
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

enum REGISTR_LIST
{
    RAX = 0,
    RBX = 1,
    RCX = 2,
    RDX = 3,
    UNKNOWN_RG = -1
};

enum COMMAND_MASKS
{
    NUMBER  = 0b00010000,
    REGISTR = 0b00100000,
    RAM     = 0b01000000
};

enum SYNTAX_ERROR_LIST
{
    SYNTAX_ERROR       = -777,
    MISSED_PLUS        = -1,
    PLUS_BEFORE        = -2,
    OUT_OF_BRACKETS    = -3,
    EMPTY_BRACKETS     = -4,
    INCORRECT_RG_NAME  = -5,
    TOO_MANY_REGISTRS  = -6,
    TOO_MANY_PLUSES    = -7,
    TOO_MANY_BRACKETS  = -8,
    NO_SECOND_ARGUMENT = -9,
    NO_CLOSING_BRACKET = -10,
    NO_OPENING_BRACKET = -11,
    NO_COLON_IN_JUMP   = -12,
    NO_COLON_IN_CALL   = -13,
    NO_ARGUMENTS       = -14,
    JUMP_SYNTAX_ERROR  = -15,
    ALL_RIGHT          = 0
};

int IsColon (const char* ptr)
{
    assert (ptr);
    return ((*ptr) == ':') ? 1 : 0;
}

int IsJump (int command_code)
{
    command_code &= 0b00001111;

    if (command_code == JUMP  ||  command_code == JMP_A  ||\
        command_code == JMP_B ||  command_code == JMP_BE ||\
        command_code == CALL  ||  command_code == JMP_AE   )
        return 1;

    return 0;
}

int ChangeColon (char* ptr)
{   
    assert (ptr);

    int i;
    for (i = 0; ptr[i] != ':'; i++);

    ptr[i] = '\0';

    return i;
}

void PrintErrorInfo (int ErrCode, int code_line)
{
    printf (BOLDRED ">>>LINE:%d " RESET, code_line + 1);

    switch (ErrCode)
    {
        case MISSED_PLUS:
            printf ("No plus between arguments, ");
            break;
        case PLUS_BEFORE:
            printf ("Plus alone infront of the argument, ");
            break;
        case OUT_OF_BRACKETS:
            printf ("Your expression is out of the brackets, ");
            break;
        case EMPTY_BRACKETS:
            printf ("You have empty brackets, ");
            break;
        case INCORRECT_RG_NAME:
            printf ("Check please registr name, ");
            break;
        case TOO_MANY_REGISTRS:
            printf ("Only one registris possible as an argument, ");
            break;
        case TOO_MANY_PLUSES:
            printf ("Too many pluses in the expression, ");
            break;
        case TOO_MANY_BRACKETS:
            printf ("Too many brackets in the expression, one pare is only possible, ");
            break;
        case NO_SECOND_ARGUMENT:
            printf ("Check please second argument in the expression, ");
            break;
        case NO_CLOSING_BRACKET:
            printf ("No closing bracket in the expression, ");
            break;
        case NO_OPENING_BRACKET:
            printf ("Trying to close the bracket without an opening one, ");
            break; 
        case NO_COLON_IN_JUMP:
            printf ("No colon after JUMP, ");
            break;
        case NO_COLON_IN_CALL:
            printf ("No colon after CALL, ");
            break;
        case NO_ARGUMENTS:
            printf ("There are no arguments for this function, ");
            break;
        case JUMP_SYNTAX_ERROR:
            printf ("JUMP or CALL pointing to metka, which does not exist, ");
            break;
        default:
            printf ("Syntax error in the expresion, ");
            break;
    }

    printf ("check pls your source file\n");

    return;
}

int DefineRegistr (char* registr)
{
    assert (registr);

    if (!strcmp (registr, "rax"))
        return RAX;
    else if (!strcmp (registr, "rbx"))
        return RBX;
    else if (!strcmp (registr, "rcx"))
        return RCX;
    else if (!strcmp (registr, "rdx"))
        return RDX;
    else return UNKNOWN_RG;
}

int FindMark (marks* jump_marks, char* mark)
{
    assert (jump_marks);
    assert (mark);

    for (int i = 0; i < jump_marks->position; i ++)
    {
        int colon_place = ChangeColon (jump_marks->marks_array[i].string_mark);

        if (!strcmp (jump_marks->marks_array[i].string_mark, mark))
        {
            jump_marks->marks_array[i].string_mark[colon_place] = ':';
            return jump_marks->marks_array[i].jump_pos; 
        }

        jump_marks->marks_array[i].string_mark[colon_place] = ':';
    }

    return -1;
}

int CheckArgumentInvariants (syntax_invariants* flags)
{
    assert (flags);

    if (flags->open_bracket_flag && !flags->open_bracket)
        return OUT_OF_BRACKETS;

    if (flags->close_bracket)
        return OUT_OF_BRACKETS; // ONLY INSIDE BRACKETS

    if (flags->arithmetic_flag > 1) // "+" ONLY ONE PLUS IS POSIBLE
        return TOO_MANY_PLUSES;

    if (!flags->arithmetic_flag && flags->argument_before)
        return MISSED_PLUS; // CHECK NO "+" BETWEEN ARGS

    return ALL_RIGHT;
}

int CheckBracketsInvariants (syntax_invariants* flags, args_set* argument)
{
    assert (flags);
    assert (argument);

    if (flags->close_bracket > 1) // ONLY ONE CLOSE BRACKET IS POSSIBLE
        return TOO_MANY_BRACKETS;

    if (!flags->open_bracket)  //CLOSE ONLY IF OPENED
        return NO_OPENING_BRACKET;

    if (flags->open_bracket > 1) //ONLY ONE OPEN BRACKET IS POSSIBLE
        return TOO_MANY_BRACKETS;

    if (argument->args_num == 0 && flags->close_bracket) // EMPTY BRACKETS CASE
        return EMPTY_BRACKETS;

    return ALL_RIGHT;
}
int CheckArithmeticInvariants (syntax_invariants* flags)
{
    assert (flags);

    if (flags->close_bracket)
        return OUT_OF_BRACKETS; // ONLY INSIDE BRACKETS

    if (flags->open_bracket_flag && !flags->open_bracket)
        return OUT_OF_BRACKETS; // LEFT SIDE FROM THE BRACKETS CASE

    if (!flags->argument_before)
        return PLUS_BEFORE; // "+" STAYS AFTER ARGUMENT ONLY

    if (flags->arithmetic_flag > 1) // "+" ONLY ONE PLUS IS POSIBLE
        return TOO_MANY_PLUSES;

    return ALL_RIGHT;
}

int CheckJumpsInvariants (marks* jump_marks, char* buffer)
{
    assert (jump_marks);
    assert (buffer);

    int* instructions = (int*)buffer;

    for (int i = 1; i < buffer[3]; i ++)
    {
        if (instructions[i] == INVALID_JUMP)
        {
            PrintErrorInfo (JUMP_SYNTAX_ERROR, -1); // can't provide code line for this error
            return JUMP_SYNTAX_ERROR;
        }
    }

    return ALL_RIGHT;
}

char* SkipSpaces (const char* str)
{
    assert (str != NULL);
    int i = 0;

    for (; str[i] != '\0'; i++)
    {
        if (str[i] != ' ')
        {
            assert (str [i] != ' ');
            return (char*)(str + i);
        }
    }
    
    return (char*)(str + i);
}

char* SkipCommand (char* str)
{
    assert (str != NULL);
    int i = 0;

    while (str[i] != ' ' && str[i] != '\0')
        i ++;

    return (char*)str + i;
}

int GetArgument (char* buffer, int* command, int code_line, args_set* argument)
{
    //TO DO: put flags in structure and write macroces to check their values done:)
    //--------------------------------------------------------------------//

    assert (buffer);
    assert (command);
    assert (argument);
    assert (code_line >= 0);

    int i = 0;
    syntax_invariants flags = {};

    while (buffer[i] != '\0')
    {
        if (buffer[i] == '[')
        {
            flags.open_bracket_flag = 1;
            break;
        }
        i ++;
    }

    i = 0;
    int start = 0;
    int end = 0;
    int position = 1; // position is for the args array

    while (buffer [i] != '\0')
    {
        while (buffer[i] == ' ' && buffer[i] != '\0')
            i ++; // to skip spaces, i think re-do SkipSpaces is neccessary

        if (buffer[i] == '[')
        {
            flags.open_bracket += 1;
            ARE_BRACKETS_GOOD (&flags, argument, code_line);

            (*command) |= 0b01000000;
            i++;
        }

        else if (buffer[i] == ']')
        { 
            flags.close_bracket ++;
            ARE_BRACKETS_GOOD (&flags, argument, code_line);

            flags.open_bracket --; // CLOSE OPENED BRACKET

            i ++;
        }

        else if (buffer[i] == '+')
        {
            IS_ARITHMETIC_GOOD (&flags, code_line);

            flags.arithmetic_flag++;
            i ++;
        }

        else if (sscanf (buffer + i, "%n%d%n", &start, argument->args + position, &end) == 1)
        {
            IS_ARGUMENT_GOOD (&flags, code_line);

            (*command) |= 0b00010000;

            int num_length = end - start;

            argument->args_num ++;
            position ++;
            i += num_length;

            if (flags.arithmetic_flag == 0)
                flags.argument_before = 1;
            else
                flags.arithmetic_flag --;
        }

        else if (buffer[i] == 'r')
        {
            IS_ARGUMENT_GOOD (&flags, code_line);

            if (flags.registr_counter > 0)
            { 
                PrintErrorInfo (ErrCode, code_line);
                return TOO_MANY_REGISTRS;
            } //ONLY ONE REGISTER POSIBLE
                

            (*command) |= 0b00100000;
            
            char registr_name[10] = "";
            sscanf (buffer + i, "%3s", registr_name);

            int registr_num = DefineRegistr (registr_name);
            
            switch (registr_num)
            {
                case RAX:
                    argument->args[0] = RAX;
                    argument->args_num ++;
                    flags.registr_counter ++;
                    i += 3;

                    if (flags.arithmetic_flag == 0)
                        flags.argument_before = 1;
                    else
                        flags.arithmetic_flag --;
                    break;

                case RBX:
                    argument->args[0] = RBX;
                    argument->args_num ++;
                    flags.registr_counter ++;
                    i += RG_LENGTH;

                    if (flags.arithmetic_flag == 0)
                        flags.argument_before = 1;
                    else
                        flags.arithmetic_flag --;
                    break;

                case RCX:
                    argument->args[0] = RCX;
                    argument->args_num ++;
                    flags.registr_counter ++;
                    i += RG_LENGTH;

                    if (flags.arithmetic_flag == 0)
                        flags.argument_before = 1;
                    else
                        flags.arithmetic_flag --;
                    break;

                case RDX:
                    argument->args[0] = RDX;
                    argument->args_num ++;
                    flags.registr_counter ++;
                    i += RG_LENGTH;

                    if (flags.arithmetic_flag == 0)
                        flags.argument_before = 1;
                    else
                        flags.arithmetic_flag --;
                    break;

                default:
                    argument->args[0] = UNKNOWN_RG;
                    PrintErrorInfo (INCORRECT_RG_NAME, code_line);
                    return INCORRECT_RG_NAME;
                    break;
            }
        }

        else if (buffer[i] == '/')
            break;

        else if (buffer[i] != ' ' && buffer[i] != '\0') //MET SOME SHIT
        {   
            
            PrintErrorInfo (SYNTAX_ERROR, code_line);
            return SYNTAX_ERROR;
        }
    }

    if (flags.open_bracket) // CHECK IF OPEN BRACKET CLOSED
    {
        PrintErrorInfo (NO_CLOSING_BRACKET, code_line);
        return NO_CLOSING_BRACKET; 
    }

    if (flags.arithmetic_flag) // CHECK IF ARITMETICK DONE
    {
        PrintErrorInfo (NO_SECOND_ARGUMENT, code_line);
        return NO_SECOND_ARGUMENT; 
    }

    if (argument->args_num == 0) // NO ARGUMENTS FOR FUMCTION
    {
        PrintErrorInfo (NO_ARGUMENTS, code_line);
        return NO_ARGUMENTS;
    }

    return ALL_RIGHT;
}

void WriteHexBytes (char* buffer, const int* elem, const int line, int* ptr_to_write)
{ 

/*
 *  This code is for explanation ONLY.
 *
 *  ((int*) buffer) [*ptr_to_write] = *elem;
 *  *ptr_to_write += sizeof (int);
 *
 *  REAL code goes below:
 */
    
    assert (buffer);
    assert (elem);
    assert (ptr_to_write);
    assert (*ptr_to_write >= 0);

    int position = *ptr_to_write;

    for (int i = 0; i < 4; i ++)
    { 
        sprintf (buffer + position, "%c", *((unsigned char*)elem + i));
        position ++;
    }

    buffer[position - 1] = (char)line;

    (*ptr_to_write) = position;

    return;
}
// @andreyid1

int CalculateArgument (int command_code, args_set* arg)
{
    assert (arg);
    assert (command_code >= 0);

    int args_num = arg->args_num;
    int sum = 0;

    if ((command_code | 0b00100000) == command_code) //REGISTER WAS FOUND
    {
        for (int i = 2; i < args_num; i ++)
        {
            sum += arg->args[i];
            arg->args_num --;
        }    
    }
    else 
    {
        for (int i = 2; i < args_num + 1; i ++)
        {
            sum += arg->args[i];
            arg->args_num --;
        }
    } 

    arg->args[1] += sum;
    return 0;
}

void WriteHexArguments (char* buffer, args_set* argument, int* instr_counter, int* ptr_to_write)
{
    assert (buffer);
    assert (argument);
    assert (instr_counter);
    assert (ptr_to_write);

    if (argument->args_num)
    {
        if (argument->args[0] == -1) // NO REGISTERS NUM IN ARGUMENT LIST
        {
            for (int i = 1; i < argument->args_num + 1; i ++)
            {
                WriteHexBytes (buffer, argument->args + i, 0, ptr_to_write);
                (*instr_counter) ++;
            }
        }
        else 
        {
            for (int i = 1; i < argument->args_num; i ++)
            {
                WriteHexBytes (buffer, argument->args + i, 0,  ptr_to_write);
                (*instr_counter) ++;
            }

            WriteHexBytes (buffer, argument->args, 0,  ptr_to_write); // write reg after number
            (*instr_counter) ++;
        }
    }
}

int GetCommandCode (str* line, marks* jump_marks, int code_line, int* instr_counter, args_set* argument)
{
    assert (line);
    assert (jump_marks);
    assert (instr_counter);
    assert (argument);

    char command[10] = "";
    char* buffer = line->string;

    buffer = SkipSpaces (buffer);
    sscanf (buffer, "%s", command);

    if (!strcmp (command, "PUSH"))
    {
        buffer = SkipCommand (buffer); 
        buffer = SkipSpaces  (buffer);

        int PUSH_CODE = PUSH;

        int result = GetArgument (buffer, &PUSH_CODE, code_line, argument); //multiple args are possible - num of registr and num itself

        if (result != 0)
            return SYNTAX_ERROR; //IN CASE IF GetArgument FAILED

        CalculateArgument (PUSH_CODE, argument);

        return PUSH_CODE;
    }
    else if (!strcmp (command, "JUMP"))
    {
        buffer = SkipCommand (buffer);
        buffer = SkipSpaces  (buffer);

        if (IsColon (buffer))
        {
            char* mark = buffer + 1; //compare here "start\0"

            argument->args[1] = FindMark (jump_marks, mark);
            argument->args_num ++;

            return JUMP | NUMBER;
        }

        else 
        {
            int JUMP_CODE = JUMP;

            int result = GetArgument (buffer, &JUMP_CODE, code_line, argument);

            if (result != 0)
                return SYNTAX_ERROR; //IN CASE IF GetArgument FAILED

            CalculateArgument (JUMP_CODE, argument);

            return JUMP_CODE;
        }
    }

    else if (!strcmp (command, "JMP_A"))
    {
        
        buffer = SkipCommand (buffer);
        buffer = SkipSpaces  (buffer);

        if (IsColon (buffer))
        {
            char* mark = buffer + 1;

            argument->args[1] = FindMark (jump_marks, mark);
            argument->args_num ++;

            return JMP_A | NUMBER;
        }
        else
        {
            int JUMP_CODE = JMP_A;

            int result = GetArgument (buffer, &JUMP_CODE, code_line, argument);

            if (result != 0)
                return SYNTAX_ERROR; 

            CalculateArgument (JUMP_CODE, argument);

            return JUMP_CODE;
        }
    }

    else if (!strcmp (command, "JMP_B"))
    {
        buffer = SkipCommand (buffer);
        buffer = SkipSpaces  (buffer);

        if (IsColon (buffer))
        {
            char* mark = buffer + 1; 

            argument->args[1] = FindMark (jump_marks, mark);
            argument->args_num ++;

            return JMP_B | NUMBER;
        }
        else 
        {
            int JUMP_CODE = JMP_B;

            int result = GetArgument (buffer, &JUMP_CODE, code_line, argument);

            if (result != 0)
                return SYNTAX_ERROR; 

            CalculateArgument (JUMP_CODE, argument);

            return JUMP_CODE;
        }
    }

    else if (!strcmp (command, "JMP_AE"))
    {
        buffer = SkipCommand (buffer);
        buffer = SkipSpaces  (buffer);

        if (IsColon (buffer))
        {
            char* mark = buffer + 1; 

            argument->args[1] = FindMark (jump_marks, mark);
            argument->args_num ++;

            return JMP_AE | NUMBER;
        }
        else 
        {
            int JUMP_CODE = JMP_AE;

            int result = GetArgument (buffer, &JUMP_CODE, code_line, argument);

            if (result != 0)
                return SYNTAX_ERROR; 

            CalculateArgument (JUMP_CODE, argument);

            return JUMP_CODE;
        }
    }

    else if (!strcmp (command, "JMP_BE"))
    {
        buffer = SkipCommand (buffer);
        buffer = SkipSpaces  (buffer);

        if (IsColon (buffer))
        {
            char* mark = buffer + 1; 

            argument->args[1] = FindMark (jump_marks, mark);
            argument->args_num ++;

            return JMP_BE | NUMBER;
        }

        else 
        {
            int JUMP_CODE = JMP_BE;

            int result = GetArgument (buffer, &JUMP_CODE, code_line, argument);

            if (result != 0)
                return SYNTAX_ERROR; 

            CalculateArgument (JUMP_CODE, argument);

            return JUMP_CODE;
        }
    }

    else if (IsColon (buffer + line->len - 1)) // meet :metka
    {
        char* read_mark = buffer; // but here i read "start:\0"

        int exist_stat = FindMark (jump_marks, read_mark);

        if (exist_stat == -1)
        {
            int position = jump_marks->position;

            mark new_mark = {}; 

            new_mark.string_mark = read_mark;
            new_mark.jump_pos = *instr_counter;

            jump_marks->marks_array [position] = new_mark;
            jump_marks->position++;
        }
        //else second metka with same name used! ERROR

        return SKIP_COMMAND;
    }

    else if (!strcmp (command, "CALL"))
    {
        buffer = SkipCommand (buffer);
        buffer = SkipSpaces  (buffer);

        if (IsColon (buffer))
        {
            char* mark = buffer + 1; 

            argument->args[1] = FindMark (jump_marks, mark);   
            argument->args_num ++;

            return CALL;
        }

        else 
        {
            PrintErrorInfo (NO_COLON_IN_CALL, code_line);
            return NO_COLON_IN_CALL;
        }
    }
    else if (!strcmp (command, "POP"))
    {
        buffer = SkipCommand (buffer);
        buffer = SkipSpaces  (buffer);

        int POP_CODE = POP;

        int result = GetArgument (buffer, &POP_CODE, code_line, argument);

        if (result)
            return SYNTAX_ERROR;
        
        if ((POP_CODE | RAM) != POP_CODE) // if pop has not ram brackets
        {
            if (POP_CODE & NUMBER) // there number without square brackets
            {
                PrintErrorInfo (SYNTAX_ERROR, code_line);
                return SYNTAX_ERROR;
            }
        }

        CalculateArgument (POP_CODE, argument);
        
        return POP_CODE;
    }

    else if (!strcmp (command, "ADD"))
        return ADD;
    
    else if (!strcmp (command, "SUB"))
        return SUB;
    
    else if (!strcmp (command, "MUL"))
        return MUL;
    
    else if (!strcmp (command, "DIV"))
        return DIV;

    else if (!strcmp (command, "HLT"))
        return HLT;

    else if (!strcmp (command, "IN"))
        return IN;

    else if (!strcmp (command, "OUT"))
        return OUT;
    
    else if (buffer[0] == '\0') //EMPTY STRING CASE
        return SKIP_COMMAND;
    
    else if (buffer[0] == '/') //COMMENT CASE
        return SKIP_COMMAND;

    else 
    {
        PrintErrorInfo (SYNTAX_ERROR, code_line);
        return SYNTAX_ERROR;
    }
}

int WriteCommands (char* buffer, TEXT* lines, marks* jump_marks, int* ptr_to_write, int* instr_counter)
{
    assert (buffer);
    assert (lines);
    assert (jump_marks);
    assert (ptr_to_write);
    assert (instr_counter);

    printf ("num of strings in code is %lu\n", lines->num_of_str);

    for (int i = 0; i < lines->num_of_str; i ++)
    {
        args_set argument = {}; // ARGS_NUM IS ZERO BY DEFAULT
        argument.args[0] = -1;  // NO REGISTER FOUND

        printf ("line now is %d\n", i);

        int code = GetCommandCode (&lines->str_array[i], jump_marks, i, instr_counter, &argument);

        if (code == SYNTAX_ERROR)
            return SYNTAX_ERROR;
    
        if (code != SKIP_COMMAND)
        {
            WriteHexBytes (buffer, &code, i + 1, ptr_to_write);
            (*instr_counter) ++;
        }

        WriteHexArguments (buffer, &argument, instr_counter, ptr_to_write);
    }

    return 0;
}

void WriteSystemInfo (char* buffer, int num_of_instr)
{
    assert (buffer);

    int Lcode = SIGN[0];
    int Pcode = SIGN[1]; 

    buffer[0] = Lcode;
    buffer[1] = Pcode;
    buffer[2] = VERSION;
    buffer[3] = num_of_instr;

    return;
}

int CompileFirst (marks* jump_marks, TEXT* lines, char* buffer)  //TODO rename BUFFER
{
    assert (jump_marks);
    assert (lines);
    assert (buffer);

    int ptr_to_write = sizeof (int);
    int instr_counter = 0;

    int error = WriteCommands (buffer, lines, jump_marks, &ptr_to_write, &instr_counter);

    printf ("commands written\n");

    if (error) 
        return SYNTAX_ERROR;

    WriteSystemInfo (buffer, instr_counter);

    FILE* file = fopen ("binary.txt", "wb" );
    fwrite (buffer, sizeof(char), ptr_to_write, file);
    
    fclose (file);

    return 0;
}

int CompileSecond (marks* jump_marks, TEXT* lines, char* buffer)
{ 
    assert (jump_marks);
    assert (lines);
    assert (buffer);

    int ptr_to_write = 1;
    int instr_counter = 0;

    args_set args = {};

    for (int i = 0; i < lines->num_of_str; i ++)
    {
        args.args_num = 0;
        args.args[0] = -1;
        
        int command_code = GetCommandCode (&lines->str_array[i], jump_marks, i, &instr_counter, &args);

        if (IsJump(command_code))
        {
            ((int*)buffer) [++ptr_to_write] = args.args[1];
            ptr_to_write ++;
        }
        else if (command_code != SKIP_COMMAND) 
            ptr_to_write += (1 + args.args_num);
    }

    FILE* file = fopen ("binary.txt", "w");
    fwrite (buffer, sizeof (int), ptr_to_write/*buffer[3] + 1*/, file);
    fclose (file);

    int error = CheckJumpsInvariants (jump_marks, buffer);

    if (error)
        return error;

    return ALL_RIGHT;
}


int main (int argc, char** argv)
{
    //TO DO: if_jumps  -- done
    //       functions -- done
    //       code_generation

    if (argc < 2)
    {
        printf ("lack of arguments! try again!\n");
        return 0;
    }

    
    //printf ("debug only: %s\n", file_name);

    TEXT lines = {};
    const char* file_name = argv[1];
    TxtCreate (&lines, file_name, "r");
    
    char* buffer = CALLOC (2 * sizeof(int) * lines.num_of_str + HEADER_SIZE, char);

    printf ("buffer size is %lu\n", 2 * sizeof(int) * lines.num_of_str + HEADER_SIZE);

    marks jump_marks = {};
    jump_marks.marks_array = CALLOC (MARKS_SIZE, mark);
    jump_marks.position = 0;

    int error1 = CompileFirst  (&jump_marks, &lines, buffer);
    printf ("first compile done\n");
    int error2 = CompileSecond (&jump_marks, &lines, buffer);

    if (error1 || error2) 
    {
        printf (RED ">>>COMPILATION ERROR\n\n" RESET);
        return 0;
    }
    
    //CompileSource (&jump_marks, &lines, buffer);

    FREE   (jump_marks.marks_array);
    FREE   (buffer);
    TxtDestroy (&lines);

    return 0;
}
