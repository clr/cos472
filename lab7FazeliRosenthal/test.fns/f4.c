/************************************************  file f4.c  ****/


/* used in random number generator below */
#define MASK 2147483647UL
#define PRIME 65539
#define SCALE 0.4656612875e-9
#define Rand()  (( Seed = ( (Seed * PRIME) & MASK) ) * SCALE )

/* function prototype */
double eval(char *str, int length, double *vect, int genes);

extern unsigned long int Seed;

double eval(char *str, int length, double *vect, int genes)
{
	register int i;
	register int k;
	double ans;
	double prod;

	ans = 0.0;
	for (i=0; i< genes; i++)
	{
		prod = 1.0;
		for (k = 0; k<4; k++)
			prod *= vect[i];
		ans += (i+1)*prod;
	}

	/* now add Gaussian noise */
	prod = 0.0;
	for (i=0; i<12; i++) prod += Rand();
	prod -= 6.0;

	ans += prod;
	return (ans);
}

/** end of file **/
