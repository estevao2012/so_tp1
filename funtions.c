int countWords(char *argv[]){
    int i=0;
    while(argv[i]!= NULL){
        i++;
    }
    return i;
}


void splitVetor(char *argv[] , int posSplit ,  char *argvIn[] ,char *argvResto[]){

    int i,j,argc;

    argc = countWords(argv);

    for(i = 0 ; i < posSplit ; i++)
        argvIn[i] = argv[i];
    argvIn[i] = NULL;

    for(j=0,i++; i < argc; i++,j++){ 
        argvResto[j] = argv[i];
    }    
    argvResto[j] = NULL;
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
   } 
}

void fechaPipes(int *pipes){
    int i;
    for(i=0;i < sizeof(pipes) ;i++)close(pipes[i]);
}

void inserePrimeiroProcesso(int *pipes,char **args,int qntPipes){
    dup2(pipes[1], 1);
    fechaPipes(pipes);
    if(verificaLeituraEscrita(args) == 0)
    execvp(args[0], args);   
    _exit(EXIT_SUCCESS);  
}
void inserePipe(int *pipes,char *args[64][64],char **argv,int qntPipes, int portaEntrada, int portaSaida,int indiceAtual){
    
   // printf("%d\n", qntPipes );
    // for(i = 1 ; i < qntPipes ; i++){
    //     if (fork() == 0){  
    //         dup2(pipes[portaEntrada], 0);
    //         dup2(pipes[portaSaida], 1);
    //         fechaPipes(pipes);
    //         if(verificaLeituraEscrita(args[i]) == 0)
    //            execvp(args[i][0], args[i]);  
    //     }
    //     portaEntrada+=2;
    //     portaSaida+=2;
    // } 

    dup2(pipes[portaEntrada], 0);
    dup2(pipes[portaSaida], 1);
    fechaPipes(pipes);
    if(verificaLeituraEscrita(args[indiceAtual]) == 0)
       execvp(args[indiceAtual][0], args[indiceAtual]);  
      
}

void processoCom2Pipes(char *argv[],int qntPipes){

    int i,status;
    int pipes[qntPipes*2];
    int portaEntrada,portaSaida;
    int posPipe;
    char *args[64][64];
    pid_t pid[3];
    int n;

    portaEntrada = 0;
    portaSaida = 3;

    for( i = 0 ; i <= qntPipes ; i+=2 ) pipe(pipes + i); 

    for( i = 0 ; i < qntPipes ; i++ ){
        posPipe = encontraPipe(argv,0);  
        splitVetor(argv,posPipe,args[i],argv);
    }

    pid[0] = fork();
    if (pid[0] == 0){  
        dup2(pipes[1], 1);
        for(i=0;i < qntPipes*2 ;i++)close(pipes[i]);
        //if(verificaLeituraEscrita(args[0]) == 0)
        execvp(args[0][0], args[0]);   
        _exit(EXIT_SUCCESS); 
    }

    // for(i = 1 ; i < qntPipes ; i++){
    pid[1] = fork();
    if (pid[1] == 0){  
        dup2(pipes[0], 0);
        dup2(pipes[3], 1);
        for(i=0;i < qntPipes*2 ;i++)close(pipes[i]);
        // if(verificaLeituraEscrita(args[i]) == 0)
           execvp(args[i][0], args[i]); 
        _exit(EXIT_SUCCESS);  
    }
        portaEntrada+=2;
        portaSaida+=2;
    // } 
    pid[2] =fork(); 
    if( pid[2] == 0){ 
        dup2(pipes[2], 0);
        for(i=0;i < qntPipes*2 ;i++)close(pipes[i]);
        //if(verificaLeituraEscrita(argv) == 0)
            execvp(argv[0], argv);
        _exit(EXIT_SUCCESS);
    }
    
    
    for(i=0;i < qntPipes*2 ;i++)close(pipes[i]);
    
    qntPipes++;
    for(n=0; n<3; n++)
    {
            waitpid(pid[n], &status, 0);
            printf("Child %d exited with status %d\n", n, WEXITSTATUS(status));
    }

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

