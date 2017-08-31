#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include "head.h"

int main(int argc,char *argv[])
{   
    clock_t start, finish;
    double  duration;
    start = clock();
    FILE *in_ssd;
    in_ssd = fopen(ssd_file, "r");
    if(in_ssd == NULL){
        printf("in_ssd file  can not be opened.\n");
        exit(1);//结束程序的执行
    }
    uint32_t delta_clusters;
    fread((void *) (&delta_clusters), sizeof(uint32_t), 1, in_ssd);
    Entry *entries = (Entry*)malloc(delta_clusters*sizeof(Entry));
    fread(entries, sizeof(Entry), delta_clusters, in_ssd);
    free(entries);
    fclose(in_ssd);
    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    printf( "%f seconds to recovery entriesfile\n", duration);
    
    return 0;
}