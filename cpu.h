#ifndef CPU_H
#define CPU_H

#include "Stack.h"

typedef struct _CPU
{
    int* instr_ptr;
    int* code_to_exec;
    int* registr;
    int* ram;
    my_stack* cpu_stack;
    my_stack* func_stack;
} my_cpu;




#endif