#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pthread.h>
#include "zadaca_protokol.h"

typedef struct 
{	
	int socket, duljina, dio;
	double *niz1, *niz2, *niz3;  // u niz3 spremamo rezultat
}param;

double *konacno;    //globalan niz u koji spremamo rezultat
int duljina_konacno; 
double zbroj=0; 
param niz_parametara[2];


void kopiraj(double *u, double *od, int poc, int kraj) 
{
	int j=0;
	for (int i=poc; i<kraj; i++) u[j++]=od[i];
}
void kopiraj1(double *kamo, double *od_kud, int pocetak, int kraj)
{
	int j=0;
	for (int i=pocetak;i<kraj;i++)kamo[i]=od_kud[j++];
}
void *obradi (void *parametar)
{
	param *info=(param*)parametar;
	int duljina;
	
	//saljemo serveru nizove;
	int error=posaljiNiz(info->socket,info->niz1, info->duljina);
	if(error==0)error1 ("greska u slanju1\n");
	error=posaljiNiz(info->socket, info->niz2, info->duljina);
	if(error==0)error1("greska u slanju2\n");
	
	//primimo odgovor
	error=primiNiz(info->socket, &info->niz3,  &duljina);
	if(error==0){error1 ("greska u primanju1");}
	
	//moramo upisati dio niza koji smo dobili u konacnan niz
	if(info->dio==1)
	{
		//printf ("kopiramo prvi dio u konacno\n");
		kopiraj1 (konacno, info->niz3,0,1);
		zbroj+=info->niz3[0];
		//printf ("sada je konacno\n");
		//ispis(konacno, duljina_konacno);
	}
	if(info->dio==2)
	{
		//printf ("kopiramo drugi dio u konacno\n");
		kopiraj1(konacno, info->niz3, 0,1);
		zbroj+=info->niz3[0];
		//printf ("sada je konacno\n");
		//ispis (konacno, duljina_konacno);
	}
	//printf ("%lf\n", konacno[0],konacno[0]);
	close (info->socket);
	free (info->niz1);
	free(info->niz2);
}



int spoji(char *dekatskiIP, int port)
{
	int sock=socket(PF_INET, SOCK_STREAM, 0);
	if(sock==-1)perror("greska u socketu\n");
	struct sockaddr_in server;
	server.sin_family=AF_INET;
	server.sin_port=htons(port);
	int error=inet_aton(dekatskiIP, &server.sin_addr);
	if(error==0)perror("greska u inet_aton\n");
	memset(server.sin_zero, '\0', 8);
	
	//connect
	error=connect(sock, (struct sockaddr*)&server, sizeof(server));
	if(error==-1)perror ("greska u connectu\n");
	else printf ("spojili smo se na %s na portu %d i uticnica je %d\n", dekatskiIP, port, sock);
	return sock;
}

int main ()
{
	//ucitavanje vektora
	printf ("upiste duljunu vektora x i y\n");
	int n;
	scanf ("%d", &n);
	
	konacno=(double*)malloc(n*sizeof(double));
	duljina_konacno=2;
	
	double x[n],y[n];
	printf ("upisite vektor x\n");
	for (int i=0; i < n; i++)
		scanf ("%lf", &x[i]);
	printf ("upsite vektor y\n");
	for (int i=0; i < n; i++)
		scanf ("%lf", &y[i]);
		
	//trebaju na adrese na kojima se nalaze serveri i portovi
	printf ("ucitajte adresu na kojoj se nalazi prvi server\n");
	char dekatskiIP[2][200];
	int port[2];
	scanf ("%s", dekatskiIP[0]);
	printf ("ucitajte port na koji se spajamo\n" );
	scanf ("%d", &port[0]);
	printf ("ucitajte adresu na kojoj se nalazi durgi server\n");
	scanf ("%s", dekatskiIP[1]);
	printf ("ucitajte port na koji se spajamo\n" );
	scanf ("%d", &port[1]);
	
	//spajamo se na servere
	niz_parametara[0].socket=spoji(dekatskiIP[0],port[0]);
	niz_parametara[1].socket=spoji(dekatskiIP[1], port[1]);
	
	
	niz_parametara[0].niz1=(double*)malloc(sizeof(double)*(n/2));
	niz_parametara[0].niz2=(double*)malloc(sizeof(double)*(n/2));
	niz_parametara[1].niz1=(double*)malloc(sizeof(double)*(n-(n/2)));
	niz_parametara[1].niz2=(double*)malloc(sizeof(double)*(n-(n/2)));
	kopiraj(niz_parametara[0].niz1,x,0,n/2);
	kopiraj(niz_parametara[0].niz2, y,  0,n/2);
	niz_parametara[0].duljina=n/2;
	kopiraj(niz_parametara[1].niz1, x, n/2, n);
	kopiraj(niz_parametara[1].niz2, y, n/2, n);
	niz_parametara[1].duljina=n-n/2;
	niz_parametara[0].dio=1;
	niz_parametara[1].dio=2;
	
	/*printf ("ispis:\n");
	ispis(niz_parametara[0].niz1,n/2);
	ispis(niz_parametara[0].niz2, n/2);
	ispis(niz_parametara[1].niz1, n-(n/2));
	ispis(niz_parametara[1].niz2,n-(n/2));
	printf ("\n \n");*/
	
	//sada cemo im istodobno poslati djelove niza i cekati odgovor
	pthread_t dretva[2];
	int error=pthread_create (&dretva[0], NULL, obradi, (void*)&niz_parametara[0]);
	if(error!=0)error1("geska u dretvi1\n");
	error=pthread_create (&dretva[1], NULL, obradi, (void*)&niz_parametara[1]);
	if(error!=0)error1("geska u dretvi1\n");
	pthread_join(dretva[0], NULL);
	pthread_join(dretva[1], NULL);
	printf ("umnozak vektora x i y je: ");
	//printf ("%lf", konacno[0]+konacno[0]);
	printf ("%lf", zbroj);
	free(konacno);
	return 0;
	
	
}
