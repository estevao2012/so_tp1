int countWords(char *argv[]){
    int i=0;
    while(argv[i]!= NULL){
        i++;
    }
    return i;
}

void splitVetor(char *argv[] , int posSplit ,  char *argvIn[] ,char *argvOut[]){

    int i,argc;

    argc = countWords(argv);

    for(i=0;i<posSplit;i++)
        argvIn[i] = argv[i];
    argvIn[i] = NULL;

    for( i = 0 ; i < (argc-(posSplit+1)) ; i++){
        if(strcmp(argv[i+(posSplit+1)],(char*)"|") == 0) break;
        argvOut[i] = argv[i+(posSplit+1)];
    }    
    argvOut[i] = NULL;

}

void restoVetor(char *argv[] , int posSplit ,  char *argvIn[]){

    int i,argc;

    argc = countWords(argv);

    for(i=posSplit;i<argc;i++)
        argvIn[i] = argv[i];
    argvIn[i] = NULL;

}

int fazLeitura(char* argvIn[],char* argvOut[]){

    int pp[2];
    int pid;
    char argumentos[512];
    int fileOpen;

    if((fileOpen = open(argvOut[0],O_RDONLY)) != -1){
        dup2(fileOpen,0);
        close(fileOpen);
        execvp(argvIn[0],argvIn);
        _exit(EXIT_SUCCESS);
    }else{
        perror("errno");
        _exit(1);
    }
}

int fazEscrita(char* argvIn[],char* argvOut[]){

    int pp[2];
    int pid;
    char argumentos[512];
    int fileOpen;

    if((fileOpen = open(argvOut[0],O_WRONLY | O_CREAT , 0644)) != -1){
        dup2(fileOpen,1);
        close(fileOpen);
        execvp(argvIn[0],argvIn);
        _exit(EXIT_SUCCESS);
    }else{
        perror("errno");
        _exit(1);
    }

}

void localizaRedirecionamentos(char *argv[],int* posLeitura, int* posEscrita){
    int i,argc;
    argc = countWords(argv);

    for(i=0; i < argc ; i++ ){
        if(strcmp(argv[i], (char*)"<=") == 0) *posLeitura = i;
        if(strcmp(argv[i], (char*)"=>") == 0) *posEscrita = i;
    }
}

int verificaLeituraEscrita(char *argv[]){

    int ler=-1,escrever=-1;
    char* argvIn[64];
    char* argvOut[64];

    localizaRedirecionamentos(argv,&ler,&escrever);

    if( ler != -1){
        splitVetor(argv,ler,argvIn,argvOut);
        fazLeitura(argvIn,argvOut);
    }

    if( escrever != -1){
        splitVetor(argv,escrever,argvIn,argvOut);
        fazEscrita(argvIn,argvOut);
    }
    if(ler == -1 && escrever == -1)return 0;
    else return 1;
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

void executaProcessoComPipe(char *argv[],int qnt){
    
    int pp[2];
    int pid;

    int posPipe;
    char *argvIn[64];
    char *argvOut[64];
    char *resto[64];
        
    if(qnt > 0){
        posPipe = encontraPipe(argv,0); 
        splitVetor(argv,posPipe,argvIn,argvOut);    
        restoVetor(argv,posPipe,resto);
        argv = resto;

        if ( pipe(pp) < 0 ) exit(1);
        if ( ( pid = fork() ) < 0 ) exit(1);
        if (pid == 0) {     
   
            dup2(pp[1], 1); 
            dup2(pp[0], 0); 
            close(pp[0]);
            close(pp[1]);   

            //if(verificaLeituraEscrita(argvIn) == 0)
            executaProcessoComPipe(resto,qnt);
            execvp(argvIn[0],argvIn); //ls - Escreve | Envia
            qnt--;

            _exit(1);
        } else {            
            close( pp[1] ); 
            dup2(pp[0], 0); 
            close(pp[0]);   
            //if(verificaLeituraEscrita(argvOut) == 0)
            wait();
            execvp(argvOut[0],argvOut); //wc - Le | Recebe

        }
    }else execvp(argv[0],argv);

}


//Retorna a primeira posicao do pipe
int encontraPipe(char* argv[] ,int begin){

    int i,argc;
    argc = countWords(argv);
    for(i=begin;i<argc;i++)
        if(strcmp(argv[i], (char*)"|") == 0)
            return i;
    return 0;
}

int quantidadePipes(char* argv[]){

    int i ;
    int argc;
    int qntPipes=0;

    argc = countWords(argv);

    for(i=0;i<argc;i++)
        if(strcmp(argv[i], (char*)"|") == 0)
            qntPipes++;

    if(qntPipes != -1)
       return qntPipes;

    return 0;
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

