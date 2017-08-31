#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "head.h"

int main(int argc,char *argv[])
{   
    FILE *in_ssd;
    in_ssd = fopen(ssd_file, "r");
    if(in_ssd == NULL){
        printf("in_ssd file  can not be opened.\n");
        exit(1);//结束程序的执行
    }
    char cache_read_Buf[5000];
    memset(cache_read_Buf, 0, 5000*sizeof(char));
    uint32_t one = 1, base_offset, ssd_offset, delta_clusters;
    Entry *entries = (Entry*)malloc(entry_size*sizeof(Entry));
    fread((void *) (&delta_clusters), sizeof(uint32_t), 1, in_ssd);
    fread(entries, sizeof(Entry), delta_clusters, in_ssd);
    for(int i = 0; i < delta_clusters; i++){
        printf("In cluster %d\n", i);
        for(int j = 0; j < 16; j++){
            if(entries[i].cache_offset[j] != 0){
                printf("In cache %d, ", j);
                base_offset = entries[i].cluster_offset + j * cache_size;
                ssd_offset = entries[i].cache_offset[j];
                printf("base_offset = %u, ssd_offset = %u", base_offset, ssd_offset);
                if((one<<j) & entries[i].bitmap_dirty)
                    printf(", dirty(write)\n");
                else
                    printf(", clean(read, without write)\n");
            }
        }
    }
    fseek(in_ssd, entry_size * sizeof(Entry), SEEK_SET);
    uint32_t now = ftell(in_ssd);
    while(fread(cache_read_Buf, sizeof(char), cache_size, in_ssd)){
        printf("%u: %s", now, cache_read_Buf);
        now = ftell(in_ssd);
    }
    free(entries);
    fclose(in_ssd);
    return 0;
}