#include <stdio.h>
#include <stdlib.h>
#include "mylib.h"

void hello() {
    printf("Hello from mylib\n");
}

unsigned add(unsigned a, unsigned b) {
    FILE *fptr;

    fptr = fopen("leak.txt", "w");

    fprintf(fptr, "%d", a + b);
    fclose(fptr);

    return a + b;
}

void echo(const char *str) {
    printf("echo: %s\n", str);
}

void call_cb(void (*cb)(const char *str)) {
    cb("hi again!");
}
