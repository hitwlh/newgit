#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "head.h"

int main(int argc,char *argv[])
{   
    FILE *in_delta = fopen(delta_file, "r");
    if(in_delta == NULL){//打开操作不成功
        printf("in_delta file can not be opened.\n");
        exit(1);//结束程序的执行
    }
    FILE *in_ssd = fopen(ssd_file, "r");
    if(in_ssd == NULL){
        printf("in_ssd file  can not be opened.\n");
        exit(1);//结束程序的执行
    }
    FILE *out_base = fopen(base_file, "r+");
    if(out_base == NULL){
        printf("out_base file  can not be opened.\n");
        exit(1);//结束程序的执行
    }
    uint32_t delta_clusters;
    char clusterBuf[65540];
    memset(clusterBuf, 0, 65540*sizeof(char));
    fread((void *) (&delta_clusters), sizeof(uint32_t), 1, in_ssd);
    Entry *entries = (Entry*)malloc(delta_clusters*sizeof(Entry));
    fread(entries, sizeof(Entry), delta_clusters, in_ssd);
    long cluster_offset, cache_offset;
    for(int i = 0; i < delta_clusters; i++){
        fread(clusterBuf, sizeof(char), cluster_size, in_delta);
        //printf("In cluster %d\n%s", i, clusterBuf);
        cluster_offset = entries[i].cluster_offset;
        cluster_offset *= cluster_size;
        fseek(out_base, cluster_offset, SEEK_SET);
        fwrite(clusterBuf, sizeof(char), cluster_size, out_base);
    }
    free(entries);
    fclose(in_delta);
    fclose(in_ssd);
    fclose(out_base);
    return 0;
}