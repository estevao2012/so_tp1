int countWords(char *argv[]){
    int i=0;
    while(argv[i]!= NULL){
        i++;
    }
    return i;
}

void splitVetor(char *argv[] , int posSplit , char *argvIn[] ,char *argvOut[]){

    int i,argc;

    argc = countWords(argv);

    for(i=0;i<posSplit;i++)
        argvIn[i] = argv[i];
    argvIn[i] = NULL;

    for( i = 0 ; i < (argc-(posSplit+1)) ; i++)
        argvOut[i] = argv[i+(posSplit+1)];
    argvOut[i] = NULL;

}

void fazLeitura(char* argvIn[],char* argvOut[]){

    int pp[2];
    int pid;
    char argumentos[512];

    FILE* fileOpen;

    if ( pipe(pp) < 0 ) exit(1);

    if ( ( pid = fork() ) < 0 ) exit(1);

    if (pid == 0) {    /* Child reads from pipe */
       close(pp[1]);          /* Close unused write end */

       dup2(pp[0],STDIN_FILENO);

       close(pp[0]);

       execvp(argvIn[0],argvIn);
       _exit(EXIT_SUCCESS);

    } else {            /* Parent writes argv[1] to pipe */
       close(pp[0]);   /* Close unused read end */
       fileOpen = fopen(argvOut[0],"r");

        while(fscanf(fileOpen,"%s",argumentos) != EOF){
            if(write(pp[1], &argumentos, strlen(argumentos)) < 1){
                perror("write");
                exit(EXIT_FAILURE);
            }
        }
        write(pp[1],"\n",1);

       fclose(fileOpen);

       close(pp[1]);          /* Reader will see EOF */
       wait(NULL);                /* Wait for child */

    }
}

int verificaLeituraEscrita(char *argv[]){

    int ler=-1,escrever=-1;
    int i,argc;
    char* argvIn[64];
    char* argvOut[64];

    argc = countWords(argv);

    for(i=0; i < argc ; i++ ){
        if(strcmp(argv[i], (char*)"<=") == 0) ler = i ;
        if(strcmp(argv[i], (char*)"=>") == 0) escrever = i;
    }

    if( ler != -1){
        splitVetor(argv,ler,argvIn,argvOut);
        fazLeitura(argvIn,argvOut);
        return 1;
    }
    if( escrever != -1){
        splitVetor(argv,escrever,argvIn,argvOut);
        return 1;
    }

    return 0;

}

void executaProcesso(char *argv[]){

   int pp[2];
   int pid;

   if ( pipe(pp) < 0 ) exit(1);

   if ( ( pid = fork() ) < 0 ) exit(1);
   if (pid == 0) {
     if(verificaLeituraEscrita(argv) == 0)
          execvp(argv[0],argv);
     _exit(1);
   } else {            /* pai */
      wait();
   }
}

void executaProcessoComPipe(char *argvIn[] ,char *argvOut[]){
    int pp[2];
    int pid;

    if ( pipe(pp) < 0 ) exit(1);

    if ( ( pid = fork() ) < 0 ) exit(1);

    if (pid == 0) {     /* filho */
       close(pp[0]);    /* fecha saída do pipe (só vai escrever) */
       dup2(pp[1], 1);  /* associa entrada do pipe com saída padrão */
       close(pp[1]);    /* fecha pp[1], pois se tornou redundante */

     //  verificaLeituraEscrita(argvIn);


       execvp(argvIn[0],argvIn); //ls - Escreve | Envia
       _exit(1);
    } else {            /* pai */
       close( pp[1] );  /* fecha entrada do pipe (só vai ler) */
       dup2(pp[0], 0);  /* associa saída do pipe com entrada padrão */
       close(pp[0]);    /* fecha pp[0], pois se tornou redundante */

       //verificaLeituraEscrita(argvIn);

       execvp(argvOut[0],argvOut); //wc - Le | Recebe
    }
}

char** parseCommands(char* comando){

    int i = 0;
    int j,h;
    char* delims = " ";
    char* result = NULL;
    char* words[512];
    char* f_words[512];

    result = strtok(comando,delims);
    while( result != NULL ) {
        words[i] = result;
        result = strtok( NULL , delims );
        i++;
    }

    j = i;
    words[i] = NULL;
    h=0;
    for(i=0;i<j;i++){
        result = strtok(words[i],"\t");
        while( result != NULL ){
            f_words[h] = result;
            result = strtok(NULL,"\t");
            h++;
        }
    }

    f_words[h] = NULL;
    return f_words;

}

int validaPipe(char* argv[], char** argvIn , char** argvOut){

    int i ,j=-1, marca_pipe = -1;
    int qntIn=0,qntOut=0;
    int argc;

    argc = countWords(argv);

    for(i=0;i<argc;i++)
        if(strcmp(argv[i], (char*)"|") == 0)
            marca_pipe = i;

    if(marca_pipe != -1) splitVetor(argv,marca_pipe,argvIn,argvOut);

    if(marca_pipe != -1) return 1;
    return 0;
}
