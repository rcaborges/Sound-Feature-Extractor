#include <stdlib.h>
#include <math.h>

/* macros */
#define TWO_PI (6.2831853071795864769252867665590057683943L)

/* function prototypes */
float rms_calc(float *sig, int win);

/* RMS */
float rms_calc(float *sig, int win)
{
  int i = 0;
  float summ =0;
  for (i=0; i<=win; i++){
      summ = summ + sqrt(pow(sig[i],2));
  }
  return summ;
}

