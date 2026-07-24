#ifndef IO_H
#define IO_H

double *readCSV(const char *file, int *rows, int *cols);

void writeCSV(
        const char *file,
        double *points,
        int *labels,
        int rows,
        int cols,
        double *centroids,
        int K);

#endif
