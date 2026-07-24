#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double *readCSV(const char *file, int *rows, int *cols)
{
    FILE *fp = fopen(file,"r");

    if(fp==NULL){
        printf("Erro.\n");
        exit(1);
    }

    char line[4096];

    *rows=0;
    *cols=0;

    while(fgets(line,4096,fp))
    {
        if(*rows==0)
        {
            char *tmp=strdup(line);
            char *tok=strtok(tmp,",");
            while(tok)
            {
                (*cols)++;
                tok=strtok(NULL,",");
            }
            free(tmp);
        }

        (*rows)++;
    }

    rewind(fp);

    double *data=malloc((*rows)*(*cols)*sizeof(double));

    int r=0;

    while(fgets(line,4096,fp))
    {
        char *tok=strtok(line,",");
        int c=0;

        while(tok)
        {
            data[r*(*cols)+c]=atof(tok);
            tok=strtok(NULL,",");
            c++;
        }

        r++;
    }

    fclose(fp);

    return data;
}

void writeCSV(
        const char *file,
        double *points,
        int *labels,
        int rows,
        int cols,
        double *centroids,
        int K)
{
    FILE *fp = fopen(file, "w");

    if(fp == NULL)
        return;

    /* Pontos e cluster */

    fprintf(fp, "Ponto");

    for(int j = 0; j < cols; j++)
        fprintf(fp, ",X%d", j + 1);

    fprintf(fp, ",Cluster\n");

    for(int i = 0; i < rows; i++)
    {
        fprintf(fp, "%d", i);

        for(int j = 0; j < cols; j++)
            fprintf(fp, ",%.6f", points[i * cols + j]);

        fprintf(fp, ",%d\n", labels[i]);
    }

    /* Centroides */

    fprintf(fp, "\nCentroides\n");

    for(int k = 0; k < K; k++)
    {
        fprintf(fp, "C%d", k);

        for(int j = 0; j < cols; j++)
            fprintf(fp, ",%.6f", centroids[k * cols + j]);

        fprintf(fp, "\n");
    }

    fclose(fp);
}
