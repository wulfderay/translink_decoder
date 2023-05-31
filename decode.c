#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

#include "translink.h"

int main(int argc, char **argv){
    char *str;
    char *query = getenv("QUERY_STRING");

    if (query){
        str = query;
    } else if (argc == 2){
        str = argv[1];
    } else {
        printf("usage: %s <bits>\n", argv[0]);
        exit(1);
    }
    decode(str);
    return 0;
}
