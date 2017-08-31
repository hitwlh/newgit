// ./analysis_equal /home/fjywade/r5ssd/1t-fl-e4_directly.q2 /home/fjywade/r5ssd/1t-fl-e4_system.q2
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "head.h"

int main(int argc,char *argv[])
{
    FILE *file1 = fopen(argv[1], "r"), *file2 = fopen(argv[2], "r");
    if(!file1 || !file2){
        if(!file1)
            printf("%s OPEN Error!\n", argv[1]);
        if(!file2)
            printf("%s OPEN Error!\n", argv[2]);
        exit(1);
    }
    char ch1 = fgetc(file1), ch2 = fgetc(file2);
    while(ch1 != EOF && ch2 != EOF){
        if(ch1 != ch2){
            printf("not equal!\n");
            return 0;
        }
        ch1 = fgetc(file1), ch2 = fgetc(file2);
    }
    if(ch1 == EOF && ch2 == EOF)
        printf("equal!\n");
    else
        printf("not equal!\n");
    fclose(file1);
    fclose(file2);
    return 0;
}