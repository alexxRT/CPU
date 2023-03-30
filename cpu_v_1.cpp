#include "Warnings.h"
#include "ILOVEDEBUG.h"
#include "Stack.h"
#include "Memory.h"
#include "CPU.h"

#include <stdio.h>
#include <stdlib.h>

#define MASK_TO_ZERO(mask)       \
    mask = mask & 0b00001111;    \
    mask = mask / pow (2, 4);    \
    mask = mask & 0b111100000000 \

// #include "Asm.h"

//TODO :
//add in cpu ability to differ JUMPS and PUSHES
//                   to execute them 
// factorial and quadro_solver

typedef struct _asm_code
{
    int* commands;
    int num_of_instr;
}asm_code;

typedef struct _header
{
    int Lcode;
    int Rcode;
    int version;
    int num_of_instr;

}LP_header;

enum COMMAND_MASKS
{
    NUMBER  = 0b00010000,
    REGISTR = 0b00100000,
    RAM     = 0b01000000
};

enum COMMAND_LIST
{
    HLT     = 0 ,
    PUSH    = 1 ,
    ADD     = 2 ,
    SUB     = 3 ,
    MUL     = 4 ,
    DIV     = 5 ,
    OUT     = 6 ,
    IN      = 7 ,
    JUMP    = 8 ,
    JMP_A   = 9 ,
    JMP_B   = 10,
    JMP_AE  = 11,
    JMP_BE  = 12,
    CALL    = 13,
    POP     = 14,
    UNKNOWN_COMMAND = 15
};



const int VERSION = 1;
const int INSTRUCTION_SIZE = 1020; // remove, we have info in header
const int RAM_SIZE = 1280;         // 5 Mb memory ram 


int* GetArgument (int* instr_ptr, int* ram, int* registr, int* code_to_exec)
{
    int code_mask = code_to_exec[*instr_ptr];
    (*instr_ptr) ++;

    static int val = 0; //????? how byla sukaaaaaaaaaaaaaaa
    val = 0;
    //printf ("val now is %d\n", val);

    // if ((code_mask & 0b00001111) == JMP_A)
    // {
    //     printf ("!!!its arg is %d\n", code_to_exec[*instr_ptr]);
    //     printf ("same in hex is %x\n", code_to_exec[*instr_ptr]);
    //     printf ("current val is %d\n", val);
    //     debug_const = 1;
    // }

    if (code_mask & NUMBER)
    {
        val += code_to_exec[*instr_ptr];
        (*instr_ptr) ++;
    }
    if (code_mask & REGISTR)
    {
        if (!(code_mask & RAM) && !(code_mask & NUMBER))
        {
            int reg_num = code_to_exec[*instr_ptr];
            (*instr_ptr) ++;

            //printf ("reg num is %d\n", reg_num);

            return registr + reg_num;
        }
        else
            val += registr[code_to_exec[*instr_ptr]];

        (*instr_ptr) ++;
    }
    if (code_mask & RAM)
    {
        return ram + val;
    }

    return &val;
}

int CheckSignature (LP_header* header)
{
    return (header->Lcode != 'L' || header->Rcode != 'P') ? 1 : 0;
}


int CheckVersion (LP_header* header)
{
    int version = header->version;

    return (version != VERSION) ? 1 : 0;
}

int GetInstructions (asm_code* binary_code, LP_header* header, char* buffer, FILE* file)
{
    binary_code->num_of_instr = header->num_of_instr;

    fread (buffer + sizeof(int), sizeof(int), (size_t)header->num_of_instr, file);

    #pragma GCC diagnostic ignored "-Wcast-align"
    binary_code->commands = (int*)(buffer + sizeof(int));
    #pragma GCC diagnostic warning "-Wcast-align"
    return 0;
}

