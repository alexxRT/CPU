#include <iostream>
#include <stdlib.h>

#include "cpu_tokens.h"


int get_file_size (FILE* file) {

    fseek(file, SEEK_SET, SEEK_END);
    int file_size = ftell(file);
    fseek(file, SEEK_SET, SEEK_SET);

    return file_size;
}


void init_string(const char* file_name, char** string){
    FILE* source_file = fopen(file_name, "r");
    assert (source_file != NULL); //bad create

    int file_size = get_file_size(source_file);
    (*string) = (char*)calloc(file_size, sizeof(char)); //

    fread(*string, sizeof(char), file_size, source_file);
    fclose(source_file);

    return;
}

void delete_string(char** string){
    free(*string);
    *string = NULL;

    return;
}




int main () {
    char* string = NULL;
    init_string("../source_file.txt", &string);

    struct set_of_tokens new_set = {};
    init_set(&new_set, strlen(string));


    struct parse_ptr ptr = {};
    ptr.origin = string;
    ptr.offset = string;

    single_token result = {};
    result = create_tokens_from_string(&ptr, &new_set);

    if (result.tag == PARSE_SUCCESS)
        printf ("SUCCESS\n");
    else 
        printf ("ERROR PARSE\n");

    print_tokens(&new_set);


    delete_string(&string);
    delete_set   (&new_set);
    return 0;
}