#include "minishell.h"

// Função para contar a quantidade de pipes em uma linha de input
int pipe_counter(int argc, char**argv){
    
    int num_pipes = 0;
    for(int i=0; i<argc; ++i) if(!strcmp(argv[i], "|")) num_pipes++;
    return num_pipes;
}


// Função para processar todo o encadeamento de comandos
void chain(int argc, char** argv){

    int status;
    pid_t pid;

    int num_pipes = pipe_counter(argc, argv);

    // Os casos em que não há pipes devem ser tratados a parte
    if(num_pipes > 0){

        // Preparação do "encanamento"
        int* pipes = malloc(2*num_pipes*sizeof(int));
        
        for(int i=0; i<num_pipes; ++i){
            if(pipe(pipes+2*i) < 0){
                perror("pipe");
                exit(EXIT_FAILURE);
            } 
        }

        // Para cada comando intercalado por pipes:
        for(int i=0; i < num_pipes + 1; ++i){

            pid = fork();
            
            if(pid < 0){
                perror("fork");
                exit(EXIT_FAILURE);
            }
            else if(pid == 0){
                
                // Começo e fim dos argumentos do processo que antecede o i-ésimo pipe
                int begin = 0, current_pipe = 0;
                while(begin < argc && current_pipe < i){
                    if(!strcmp(argv[begin], "|")) ++current_pipe;
                    ++begin;
                }

                int end = begin;
                while(end < argc && strcmp(argv[end], "|") && strcmp(argv[end], "<") && strcmp(argv[end], ">")) ++end;
            
                // Caso esteja o primeiro elemento da cadeia de pipes não é necessário tomar o input 
                // a partir do output do processo anterior
                if(i == 0){

                    dup2(pipes[2*i + 1], STDOUT_FILENO);
                    
                    // Caso tenha entrada por arquivo, redirecionar o arquivo de entrada
                    if(end < argc && !strcmp(argv[end], "<")){
                        
                        int infile = open(argv[end+1], O_RDONLY);    
                        
                        dup2(infile, STDIN_FILENO);
                        close(infile);
                    }
                }
                // Caso seja o último processo da cadeia de pipes não é necessário redirecionar o output 
                // para o próximo processo
                else if(i == num_pipes){

                    // Redirecionamento para o arquivo de saída caso necessário
                    if(!strcmp(argv[argc-2], ">")){
                        
                        // É dada a permissão para criar o arquivo de saída, caso ele não exista
                        int outfile = open(argv[argc-1], O_WRONLY | O_CREAT, 0666);
                        
                        dup2(outfile, STDOUT_FILENO);
                        close(outfile);
                        
                    }
                    dup2(pipes[2*(i-1)], STDIN_FILENO);
                }
                // Caso default, input deste é o output do antecessor e output deste é o input do sucessor
                else{
                    dup2(pipes[2*i + 1], STDOUT_FILENO);
                    dup2(pipes[2*(i-1)], STDIN_FILENO);
                }

                // Fechar todo o encanamento
                for(int j=0; j<2*num_pipes; ++j) close(pipes[j]);

                // Execução do processo de interesse            
                execute(argv, begin, end);
                exit(EXIT_SUCCESS);
            }
        }

        // Fechar todo o encanamento
        for(int i=0; i<2*num_pipes; ++i) close(pipes[i]);

        // Espera pelo término de todos os processos filhos
        for(int i=0; i<num_pipes+1; ++i) wait(&status);

    }

    else{

        int status;
        pid_t pid = fork();

        if(pid < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if(pid == 0){

            // Obtenção do fim dos parametros da execução do processo
            int end = 0;
            while(end < argc && strcmp(argv[end], "<") && strcmp(argv[end], ">")) end++;

            // Caso necessário, fazer os devidos redirecionamentos a/de arquivos
            if(end < argc && !strcmp(argv[end], "<")){
                int infile = open(argv[end+1], O_RDONLY);    
                dup2(infile, STDIN_FILENO);
                close(infile);
            }
            if(argc > 1 && !strcmp(argv[argc-2], ">")){
                int outfile = open(argv[argc-1], O_WRONLY | O_CREAT, 0666);
                dup2(outfile, STDOUT_FILENO);
                close(outfile);
            }

            execute(argv, 0, end);
            exit(EXIT_SUCCESS);
        }
        // Esperar execução
        else wait(&status);
    }
 
}