#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "funtions.c"


int main(int argc , char* argv[])
{
    char* fim = "fim";
    char* vazio = "";
    char argumentos[512];
    int pp[2];
    int pid;
    int i;

    //Valida o que for escrito pelo usuario
    printf("Quais são suas ordens?");
    fgets(argumentos,512 , stdin);
    strtok(argumentos,"\n");
    if(!strcmp(argumentos,fim) ) return 0;

    while( strcmp(argumentos,fim) || !strcmp(argumentos,vazio)){

        if ( pipe(pp) < 0 ) exit(1);
        if ( ( pid = fork() ) < 0 ) exit(1);

        if (pid == 0){
            execlp("./proc","./proc", argumentos); //Executa o bash da linha
            _exit(1);
        }else{
            wait();
            printf("Quais são suas ordens?");
            fgets(argumentos,512, stdin);
            strtok(argumentos,"\n");
        }

    }

    return 0;
}
