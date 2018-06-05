#include <file.h>

#include <cstdlib>
#include <vector>
#include <error.h>
#include <iostream>
#include <fstream>
#include <cassert>

extern std::vector<file_t *> file_list;

static char file_readc(file_t *f)
{
    char c = (*f->contents)[f->character_count++];
    if(c == '\0')
    {
        c = EOF;
    }
    return c;
}

static char file_peekc(file_t *f)
{
    char c = (*f->contents)[f->character_count];
    if(c == '\0')
        c = EOF;
    return c;
}

static char get()
{
    file_t *f = current_file();
    if(f == NULL) return EOF;
    char c = file_readc(f);
    if(c == '\n')
    {
        f->row++;
        f->column = 1;
    }
    else if(c != EOF)
        f->column++;
    return c;
}

static char peek()
{
    file_t *f = current_file();
    char c = file_peekc(f);
    return c;
}

file_t *new_file(char *name)
{
    file_t *f = new file_t;
    std::ifstream ifs(name);
    std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
    content += '\0';
    f->contents = new std::string(content);
    assert(f->contents->empty() == false);
    f->name = new std::string(name);
    f->row = 1;
    f->column = 1;
    f->character_count = 0;
    f->last_char = 0;
    return f;
}

static void delete_file(file_t *f)
{
    f->name = NULL;
    delete f->name;
    f->contents = NULL;
    delete f->contents;
    f = NULL;
    delete f;
}

file_t *current_file()
{
    if(file_list.size() == 0) return NULL;
    assert(file_list.size() == 0 || file_list.back() != NULL);
    assert(file_list.back()->contents->empty() == false);
    return file_list[file_list.size()-1];
}

char readc()
{
    char c = get();
    if(c == EOF)
    {
        if(current_file() == NULL) return EOF;
        else
        {
            delete_file(current_file());
            file_list.pop_back();
            return readc();
        }
    }
    return c;
}

char peekc()
{
    return peek();
}

void print_file(file_t *f)
{
    printf("Name: %s\nCount: %lu\n", f->name->c_str(), f->character_count);
}