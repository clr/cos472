/************************************************  file f1.c  ****/

/* function prototype */
double eval(char *str, int length, double *vect, int genes);

double eval(char *str, int length, double *vect, int genes)
{
	register int i;
	double sum;

	sum = 0.0;
	for (i = 0; i < genes; i++)
		sum += vect[i] * vect[i];

	return (sum);
}

/************************************************ end of file ****/
