//./write /home/fjywade/traces/msr-cowcache/mds_0.csv.data 
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include "head.h"

int main(int argc,char *argv[]){
    clock_t start, finish;
    double  duration;
    start = clock();
    FILE *read_2g = fopen(ssd_file, "r"), *write_2g = fopen(delta_file, "w");
    //setbuf(write_2g, NULL);
    if(!write_2g || !read_2g){
        printf("fopen error!\n");
        exit(1);
    }
    char cache[4096];
    for(int i = 0; i < 4096; i++)
        cache[i] = i%3+65;
    for(int i = 0; i < 524288; i++){
        fread(cache, sizeof(char), 4096, read_2g);
        fwrite(cache, sizeof(char), 4096, write_2g);
    }
    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    printf( "%f seconds to recovery deltafile\n", duration);
    return 0;
}