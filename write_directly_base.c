// ./a_directly_base /home/fjywade/del/traces
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "head.h"

int main(int argc,char *argv[])
{   
    FILE *out = fopen(base_file, "r+");
    FILE *traces;
    printf("argc = %d\n", argc);
    if(argc > 1)
        traces = fopen(argv[1], "r");
    else
        traces = fopen(trace_file, "r");
    if(!out || !traces){
        if(!out) printf("out fopen error!\n");
        if(!traces) printf("traces fopen error!\n");
        exit(1);
    }
    char ops;
    long long fromwhere;
    int iosize, i = 0;
    char cacheBuf[5050];
    for(int i = 0; i < 5000; i++){
        if(i % 3 == 0)
            cacheBuf[i] = 'w';
        else if(i % 3 == 1)
            cacheBuf[i] = 'l';
        else
            cacheBuf[i] = 'h';
    }
    cacheBuf[4095] = '\n';
    while(fscanf(traces, " %c %lld %d", &ops, &fromwhere, &iosize) != EOF){
        printf("%c %lld %d\n", ops, fromwhere, iosize);
        if(ops != 'R' && ops != 'W'){
            printf("ops error!, ops is %c\n", ops);
            exit(1);
        }
        if(iosize != 4096){
            printf("iosize error!, iosize is %d\n", iosize);
            exit(1);
        }
        if(ops == 'R')
            continue;
        fseek(out, fromwhere, SEEK_SET);
        fwrite(cacheBuf, sizeof(char), 4096, out);
        //printf("%d, in %lld, data is %s\n", i++, fromwhere, cacheBuf);
    }
    fclose(out);
    return 0;
}