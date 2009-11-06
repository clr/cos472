/************************************************  file f6.c  ****/

/* function prototype */
double eval(char *str, int length, double *vect, int genes);

double eval(char *str, int length, double *vect, int genes)
{
	register int i;
	double sum;

	sum = 0.0;
	for (i = 0; i < length; i++)
		sum += (str[i] == '0');

	return (sum);
}

/************************************************ end of file ****/
