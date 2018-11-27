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
    printf("Valor: %d\n", n);
}


int sum(int a, int b) {
    return a + b;
}

void print2(int a, int b) {
    printf("los val son: %d y %d \n", a, b);
}