#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct conf{
	char rutaFiles[25];
	char invFile[25];
	char logFile[25];
	int nProc;
	int simSlp;
}confstruct;

void *funcionThread (void *parametro);

int main(){
char str[50], *tok, tConf[25];
confstruct configuracion;
pthread_t idHilo[5];

FILE *pfich = fopen("fp.conf.txt", "r");								//Fichero de configuración
if(pfich == NULL)
	printf("Error al abrir el fichero de configuración");
else{
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
		else if(strcmp(tConf,"SIMULATE_SLEEP")==0)
                        configuracion.simSlp = atoi(tok);
	}
	fclose(pfich);
}

}