int ExecuteInstructions (asm_code* binary_code, my_stack* cpu_stack, my_stack* func_stack, int* registr, int* ram)
{
    int* code_to_exec = binary_code->commands;
    //int num_of_et = binary_code->num_of_instr;
    
    // int val = 0;
    // int val1 = 0;
    // int val2 = 0;
    // int res = 0; 
    int instr_ptr = 0;

    my_cpu CPU = {};

    CPU.code_to_exec = code_to_exec;
    CPU.cpu_stack    = cpu_stack;
    CPU.func_stack   = func_stack;
    CPU.instr_ptr    = &instr_ptr;
    CPU.ram          = ram;
    CPU.registr      = registr;

    printf ("cpu address %p\n", &CPU);

    while (TRUE)
    {
        switch (code_to_exec[instr_ptr] & 0b00001111)
        {
            case PUSH:
            {
                DumpCPU (&CPU);
                //printf ("NOW PUSH EXECUTING\n");
                int* val = GetArgument (&instr_ptr, ram, registr, code_to_exec);
                //printf ("val in push after get_arg %d\n", *val);

                StackPush (cpu_stack, val);
            }
            break;

            case ADD:
            {
                //printf ("NOW ADD EXECUTING\n");

                int val1 = 0, val2 = 0;
                StackPop (cpu_stack, &val1);
                StackPop (cpu_stack, &val2);

                int res = val1 + val2;  
                StackPush (cpu_stack, &res);
                instr_ptr++;
            }
            break;

            case OUT:
            {
                //printf ("NOW OUT EXECUTING\n");

                int val = 0;
                StackPop (cpu_stack, &val);

                printf ("%d\n", val);

                instr_ptr++;
            }
            break;
            
            case POP:
            {
                //printf ("NOW POP EXECUTING\n");

                //add cases when pop dies
                int* ptr = GetArgument (&instr_ptr, ram, registr, code_to_exec);
                int val = 0;

                StackPop (cpu_stack, &val);
                //printf ("value in stack was %d\n", val);

                *ptr = val;
            }
            break;

            case IN:
            {
                ///printf ("NOW IN EXECUTING\n");

                int val = 0;
                scanf ("%d", &val);

                StackPush (cpu_stack, &val);
                instr_ptr++;
            }
            break;
            
            case MUL:
            {
                //printf ("NOW MUL EXECUTING\n");

                int val1 = 0, val2 = 0;
                StackPop (cpu_stack, &val1);
                StackPop (cpu_stack, &val2);

                int res = val1 * val2;
                StackPush (cpu_stack, &res);
                instr_ptr++;
            }
            break;

            case SUB:
            {
                //printf ("NOW SUB EXECUTING\n");

                int val1 = 0, val2 = 0;
                StackPop (cpu_stack, &val1);
                StackPop (cpu_stack, &val2);

                int res = val2 - val1;
                StackPush (cpu_stack, &res);
                instr_ptr++;
            }
            break;
            
            case DIV:
            {
                int val1 = 0, val2 = 0;
                StackPop (cpu_stack, &val1);
                StackPop (cpu_stack, &val2);

                int res = val2 / val1;
                StackPush (cpu_stack, &res);
                instr_ptr++;
            }
            break;

            case JUMP:
            {
                int* val = GetArgument (&instr_ptr, ram, registr, code_to_exec);

                instr_ptr = *val;
            }
            break;

            case JMP_A:
            {
                //printf ("NOW JUMP EXECUTING\n");

                int val1 = 0, val2 = 0;
                int command_code = code_to_exec[instr_ptr];

                StackPop (cpu_stack, &val1);
                StackPop (cpu_stack, &val2);

                if (val1 - val2 > 0)
                {
                    int* val = GetArgument (&instr_ptr, ram, registr, code_to_exec);
                    //printf ("jmp_a returned %d\n", *val);
                    
                    instr_ptr = *val;
                }
                else 
                {
                    if (command_code & REGISTR)
                        instr_ptr += 3;
                    else 
                        instr_ptr += 2;
                }
            }
            break;

            case JMP_AE:
            {
                int val1 = 0, val2 = 0;
                int command_code = code_to_exec[instr_ptr];

                StackPop (cpu_stack, &val1);
                StackPop (cpu_stack, &val2);

                if (val1 - val2 >= 0)
                {
                    int* val = GetArgument (&instr_ptr, ram, registr, code_to_exec);
                    
                    instr_ptr = *val;
                }
                else 
                {
                    if (command_code & REGISTR)
                        instr_ptr += 3;
                    else 
                        instr_ptr += 2;
                }
            } 
            break;
            
            case JMP_B:
            {
                int val1 = 0, val2 = 0;
                int command_code = code_to_exec[instr_ptr];

                StackPop (cpu_stack, &val1);
                StackPop (cpu_stack, &val2);

                if (val1 - val2 < 0)
                {
                    int* val = GetArgument (&instr_ptr, ram, registr, code_to_exec);
                    
                    instr_ptr = *val;
                }

                else 
                {
                    if (command_code & REGISTR)
                        instr_ptr += 3;
                    else 
                        instr_ptr += 2;
                }
            }
            break;
             
            case JMP_BE:
            {
                int val1 = 0, val2 = 0;
                int command_code = code_to_exec[instr_ptr];

                StackPop (cpu_stack, &val1);
                StackPop (cpu_stack, &val2);

                if (val1 - val2 <= 0)
                {
                    instr_ptr ++;
                    int* val = GetArgument (&instr_ptr, ram, registr, code_to_exec);
                    
                    instr_ptr = *val;
                }

                else 
                {
                    if (command_code & REGISTR)
                        instr_ptr += 3;
                    else 
                        instr_ptr += 2;
                }
            }
            break;

            case CALL:
            {   
                int return_addr = instr_ptr + 2;
                instr_ptr = code_to_exec [instr_ptr + 1];
                
                StackPush (func_stack, &return_addr);
            }
            break;

            case HLT:
                if (func_stack->size == 0)
                    return 0;
                else
                    StackPop (func_stack, &instr_ptr);
            break;
            
            case UNKNOWN_COMMAND:
                printf ("ERROR SYNTAX\n");
                return 0;
            break;

            default: 
                printf ("UNEXPECTED ERROR OCCURED. ALL YOUR DATA IS ERASED. GO OUT AND DIE.\n");
                return 0;
            break;
        }
    }
    return 0;
}

