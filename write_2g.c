//./write /home/fjywade/traces/msr-cowcache/mds_0.csv.data 
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

int main(int argc,char *argv[]){
    clock_t start, finish;
    double  duration;
    start = clock();
    FILE *write_2g = fopen("/home/fjywade/r5ssd/2g_file", "w");
    //setbuf(write_2g, NULL);
    if(!write_2g){
        printf("fopen error!\n");
        exit(1);
    }
    char cache[4096];
    for(int i = 0; i < 4096; i++)
        cache[i] = i%3+65;
    for(int i = 0; i < 524288; i++){
        fwrite(cache, sizeof(char), 4096, write_2g);
        fflush(write_2g);
    }
    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    printf( "%f seconds to recovery deltafile\n", duration);
    //int fd = fileno(write_2g); //获取文件描述符
    //fsync(write_2g); //强制写硬盘
    //fflush(write_2g);
    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    printf( "%f seconds to recovery deltafile\n", duration);
    return 0;
}