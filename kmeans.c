#include <mpi.h>
#include <omp.h>
#include <stdlib.h>
#include <math.h>

void kmeansMPI(
        double *localData,
        int localRows,
        int cols,
        int K,
        int maxIter,
        int *labels,
        double *centroids)
{

    int i,j,k,it;

    for(it=0;it<maxIter;it++)
    {

#pragma omp parallel for private(j,k)
        for(i=0;i<localRows;i++)
        {
            double best=1e30;

            int cluster=0;

            for(k=0;k<K;k++)
            {
                double dist=0;

                for(j=0;j<cols;j++)
                {
                    double d=
                    localData[i*cols+j]
                    -centroids[k*cols+j];

                    dist+=d*d;
                }

                if(dist<best)
                {
                    best=dist;
                    cluster=k;
                }
            }

            labels[i]=cluster;
        }

        double *localSum=
        calloc(K*cols,sizeof(double));

        int *localCount=
        calloc(K,sizeof(int));

#pragma omp parallel for private(j)
        for(i=0;i<localRows;i++)
        {
            int c=labels[i];

#pragma omp critical
            {
                localCount[c]++;

                for(j=0;j<cols;j++)
                    localSum[c*cols+j]+=localData[i*cols+j];
            }
        }

        double *globalSum=
        calloc(K*cols,sizeof(double));

        int *globalCount=
        calloc(K,sizeof(int));

        MPI_Allreduce(
            localSum,
            globalSum,
            K*cols,
            MPI_DOUBLE,
            MPI_SUM,
            MPI_COMM_WORLD);

        MPI_Allreduce(
            localCount,
            globalCount,
            K,
            MPI_INT,
            MPI_SUM,
            MPI_COMM_WORLD);

        for(k=0;k<K;k++)
            if(globalCount[k]>0)
                for(j=0;j<cols;j++)
                    centroids[k*cols+j]=
                    globalSum[k*cols+j]/globalCount[k];

        free(localSum);
        free(globalSum);
        free(localCount);
        free(globalCount);
    }

}
