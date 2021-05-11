// ALUNO: CAIO GRAÇA GOMES

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define FALSE 0
#define TRUE !(FALSE)

#define MAX_ARGS 20
#define MAX_ARG_SIZE 20

#define READ_END	0
#define WRITE_END	1

void execute(char **argv, int begin, int end);

void chain(int argc, char** argv);

void parser(void);