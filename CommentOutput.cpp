#define _COMMENT_OUTPUT
#include "CommentOutput.h"
#include "stdio.h"
#include "TimeFuncs.h"
#include "FileIO.h"

#define _FILE_INPUT_BUFFER_LENGTH 4096

void CommentOutput(InputData *inputData)
{
	FILE *cFp;
	int i;
	cFp = fopen(_COMMENT_FILENAME, "a");
        if(!cFp)
        {
         printf("Comment file can't be opened\n");
         return;
        }
	fprintf(cFp, "==========================================\n");

	fprintf(cFp, "%s\n\n", GetDateTime());
	fprintf(cFp, "MinCurrent=%le\n", inputData->minCurrent);
	fprintf(cFp, "MaxCurrent=%le\n", inputData->maxCurrent);
	fprintf(cFp, "NumberOfCurrentPoints=%u\n", inputData->numberOfCurrentPoints);
	fprintf(cFp, "MaxTime=%le\n", inputData->maxTime);
	fprintf(cFp, "NumberOfTimePoints=%u\n", inputData->numberOfTimePoints);
	fprintf(cFp, "WaveformCurrent=%le\n", inputData->waveformCurrent);
	fprintf(cFp, "LineZ=%le\n", inputData->lineZ);
	fprintf(cFp, "LineLength=%le\n", inputData->lineLength);
	fprintf(cFp, "LineDampingTau=%le\n", inputData->lineDampingTau);
	fprintf(cFp, "NumberOfLinePoints=%u\n", inputData->numberOfLinePoints);
	fprintf(cFp, "JunctionLocation=%le\n", inputData->junctionLocation);
	fprintf(cFp, "NumberOfThreads=%d\n", inputData->numberOfThreads);
	if (inputData->resonators)
	{
		for (i = 0; i < inputData->numberOfResonators; i++)
		{
			fprintf(cFp, "Resonator=%d\n", i + 1);
			fprintf(cFp, "ResonatorLocation=%le\n", inputData->resonators[i].location);
			fprintf(cFp, "ResonatorFrequency=%le\n", inputData->resonators[i].frequency);
			fprintf(cFp, "ResonatorR=%le\n", inputData->resonators[i].R);
			fprintf(cFp, "ResonatorAlpha=%le\n", inputData->resonators[i].alpha);
			fprintf(cFp, "ResonatorC=%le\n", inputData->resonators[i].C);
			fprintf(cFp, "ResonatorL=%le\n", inputData->resonators[i].L);
			fprintf(cFp, "EndResonator=%d", i + 1);
		}
	}
	fprintf(cFp, "-----------------------------------------\n");
	fclose(cFp);
	return;
}

void SetComment(char *commentFileName, char *comment)
{
	FILE *cFp = fopen(commentFileName, "a");
        if(!cFp) 
        {
         printf("Comment file can't be opened\n");
         return;
        } 
	fprintf(cFp, "%s\n", comment);
	fclose(cFp);
	return;
}


