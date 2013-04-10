void removeSimboloBackground(char *argv[] , int posSplit){
    int i;
    for(i = 0 ; i < posSplit ; i++)
        argv[i] = argv[i];
    argv[i] = NULL;

}

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


void fazLeitura(char* argvIn[],char* argvOut[]){

    int pp[2];
    char *buf;
    int fileOpen;

    pipe(pp);
    if(fork()==0){
        fileOpen = open(argvOut[0],O_RDONLY);
        if(fileOpen == -1){
            perror("errno");
            exit(EXIT_FAILURE);
        }

        close(pp[0]);   
        dup2(fileOpen,STDIN_FILENO);
        dup2(pp[1],STDOUT_FILENO);   
        close(pp[1]);
        close(fileOpen);

        while(read(STDIN_FILENO,&buf,1)>0)
            write(STDOUT_FILENO,&buf,1);
        _exit(EXIT_SUCCESS);     
        
    }else{  
        close(pp[1]);   
        dup2(pp[0],STDIN_FILENO);
        close(pp[0]);         
        execvp(argvIn[0],argvIn);
        exit(EXIT_SUCCESS);
    }
}

void fazEscrita(char* argvIn[],char* argvOut[]){

    int pp[2]; 
    char *buf;
    int fileOpen;
    if((fileOpen = open(argvOut[0],O_WRONLY | O_CREAT , 0644)) != -1){
        pipe(pp);
        if(fork()==0){
            close(pp[1]);
            dup2(fileOpen,1);
            dup2(pp[0],0);
            close(pp[0]);   

            while(read(STDIN_FILENO,&buf,1)>0)
                write(STDOUT_FILENO,&buf,1);

            _exit(EXIT_SUCCESS);
        }else{  
            close(pp[0]);   
            dup2(pp[1],1);
            close(pp[1]); 
            execvp(argvIn[0],argvIn);
            exit(EXIT_SUCCESS); 
        }
    }else{
        perror("errno");
        exit(1);
    }

}


void LeDepoisfazEscrita(char* argvIn[],char* arquivoIn,char* arquivoOut){

    int pp[2];
    char *buf;
    int fileOpen,fileOut;

    pipe(pp);
    if(fork()==0){
        
        fileOpen = open(arquivoIn,O_RDONLY);

        if(fileOpen == -1){
            perror("errno");
            exit(EXIT_FAILURE);
        }

        close(pp[0]);   
        dup2(fileOpen,STDIN_FILENO);
        dup2(pp[1],STDOUT_FILENO);   
        close(pp[1]);
        close(fileOpen);   

        while(read(STDIN_FILENO,&buf,1)>0)
            write(STDOUT_FILENO,&buf,1);
    
        _exit(EXIT_SUCCESS);            
        
    }else{  
        wait(NULL); 
        fileOut = open(arquivoOut,O_WRONLY | O_CREAT , 0644);
        if(fileOut == -1) {
            perror("errno");
            exit(EXIT_FAILURE);
        }

        dup2(pp[0],STDIN_FILENO);
        dup2(fileOut,1);
        close(pp[1]);
        close(pp[0]);
        close(fileOut);   

        execvp(argvIn[0],argvIn);
        exit(EXIT_SUCCESS);
    }
}

void localizaRedirecionamentos(char *argv[],int* posLeitura, int* posEscrita,int* posBackground){
    int i,argc;
    argc = countWords(argv);

    for(i=0; i < argc ; i++ ){
        if(strcmp(argv[i], (char*)"<=") == 0) *posLeitura = i;
        if(strcmp(argv[i], (char*)"=>") == 0) *posEscrita = i;
        if(strcmp(argv[i], (char*)"&") == 0) *posBackground = i;
    }
}

int verificaSinal(char *argv[]){

    int ler=-1,escrever=-1,background=-1;
    char* argvIn[64];
    char* argvOut[64]; 
    int pp[2];

    localizaRedirecionamentos(argv,&ler,&escrever,&background);

    pipe(pp);

    if(ler != -1 && escrever != -1){
        if(ler > escrever){
            splitVetor(argv,escrever,argvIn,argvOut);
            LeDepoisfazEscrita(argvIn,argvOut[2],argvOut[0]);
        }else{
            splitVetor(argv,ler,argvIn,argvOut);
            LeDepoisfazEscrita(argvIn,argvOut[0],argvOut[2]);
        }
    }else{    
        if( ler != -1){
            splitVetor(argv,ler,argvIn,argvOut);
            if(fork() == 0)fazLeitura(argvIn,argvOut);
            else{
                wait(NULL);
                exit(EXIT_SUCCESS);
            }
        }

        if( escrever != -1){
            splitVetor(argv,escrever,argvIn,argvOut);
            if(fork() == 0)fazEscrita(argvIn,argvOut);
            else{
                wait(NULL);
                exit(EXIT_SUCCESS);
            }
        }
    }

    if(ler == -1 && escrever == -1)return 0;
    else return 1;
}

void executaUmProcesso(char *argv[]){

   int pp[2];
   int pid;

   if ( pipe(pp) < 0 ) exit(1);

   if ( ( pid = fork() ) < 0 ) exit(1);
   if (pid == 0) {
        if(verificaSinal(argv) == 0)
            execvp(argv[0],argv);
        _exit(EXIT_SUCCESS);
   }else{
        wait(NULL);
        exit(EXIT_SUCCESS);
   }
}


void executarComandos(char *argv[],int qntPipes){

    int i,j,status;
    int pipes[qntPipes*2];
    int portaEntrada,portaSaida;
    int posPipe;
    char *args[64][64]; 
    int n;

    portaEntrada = 0;
    portaSaida = 3;

    if(qntPipes == 0){
        executaUmProcesso(argv);
        exit(EXIT_SUCCESS);
    }


    for( i = 0 ; i < (qntPipes*2) ; i+=2 ){ pipe(pipes + i); } 

    for( i = 0 ; i < qntPipes ; i++ ){
        posPipe = encontraPipe(argv,0);  
        splitVetor(argv,posPipe,args[i],argv);
    }
 
    if (fork() == 0){  
        dup2(pipes[1], 1);
        
        for(i=0;i < qntPipes*2 ;i++)close(pipes[i]);
        
        if(verificaSinal(args[0]) == 0)
            execvp(args[0][0], args[0]);   
        _exit(EXIT_SUCCESS); 
    }

    for(j = 1 ; j < qntPipes ; j++,portaEntrada+=2,portaSaida+=2){ 
	    
        if (fork() == 0){  
            dup2(pipes[portaEntrada], 0);
            dup2(pipes[portaSaida], 1);

            for(i=0;i < qntPipes*2 ;i++)close(pipes[i]);
            
            if(verificaSinal(args[i]) == 0)
                execvp(args[j][0], args[j]); 
            
            exit(EXIT_SUCCESS);  
	    }
    } 
    
    if( fork() == 0){ 
        
        dup2(pipes[portaEntrada], 0);
        
        for(i=0;i < qntPipes*2 ;i++)close(pipes[i]);
        
        if(verificaSinal(argv) == 0)
            execvp(argv[0], argv);
        
        _exit(EXIT_SUCCESS);
    }
    
    
    for(i=0;i < qntPipes*2 ;i++)close(pipes[i]);
    
    qntPipes++;
    for(n=0; n<qntPipes; n++){wait(&status);}

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

