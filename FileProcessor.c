#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <dirent.h>

typedef struct conf{
        char rutaFiles[25];
        char invFile[25];
        char logFile[25];
        int nProc;
        int simSlpMax;
        int simSlpMin;
        char rutaBin[25];
	int nCasas;
}confstruct;
//Lo declaramos globalmente para tener la info del fichero de configuración en todos los hilos
confstruct configuracion;

typedef struct {
    char filename[50]; // Nombre del archivo a leer
    int thread_num; // Número del hilo
}thread_args;

void* funcionThread(void *args);

int main(){
char str[50], *tok, tConf[25];
int interruptor, i;
FILE *pfich = fopen("fp.conf.txt", "r");                        //Fichero de configuración
if(pfich == NULL){
	printf( "Error al abrir el fichero de configuración\n");
}
else{
        printf("Fichero de configuración abierto correctamente\n");
        while(fgets(str,50,pfich)!=NULL){
        	tok = strtok(str, "=");
                strcpy(tConf,tok);
                tok = strtok(NULL,"=");
                if(strcmp(tConf,"PATH_FILES")==0){
	                strcpy(configuracion.rutaFiles, tok);
                        configuracion.rutaFiles[strlen(configuracion.rutaFiles)-1]='\0';
                }
		else if(strcmp(tConf,"INVENTORY_FILE")==0){
                        strcpy(configuracion.invFile, tok);
                        configuracion.invFile[strlen(configuracion.invFile)-1]='\0';
                }
                else if(strcmp(tConf,"LOG_FILE")==0){
                        strcpy(configuracion.logFile, tok);
                        configuracion.logFile[strlen(configuracion.logFile)-1]='\0';
                }
                else if(strcmp(tConf,"NUM_PROCESOS")==0)
                        configuracion.nProc = atoi(tok);
                else if(strcmp(tConf,"SIMULATE_SLEEP_MAX")==0)
                        configuracion.simSlpMax = atoi(tok);
                else if(strcmp(tConf,"SIMULATE_SLEEP_MIN")==0)
                        configuracion.simSlpMin = atoi(tok);
                else if(strcmp(tConf,"PATH_BIN")==0){
                        strcpy(configuracion.rutaBin, tok);
                        configuracion.rutaBin[strlen(configuracion.rutaBin)-1]='\0';
                }
        }
        fclose(pfich);
	//Inicializar dalos de los hilos
	thread_args args[configuracion.nProc];
	for(i=1;i<=configuracion.nProc;i++){
		strcpy(args[i-1].filename, "");
		args[i-1].thread_num = i;
	}
	//Fin de la inicialización
	DIR *d;
	struct dirent *dir;
	do{
		//Desde la apertura de la ruta de archivos hasta su cierre es el código que se encarga de asignar una casa de apuesta a cada hilo
		d = opendir(configuracion.rutaFiles);
		if (d) {
			while ((dir = readdir(d)) != NULL)
			{
				tok = strtok(dir->d_name, "_");

				interruptor = 1;
				for(i = 0; i < configuracion.nProc; i++){
					if(strcmp(args[i].filename,tok)==0)
						interruptor = 0;
				}
				if(strcmp(dir->d_name, "..")!=0 && strcmp(dir->d_name, ".")!=0 && interruptor){
					i=0;
					while(strlen(args[i].filename)!=0 && i < configuracion.nProc){
						i++;
					};
					if(i<configuracion.nProc)
						strcpy(args[i].filename,tok);
				}
			}

			closedir(d);

		}
		//Fin de la asignación de las casas de apuestas
		//Creación de los hilos
		pthread_t threads[configuracion.nProc];
		for (int i = 0; i < configuracion.nProc; i++) {
        		int result = pthread_create(&threads[i], NULL, funcionThread, (void*) &args[i]);
        		if (result != 0) {
            			perror("Error al crear hilo");
            			exit(EXIT_FAILURE);
        		}
    		}
		for (int i = 0; i < configuracion.nProc; i++) {
        		pthread_join(threads[i], NULL);
		}
	}while(0);

}
}


void* funcionThread(void *args) {
thread_args *t_args = (thread_args*) args;
char* filename = t_args->filename;
int num = t_args->thread_num;
printf("%d:     %s\n",num ,filename);
pthread_exit(NULL);
}
