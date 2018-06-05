#include <parser.h>

#include <cstdlib>
#include <error.h>

using namespace std;

size_t current = 0;

static token_t *get_token(vector<token_t *> tokens);
static token_t *peek_token(vector<token_t *> tokens);
static void push_child(ASTNode *to_push, ASTNode *child);
static ASTNode *new_ast_node(label_t label, string *value);
static ASTNode *new_ast_node(label_t label);
static ASTNode *parse_expression(vector<token_t *> tokens);
static ASTNode *parse_expression(vector<token_t *> tokens, ASTNode *last_expr);
static ASTNode *parse_statement(vector<token_t *> tokens);
static ASTNode *parse_function_declaration(vector<token_t *> tokens);
static ASTNode *parse_unary_expression(vector<token_t *> tokens, label_t type);
static ASTNode *parse_binary_expression(vector<token_t *> tokens, ASTNode *last_expr, label_t type);

token_t *get_token(vector<token_t *> tokens)
{
    if(current >= tokens.size())
        return tokens.back();
    else
    {
        token_t *cur = tokens[current++];
        //printf("Current: %d\n", cur->type);
        return cur;
    }
}

token_t *peek_token(vector<token_t *> tokens)
{
    if(current >= tokens.size())
        return tokens.back();
    else
        return tokens[current];
}

void push_child(ASTNode *to_push, ASTNode *child)
{
    to_push->children.push_back(child);
}

ASTNode *new_ast_node(label_t label)
{
    ASTNode *astnode = new ASTNode;
    astnode->ast_type = label;
    astnode->value = NULL;
    return astnode;
}

ASTNode *new_ast_node(label_t label, string *value)
{
    ASTNode *astnode = new ASTNode;
    astnode->ast_type = label;
    astnode->value = value;
    return astnode;
}

ASTNode *parse_expression(vector<token_t *> tokens)
{
    token_t *next = get_token(tokens);
    //printf("Expression on %d\n", next->type);
    if(next->type == INTEGER_LITERAL)
    {
        ASTNode *res = new_ast_node(LITERAL_EXPRESSION);
        push_child(res, new_ast_node(INTEGER_LITERAL, next->value));
        return parse_expression(tokens, res);
    }
    else if(next->type == STRING_LITERAL)
    {
        ASTNode *res = new_ast_node(LITERAL_EXPRESSION);
        push_child(res, new_ast_node(STRING_LITERAL, next->value));
        return parse_expression(tokens, res);
    }
    else if(next->type == IDENTIFIER)
    {
        ASTNode *res = new_ast_node(LITERAL_EXPRESSION);
        push_child(res, new_ast_node(IDENTIFIER, next->value));
        return parse_expression(tokens, res);
    }
    else if(next->type == CHAR_LITERAL)
    {
        ASTNode *res = new_ast_node(LITERAL_EXPRESSION);
        push_child(res, new_ast_node(CHAR_LITERAL, next->value));
        return parse_expression(tokens, res);
    }
    else if(next->type == LEFT_PAREN) // is information lost here?
    {
        ASTNode *expr = parse_expression(tokens);
        next = get_token(tokens);
        if(next->type != RIGHT_PAREN)
            errorp(pos_from_token(next).c_str(), "Expected ) to close expression");
        return parse_expression(tokens, expr);
    }
    else // test unary expressions
        switch(next->type)
        {
            case BITWISE_OR:
                return parse_expression(tokens, parse_unary_expression(tokens, ADDRESS_OF));
            case ASTERISK:
                return parse_expression(tokens, parse_unary_expression(tokens, DEFREF));
            case INCRE_OPERATOR:
                return parse_expression(tokens, parse_unary_expression(tokens, BEFORE_INCRE));
            case DECRE_OPERATOR:
                return parse_expression(tokens, parse_unary_expression(tokens, BEFORE_DECRE));
            case EXCLAMATION_MARK:
                return parse_expression(tokens, parse_unary_expression(tokens, UNARY_NOT));
            default:
                errorp(pos_from_token(next).c_str(), "Expected some valid expression"); // well shit
        }
    return NULL;
}

/**
 * After some token or sequence of tokens is said to be an expression, what
 * is ahead of the expression must also be considered. That is why this
 * function is called on return expressions in the inital parse expression
 * function. This function could be called the second pass of the first.
 */
