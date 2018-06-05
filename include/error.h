#ifndef ERROR_H
#define ERROR_H

#include <cstdarg>
#include <lex.h>

extern int enable_warning;
extern int warning_is_error;

#define STR2(x) #x
#define STR(x) STR2(x)
#define error(...) errorf(__FILE__ ":" STR(__LINE__), NULL, __VA_ARGS__)
#define warn(...) warnf(__FILE__ ":" STR(__LINE__), NULL, __VA_ARGS__)

#define errorp(pos, ...) errorf(__FILE__ ":" STR(__LINE__), pos, __VA_ARGS__)

void errorf(const char *line, const char *pos, const char *fmt, ...);
void warnf(const char *line, const char *pos, const char *fmt, ...);

std::string pos_from_token(token_t *token);

#endif