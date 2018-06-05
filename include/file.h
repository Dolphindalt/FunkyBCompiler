#ifndef FILE_H
#define FILE_H

#include <cstdio>
#include <string>

typedef struct file {
    std::string *contents;
    std::string *name;
    size_t row;
    size_t column;
    size_t character_count;
    char last_char;
} file_t;

file_t *new_file(char *name);

file_t *current_file();

char readc();

char peekc();

void print_file(file_t *f);

#endif