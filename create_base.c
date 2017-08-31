#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "head.h"

int main(int argc,char *argv[])
{
    FILE *out = fopen(base_file, "w");
    if(!out){
        printf("open error!\n");
        exit(1);
    }
    for(int i = 0; i < 160; i++){
        char start = 'a'+(i%26);
        for(int j = 0; j < 26; j++){
            char put;
            if(start+j <= 'z')
                put = start+j;
            else
                put = start+j-'z'+'a'-1;
            fputc(put, out);
        }
        for(int j = '0'; j <= '9'; j++)
            fputc(j, out);
        for(int j = 0; j < 4059; j++)
            fputc(' ', out);
        fputc('\n', out);
    }
    fclose(out);
    return 0;
}