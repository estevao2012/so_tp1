void gera_pipe(int num_args ,char *argv[]){
    int pp[2];
    int pid;

    if ( pipe(pp) < 0 ) exit(1);

    if ( ( pid = fork() ) < 0 ) exit(1);
    if (pid == 0) {     /* filho */
       //close(pp[0]);    /* fecha saída do pipe (só vai escrever) */
       //dup2(pp[1], 1);  /* associa entrada do pipe com saída padrão */
       //close(pp[1]);    /* fecha pp[1], pois se tornou redundante */
       execlp(argv[1],argv[1],NULL);
    } else {            /* pai */
       wait();
       //teste_alert();
       //close( pp[1] );  /* fecha entrada do pipe (só vai ler) */
       //dup2(pp[0], 0);  /* associa saída do pipe com entrada padrão */
       //close(pp[0]);    /* fecha pp[0], pois se tornou redundante */
       //execlp(argv[1],argv[1],NULL);
    }
}

char* parse_commands(char comando){
}
