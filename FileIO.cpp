#define _FILE_IO

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <float.h> 
#include <limits.h> 
#include <math.h>
#include <ctype.h>
#include <omp.h>
#include "FileIO.h"


size_t VerifyInputData(InputData *data, size_t **errors)
{
	size_t nerrors = 0;
	*errors = NULL;
        int dwNumberOfProcessors;
	int i;
	FILE* fp = NULL;

        dwNumberOfProcessors=omp_get_num_procs();
	if ((data->maxCurrent == DBL_MAX) || (data->maxCurrent <= 0))
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_MaxCurrent;
		nerrors++;
	}


	if ((data->minCurrent == DBL_MAX) || (data->minCurrent <= 0))
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_MinCurrent;
		nerrors++;
	}

	if ((data->maxTime == DBL_MAX) || (data->maxTime <= 0))
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_MaxTime;
		nerrors++;
	}
	if ((data->waveformCurrent < data->minCurrent) || (data->waveformCurrent > data->maxCurrent))
	{
			data->waveformCurrent = -1.;
			data->waveform = NULL;
			data->x = NULL;
			printf("No waveform will be written\n");
	}

	if (data->numberOfTimePoints == 0)
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_NumberOfTimePoints;
		nerrors++;
	}
	if (data->lineZ == DBL_MAX)
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_LineZ;
		nerrors++;
	}

	if (data->lineLength == DBL_MAX)
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_LineLength;
		nerrors++;
	}

	if (data->lineDampingTau == DBL_MAX)
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_LineDampingTau;
		nerrors++;
	}

	if (data->numberOfLinePoints ==INT_MAX || data->numberOfLinePoints<3)
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_LineNpoints;
		nerrors++;
	}

	
	if (data->lineLength == DBL_MAX || data->lineLength <0.)
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1)*sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_LineLength;
		nerrors++;
	}
	for (i = 0; i < data->numberOfResonators; i++)
	{
		if (data->resonators[i].location == DBL_MAX || data->resonators[i].location <0. || data->resonators[i].location> data->lineLength)
		{
			*errors = (size_t*)realloc(*errors, (nerrors + 1) * sizeof(**errors));
			(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_ResonatorLocation;
			nerrors++;
		}
	}

	if (data->junctionLocation == DBL_MAX || data->junctionLocation <0. || data->junctionLocation>data->lineLength)
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1) * sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_JunctionLocation;
		nerrors++;
	}


	if (data->numberOfThreads > dwNumberOfProcessors)
	{
		*errors = (size_t*)realloc(*errors, (nerrors + 1) * sizeof(**errors));
		(*errors)[nerrors] = (size_t)ERROR_MESSAGE_INDEX::_NumberOfThreadsError;
		nerrors++;
	}


	return nerrors;

}

int ProcessResonators(InputData *data)
{
	int i, iret=0;
	if (!data->numberOfResonators) return 0;

	for (i = 0; i < data->numberOfResonators; i++)
	{
		if (data->resonators[i].R < 0.)
		{
			printf("Resonator # %d: resistance have not defined\n", i + 1);
			iret = -1;
		}
		else
		{
			if (data->resonators[i].L > 0. && data->resonators[i].C > 0.)
			{
				data->resonators[i].frequency = 1.L / sqrt(data->resonators[i].L*data->resonators[i].C);
				data->resonators[i].alpha = data->resonators[i].R*sqrt(data->resonators[i].C / data->resonators[i].L);
				printf("Resonator circuit #%d: frequency=%le, alpha=%le\n",
					i + 1, data->resonators[i].frequency, data->resonators[i].alpha);
			}
			else
				if (data->resonators[i].alpha > 0. && data->resonators[i].frequency > 0.)
				{
					data->resonators[i].L = data->resonators[i].R / (data->resonators[i].alpha*data->resonators[i].frequency);
					data->resonators[i].C = data->resonators[i].alpha/(data->resonators[i].frequency * data->resonators[i].R);
					printf("Resonator circuit #%d: L=%le C=%le\n",
						i + 1, data->resonators[i].L, data->resonators[i].C);
				}
				else
				{
					printf("Resonator # %d: parameters have not defined\n", i + 1);
					iret = -1;
				}
		}
	}
	return iret;
}

const char *GetErrorMessage(size_t index)
{
	if (index >= NUMBER_OF_ERROR_MESSAGES) return NULL;
	return ERROR_MESSAGES[index];
}


