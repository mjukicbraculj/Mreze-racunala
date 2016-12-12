int primiNiz(int sock, double **niz, int *duljina);
int posaljiNiz(int sock, double *niz, int duljina);
void ispis(double *niz, int n);
#define error1( s ) { printf( s ); exit( 0 ); }
#define error2( s1, s2 ) { printf( s1, s2 ); exit( 0 ); }
#define myperror( s ) { perror( s ); exit( 0 ); }
