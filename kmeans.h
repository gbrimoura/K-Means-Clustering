#ifndef KMEANS_H
#define KMEANS_H

void kmeansMPI(
        double *localData,
        int localRows,
        int cols,
        int K,
        int maxIter,
        int *labels,
        double *centroids);

#endif
