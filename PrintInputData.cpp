#include <stdio.h>
#include "PrintInputData.h"

void PrintInputData(InputData *inputData)
{
	int i;
	printf("minCurrent=%le\n", inputData->minCurrent);
	printf("maxCurrent=%le\n", inputData->maxCurrent);
	printf("numberOfCurrentPoints=%u\n", inputData->numberOfCurrentPoints);
	printf("maxTime=%le\n", inputData->maxTime);
	printf("numberOfTimePoints=%u\n", inputData->numberOfTimePoints);
	printf("waveformCurrent=%le\n", inputData->waveformCurrent);
	printf("lineZ=%le\n", inputData->lineZ);
	printf("lineLength=%le\n", inputData->lineLength);
	printf("lineDampingTau=%le\n", inputData->lineDampingTau);
	printf("numberOfLinePoints=%u\n", inputData->numberOfLinePoints);
	printf("junctionLocation=%le\n", inputData->junctionLocation);
	printf("numberOfThreads=%d\n", inputData->numberOfThreads);
	printf("numberOfResonators=%d\n", inputData->numberOfResonators);
	for (i = 0; i < inputData->numberOfResonators; i++)
		printf("Resonator #%d: Location=%le R=%le Alpha=%le Frequency=%le L=%le C=%le\n", i + 1,
			inputData->resonators[i].location, inputData->resonators[i].R, inputData->resonators[i].alpha,
			inputData->resonators[i].frequency, inputData->resonators[i].L,
			inputData->resonators[i].C);
	printf("****************************\n\n");

}