ASTNode *parse_expression(vector<token_t *> tokens, ASTNode *last_expr)
{
    token_t *ahead = peek_token(tokens), *next;
    //printf("Ahead: %d\n", ahead->type);
    switch(ahead->type)
    {
        case LEFT_BRACKET:
        {
            get_token(tokens); // skip [
            ASTNode *parent_expr = new_ast_node(INDEXING_EXPRESSION);
            push_child(parent_expr, last_expr);
            push_child(parent_expr, parse_expression(tokens));
            next = get_token(tokens);
            if(next->type != RIGHT_BRACKET)
                errorp(pos_from_token(ahead).c_str(), "Expected closing ] after expression");
            return parent_expr;
        }
        case LEFT_PAREN: // function call expression
        {
            get_token(tokens); // skip (
            ASTNode *func_expr = new_ast_node(FUNCTION_CALL_EXPRESSION);
            push_child(func_expr, last_expr);
            push_child(func_expr, parse_expression(tokens));
            while((next = get_token(tokens))->type != RIGHT_PAREN)
            {
                push_child(func_expr, parse_expression(tokens));
                next = get_token(tokens);
                if(next->type != COMMA && next->type != RIGHT_PAREN)
                    errorp(pos_from_token(ahead).c_str(), "Expected a , or ) for a function call");
            }
            return func_expr;
        }
        case COMPARISON_OPERATOR:
            get_token(tokens); // skip
            return parse_binary_expression(tokens, last_expr, BINARY_COMPARISON);
        case ASSIGNMENT_OPERATOR:
            get_token(tokens); // skip
            return parse_binary_expression(tokens, last_expr, ASSIGNMENT_OPERATOR);
        case ADDITION:
            get_token(tokens); // skip
            return parse_binary_expression(tokens, last_expr, BINARY_ADDITION);
        case SUBTRACTION:
            get_token(tokens); // skip
            return parse_binary_expression(tokens, last_expr, BINARY_SUBTRACTION);
        case ASTERISK:
            get_token(tokens); // skip
            return parse_binary_expression(tokens, last_expr, BINARY_MULTIPLICATION);
        case DIVISION:
            get_token(tokens); // skip
            return parse_binary_expression(tokens, last_expr, BINARY_DIVISION);
        case LOGICAL_AND:
            get_token(tokens); // skip
            return parse_binary_expression(tokens, last_expr, BINARY_LOGICAL_AND);
        case LOGICAL_OR:
            get_token(tokens); // skip
            return parse_binary_expression(tokens, last_expr, BINARY_LOGICAL_OR);
        case BITWISE_AND:
            get_token(tokens); // skip
            return parse_binary_expression(tokens, last_expr, BINARY_BITWISE_AND);
        case BITWISE_OR:
            get_token(tokens); // skip
            return parse_binary_expression(tokens, last_expr, BINARY_BITWISE_OR);
        case QUESTION_MARK:
        {
            get_token(tokens); // skip ?
            ASTNode *ternary_conjuction = new_ast_node(TERNARY_CONJUCTION);
            push_child(ternary_conjuction, last_expr); // the expression to evaluate, of course
            push_child(ternary_conjuction, parse_expression(tokens));
            if(get_token(tokens)->type != COLON)
                errorp(pos_from_token(ahead).c_str(), "Expected : in unary expression");
            push_child(ternary_conjuction, parse_expression(tokens));
            return ternary_conjuction;
        }
        default:
            break;
    }
    return last_expr;
}

