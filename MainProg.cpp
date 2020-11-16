#include <stdio.h>
#include <stdlib.h>
#include <omp.h> 
#include <math.h>
#include <windows.h>
#include "FileIO.h"
#include "MainProg.h"
#include "CommentOutput.h"
#include "Lineq.h"
#include "TimeFuncs.h"
#include "PrintInputData.h"



/*
double fi_tmp = 0., hi_tmp = 0.;
double *voltages_tmp = NULL;
double hi_tmp_old = 0.;
double il_tmp = 0.;
double dx_tmp;

LARGE_INTEGER lpFrequency;
*/

#define _NUMBER_OF_TMP_BUFFERS 3 // alpha, beta, bBuffer

#define _SIZE_OF_TMP_BUFFER  inputData->numberOfLinePoints*_NUMBER_OF_TMP_BUFFERS


int MainProg(InputData *inputData)
{
	double dI;
	double dx;
	double t = 0.;
	wchar_t *wcharPtr = NULL;
	size_t i, j,  *errors = NULL;
	AuxiliaryData auxiliaryData;
	ParallelData *parallelData = NULL;
	double *currents = NULL, *voltages = NULL, *autoVoltages = NULL, *rDs = NULL;
	double *waveform = NULL;

	double *fi = NULL, *fiOld = NULL;
	double *lineCurrent = NULL;
	double *fi0 = NULL, *hi0 = NULL;
	double *tmpBuffer = NULL;
	ResonatorValues *resonatorValues = NULL;
	int nThreadCurrents;
	double hiOld0;
	FILE *fp = NULL;
	double v;

	int nThread;
	double hiOld;
	double fiMin, hiMin, maxTime;
	int nStepsCorr, jM;
	int numberOfTimePoints;

	PrintInputData(inputData);
	auxiliaryData.inputData = inputData;

	i = (inputData->numberOfCurrentPoints / inputData->numberOfThreads)*inputData->numberOfThreads;
	if (!i)
		inputData->numberOfThreads = inputData->numberOfCurrentPoints;
	else
		inputData->numberOfCurrentPoints = i;
	nThreadCurrents = inputData->numberOfCurrentPoints / inputData->numberOfThreads;

	omp_set_num_threads(inputData->numberOfThreads);
	printf("Parallel processing: %d threads.\n", inputData->numberOfThreads);


	if (ParametersCalculation(&auxiliaryData)) goto _END;
	if (inputData->numberOfCurrentPoints > 1)
		dI = (inputData->maxCurrent - inputData->minCurrent) / (inputData->numberOfCurrentPoints - 1);
	else dI = 0.;


	parallelData = (ParallelData*)calloc(inputData->numberOfThreads, sizeof(ParallelData));
	fi = (double*)calloc(inputData->numberOfLinePoints*inputData->numberOfThreads, sizeof(double));
	fiOld = (double*)calloc(inputData->numberOfLinePoints*inputData->numberOfThreads, sizeof(double));
	lineCurrent = (double*)calloc(inputData->numberOfLinePoints*inputData->numberOfThreads, sizeof(double));
	tmpBuffer = (double*)calloc(_SIZE_OF_TMP_BUFFER*inputData->numberOfThreads, sizeof(double));
	resonatorValues = (ResonatorValues*)calloc(inputData->numberOfResonators*inputData->numberOfThreads, sizeof(ResonatorValues));
	currents = (double*)calloc(inputData->numberOfCurrentPoints, sizeof(double));
	autoVoltages = (double*)calloc(inputData->numberOfCurrentPoints, sizeof(double));
	voltages = (double*)calloc(inputData->numberOfCurrentPoints, sizeof(double));
	rDs = (double*)calloc(inputData->numberOfCurrentPoints, sizeof(double));

	fi0 = (double*)calloc(inputData->numberOfCurrentPoints, sizeof(double));
	hi0 = (double*)calloc(inputData->numberOfCurrentPoints, sizeof(double));
	for (i = 0; i < inputData->numberOfThreads; i++)
	{
		parallelData[i].currents = currents + i*nThreadCurrents;
		parallelData[i].voltages = voltages + i*nThreadCurrents;
		parallelData[i].b = tmpBuffer + i*_SIZE_OF_TMP_BUFFER;
		parallelData[i].alpha = parallelData[i].b + inputData->numberOfLinePoints;
		parallelData[i].beta = parallelData[i].alpha + inputData->numberOfLinePoints;
		parallelData[i].fi = fi + i*inputData->numberOfLinePoints;
		parallelData[i].fiOld = fiOld + i*inputData->numberOfLinePoints;
		parallelData[i].lineCurrent = lineCurrent + i*inputData->numberOfLinePoints;
		parallelData[i].resonatorValues = resonatorValues + i*inputData->numberOfResonators;
		parallelData[i].waveformIndex = -1;
		parallelData[i].numberOfLinePoints = inputData->numberOfLinePoints;
	}

	for (i = 0; i < inputData->numberOfCurrentPoints; i++)
	{
		currents[i] = inputData->minCurrent + i*dI;
		hiOld0 = hi0[i];
		for (j = 0; j < inputData->numberOfTimePoints; j++)
		{
			fi0[i] = currents[i] - sin(hi0[i]);
			hi0[i] += fi0[i] * auxiliaryData.dt;
		}
		autoVoltages[i] = (hi0[i] - hiOld0) / inputData->maxTime;
	}

	if (inputData->x && inputData->waveform)
	{
		waveform = (double*)calloc(inputData->numberOfLinePoints, sizeof(double));
		for (i = 0; i < inputData->numberOfCurrentPoints; i++)
		{
			if (inputData->waveformCurrent <= currents[i])
				break;
		}
		printf("Waveform index: %d\n", i);
		if (i < inputData->numberOfCurrentPoints)
			parallelData[i / nThreadCurrents].waveformIndex = i - (i / nThreadCurrents)*nThreadCurrents;
	}

	for (i = 0; i < inputData->numberOfThreads; i++)
	{
		parallelData[i].fi[auxiliaryData.junctionPoint] = parallelData[i].fiOld[auxiliaryData.junctionPoint] = fi0[i*nThreadCurrents];
		parallelData[i].hi = hi0[i*nThreadCurrents];
		for (j = 0; j < inputData->numberOfResonators; j++)
			parallelData[i].resonatorValues[j].fi = parallelData[i].resonatorValues[j].current = 0.;
	}

#pragma omp parallel default(shared) private(nThread, hiOld,i,j, v, jM, nStepsCorr, fiMin, hiMin,numberOfTimePoints, maxTime)
	{
		nThread = omp_get_thread_num();
		numberOfTimePoints = inputData->numberOfTimePoints;
		maxTime = inputData->maxTime;
		for (i = 0; i < nThreadCurrents; i++)
		{
			hiOld = parallelData[nThread].hi;
			for (j = 0; j < numberOfTimePoints; j++)
			{
				if (NextStep(i, &auxiliaryData, &parallelData[nThread]))
					printf("Error in linear equation solving\n");
			}
			v = (parallelData[nThread].hi - hiOld) / maxTime;
			jM = 0;
			hiMin = parallelData[nThread].hi;
			if (fabs(v) > _V_MIN)
			{
				nStepsCorr = (int)(2.*_PI / (v*auxiliaryData.dt));
				fiMin = parallelData[nThread].fi[auxiliaryData.junctionPoint];
				for (j = 0; j < nStepsCorr; j++)
				{
					if (NextStep(i, &auxiliaryData, &parallelData[nThread]))
						printf("Error in linear equation solving\n");
					if (parallelData[nThread].fi[auxiliaryData.junctionPoint] < fiMin)
					{
						hiMin = parallelData[nThread].hi;
						fiMin = parallelData[nThread].fi[auxiliaryData.junctionPoint];
						jM = j;
					}
				}

			}

			parallelData[nThread].voltages[i] = (hiMin - hiOld) / (maxTime + jM*auxiliaryData.dt);
			if (parallelData[nThread].waveformIndex == i)
				memcpy(waveform, parallelData[nThread].fi, inputData->numberOfLinePoints * sizeof(double));
			if ((i / 10) * 10 == i)
			{
				printf(".");
				fflush(stdout);
			}
		}
	}
	printf("\n");
	Derivative(voltages, inputData->numberOfCurrentPoints, dI, rDs);
	if (inputData->numberOfCurrentPoints > 3)
	{
		rDs[0] = rDs[1] = rDs[2];
		for (i = 3; i < inputData->numberOfCurrentPoints; i++)
		{
			if ((i / nThreadCurrents)*nThreadCurrents == i)
			{
				rDs[i - 1] = rDs[i - 2];
				rDs[i + 1] = rDs[i];
			}
		}
	}
	if (inputData->current)
		memcpy(inputData->current, currents, inputData->numberOfCurrentPoints * sizeof(double));
	if (inputData->voltage)
		memcpy(inputData->voltage, voltages, inputData->numberOfCurrentPoints * sizeof(double));
	if (inputData->rd)
		memcpy(inputData->rd, rDs, inputData->numberOfCurrentPoints * sizeof(double));
	if (inputData->voltage0)
		memcpy(inputData->voltage0, autoVoltages, inputData->numberOfCurrentPoints * sizeof(double));
	if (inputData->x && inputData->waveform && waveform)
	{
		dx = inputData->lineLength / (inputData->numberOfLinePoints - 1);
		for (i = 0; i < inputData->numberOfLinePoints; i++)
			inputData->x[i] = dx*i;
		memcpy(inputData->waveform, waveform, inputData->numberOfLinePoints * sizeof(double));
	}
_END:
	free(parallelData);
	free(currents);
	free(voltages);
	free(autoVoltages);
	free(rDs);
	free(waveform);
	free(fi);
	free(fiOld);
	free(lineCurrent);
	free(fi0);
	free(hi0);
	free(tmpBuffer);
	free(auxiliaryData.resonatorCoeffs);
	free(resonatorValues);




	//	getchar();
	return 0;
}

