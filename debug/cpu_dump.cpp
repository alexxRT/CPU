#include "cpu_dump.h"
#include "Stack.h"
#include "CPU.h"
#include <strings.h>
#include <stdio.h>

FILE* dump_file = NULL; 

const char* COMMAND_NAME_LIST[] = 
{
"HLT",
"PUSH",
"ADD",
"SUB",
"MUL",
"DIV",
"OUT",
"IN",
"JUMP",
"JMP_A",
"JMP_B",
"JMP_AE",
"JMP_BE",
"CALL",
"POP",
"UNKNOWN_COMMAND",
};


const char* GetName (int mask)
{
    char num_buff[4] = "";
    memcpy (num_buff, &mask, 4);

    int command_code = (int)num_buff[0];
    command_code &= 0b00001111;


    assert (COMMAND_NAME_LIST[command_code] != NULL);
    return   COMMAND_NAME_LIST[command_code];
}

int GetLine (int mask)
{
    char num_buff[4] = "";
    memcpy (num_buff, &mask, 4);

    int line = (int)num_buff[3];
    
    assert (line > 0);
    return line;
}

void InitDumpFile ()
{
    dump_file = fopen ("DUMP.txt", "a");
    assert (dump_file != NULL);
}

void DestroyDumpFile ()
{
    fflush (dump_file);
    fclose (dump_file);
    dump_file = NULL;
}

void StackDump (my_stack* stack, const char* func, int line, int ErrCode, int MODE)
{
    if (MODE)
    {
        fprintf (dump_file, "--------------STACK DUMP occurred----------------\n");
        fprintf (dump_file, "It was called in func %s, and on the line %d\n", func, line);
    }
    
    if (MODE)
    {
        if (ErrCode == RIGHT_CANARY_DAMAGED || ErrCode == LEFT_CANARY_DAMAGED || ErrCode == HASH_DAMAGED)
        {
            fprintf (dump_file, "Your data might be damaged, check the log file\n");
        }
    }

    fprintf (dump_file, "The stack address: [%p]\n", stack);

    if (MODE)
    {
        if (ErrCode == STACK_NULL)
        {
            fprintf (dump_file, "Dump cannot be processed further becouse stack has NULL pointer\n");
            fprintf (dump_file, "------------- DUMP finished ---------------\n\n");

            fflush(dump_file);
            return;
        }
    }

    fprintf (dump_file, "The data begining address: [%p]\n", stack->data);

    if (MODE)
    {
        if (ErrCode == DATA_NULL)
        {
            fprintf (dump_file, "Data cannot be revealed becouse data has NULL pointer\n");
            fprintf (dump_file, "------------- DUMP finished ---------------\n\n");

            fflush (dump_file);
            return;
        }
    }

    if (MODE)
    {
        if (ErrCode == STACK_OVERFLOW)
        {
            fprintf (dump_file, "!!!OVERFLOW!!!\n");
            StackPrint (dump_file, stack, stack->capacity);
            fprintf (dump_file, "------------- DUMP finished ---------------\n\n");

            fflush (dump_file);
            return;
        }
    }

    fprintf (dump_file, "Stack capacity now is [%lu]\n", stack->capacity);
    fprintf (dump_file, "Number of elems in stack now is [%lu]\n", stack->size);

    StackPrint (dump_file, stack, stack->size);

    if (MODE) 
    {
        fprintf (dump_file, "------------- DUMP finished ---------------\n\n");
    }

    fflush (dump_file);

    return;
}

void DumpCPU (my_cpu* CPU)
{
    assert (CPU != NULL);
    assert (CPU->code_to_exec != NULL);
    assert (CPU->ram != NULL);
    assert (CPU->registr != NULL);

    //dump structure:
    fprintf (dump_file, "\n\n---------------CPU DUMP OCCURED------------------------\n\n");

    int func_mask = CPU->code_to_exec[*(CPU->instr_ptr)];
    

    const char* func_name = GetName (func_mask);
    int line              = GetLine (func_mask);
    
    fprintf (dump_file, "Function: %s\n", func_name);
    fprintf (dump_file, "Line:     %d\n", line);

    fprintf   (dump_file, "\nCPU stack info:\n");
    StackDump (CPU->cpu_stack , NULL, 0, 0, SHORT_DUMP);

    fprintf   (dump_file, "\nFunctions return addresses info:\n");
    StackDump (CPU->func_stack, NULL, 0, 0, SHORT_DUMP);

    fprintf (dump_file, "\n\n--------------------DUMP FINISHED-------------------------\n\n");

}
