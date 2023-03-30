#include <strings.h>
#include "memory.h"

//TO DO
//implement axp memory safe alloc lib

void* Recalloc (void* ptr, size_t new_size)
{
    if (ptr == NULL) return NULL;
    
    size_t old_size = malloc_size (ptr);
    ptr = realloc (ptr, new_size);

    if (ptr == NULL) return NULL;

    if (new_size > old_size) 
    bzero ((char*)ptr + old_size, new_size - old_size);

    return ptr;
}