int ParametersCalculation(AuxiliaryData *auxiliaryData)
{
	int iret = 0, i, j;
	double dXdT;
	double dampCoeff;
	double dXdT2;
	double zDxDt;
	double resonatorDenom;
	double alpha_OmegaDt;
	double ai;
	double omegaDt;
	double omegaDt2;


	InputData *inputData = auxiliaryData->inputData;

	auxiliaryData->a = (double*)calloc(inputData->numberOfLinePoints, sizeof(double));
	auxiliaryData->al = (double*)calloc(inputData->numberOfLinePoints, sizeof(double));
	auxiliaryData->ar = (double*)calloc(inputData->numberOfLinePoints, sizeof(double));

	auxiliaryData->dt = inputData->maxTime / (inputData->numberOfTimePoints - 1);
	dXdT = inputData->lineLength / ((inputData->numberOfLinePoints - 1)*auxiliaryData->dt);
	dXdT2 = dXdT*dXdT;
	zDxDt = inputData->lineZ * dXdT;
	dampCoeff = 1. + auxiliaryData->dt / inputData->lineDampingTau;
	ai = 2. + dXdT2*dampCoeff;

	auxiliaryData->junctionPoint = (unsigned int)((inputData->numberOfLinePoints - 1)*(inputData->junctionLocation / inputData->lineLength));

	printf("Josephson junction point=%d\n", auxiliaryData->junctionPoint);
	for (i = 0; i < inputData->numberOfLinePoints; i++)
	{
		auxiliaryData->a[i] = ai;
		auxiliaryData->al[i] = 1;
		auxiliaryData->ar[i] = 1;
	}
	auxiliaryData->a[0] = 1. + dXdT2*dampCoeff;

	auxiliaryData->a[inputData->numberOfLinePoints - 1] = 1. + dXdT2*dampCoeff;
	auxiliaryData->al[0] = auxiliaryData->ar[inputData->numberOfLinePoints - 1] = 0.;
	auxiliaryData->a[auxiliaryData->junctionPoint] += zDxDt*dampCoeff;
	auxiliaryData->bJJ = zDxDt*dampCoeff;
	auxiliaryData->bFi = dXdT2*dampCoeff;
	auxiliaryData->bIminusI = zDxDt;
	auxiliaryData->iKiK = 1. / dampCoeff;
	auxiliaryData->ikDfi = 1. / (zDxDt*dampCoeff);

	auxiliaryData->resonatorCoeffs = (ResonatorCoeffs*)calloc(inputData->numberOfResonators, sizeof(ResonatorCoeffs));

	for (i = 0; i < inputData->numberOfResonators; i++)
	{
		auxiliaryData->resonatorCoeffs[i].resonatorPoint = (unsigned int)((inputData->numberOfLinePoints - 1)*(inputData->resonators[i].location / inputData->lineLength));
		printf("Resonator #%d: Point %d\n", i + 1, auxiliaryData->resonatorCoeffs[i].resonatorPoint);
		omegaDt = inputData->resonators[i].frequency*auxiliaryData->dt;
		omegaDt2 = omegaDt*omegaDt;
		alpha_OmegaDt = inputData->resonators[i].alpha / omegaDt;
		resonatorDenom = 1. / (1. + alpha_OmegaDt + 1. / omegaDt2);
		auxiliaryData->a[auxiliaryData->resonatorCoeffs[i].resonatorPoint] += dampCoeff*resonatorDenom * alpha_OmegaDt*zDxDt / inputData->resonators[i].R;
		auxiliaryData->resonatorCoeffs[i].bmFiR = dampCoeff*resonatorDenom*zDxDt*alpha_OmegaDt / inputData->resonators[i].R;
		auxiliaryData->resonatorCoeffs[i].bmIr = -dampCoeff*resonatorDenom*zDxDt / omegaDt2;
		auxiliaryData->resonatorCoeffs[i].irFiFiR = resonatorDenom*alpha_OmegaDt / inputData->resonators[i].R;
		auxiliaryData->resonatorCoeffs[i].irIr = resonatorDenom / omegaDt2;
		auxiliaryData->resonatorCoeffs[i].firIr = inputData->resonators[i].R / alpha_OmegaDt;
	}
	auxiliaryData->numberOfResonators = inputData->numberOfResonators;
	return iret;
}


