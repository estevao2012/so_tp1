#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int pp[2];
int pid;

/************************** ATENÇÃO: **************************************
 * Este programa é apenas um exemplo de como se usar dup2 para redirecionar
 * a entrada e a saída padrão de um programa. Por brevidade, ele foi
 * escrito sem maiores preocupações com o teste dos valores de retorno das
 * chamadas de sistema usadas, mas qualquer código completo desenvolvido
 * deve obrigatoriamente fazer o tratamento adequado de erro em todos os
 * casos em que isso for necessário.
 **************************************************************************/

int main(int argc , char* argv[])
{
    int pid;

    if ( pipe(pp) < 0 ) exit(1);

    if ( ( pid = fork() ) < 0 ) exit(1);
    if (pid == 0) {     /* filho */
       close(pp[0]);    /* fecha saída do pipe (só vai escrever) */
       dup2(pp[1], 1);  /* associa entrada do pipe com saída padrão */
       close(pp[1]);    /* fecha pp[1], pois se tornou redundante */
       execlp(argv[2],argv[2],NULL);
    } else {            /* pai */
       close( pp[1] );  /* fecha entrada do pipe (só vai ler) */
       dup2(pp[0], 0);  /* associa saída do pipe com entrada padrão */
       close(pp[0]);    /* fecha pp[0], pois se tornou redundante */
       execlp(argv[1],argv[1],NULL);
    }
    return 0;
}
