#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "funtions.c"


int main(int argc , char* argv[])
{
    char* fim = "fim";
    char* vazio = "";
    char** args;
    char argumentos[512];
    int pp[2];
    int pid;
    int i;
    int argcAll;
    char* argvIn[64];
    char* argvOut[64];
    int fileOpen;

    fileOpen = open(argv[1],O_RDONLY);


    /* Open File For Standard Input */

    if( ( fileOpen ) == NULL ){
        printf( "Cannot Open File: %s", fileOpen ) ;
        exit( 1 ) ;
    }

    if( fileOpen != 0 ){
        dup2( fileOpen, 0 ); /* Make fp stdin */
        close( fileOpen ) ; /* Close original fp */
    }

    //Valida o que for escrito pelo usuario
    printf("Quais são suas ordens? ");
    while(fgets(argumentos,512 , stdin) != NULL){

        strtok(argumentos,"\n");
        args = parseCommands(argumentos ,&argcAll);

        if(validaPipe(argcAll,args,argvIn,argvOut) == 1){

            if ( ( pid = fork() ) < 0 ) exit(1);

            if (pid == 0) {     /* filho */
               executaProcessoComPipe(argvIn,argvOut); // Como o processo precisa ser fechado , é necessario abrir um novo processo para ele não finalizar o bash
            } else {            /* pai */
               wait();
            }
        }
        else
          executaProcesso(args);


        if(strcmp(argumentos,fim) == 0) break;
        printf("Quais são suas ordens? ");
        //printf("Quais são suas ordens? ");
        //fgets(argumentos,512 , stdin);
        //strtok(argumentos,"\n");




    }

    return 0;
}
