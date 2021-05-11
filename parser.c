#include "minishell.h"

void parser(void){
    
    fprintf(stdout, "cmd> ");

    // Leitura da linha de comando
    int argc = 0;
    char** argv = malloc(MAX_ARGS * sizeof(char*));

    char arg[MAX_ARG_SIZE];
    char symbol;
    
    while(fscanf(stdin, "%s%c", arg, &symbol) != EOF){
        argv[argc++] = malloc(MAX_ARG_SIZE * sizeof(char));
        strcpy(argv[argc-1], arg);
        if(symbol == '\n') break;
    }

    // Processamento da linha de comando
    chain(argc, argv);
}