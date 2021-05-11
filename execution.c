#include "minishell.h"

// Função para executar um processo de uma linha de comando, assumindo que o processo 
// começa em begin e termina em end
void execute(char** argv, int begin, int end){

    char** new_argv = malloc(MAX_ARGS * sizeof(char*));

    for(int j=0; j<end-begin; ++j) new_argv[j] = argv[begin+j];
    new_argv[end-begin] = NULL;

    int status;
    pid_t w, pid = fork();
    
    if(pid < 0){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0){
        execv(new_argv[0], new_argv);
        exit(EXIT_SUCCESS);
    }
    else{
        w = wait(&status);
    }
}