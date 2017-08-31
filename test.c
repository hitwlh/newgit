//./write /home/fjywade/traces/msr-cowcache/mds_0.csv.data 
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

int main(int argc,char *argv[]){
    printf("%d\n", sizeof(int));
    printf("%d\n", sizeof(long));
    printf("%d\n", sizeof(long long));
    long sd = 1000 * 1000 * 1000 * 1000;
    printf("%ld\n", sd);
    sd = 1000 * 1000;
    sd *= sd;
    printf("%ld\n", sd);
    return 0;
}