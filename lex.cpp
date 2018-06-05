#include <lex.h>

#include <file.h>
#include <ctype.h>
#include <error.h>

using namespace std;

size_t token_count = 0;

#define KEYLEN 4
string keywords[] = { "if", "while", "return", "var" };
label_t keyword_values[] = { IF, WHILE, RETURN, VAR };

static token_t *new_token(string str, label_t id)
{
    file_t *cf = current_file();
    token_t *token = new token_t;
    if(token == NULL)
        error("Malloc failed");
    token->value = new string(str);
    token->type = id;
    token->file = cf->name;
    token->row = cf->row;
    token->column = cf->column;
    token->count = token_count++;
    return token;
}

static token_t *new_token(label_t id)
{
    file_t *cf = current_file();
    token_t *token = new token_t;
    token->type = id;
    token->file = cf->name;
    token->row = cf->row;
    token->column = cf->column;
    token->count = token_count++;
    token->value = NULL;
    return token;
}

static token_t *end_of_file()
{
    token_t *token = new token_t;
    token->type = END_OF_FILE;
    token->value = NULL;
    return token;
}

static token_t *new_identifier(string str)
{
    return new_token(str, IDENTIFIER);
}

static token_t *new_keyword(label_t id)
{
    string thing;
    return new_token(id);
}

static token_t *new_other(label_t id, string str)
{
    return new_token(str, id);
}

static label_t is_keyword(string word)
{
    for(size_t i = 0; i < KEYLEN; i++)
        if(word == keywords[i])
            return keyword_values[i];
    return INVALID;
}

vector<token_t *> lex()
{
    vector<token_t *> tokens = vector<token_t *>();

    char c, n;
    label_t tmp;
    do {
        c = readc();

        switch(c)
        {
            case '(':
                tokens.push_back(new_keyword(LEFT_PAREN));
                goto exit_automata;
            case ')':
                tokens.push_back(new_keyword(RIGHT_PAREN));
                goto exit_automata;
            case '{':
                tokens.push_back(new_keyword(LEFT_BRACKET));
                goto exit_automata;
            case '}':
                tokens.push_back(new_keyword(RIGHT_BRACKET));
                goto exit_automata;
            case ',':
                tokens.push_back(new_keyword(COMMA));
                goto exit_automata;
            case ';':
                tokens.push_back(new_keyword(SEMICOLON));
                goto exit_automata;
            case ':':
                tokens.push_back(new_keyword(COLON));
                goto exit_automata;
            case '>':
                n = peekc();
                if(n == '=')
                {
                    tokens.push_back(new_keyword(GREATER_THAN_OR_EQUAL));
                    readc(); // skip =
                }
                else
                    tokens.push_back(new_keyword(GREATER_THAN));
                goto exit_automata;
            case '<':
                n = peekc();
                if(n == '=')
                {
                    tokens.push_back(new_keyword(LESS_THAN_OR_EQUAL));
                    readc(); // skip =
                }
                else
                    tokens.push_back(new_keyword(LESS_THAN));
                goto exit_automata;
            case '=':
                n = peekc();
                if(n == '=')
                {
                    tokens.push_back(new_keyword(COMPARISON_OPERATOR));
                    readc(); // skip =
                }
                else
                    tokens.push_back(new_keyword(ASSIGNMENT_OPERATOR));
                goto exit_automata;
            case '|':
                n = peekc();
                if(n == '|')
                {
                    tokens.push_back(new_keyword(LOGICAL_OR));
                    readc(); // skip |
                }
                else
                    tokens.push_back(new_keyword(BITWISE_OR));
                goto exit_automata;
            case '&':
                n = peekc();
                if(n == '&')
                {
                    tokens.push_back(new_keyword(LOGICAL_AND));
                    readc(); // skip &
                }
                else
                    tokens.push_back(new_keyword(BITWISE_AND));
                goto exit_automata;
            case '?':
                tokens.push_back(new_keyword(QUESTION_MARK));
                goto exit_automata;
            case '*':
                tokens.push_back(new_keyword(ASTERISK));
                goto exit_automata;
            case '/':
                tokens.push_back(new_keyword(BACKSLASH));
                goto exit_automata;
            case '+':
                n = peekc();
                if(n == '+')
                {
                    tokens.push_back(new_keyword(INCRE_OPERATOR));
                    readc(); // skip +
                }
                else
                    tokens.push_back(new_keyword(ADDITION));
                goto exit_automata;
            case '-':
                n = peekc();
                if(n == '-')
                {
                    tokens.push_back(new_keyword(DECRE_OPERATOR));
                    readc(); // skip -
                }
                else
                    tokens.push_back(new_keyword(SUBTRACTION));
                goto exit_automata;
            case '!':
                n = peekc();
                if(n == '=')
                {
                    tokens.push_back(new_keyword(NOT_EQUAL_OPERATOR));
                    readc(); // skip =
                }
                else
                    tokens.push_back(new_keyword(EXCLAMATION_MARK));
                goto exit_automata;
            default:
                break;
        }

        if(c == '\"')
        {
            string string_literal = "";
            while(peekc() != '\"')
            {
                c = readc();
                if(c == EOF)
                    error("Reached end of file, expected \"");
                string_literal += c;
            }
            readc();
            tokens.push_back(new_other(STRING_LITERAL, string_literal));
            continue;
        }

        if(c == '\'')
        {
            c = readc();
            string char_literal = to_string(c);
            tokens.push_back(new_other(CHAR_LITERAL, char_literal));
            if(readc() != '\'')
                error("Expected \'");
            continue;
        }

        if(isdigit(c))
        {
            string number_literal = "";
            number_literal += c;
            c = peekc();
            while(isdigit(c))
            {
                number_literal += c;
                readc();
                c = peekc();
            }
            tokens.push_back(new_other(INTEGER_LITERAL, number_literal));
            continue;
        }

        if(isalpha(c) || c == '_')
        {
            string ident = "";
            ident += c;
            c = peekc();
            while(isalpha(c) || isdigit(c) || c == '_')
            {
                ident += c;
                readc();
                c = peekc();
            }
            if((tmp = is_keyword(ident)) == INVALID)
                tokens.push_back(new_identifier(ident));
            else
                tokens.push_back(new_keyword(tmp));
            continue;
        }

        exit_automata: continue;
    } while(c != EOF);

    tokens.push_back(end_of_file());

    return tokens;
}

void print_token_list(vector<token_t *> &token_list)
{
    for(size_t i = 0; i < token_list.size(); i++)
    {
        token_t *token = token_list[i];
        if(token->value != NULL)
            printf("Val: %s Row: %ld Column: %ld Count: %ld Type: %d\n", 
                (*token->value).c_str(), token->row, token->column, token->count, token->type);
        else
            printf("Type: %d\n", token->type);
    }
}

void destroy_token_list(vector<token_t *> &token_list)
{
    for(size_t i = 0; i < token_list.size(); i++)
    {
        token_t *current = token_list[i];
        if(current->value != NULL)
            delete current->value;
        delete current;
    }
}