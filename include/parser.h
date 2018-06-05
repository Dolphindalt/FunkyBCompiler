#ifndef PARSER_H
#define PARSER_H

#include <mlang.h>
#include <vector>
#include <string>
#include <lex.h>

typedef struct ast_node {
    label_t ast_type;
    std::string *value = NULL;
    std::vector<struct ast_node *> children;
} ASTNode;

typedef ASTNode ASTTree;

ASTTree *parser(std::vector<token_t *> tokens);

void print_ast_tree(ASTNode *tree, int layer);

void destroy_ast_tree(ASTNode *tree);

#endif