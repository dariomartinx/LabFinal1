#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Definimos la estructura que va a contener la información para cada hilo
typedef struct {
    char* filename; // Nombre del archivo a leer
    int thread_num; // Número del hilo
} thread_args;

// Función que se va a ejecutar en cada hilo
void* read_file(void* args) {
    thread_args* t_args = (thread_args*) args;
    char* filename = t_args->filename;
    int thread_num = t_args->thread_num;
    
    printf("Hilo %d: Leyendo archivo %s...\n", thread_num, filename);
    
    // Abrimos el archivo en modo lectura
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error al abrir archivo");
        pthread_exit(NULL);
    }
    
    // Leemos el contenido del archivo
    char buffer[256];
    while (fgets(buffer, 256, file)) {
        printf("Hilo %d: %s", thread_num, buffer);
    }
    
    // Cerramos el archivo
    fclose(file);
    
    printf("Hilo %d: Archivo %s leido con exito.\n", thread_num, filename);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    // Creamos la estructura de argumentos para cada hilo
    thread_args args[3] = {
        {"archivo1.txt", 1},
        {"archivo2.txt", 2},
        {"archivo3.txt", 3}
    };
    
    // Creamos los hilos
    pthread_t threads[3];
    for (int i = 0; i < 3; i++) {
        int result = pthread_create(&threads[i], NULL, read_file, (void*) &args[i]);
        if (result != 0) {
            perror("Error al crear hilo");
            exit(EXIT_FAILURE);
        }
    }
    
    // Esperamos a que los hilos terminen
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("Todos los hilos han terminado.\n");
    return 0;
}

