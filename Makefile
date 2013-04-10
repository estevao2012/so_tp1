all: compila executa

compila:
	gcc -c main.c -Wall
	gcc main.c -o shellso 
 
executa:
	./shellso

valgrind:
	@echo "Executando o valgrind"
	valgrind ./bashso
