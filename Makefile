all: compila executa

compila:
	gcc -c main.c proc.c
	gcc main.c -o bashso
	gcc proc.c -o proc
 
executa:
	./bashso

valgrind:
	@echo "Executando o valgrind"
	valgrind ./bashso
