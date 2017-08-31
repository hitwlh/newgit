//./write /home/fjywade/traces/msr-cowcache/mds_0.csv.data 
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
//#include "head.h"


#define entry_size 18500000//1.1*1024*1024*1024k/64k
#define cluster_size 65536
#define cache_size 4096
typedef struct Entry {
    uint32_t cluster_offset;//max 1.1*1024*1024*1024k/64k
    uint32_t cache_offset[16];
    uint16_t bitmap_dirty;
    char visited;
    //uint16_t bitmap_exist;  /* not */
    //uint64_t reference;     /* not */
} Entry;

char trace_file[100] = "/home/fjywade/del/traces";
char base_file[100] = "/home/fjywade/r5ssd/1t-fl-e4.q2";
//char delta_file[100] = "/home/fjywade/r5ssd/d1t-fl-e4.q2";
char ssd_file[100] = "/dev/nvme0n1";



int write_delta(char *trace)
{
    char *delta_file = strcat(trace, "_delta");
    //char delta_file[100] = "/home/fjywade/r5ssd/d1t-fl-e4.q2";
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
    uint32_t one = 1, base_offset, ssd_offset, delta_clusters = 0;
    uint64_t offset;
    fread((void *) (&delta_clusters), sizeof(uint32_t), 1, in_ssd);
    Entry *entries = (Entry*)malloc(delta_clusters*sizeof(Entry));
    fread(entries, sizeof(Entry), delta_clusters, in_ssd);
    for(int i = 0; i < delta_clusters; i++){
        if(entries[i].visited <= 4){
            offset = entries[i].cluster_offset * cluster_size;
            fseek(in_base, offset, SEEK_SET);
            fread(clusterBuf, sizeof(char), cluster_size, in_base);
            //printf("reading %d\n%s\n", entries[i].cluster_offset, clusterBuf);
            for(int j = 0; j < 16; j++){
                if(entries[i].bitmap_dirty & (one << j)){
                    fseek(in_ssd, entries[i].cache_offset[j], SEEK_SET);
                    fread(cache_read_Buf, sizeof(char), cache_size, in_ssd);
                    memcpy(clusterBuf + j * cache_size, cache_read_Buf, cache_size);
                }
            }
            //printf("writing\n%s\n", clusterBuf);
        }else{
            for(int j = 0; j < 16; j++){
                if(entries[i].bitmap_dirty & (one << j)){
                    fseek(in_ssd, entries[i].cache_offset[j], SEEK_SET);
                    fread(cache_read_Buf, sizeof(char), cache_size, in_ssd);
                    memcpy(clusterBuf + j * cache_size, cache_read_Buf, cache_size);
                }else{
                    fseek(in_base, entries[i].cluster_offset + j * cache_size, SEEK_SET);
                    fread(cache_read_Buf, sizeof(char), cache_size, in_base);
                    memcpy(clusterBuf + j * cache_size, cache_read_Buf, cache_size);
                }
            }
        }
        fwrite(clusterBuf, sizeof(char), cluster_size, out_delta);
    }
    free(entries);
    fclose(in_ssd);
    fclose(out_delta);
    fclose(in_base);
    return 0;
}

int write_base(char *trace)
{
    FILE *in_base, *out_ssd, *traces;
    char cache_read_Buf[5000], cache_write_Buf[5000], clusterBuf[65540];
    for(int i = 0; i < 5000; i++){
        if(i % 3 == 0)
            cache_write_Buf[i] = 'w';
        else if(i % 3 == 1)
            cache_write_Buf[i] = 'l';
        else
            cache_write_Buf[i] = 'h';
    }
    cache_write_Buf[4095] = '\n';
    traces = fopen(trace, "r");
    if(!traces){//打开操作不成功
        printf("traces file can not be opened.\n");
        exit(1);//结束程序的执行
    }
    out_ssd = fopen(ssd_file, "w");
    if(!out_ssd){
        printf("out_ssd file  can not be opened.\n");
        exit(1);//结束程序的执行
    }
    in_base = fopen(base_file, "r");
    if(in_base == NULL){//打开操作不成功
        printf("in_base file can not be opened.\n");
        exit(1);//结束程序的执行
    }
    Entry *entries = (Entry*)malloc(entry_size*sizeof(Entry));
    memset(entries, 0, entry_size*sizeof(Entry));
    uint32_t ssd_offset = entry_size * sizeof(Entry), cache, cluster, delta_clusters = 0, iosize, one = 1, total_writes = 0;
    uint64_t fromwhere;
    //printf("ssd_offset: %u\n", ssd_offset);
    fseek(out_ssd, ssd_offset, SEEK_SET);
    char ops;
    while(fscanf(traces, " %c %llu %d", &ops, &fromwhere, &iosize) != EOF){
        if(iosize != cache_size){
            printf("io error: iosize=%d, cache_size=%d\n", iosize, cache_size);
            exit(1);
        }
        cluster = fromwhere / cluster_size;
        cache = (fromwhere % cluster_size) / cache_size;
        if(cluster >= entry_size || cache >= 16){
            printf("ERROR!!!fromwhere=%u, cluster=%u, cache=%u\n", fromwhere, cluster, cache);
            exit(1);
        }
        if(ops != 'W' && ops != 'R'){
            printf("ops error: %c\n", ops);
            exit(1);
        }
        if(entries[cluster].bitmap_dirty & (one << cache)){
            if(ops == 'W')
                total_writes++;
            continue;
        }
        entries[cluster].cluster_offset = cluster;
        if(ops == 'W'){
            if(entries[cluster].visited == 0){
                delta_clusters++;
                printf("%u\n", fromwhere);
            }
            total_writes++;
            if(!(entries[cluster].bitmap_dirty &  (one << cache))){
                entries[cluster].visited += 1;
            }
            entries[cluster].bitmap_dirty |= (one << cache);
            entries[cluster].cache_offset[cache] = ftell(out_ssd);
            fwrite(cache_write_Buf, sizeof(char), iosize, out_ssd);
        }else{
            if(entries[cluster].cache_offset[cache] == 0){//first met
                fseek(in_base, fromwhere, SEEK_SET);
                fread(cache_read_Buf, sizeof(char), cache_size, in_base);
                entries[cluster].cache_offset[cache] = ftell(out_ssd);
                fwrite(cache_read_Buf, sizeof(char), cache_size, out_ssd);
            }
        }
    }
    Entry *nu_entries = (Entry*)malloc(delta_clusters*sizeof(Entry));
    uint32_t nu_entries_index = 0;
    for(int i = 0; i < entry_size; i++){
        if(entries[i].visited){
            if(nu_entries_index >= delta_clusters){
                printf("compress error1\n");
                exit(1);
            }
            memcpy(nu_entries+nu_entries_index, entries+i, sizeof(Entry));
            nu_entries_index++;
        }
    }
    if(nu_entries_index != delta_clusters){
        printf("compress error2\n");
        exit(1);
    }
    fseek(out_ssd, 0, SEEK_SET);
    fwrite((void *) (&delta_clusters), sizeof(uint32_t), 1, out_ssd);
    fwrite(nu_entries, sizeof(Entry), delta_clusters, out_ssd);
    free(entries);
    free(nu_entries);
    fclose(out_ssd);
    fclose(in_base);
    fclose(traces);
    printf("unique block = %u\n", delta_clusters);
    return 0;
}
int main(int argc,char *argv[]){
    if(argc>1){
        write_base(argv[1]);
        write_delta(argv[1]);
    }
    else{
        write_base(trace_file);
        write_delta(trace_file);
    }
    return 0;
}