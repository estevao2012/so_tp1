all: compila executa

compila:
	gcc -c main.c
	gcc main.c -o tp1
 
executa:
	./tp1	

valgrind:
	@echo "Executando o valgrind"
	valgrind ./tp1
