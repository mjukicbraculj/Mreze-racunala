#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "zadaca_protokol.h"
void ispis(double *niz, int n)
{
	for (int i=0;i<n;i++)printf ("%.10lf ", niz[i]);
	printf ("\n");
}

int posaljiNiz(int sock, double* niz, int n)
{
	int poslanoZadnje;
	
	//posaljemo duljinu niza
	int n_n=htonl(n);
	//ptp da int saljemo odjednom
	poslanoZadnje=send(sock, &n_n, sizeof (n_n), 0);
	if(poslanoZadnje!=sizeof(n_n)){printf ("greska u sendu inta\n");return 0;}
	
	//svaki broj saljemo kao string
	for (int i=0;i<n;i++)
	{
		char broj[100];
		sprintf (broj, "%lf", niz[i]);
		//posaljemo duljinu broja
		int duljina=sizeof(broj);
		int duljina_n=htonl(duljina);
		poslanoZadnje=send(sock, &duljina_n, sizeof(duljina_n),0);
		if(poslanoZadnje!=sizeof(duljina_n)){printf ("greska u sendu inta\n"); return 0;};
		int poslano=0;
		while(poslano!=duljina)
		{
			poslanoZadnje=send(sock, broj+poslano, duljina-poslano, 0);
			if(poslanoZadnje==-1||poslanoZadnje==0){printf ("greska u sendu char\n"); return 0;}
			poslano+=poslanoZadnje;
		}
	}
	return 1;
}
int primiNiz(int sock, double **niz, int *duljina)
{
	//prvo primimo duljinu niza
	int duljina_n, primljenoZadnje, primljeno;
	primljenoZadnje=recv(sock, &duljina_n, sizeof(duljina_n), 0);
	if(primljenoZadnje!=sizeof(duljina_n)){printf ("greska u recv inta\n"); return 0;}
	*duljina=ntohl(duljina_n);
	
	//alociramo memoriju
	*niz=(double*)malloc(*duljina*sizeof(double));
	int k=0;
	for (int i=0;i<*duljina;i++)
	{
		//dobijemo duljinu broja
		int n_n, n;
		primljeno=0;
		primljenoZadnje=recv(sock, &n_n, sizeof(n_n), 0);
		if(primljenoZadnje!=sizeof(n_n)){printf ("greska u recv inta\n"); return 0;}
		n=ntohl(n_n);
		char broj[n+1];
		while(primljeno!=n)
		{
			primljenoZadnje=recv(sock, broj+primljeno,n-primljeno,0);
			if(primljenoZadnje==-1|| primljenoZadnje==0)return 0;
			primljeno+=primljenoZadnje;
		}
		broj[n]='\0';
		//printf ("\n");
		//printf ("dobio sam broj %s\n", broj);
		//printf ("\n");
		double broj1;
		sscanf(broj, "%lf", &broj1);
		(*niz)[k++]=broj1;
	}
	return 1;
}


/*int posaljiNiz(int sock, double *niz, int n)
{
	//posaljemo duljinu niza
	int poslanoZadnje, poslano=0;
	int n_n=htonl(n);
	//ptp da int saljemo odjednom
	poslanoZadnje=send(sock, &n_n, sizeof (n_n), 0);
	if(poslanoZadnje!=sizeof(n_n)){printf ("greska u sendu inta\n");return 0;}
	int ukupnoZaPoslat=sizeof(double)*n;
	while(poslano!=ukupnoZaPoslat)
	{
		poslanoZadnje=send(sock, (char*)niz+poslano, ukupnoZaPoslat-poslano, 0);
		if(poslanoZadnje==-1||poslanoZadnje==0){printf ("greska u slanju byts\n"); return 0;}
		poslano+=poslanoZadnje;
	}
	return 1;
}

int primiNiz(int sock, double **niz, int *duljina)
{
	//primimo duljinu niza;
	int primljenoZadnje, primljeno=0;
	int duljina_n;
	primljenoZadnje=recv(sock, &duljina_n, sizeof(duljina_n), 0);
	if(primljenoZadnje!=sizeof(duljina_n)){printf("primiNiz:greska u primanju inta\n");return 0;}
	*duljina=ntohl(duljina_n);
	*niz=(double*)malloc((*duljina)*sizeof(double));
	int ukupnoZaPrimit=sizeof(double)*(*duljina);
	while(primljeno!=ukupnoZaPrimit)
	{
		primljenoZadnje=recv(sock, (char*)(*niz)+primljeno, ukupnoZaPrimit-primljeno, 0);
		if(primljenoZadnje==-1||primljenoZadnje==0){printf("primiNiz: greska u primanju byts\n");return 0;}
		primljeno+=primljenoZadnje;
	}
	//printf ("primio sam nizu PrimiNIZ:\n");
	//ispis((double*)(*niz), (*duljina));
	return 1;
}*/
