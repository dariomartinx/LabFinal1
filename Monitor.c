#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int linea (int df, char *cad);

int main ()
{
    int descr;
    char cadena[100];

    unlink ("/tmp/patronludopata");
    mkfifo ("/tmp/patronludopata", 0);
    chmod ("/tmp/patronludopata", 460);
    
    while(1)
	{
		descr = open ("/tmp/patronludopata", O_RDONLY);	
		if (linea (descr, cadena))
		{
			printf ("%s\n", cadena);
		}
		close (descr);
	}
}
	

int linea (int df, char *cad)
    {
    int n;

    do
      {
      n = read (df, cad, 1);
      }
    while (n > 0 && cad++ != NULL);
    return (n > 0);
    }
    
int linea2 (int df, char *cad)
    {
    	
    int readbytes;
    readbytes = read(df, cad, sizeof(cad));
    cad[readbytes]='\0';
    return 1;
	
    }
