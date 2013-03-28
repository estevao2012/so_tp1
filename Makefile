all: compila_seq compila_thread

compila_seq:
  gcc -c -pthread tp4_seq.c tp.c lista.c grafo.c -c
	gcc -pthread tp4_seq.c tp.c lista.c grafo.c -o tp4_seq

compila_thread:
	gcc -c -pthread tp4_threads.c tp.c lista.c grafo.c
	gcc -pthread tp4_threads.c tp.c lista.c grafo.c -o  tp4_threads

executa:
	@echo "Executando o arquivo"
	./tp0 $(IN) $(OUT)
	@echo "\n"
	@echo "Abrindo o arquivo de saida"

valgrind:
	@echo "Executando o valgrind"
	valgrind ./tp0 $(IN) $(OUT)
gera_teste:
	@echo "gerando teste"
	gcc gerador_matriz.c -o gerar_matriz
	./gerar_matriz
