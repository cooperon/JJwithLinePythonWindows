#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h> 
#include <math.h>
#include "FileIO.h"
#include "MainProg.h"
#include "Main.h"
#include "CommentOutput.h"
#include "TimeFuncs.h"



int __declspec(dllexport)JJwithLine(InputData *inputData)
{
	double dI;
	double dx;
	double t = 0.;
	double v;
	wchar_t *wcharPtr = NULL;
	size_t i, j, nerrors, *errors = NULL;
	AuxiliaryData auxiliaryData;
	ParallelData *parallelData = NULL;
	double *currents = NULL, *voltages=NULL, *autoVoltages = NULL, *rDs=NULL;
	double *waveform = NULL;
	double *fi = NULL, *fiOld=NULL;
	double *lineCurrent = NULL;
	double *fi0 = NULL, *hi0 = NULL;
	double *tmpBuffer = NULL;
	ResonatorValues *resonatorValues = NULL;
	int nThreadCurrents;
	double hiOld0;

	printf("Dimensionless RSJ model with tansmission line\n");
        InitializeTimeMeasure();

	if (ProcessResonators(inputData)) return -1;
	
	if ((nerrors = VerifyInputData(inputData, &errors)))
	{
		for (i = 0; i<nerrors; i++)
			printf("%s\n", GetErrorMessage(errors[i]));
		free(errors);
		return -1;
	}

	CommentOutput(inputData);

	if (MainProg(inputData))
	 SetComment(_COMMENT_FILENAME, "===ERROR==\n");
	SetComment(_COMMENT_FILENAME, "==========================================\n");
	printf("Calculation time: %f s\n", GetRunTime());
_END:
//	free(inputData->resonators);

        

//	getchar();
	return 0;
}

