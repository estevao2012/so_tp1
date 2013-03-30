void executaProcesso(int num_args ,char *argv[]){
    int pp[2];
    int pid;

    if ( pipe(pp) < 0 ) exit(1);

    if ( ( pid = fork() ) < 0 ) exit(1);
    if (pid == 0) {
       execvp(argv[0],argv);
       _exit(1);
    } else {            /* pai */
       wait();
       exit(1);
    }
}

char** parseCommands(char* comando){

    int i = 0;
    char* delims = " ";
    char* result = NULL;
    char* words[512];
    result = strtok(comando,delims);
    while( result != NULL ) {
        words[i] = result;
        result = strtok( NULL , delims );
        i++;
    }
    words[i] = NULL;
    return words;

}
