#include "minishell.h"

#include "chain.c"
#include "execution.c"
#include "parser.c"

int main(){

    int shell_status;
    
    pid_t shell_pid, pid = fork();

    if(pid < 0){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0){
        
        fprintf(stdout, "Welcome to Caio's Minishell!\n");
        
        int status2;
        pid_t pid2;

        do{
            // Cada linha de comando é um novo processo e deve esperar o anterior terminar
            pid2 = fork();
            if(pid2 < 0){
                perror("fork");
                exit(EXIT_FAILURE);
            }
            else if(pid2 == 0){
                parser();
                exit(EXIT_SUCCESS);
            }
            else wait(&status2);
        }while(TRUE);
        
        exit(EXIT_SUCCESS);
    }
    else{
        shell_pid = wait(&shell_status);
        fprintf(stdout, "Minishell closed!\n");
    }
    return 0;
}