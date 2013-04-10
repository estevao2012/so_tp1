#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include "funtions.c"

int main(int argc , char* argv[])
{
    char* fim = "fim"; 
    char** args;
    char argumentos[512];  
    int qntPipes,tmp_entrada,tmp_saida,bg=0; 
    char* prompt = "Quais são suas ordens ? ";
    pid_t processo,w;
    int fileOpen,status;

    fileOpen = open(argv[1],O_RDONLY);

    /* Open File For Standard Input */
    if( ( fileOpen ) == 0 ){
        printf( "Cannot Open File ") ;
        exit( 1 ) ;
    }

    if( fileOpen != 0 ){
        dup2( fileOpen, 0 ); /* Make fp stdin */
        close( fileOpen ) ; /* Close original fp */
    }

    //Valida o que for escrito pelo usuario
    printf("%s",prompt);

    while(fgets(argumentos,512,stdin) != NULL){

        strtok(argumentos,"\n");

        args = parseCommands(argumentos);

        localizaRedirecionamentos(args,&tmp_entrada,&tmp_saida,&bg);
        if(bg != 0){
            removeSimboloBackground(args,bg);
        }
        qntPipes = quantidadePipes(args); 

        processo = fork();
        if (processo == 0){
            if(bg != 0)printf("Child PID is %ld\n", (long) getpid());
            executarComandos(args,qntPipes);
            _exit(EXIT_SUCCESS);
        }else{
            if(bg != 0){
                do {
                    w = waitpid(processo, &status, WUNTRACED | WCONTINUED);
                    if (w == -1) {
                        perror("waitpid");
                        exit(EXIT_FAILURE);
                    }

                    if (WIFEXITED(status)) {
                        printf("Concluido status=%d\n", WEXITSTATUS(status));
                    } else if (WIFSIGNALED(status)) {
                        printf("killed by signal %d\n", WTERMSIG(status));
                    } else if (WIFSTOPPED(status)) {
                        printf("stopped by signal %d\n", WSTOPSIG(status));
                    } else if (WIFCONTINUED(status)) {
                        printf("continued\n");
                    }
                } while (!WIFEXITED(status) && !WIFSIGNALED(status));
            }else{
                wait(NULL);
                //exit(EXIT_SUCCESS);
            }
        }            
        

        if(strcmp(argumentos,fim) == 0) exit(EXIT_SUCCESS);
        printf("%s",prompt);

    }

    return 0;
}
