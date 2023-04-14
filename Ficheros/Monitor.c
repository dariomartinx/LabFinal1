#include <pthread.h>
#include <stdio.h>


void *funcion_hilo(void *arg) {
    printf("¡Hola desde el hilo %ld!\n", (long)arg);
    pthread_exit(NULL);
}

int main() {
    pthread_t hilo1, hilo2, hilo3;
    long id_hilo1 = 1, id_hilo2 = 2, id_hilo3 = 3;
    
    pthread_create(&hilo1, NULL, funcion_hilo, (void *)id_hilo1);
    pthread_create(&hilo2, NULL, funcion_hilo, (void *)id_hilo2);
    pthread_create(&hilo3, NULL, funcion_hilo, (void *)id_hilo3);
    
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);
    pthread_join(hilo3, NULL);
    
    printf("Los hilos han terminado\n");
    return 0;
}
