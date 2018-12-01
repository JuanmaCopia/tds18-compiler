#include <stdio.h>

void printBool(int x) {
    if (x == 0)
        printf("false\n");
    else {
        if (x == 1)
            printf("true\n");
    }
}

void printGuiones( ){
    printf("----------------------------------------------------------\n");
}

void printSpace( ){
    printf("\n\n");
}

void print(int n) {
    printf("Value: %d\n", n);
}