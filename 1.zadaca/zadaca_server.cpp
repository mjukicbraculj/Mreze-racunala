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

void zbroji(double *niz1, double *niz2,double **niz3, int duljina)
{
	*niz3=(double*)malloc(sizeof(double));
	(*niz3)[0]=0;
	for (int i=0;i<duljina;i++)(*niz3)[0]+=niz1[i]*niz2[i];
	printf ("saljem %lf", (*niz3)[0]);
}

void obradiKlijenta(int uticnica)
{
	int duljina1, duljina2;
	int error;
	double *niz1, *niz2, *niz3;
	
	error=primiNiz(uticnica, &niz1, &duljina1);
	if(error==0){error1 ("greska u primanju niza1 kod servera\n");}
	else ispis(niz1, duljina1);
	
	error=primiNiz(uticnica, &niz2, &duljina2);
	if(error==0){error1("greska u primanju niza2 kod servera\n");}
	else ispis(niz2, duljina2);
	
	zbroji(niz1, niz2, &niz3, duljina1);
	error=posaljiNiz(uticnica,niz3, 1);
	if(error==0){error1 ("greska u slanju kod servera\n");}
	else printf ("poslao sam niz3 klijentu\n");
	
	free(niz3);
	free(niz2);
	free(niz1);
}

int main()
{
	int port;
	printf ("koji port koristimo?\n");
	scanf ("%d", &port);
	int sock=socket (PF_INET, SOCK_STREAM, 0);
	if(sock==-1)perror ("greska u socketu\n");
	struct sockaddr_in mojAddr;
	mojAddr.sin_family=AF_INET;
	mojAddr.sin_port=htons(port);
	mojAddr.sin_addr.s_addr=INADDR_ANY;
	memset(mojAddr.sin_zero, '\0', 8);
	
	//bind
	int error=bind(sock, (struct sockaddr*)&mojAddr, sizeof(mojAddr));
	if(error==-1)perror ("greska u bindu\n");
	
	//listen
	error=listen(sock, 10);
	if(error==-1)perror ("greska u listenu\n");
	
	//accept
	while(1)
	{
		struct sockaddr_in klijent;
		int len=sizeof(klijent);
		int uticnica=accept(sock, (struct sockaddr*)&klijent,  &len);
		char *dekatskiIP=inet_ntoa(klijent.sin_addr);
		if(uticnica==-1)perror ("greska u acceptu\n");
		else printf ("spojio sam se s: %s\n", dekatskiIP);	
		obradiKlijenta(uticnica);
		close (uticnica);
	}
	return 0;
}
