#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "funtions.c"


int main(int argc , char* argv[])
{


    char* argvIn[64];
    char* argvOut[64];

    if(validaPipe(argc,argv,argvIn,argvOut) == 1)
      executaProcessoComPipe(argvIn,argvOut);
    else
      executaProcesso(argv);

    return 0;
}
