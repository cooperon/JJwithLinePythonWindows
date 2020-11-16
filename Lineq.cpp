#include <stdio.h>
#include <math.h>



#define _D0MIN 1.e-20
/* ********************************************************************
 *                                                                    *
 *       Solving of linear equation system with tridiagonal           *
 *                           matrix                                   *
 *                                                                    *
 *   a0[i]*y[i]-al[i]*y[i-1]-ar[i]*y[i+1]=f[i] i=1...M                *
 *                                                                    *
 *  Input parameters:                                                 *
 *                   double a0[],al[],ar[],f[] -arrays with           *
 *                   dimension at least M                             *
 *                   integer M - number of points                     *
 *                   double alf[],bet[] - arrays with dimension       *
 *                   at least M-1 - working space                     *
 *                                                                    *
 *  Output parameters:                                                *
 *                   double y[] - array with solution, dimension      *
 *                   at least  M                                      *
 *                   int err_param - index of zero divider, if return *
 *                   value = 102, 0-in other cases                    *
 *                                                                    *
 *  Return value:                                                     *
 *                   0 - sucsessfull exit                             *
 *                -101 - c[0] == 0                                    *
 *                -102 - c[i]-a[i]*alf[i] == 0                        *
 *                       (err_param = i)                              *
 *                -103 - N < 2                                        *
 *                                                                    *
 * Common block LINEAR:                                               *
 *                          real*8 D0MIN - very low number            *
 *                          number below this number are regarded as  *
 *                          zero                                      *
 *                                                                    *
 ********************************************************************** */



int LinearSystem(double *a0, double *ar, double *al, double *f, int M,
	             double *alf, double *bet, double *y)
{
 int  err_param, i, linearsystem_v;
 double xx;

 err_param = 0;
 linearsystem_v = 0;


 if( M < 2 ) { linearsystem_v = -103; goto _END; }

/*  Direct phase */

 alf[0] = 0.;
 bet[0] = 0.;
 for( i = 0; i<M-1; i++ )
 {
  xx = a0[i]-al[i]*alf[i];
  if( fabs( xx ) < _D0MIN )
  {
   linearsystem_v = -102;
   goto _END;
  }

  alf[i + 1] = ar[i]/xx;
  bet[i + 1] = (al[i]*bet[i]+ f[i])/xx;
 }

/*  Inverse phase */

 y[M-1] = (f[M-1]+al[M-1]*bet[M-1])/(a0[M-1]-al[M-1]*alf[M-1]);
 for( i = M-2; i>=0; i-- )
 {
  y[i] = alf[i + 1]*y[i + 1]+bet[i + 1];
 }


_END:
 return( linearsystem_v );
} 


