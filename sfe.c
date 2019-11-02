#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sndfile.h>

#include "fft.c"
#include "rms.c"

#define N 1024
#define TWOPOWER 14
#define TWOPI 6.283185307179586476925287
#define PI 3.141592653589793238462643

char *replace_str(char *str, char *orig, char *rep)
{
  static char buffer[4096];
  char *p;
  if(!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
    return str;
  strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
  buffer[p-str] = '\0';
  sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));
  return buffer;
}

char *calcDesc(char * file)
{
        file = replace_str(file, "//", "/");

	int win= 1024;
	int hop = 512;
	double (*x)[2];           /* pointer to time-domain samples */
	double (*X)[2];           /* pointer to frequency-domain samples */
	double dummy;             /* scratch variable */
	double r, prob, z;
	
	// Open sound file
	SF_INFO sndInfo;
	SNDFILE *sndFile = sf_open(file, SFM_READ, &sndInfo);
	if (sndFile == NULL) {
		fprintf(stderr, "Error reading source file '%s': %s\n", file, sf_strerror(sndFile));
	}

        float *buffer = malloc(sndInfo.frames * sizeof(float));
        long numFrames;
        int m,n;
        if (sndInfo.channels == 2) {
        
             // Allocate memory
             float *fbuffer = malloc(2*sndInfo.frames * sizeof(float));
             if (fbuffer == NULL) {
        	 fprintf(stderr, "Could not allocate memory for file\n");
                 sf_close(sndFile);
             }
             // Load data
             numFrames = sf_readf_float(sndFile, fbuffer, sndInfo.frames);
             // Check correct number of samples loaded
             if (numFrames != sndInfo.frames) {
        	 fprintf(stderr, "Did not read enough frames for source\n");
                 sf_close(sndFile);
                 free(fbuffer);
             }
             // Aloca canal direito
             //TODO
             n=1;
             buffer[0]=fbuffer[1];
             for(m=3; m<(int)numFrames*2; m=m+2)
             {	
                 buffer[n] = fbuffer[m];
                 n=n+1;
             }
             free(fbuffer);
        }
        
        if (sndInfo.channels == 1) {
 
            // Allocate memory
            buffer = malloc(sndInfo.frames * sizeof(float));
            if (buffer == NULL) {
                fprintf(stderr, "Could not allocate memory for file\n");
                sf_close(sndFile);
            }
            // Load data
            numFrames = sf_readf_float(sndFile, buffer, sndInfo.frames);
            // Check correct number of samples loaded
            if (numFrames != sndInfo.frames) {
                fprintf(stderr, "Did not read enough frames for source\n");
                sf_close(sndFile);
                free(buffer);
            }		
        }
 
        //Hamming
 	float *ham = malloc(win * sizeof(float));
 	int i;
 	for (i = 0; i < win; i++) {
 	    ham[i] = 0.5 * (1 - cos(2*PI*i/(win-1)));
 	}

 	float *sig = malloc(win * sizeof(float));
        float *rms = malloc(ceil((int)numFrames/hop) * sizeof(float));
        double (*vft)[(int)ceil((int)numFrames/hop)];
 	int j, k;
 	float sum = 0.0;
 	float half = floor(win/2);
 	float summ =0;
 
 	/* Allocate time- and frequency-domain memory. */
 	x = malloc(2 * win * sizeof(double));
 	X = malloc(2 * N * sizeof(double));
        vft = malloc ((int)ceil((int)numFrames/hop)* N * sizeof(double)); 

 	for (j=0; j<(int)ceil((int)numFrames/hop); j++){ 
 		i=0; 
 		for (k=(j*hop); k<=((j*hop)+win); k++){
 		    sig[i] = buffer[k]*ham[i];
 		    i++;	
 		}
                // Calcula a energia RMS do quadro atual
                rms[j] = rms_calc(sig,win);

 		//Calcula FFT do quadro atual
 		for (i=0; i<win; i++){
             		x[i][0] = (double)sig[i];
 		}		
 		fft(N, x, X);
		for (i=0; i<N; i++){
            	    vft[i][j]=sqrt(pow(X[i][0],2))*i; //FFT
		}
 	}	
 	
        char chardesc[(int)(numFrames/hop) * sizeof(float)];
        char str[15 * (int)numFrames/hop * sizeof(float)];
        printf("{\"fft\":[\n[");
        for(m=1; m<ceil((int)numFrames/hop)-3; m++)
        {
            printf("%f",vft[0][m]);
            //METADE DO SPECTRO PARA QUE NAO SE REPITA		
	    for(n=1; n<N/2; n++)
	        printf(",%f",vft[n][m]);
	    printf("],\n[");
        }
        //ULTIMA INSERÇAO
	printf("%f",vft[0][m]);
	for(n=1; n<N/2; n++)
	    printf(",%f",vft[n][m]);
        printf("]],\n");
        
        printf("\"rms\":[\n");
        for(m=1; m<ceil((int)numFrames/hop)-3; m++)
        {
            printf("%f,\n",rms[m]);
        }
        //ULTIMA INSERÇAO
        printf("%f\n",rms[m]);
        printf("]}");

 	free(sig);
 	free(vft);
 	free(ham);
 	free(rms);
 	free(x);
 	free(X);
 	return 0;

}


int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("error: invalid number of arguments...\n");
        printf("usage: %s [file]\n", argv[0]);
        return 1;
    }
    calcDesc(argv[1]);
    return 0;      
}
