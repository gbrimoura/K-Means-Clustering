#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "io.h"
#include "kmeans.h"

int main(int argc,char **argv)
{
    MPI_Init(&argc,&argv);

    int rank,size;

    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    int rows,cols;

    double *data=NULL;

    if(rank==0)
        data=readCSV("dados.csv",&rows,&cols);

    MPI_Bcast(&rows,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&cols,1,MPI_INT,0,MPI_COMM_WORLD);

    int *counts=malloc(size*sizeof(int));
    int *disp=malloc(size*sizeof(int));

    int base=rows/size;
    int extra=rows%size;

    int i;

    disp[0]=0;

    for(i=0;i<size;i++)
    {
        counts[i]=(base+(i<extra))*cols;

        if(i>0)
            disp[i]=disp[i-1]+counts[i-1];
    }

    int localRows=counts[rank]/cols;

    double *localData=
    malloc(counts[rank]*sizeof(double));

    MPI_Scatterv(
        data,
        counts,
        disp,
        MPI_DOUBLE,
        localData,
        counts[rank],
        MPI_DOUBLE,
        0,
        MPI_COMM_WORLD);

    int K=3;

    double *centroids=
    malloc(K*cols*sizeof(double));

    if(rank==0)
        for(i=0;i<K*cols;i++)
            centroids[i]=data[i];

    MPI_Bcast(
        centroids,
        K*cols,
        MPI_DOUBLE,
        0,
        MPI_COMM_WORLD);

    int *labels=
    malloc(localRows*sizeof(int));

    kmeansMPI(
        localData,
        localRows,
        cols,
        K,
        100,
        labels,
        centroids);

    MPI_Finalize();

    return 0;
}
