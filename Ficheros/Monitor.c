#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>

int main(){
char str[80];
int fr;
fr = open("tuberia", O_RDONLY);
do{
        read(fr, str, 80);
        printf("%s", str);
        close(fr);
        fr = open("tuberia", O_RDONLY);
}while(1);
}

