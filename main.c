#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "io.h"
#include "kmeans.h"

#define K 3
#define MAX_ITER 100

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rows = 0;
    int cols = 0;

    double *data = NULL;

    /* Apenas o processo 0 lê o arquivo */

    if(rank == 0)
    {
        data = readCSV("dados.csv", &rows, &cols);
    }

    /* Envia dimensões para todos */

    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

    /* Vetores para Scatterv */

    int *counts = malloc(size * sizeof(int));
    int *displs = malloc(size * sizeof(int));

    int base = rows / size;
    int extra = rows % size;

    displs[0] = 0;

    for(int i = 0; i < size; i++)
    {
        int linhas = base + (i < extra);

        counts[i] = linhas * cols;

        if(i > 0)
            displs[i] = displs[i - 1] + counts[i - 1];
    }

    int localRows = counts[rank] / cols;

    double *localData = malloc(counts[rank] * sizeof(double));

    MPI_Scatterv(
        data,
        counts,
        displs,
        MPI_DOUBLE,
        localData,
        counts[rank],
        MPI_DOUBLE,
        0,
        MPI_COMM_WORLD);

    /* Inicialização dos centroides */

    double *centroids = malloc(K * cols * sizeof(double));

    if(rank == 0)
    {
        for(int i = 0; i < K; i++)
            for(int j = 0; j < cols; j++)
                centroids[i * cols + j] = data[i * cols + j];
    }

    MPI_Bcast(
        centroids,
        K * cols,
        MPI_DOUBLE,
        0,
        MPI_COMM_WORLD);

    /* Vetor de clusters locais */

    int *labels = malloc(localRows * sizeof(int));

    /* Executa K-Means */

    kmeansMPI(
        localData,
        localRows,
        cols,
        K,
        MAX_ITER,
        labels,
        centroids);

    /* Preparação para Gatherv */

    int *recvCounts = NULL;
    int *recvDispls = NULL;
    int *allLabels = NULL;

    if(rank == 0)
    {
        recvCounts = malloc(size * sizeof(int));
        recvDispls = malloc(size * sizeof(int));

        recvDispls[0] = 0;

        for(int i = 0; i < size; i++)
        {
            recvCounts[i] = counts[i] / cols;

            if(i > 0)
                recvDispls[i] = recvDispls[i - 1] + recvCounts[i - 1];
        }

        allLabels = malloc(rows * sizeof(int));
    }

    /* Reúne os clusters */

    MPI_Gatherv(
        labels,
        localRows,
        MPI_INT,
        allLabels,
        recvCounts,
        recvDispls,
        MPI_INT,
        0,
        MPI_COMM_WORLD);

    /* Apenas o processo 0 grava o resultado */

    if(rank == 0)
    {
        writeCSV(
            "resultado.csv",
            data,
            allLabels,
            rows,
            cols,
            centroids,
            K);

        printf("Resultado salvo em resultado.csv\n");
    }

    /* Liberação de memória */

    free(localData);
    free(labels);
    free(centroids);
    free(counts);
    free(displs);

    if(rank == 0)
    {
        free(data);
        free(allLabels);
        free(recvCounts);
        free(recvDispls);
    }

    MPI_Finalize();

    return 0;
}
