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
