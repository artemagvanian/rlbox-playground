#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "mylib.h"

void hello() {
    printf("Hello from mylib\n");
}

unsigned add(unsigned a, unsigned b) {
    return a + b;
}

unsigned add_file_side_effect(unsigned a, unsigned b) {
    FILE *fptr = fopen("leak.txt", "w");
    printf("fptr = %p\n", fptr);
    fflush(stdout);
    if (fptr == NULL) {
        printf("%s\n", strerror(errno));
        fflush(stdout);
    } else {
        fprintf(fptr, "%d", a + b);
        fclose(fptr);
    }
    return a + b;
}

unsigned add_global_side_effect(unsigned a, unsigned b) {
    if (global == 0) {
        global = a + b;
    }
    return global;
}

unsigned add_printf_side_effect(unsigned a, unsigned b) {
    printf("printf-ing from inside the sandbox: a + b = %d\n", a + b);
    fflush(stdout);
    return global;
}

unsigned add_ptr_side_effect(unsigned a, unsigned b, unsigned *ptr) {
    *ptr = a + b;
    return global;
}

void echo(const char *str) {
    printf("echo: %s\n", str);
}

void call_cb(void (*cb)(const char *str)) {
    cb("hi again!");
}
