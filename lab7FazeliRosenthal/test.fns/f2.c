/************************************************  file f2.c  ****/

/* function prototype */
double eval(char *str, int length, double *vect, int genes);

double eval(char *str, int length, double *vect, int genes)
{
	double ans;

	ans = (vect[0]*vect[0] - vect[1]);
	ans *= ans;
	ans = 100.0*ans + (1.0 - vect[0])*(1.0 - vect[0]);
	return (ans);
}

/** end of file **/
