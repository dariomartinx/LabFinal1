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
#include <sys/ipc.h>
#include <semaphore.h>
#include <time.h>

typedef struct conf{
        char rutaFiles[25];
        char invFile[35];
        char logFile[25];
        int nProc;
        int simSlpMax;
        int simSlpMin;
        char rutaBin[25];
	int nCasas;
}confstruct;
//Lo declaramos globalmente para tener la info del fichero de configuración en todos los hilos
confstruct configuracion;

typedef struct{
	int dia;
	int mes;
	int anio;
	int min;
	int seg;
	int hora;
	int nProc;
	char *hIn;
	char *hFin;
	char file[25];
	int num;
}logstruct;

typedef struct {
    char filename[50]; // Nombre del archivo a leer
    int thread_num; // Número del hilo
}thread_args;

sem_t sem1, sem2, sem3;

//La funcionalidad de las diversas pipes es avisar al resto de hilos del fichero que se va a procesar y que asi no se procese dos veces el mismo

void* funcionThread(void *args);

int main(){
char str[50], *tok, tConf[25];
int interruptor, i, j=0;
sem_init(&sem1, 0, 1);
sem_init(&sem2, 0, 1);
sem_init(&sem3, 0, 1);
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
		sem_wait(&sem3);
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
			sem_post(&sem3);
		}
		//Fin de la asignación de las casas de apuestas
		//Creación de los hilos
		pthread_t threads[configuracion.nProc];
		for (int i = j; i < configuracion.nProc; i++) {
        		int result = pthread_create(&threads[i], NULL, funcionThread, (void*) &args[i]);
        		if (result != 0) {
            			perror("Error al crear hilo");
            			exit(EXIT_FAILURE);
        		}
    		}
		j=0;
		//Espera a que acabe los hilos que no tengan una casa asignada y se cuentan los que si la tienen para no llamarlos otra vez
		//(He Hecho esto para que si se agrega una nueva casa de apuestas mientras está el programa en ejecución, se le pueda asignar a los hilos libres)
		for (int i = 0; i < configuracion.nProc; i++) {
			if(strlen(args[i].filename)==0)
        			pthread_join(threads[i], NULL);
			else
				j++;
		}
	}while(1);

}
}


void* funcionThread(void *args) {
thread_args *t_args = (thread_args*) args;
char* filename = t_args->filename, comando[75], ruta[35],rutaBin[35], *tok, name[20], str[250];
int num = t_args->thread_num, contador, interruptor, sueno = (rand()%(configuracion.simSlpMax+1-configuracion.simSlpMin))+configuracion.simSlpMin;
FILE *pfich, *pfich2;
logstruct log;
time_t now, inicio, fin;
struct tm *local;
if(strlen(filename)!=0){
	do{
		interruptor = 0;
		contador = 0;
		strcpy(ruta,configuracion.rutaFiles);
		strcpy(rutaBin,configuracion.rutaBin);
		sem_wait(&sem3);
		DIR *d = opendir(configuracion.rutaFiles);
        	struct dirent *dir;
        	if(d){
			while ((dir = readdir(d)) != NULL){
				strcpy(name,dir->d_name);
				tok = strtok(dir->d_name, "_");
				if(strcmp(tok, filename)==0){
					strcat(ruta,name);
					sem_wait(&sem2);
					pfich = fopen(configuracion.invFile, "a+");
					if(pfich == NULL){
                                               printf( "Error al abrir el fichero consolidado\n");
                                        }
					else{
						printf("Fichero %s abierto correctamente\n", configuracion.invFile);
						pfich2 = fopen(ruta, "r");
        	                                if(pfich2 == NULL){
                	                                printf( "Error al abrir el fichero %s\n",ruta);
                        	                }
                                	        else{
                                        		printf("Fichero %s abierto correctamente\n",ruta);
							time(&inicio);
							log.hIn=ctime(&inicio);
							log.hIn[strlen(log.hIn)-1]='\0';
       	 						while(fgets(str,250,pfich2)!=NULL){
								if(interruptor==1){
	        	        					fprintf(pfich,"%s",str);
									contador++;
								}
								interruptor=1;
							}
							log.num=contador;
							time(&fin);
							log.hFin=ctime(&fin);
							log.hFin[strlen(log.hFin)-1]='\0';
							fclose(pfich2);
							strcat(rutaBin, name);
                        				strcpy(comando, "mv ");
                        				strcat(comando, ruta);
							strcat(comando, " ");
                        				strcat(comando, rutaBin);
                        				system(comando);
						}
					}
					fclose(pfich);
	                		sem_post(&sem2);

					sem_wait(&sem1);
	                        	pfich = fopen(configuracion.logFile, "a+");
        	                	if(pfich == NULL){
                	        	        printf( "Error al abrir el fichero de log\n");
                        		}
	                        	else{
        	                        	printf("Fichero %s abierto correctamente\n", configuracion.logFile);
						time(&now);
						local = localtime(&now);
						log.dia = local->tm_mday;
						log.mes = local->tm_mon + 1;
						log.anio = local->tm_year + 1900;
						log.hora = local->tm_hour;
						log.min = local->tm_min;
						log.seg = local->tm_sec;
						log.nProc = getpid();
						strcpy(log.file, name);

						fprintf(pfich,"%d/%d/%d;%d:%d:%d;%d;%s;%s;%s;%d\n",log.dia,log.mes,log.anio,log.seg,log.min,log.hora,log.nProc,log.hIn,log.hFin,log.file,log.num);
						printf("%d/%d/%d;%d:%d:%d;%d;%s;%s;%s;%d\n",log.dia,log.mes,log.anio,log.seg,log.min,log.hora,log.nProc,log.hIn,log.hFin,log.file,log.num);

        	                        	fclose(pfich);
                	        	}
                        		sem_post(&sem1);
				}
			}
			closedir(d);
			sem_post(&sem3);
		}
		sleep(sueno);
	}while(1);
}
else{
	pthread_exit(NULL);
}
}