void FinishExecution (FILE* file, my_stack* cpu_stack, my_stack* func_stack, char* buf, int* ram)
{
    FREE   (ram);
    FREE   (buf);
    fclose (file);
    StackDestroy (cpu_stack);
    StackDestroy (func_stack);
}

void GetHeader (LP_header* header, char* buffer)
{
    header->Lcode        = buffer[0];
    header->Rcode        = buffer[1];
    header->version      = VERSION; 
    header->num_of_instr = buffer[3];
}


int main ()
{
    FILE* file = fopen ("binary.txt", "rb");
    char* buffer = CALLOC (INSTRUCTION_SIZE, char); //not neccessary

    fread (buffer, sizeof(char), sizeof(int), file); 

    LP_header header = {};
    GetHeader (&header, buffer);

    int sign_err = CheckSignature (&header);
    int vers_err = CheckVersion   (&header);

    if (sign_err || vers_err)
    {
        printf ("This file format is not supported\n");
        fclose (file);
        return 0;
    }

    int registrs [4] = {};
    int* ram = (int*)calloc(RAM_SIZE, sizeof(int));
    

    asm_code binary_code = {};
    GetInstructions (&binary_code, &header, buffer, file);

    my_stack cpu_stack = {};
    StackInit (&cpu_stack, 10, 10);

    my_stack func_stack = {};
    StackInit (&func_stack, 10, 10);

    InitDumpFile ();
    ExecuteInstructions (&binary_code, &cpu_stack, &func_stack, registrs, ram);

    FinishExecution (file, &cpu_stack, &func_stack, buffer, ram);
    DestroyDumpFile ();
}
