/************************************************  file f3.c  ****/

/* function prototype */
double eval(char *str, int length, double *vect, int genes);

double eval(char *str, int length, double *vect, int genes)
{
	register int k[5];
	register int i;
	double ans;

	ans = 0;
	for (i=0; i<5; i++)
	{
		k[i] = vect[i];
		if ( k[i] > vect[i]) k[i] -= 1;
		ans += k[i];
	}
	return (ans + 30.0);
}

/** end of file **/
