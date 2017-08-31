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

    FILE *in_ssd, *out_delta, *in_base;
    in_ssd = fopen(ssd_file, "r");
    if(in_ssd == NULL){
        printf("in_ssd file  can not be opened.\n");
        exit(1);//结束程序的执行
    }
    in_base = fopen(base_file, "r");
    if(in_base == NULL){
        printf("in_base file  can not be opened.\n");
        exit(1);//结束程序的执行
    }
    out_delta = fopen(delta_file, "w");
    if(out_delta == NULL){
        printf("out_delta file  can not be opened.\n");
        exit(1);//结束程序的执行
    }
    char cache_read_Buf[5000], clusterBuf[65540];
    memset(cache_read_Buf, 0, 5000*sizeof(char));
    //printf("clusdddd: %s\n", clusterBuf);
    memset(clusterBuf, 0, 65540*sizeof(char));
    uint32_t one = 1, delta_clusters, offset = entry_size * sizeof(Entry);
    long cluster_offset, cache_offset;
    fread((void *) (&delta_clusters), sizeof(uint32_t), 1, in_ssd);
    Entry *entries = (Entry*)malloc(delta_clusters*sizeof(Entry));
    fread(entries, sizeof(Entry), delta_clusters, in_ssd);
    for(int i = 0; i < delta_clusters; i++){
        cluster_offset = entries[i].cluster_offset;
        cluster_offset *= cluster_size;
        for(int j = 0; j < 16; j++){
            cache_offset = entries[i].cache_offset[j];
            cache_offset *= cache_size;
            cache_offset += offset;
            if(entries[i].bitmap_dirty & (one << j)){
                fseek(in_ssd, cache_offset, SEEK_SET);
                fread(cache_read_Buf, sizeof(char), cache_size, in_ssd);
                memcpy(clusterBuf + j * cache_size, cache_read_Buf, cache_size);
            }else{
                fseek(in_base, cluster_offset + j * cache_size, SEEK_SET);
                fread(cache_read_Buf, sizeof(char), cache_size, in_base);
                memcpy(clusterBuf + j * cache_size, cache_read_Buf, cache_size);
            }
        }
        //}
        fwrite(clusterBuf, sizeof(char), cluster_size, out_delta);
    }
    free(entries);
    fclose(in_ssd);
    fclose(out_delta);
    fclose(in_base);
    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    printf( "%f seconds to recovery deltafile\n", duration);
    return 0;
}