#ifndef ILOVEDEBUG_H
#define ILOVEDEBUG_H

#include "Stack.h"
#include "CPU.h"
#include <stdio.h>

#define FULL_DUMP  1
#define SHORT_DUMP 0

enum ERROR_LIST  
{
    SUCCESS = 0,
    STACK_NULL,
    STACK_DEAD,
    DATA_NULL,
    REALLOC_FAILED,
    STACK_OVERFLOW,
    INIT_FAILED,
    DESTROY_FAILED, 
    STACK_UNDERFLOW,
    HASH_DAMAGED,
    RIGHT_CANARY_DAMAGED, 
    LEFT_CANARY_DAMAGED
};

void InitDumpFile ();

void DestroyDumpFile ();

void StackDump (my_stack* stack, const char* func, int line, int ErrCode, int MODE);

void DumpCPU   (my_cpu* CPU);

#endif