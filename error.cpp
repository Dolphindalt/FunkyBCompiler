#include <error.h>

#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <string>

int enable_warning = 1;
int warning_is_error = 0;

static void print_error(const char *line, const char *pos, const char *label, const char *fmt, va_list args) 
{
    fprintf(stderr, isatty(fileno(stderr)) ? "\e[1;31m[%s]\e[0m " : "[%s] ", label);
    fprintf(stderr, "%s: %s: ", line, pos);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}

void errorf(const char *line, const char *pos, const char *fmt, ...) 
{
    va_list args;
    va_start(args, fmt);
    print_error(line, pos, "ERROR", fmt, args);
    va_end(args);
    exit(1);
}

void warnf(char *line, char *pos, char *fmt, ...) 
{
    if (!enable_warning)
        return;
    const char *label = warning_is_error ? "ERROR" : "WARN";
    va_list args;
    va_start(args, fmt);
    print_error(line, pos, label, fmt, args);
    va_end(args);
    if (warning_is_error)
        exit(1);
}

std::string pos_from_token(token_t *token)
{
    std::string str = "Row: ";
    str += std::to_string(token->row);
    str += " Column: ";
    str += std::to_string(token->column);
    return str;
}