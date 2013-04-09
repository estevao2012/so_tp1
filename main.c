#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "funtions.c"


int main(int argc , char* argv[])
{
    char* fim = "fim"; 
    char** args;
    char argumentos[512];  
    int qntPipes; 
    char* prompt = "Quais são suas ordens ? ";

    int fileOpen;

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
 
        qntPipes = quantidadePipes(args); 

        if( qntPipes >= 1 ){
            if(qntPipes == 1){
                if (fork() == 0){  
                    processoComPipes(args,qntPipes);
                }else
                    wait();
            }
            else 
                processoComPipes(args,qntPipes); // Como o processo precisa ser fechado , é necessario abrir um novo processo para ele não finalizar o bash
        }
        else
          executaProcesso(args);

        if(strcmp(argumentos,fim) == 0) break;
        printf("%s",prompt);

    }

    return 0;
}