int NextStep(int k, AuxiliaryData *aData, ParallelData *cData)
{
	int i, iret = 0;

	for (i = 1; i < cData->numberOfLinePoints - 1; i++)
		cData->b[i] = aData->bFi*cData->fi[i] + aData->bIminusI*(cData->lineCurrent[i] - cData->lineCurrent[i + 1]);
	cData->b[cData->numberOfLinePoints - 1] = aData->bFi* cData->fi[cData->numberOfLinePoints - 1] +
		aData->bIminusI* cData->lineCurrent[cData->numberOfLinePoints - 1];
	cData->b[0] = aData->bFi*cData->fi[0] - aData->bIminusI* cData->lineCurrent[1];
	for (i = 0; i<aData->numberOfResonators; i++)
		cData->b[aData->resonatorCoeffs[i].resonatorPoint] += aData->resonatorCoeffs[i].bmFiR* cData->resonatorValues[i].fi +
		aData->resonatorCoeffs[i].bmIr* cData->resonatorValues[i].current;
	cData->b[aData->junctionPoint] += aData->bJJ* (cData->currents[k] - sin(cData->hi));
	memcpy(cData->fiOld, cData->fi, cData->numberOfLinePoints * sizeof(double));
	if (iret = LinearSystem(aData->a, aData->ar, aData->al, cData->b, cData->numberOfLinePoints,
		cData->alpha, cData->beta, cData->fi)) return iret;
	cData->hi += cData->fi[aData->junctionPoint] * aData->dt;
	for (i = 0; i < aData->numberOfResonators; i++)
	{
		cData->resonatorValues[i].current = aData->resonatorCoeffs[i].irFiFiR*(cData->fi[aData->resonatorCoeffs[i].resonatorPoint] - cData->resonatorValues[i].fi) + aData->resonatorCoeffs[i].irIr* cData->resonatorValues[i].current;
		cData->resonatorValues[i].fi += aData->resonatorCoeffs[i].firIr* cData->resonatorValues[i].current;
	}
	for (i = 1; i < cData->numberOfLinePoints; i++)
		cData->lineCurrent[i] = aData->iKiK*cData->lineCurrent[i] + aData->ikDfi*(cData->fi[i - 1] - cData->fi[i]);

	return 0;
}



void Derivative(double *function, int numberOfPoints, double step, double *derivative)
{
	int i;
	derivative[0] = (4.*function[1] - 3.*function[0] - function[2]) / step*0.5;
	derivative[numberOfPoints - 1] = (function[numberOfPoints - 3] - 4.*function[numberOfPoints - 2] +
		3.*function[numberOfPoints - 1]) / step*0.5;
	for (i = 1; i<numberOfPoints - 1; i++)
		derivative[i] = (function[i + 1] - function[i - 1]) / step*0.5;
}