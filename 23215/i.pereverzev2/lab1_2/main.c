#define _REENTRANT
#include <pthread.h>
#include <thread.h>
#include <stdio.h>

void print_text()
{
    for(int i = 0; i < 10; i++) {
        printf("line %d\n", i);
    }
}

int main()
{
    pthread_t *th;
    pthread_create(th, NULL, print_text);
    print_text();
    return 0;
}