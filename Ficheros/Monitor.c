#include <stdio.h>
#include <pthread.h>

void* hilo1(void* arg) {
    printf("Soy el hilo 1\n");
    pthread_exit(NULL);
}

void* hilo2(void* arg) {
    printf("Soy el hilo 2\n");
    pthread_exit(NULL);
}

void* hilo3(void* arg) {
    printf("Soy el hilo 3\n");
    pthread_exit(NULL);
}

int main() {
    pthread_t t1, t2, t3;

    pthread_create(&t1, NULL, hilo1, NULL);
    pthread_create(&t2, NULL, hilo2, NULL);
    pthread_create(&t3, NULL, hilo3, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    return 0;
}
