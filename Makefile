CC=mpicc

CFLAGS=-O3 -fopenmp

all:
	$(CC) $(CFLAGS) main.c kmeans.c io.c -o kmeans -lm
	chmod +x executar

clean:
	rm -f kmeans
