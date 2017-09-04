//./write_ssd /home/fjywade/traces/msr-cowcache/mds_0.csv.data 
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "head.h"

int main(int argc,char *argv[])
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
    if(argc > 1)
        traces = fopen(argv[1], "r");
    else{
        //traces = fopen(trace_file, "r");
        printf("please input trace_file \n");
        exit(1);
    }
    if(!traces){//打开操作不成功
        printf("traces file can not be opened.\n");
        exit(1);//结束程序的执行
    }
    out_ssd = fopen(ssd_file, "w");
    if(!out_ssd){
        printf("out_ssd file can not be opened.\n");
        exit(1);//结束程序的执行
    }
    in_base = fopen(base_file, "r");
    if(in_base == NULL){//打开操作不成功
        printf("in_base file can not be opened.\n");
        exit(1);//结束程序的执行
    }
    Entry *entries = (Entry*)malloc(entry_size*sizeof(Entry));
    memset(entries, 0, entry_size*sizeof(Entry));
    uint32_t offset = entry_size * sizeof(Entry), cache, cluster, delta_clusters, iosize, one = 1;
    long fromwhere, cache_offset;
    //printf("ssd_offset: %u\n", ssd_offset);
    fseek(out_ssd, offset, SEEK_SET);
    char ops;
    uint32_t total_read_4k = 0, total_write_4k = 0, unique_write_4k = 0, unique_write_64k = 0, full_64k = 0;
    while(fscanf(traces, " %c %ld %d", &ops, &fromwhere, &iosize) != EOF){
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
        if(ops == 'W')
            total_write_4k++;
        else
            total_read_4k++;
        if(entries[cluster].bitmap_dirty & (one << cache))
            continue;
        entries[cluster].cluster_offset = cluster;
        if(ops == 'W'){
            if(entries[cluster].visited == 0){
                delta_clusters++;
                unique_write_64k++;
            }
            if(!(entries[cluster].bitmap_dirty &  (one << cache))){
                entries[cluster].visited += 1;
                unique_write_4k++;
            }
            entries[cluster].bitmap_dirty |= (one << cache);
            cache_offset = ftell(out_ssd);
            cache_offset = (cache_offset - offset ) / cache_size;
            entries[cluster].cache_offset[cache] = cache_offset;
            fwrite(cache_write_Buf, sizeof(char), iosize, out_ssd);
        }else{
            if(entries[cluster].cache_offset[cache] == 0){//first met
                fseek(in_base, fromwhere, SEEK_SET);
                fread(cache_read_Buf, sizeof(char), cache_size, in_base);
                cache_offset = ftell(out_ssd);
                cache_offset = (cache_offset - offset ) / cache_size;
                entries[cluster].cache_offset[cache] = cache_offset;
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
        if(entries[i].visited == 16)
            full_64k++;
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
    printf("total read = %u (%.1fMB, %.1fGB), 4k based\n", total_read_4k, (float)total_read_4k*4096/1000000, (float)total_read_4k*4096/1000000000);
    printf("total write = %u (%.1fMB, %.1fGB), 4k based\n", total_write_4k, (float)total_write_4k*4096/1000000, (float)total_write_4k*4096/1000000000);
    printf("unique write = %u (%.1fMB, %.1fGB), 4k based\n", unique_write_4k, (float)unique_write_4k*4096/1000000, (float)unique_write_4k*4096/1000000000);
    printf("unique write = %u (%.1fMB, %.1fGB), 64k based\n", unique_write_64k, (float)unique_write_64k*65536/1000000, (float)unique_write_64k*65536/1000000000);
    //printf("cow = %.4f\n", (float)unique_write_64k / total_write_4k);
    printf("fully writed clusters = %u\n", full_64k);
    printf("fully writed clusters / unique write clusters = %.3f",(float)full_64k / unique_write_64k);
    printf("read : write = %d : %d", (float)total_read_4k / (total_read_4k + total_write_4k), 1 - (float)total_read_4k / (total_read_4k + total_write_4k));
    return 0;
}