#ifndef LEX_H
#define LEX_H

#include <mlang.h>
#include <cstdio>
#include <string>
#include <vector>

typedef struct token {
    std::string *value;
    label_t type;
    std::string *file;
    size_t row;
    size_t column;
    size_t count; // from 0
} token_t;

std::vector<token_t *> lex();

void print_token_list(std::vector<token_t *> &token_list);

void destroy_token_list(std::vector<token_t *> &token_list);

#endif