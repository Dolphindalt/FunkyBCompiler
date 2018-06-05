#include <mlang.h>

#include <cstdio>
#include <file.h>
#include <lex.h>
#include <parser.h>

std::vector<file_t *> file_list;

int debug = 0;

int main(int argc, char *argv[])
{
    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-')
        {
            switch(argv[i][1])
            {
                case 'o':
                    // rename the produced executable
                    break;
                case 'd':
                    if(argv[i][2] == 'l')
                        debug |= 1;
                    else if(argv[i][2] == 'p')
                        debug |= 2;
                    else
                        goto error;
                    break;
                default:
                    error: 
                    fprintf(stderr, "Unknown flag: -%c\n", argv[i][1]);
                    return 1;
            }
        }
        else
        {
            file_list.push_back(new_file(argv[i]));
        }
    }

    if(file_list.size() == 0)
    {
        fprintf(stderr, "No input files provided\n");
        return 0;
    }

    std::vector<token_t *> tokens = lex();

    if(debug & 1)
        print_token_list(tokens);

    ASTTree *ast_tree = parser(tokens);

    if(debug & 2)
        print_ast_tree(ast_tree, 0);

    destroy_token_list(tokens);
    destroy_ast_tree(ast_tree);
}