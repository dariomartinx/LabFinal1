#include <stdio.h>
#include <pthread.h>

// Función que será ejecutada por los hilos
void *thread_function(void *arg) {
    int thread_number = *(int *)arg;
    printf("Hilo %d ejecutándose\n", thread_number);
    return NULL;
}

int main() {
    pthread_t threads[5]; // Array que almacenará los hilos
    int thread_args[5]; // Array que almacenará los argumentos de los hilos
    int i;

    // Crear los hilos
    for (i = 0; i < 5; i++) {
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, thread_function, &thread_args[i]);
    }

    // Esperar a que los hilos terminen
    for (i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Todos los hilos han terminado\n");

    return 0;
}
