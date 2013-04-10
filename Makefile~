all: compila executa

compila:
	gcc -c main.c -Wall
	gcc main.c -o bashso 
 
executa:
	./bashso

valgrind:
	@echo "Executando o valgrind"
	valgrind ./bashso