ASTNode *parse_statement(vector<token_t *> tokens)
{
    token_t *next = get_token(tokens);
    //printf("Statement on %d\n", next->type);
    if(next->type == LEFT_BRACKET)
    {
        ASTNode *nested_statement = parse_statement(tokens);
        next = get_token(tokens);
        while(next->type != RIGHT_BRACKET)
        {
            next = get_token(tokens);
            if(next->type == END_OF_FILE)
                errorp(pos_from_token(next).c_str(), "Reached end of stream looking for }");
            push_child(nested_statement, parse_statement(tokens));
        }
        return nested_statement;
    }
    else if(next->type == IF)
    {
        next = get_token(tokens);
        if(next->type != LEFT_PAREN)
            errorp(pos_from_token(next).c_str(), "Expected ( in front of if keyword");
        ASTNode *conditional_statement = new_ast_node(CONDITIONAL_STATEMENT);
        push_child(conditional_statement, parse_expression(tokens));
        next = get_token(tokens);
        if(next->type != RIGHT_PAREN)
            errorp(pos_from_token(next).c_str(), "Expected ) in front of conditional statement expression");
        push_child(conditional_statement, parse_statement(tokens));
        return conditional_statement;
    }
    else if(next->type == WHILE)
    {
        next = get_token(tokens);
        if(next->type != LEFT_PAREN)
            errorp(pos_from_token(next).c_str(), "Expected ( in front of while keyword");
        ASTNode *loop_statement = new_ast_node(LOOP_STATEMENT);
        push_child(loop_statement, parse_expression(tokens));
        next = get_token(tokens);
        if(next->type != RIGHT_PAREN)
            errorp(pos_from_token(next).c_str(), "Expected ) in front of while statement expression");
        push_child(loop_statement, parse_statement(tokens));
        return loop_statement;
    }
    else if(next->type == RETURN)
    {
        ASTNode *return_statement = new_ast_node(RETURN_STATEMENT);
        push_child(return_statement, parse_expression(tokens));
        next = get_token(tokens);
        if(next->type != SEMICOLON)
            errorp(pos_from_token(next).c_str(), "Expected ; after expression");
        return return_statement;
    }
    else if(next->type == VAR)
    {
        read_identifier:
        next = get_token(tokens);
        if(next->type != IDENTIFIER)
            errorp(pos_from_token(next).c_str(), "Expected identifier in variable declaration");
        ASTNode *variable_dec = new_ast_node(VARIABLE_DEFINITION_STATEMENT);
        push_child(variable_dec, new_ast_node(IDENTIFIER, next->value));
        
        next = get_token(tokens);
        if(next->type == ASSIGNMENT_OPERATOR)
        {
            push_child(variable_dec, parse_expression(tokens));
            next = get_token(tokens);
            if(next->type == COMMA)
                goto read_identifier; // once again, I am greatful for goto and labels
            else if(next->type == SEMICOLON)
                return variable_dec;
            else
                errorp(pos_from_token(next).c_str(), "Expected ; after expression");
        }
        else if(next->type == COMMA)
            goto read_identifier; // god bless this statement
        else if(next->type == SEMICOLON)
            return variable_dec; // we are done lol
        else
            errorp(pos_from_token(next).c_str(), "Expected ; after identifier");
    }
    else if(next->type == SEMICOLON)
        errorp(pos_from_token(next).c_str(), "Hanging semicolon");
    else
    {
        current--; // shit, go back, because we would skip a token
        ASTNode *expression_statement = parse_expression(tokens);
        if((next = get_token(tokens))->type != SEMICOLON)
            errorp(pos_from_token(next).c_str(), "Expected semicolon after expression statement\n");
        //printf("Next: %d\n", next->type);
        return expression_statement;
    }
    return NULL;
}

ASTNode *parse_function_declaration(vector<token_t *> tokens)
{
    //printf("Function declaration\n");
    ASTNode *function = new_ast_node(FUNCTION);
    token_t *next = get_token(tokens);
    if(next->type == END_OF_FILE)
    {
        return NULL;
    }
    else if(next->type != IDENTIFIER)
        errorp(pos_from_token(next).c_str(), "Expected an identifier");
    ASTNode *return_type = new_ast_node(next->type, next->value);
    push_child(function, return_type);
    while((next = get_token(tokens))->type != COLON)
    {
        if(next->type != IDENTIFIER)
            errorp(pos_from_token(next).c_str(), "Expected an identifier");
        push_child(function, new_ast_node(IDENTIFIER, next->value));
        next = get_token(tokens);
        if(next->type == COLON)
            break;
        if(next->type != COMMA)
            errorp(pos_from_token(next).c_str(), "Expected comma in function declaration");
    }
    if((next = get_token(tokens))->type != LEFT_BRACKET)
        errorp(pos_from_token(next).c_str(), "Expected { to begin function body");
    while(peek_token(tokens)->type != RIGHT_BRACKET)
        push_child(function, parse_statement(tokens));
    next = get_token(tokens); // skip }
    return function;
}

ASTNode *parse_unary_expression(vector<token_t *> tokens, label_t type)
{
    ASTNode *unary_expression = new_ast_node(type);
    push_child(unary_expression, parse_expression(tokens));
    return unary_expression;
}

ASTNode *parse_binary_expression(vector<token_t *> tokens, ASTNode *last_expr, label_t type)
{
    ASTNode *next_expr = parse_expression(tokens);
    ASTNode *expr_node = new_ast_node(type);
    push_child(expr_node, last_expr);
    push_child(expr_node, next_expr);
    return expr_node;
}

ASTTree *parser(vector<token_t *> tokens)
{
    ASTTree *library = new_ast_node(LIBRARY);
    ASTNode *next_append;
    while((next_append = parse_function_declaration(tokens)) != NULL)
    {
        push_child(library, next_append);
    }
    return library;
}

void print_ast_tree(ASTNode *tree, int layer)
{
    for(int i = 0; i < layer; i++)
        printf("   ");
    layer++;
    printf("Label: %d ", tree->ast_type);
    if(tree->value != NULL)
        printf("Value: %s", (*tree->value).c_str());
    printf("\n");
    for(size_t i = 0; i < tree->children.size(); i++)
    {
        print_ast_tree(tree->children[i], layer);
    }
}

void destroy_ast_tree(ASTNode *tree)
{
    for(size_t i = 0; i < tree->children.size(); i++)
        destroy_ast_tree(tree->children[i]);
    delete tree;
}