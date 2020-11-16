#ifndef _FILE_IO_H
#define _FILE_IO_H
#include <limits.h>
#include "DataDefinitions.h"


#ifdef _FILE_IO


static const char *ERROR_MESSAGES[] =
{
	"Wrong \"MinCurrent\" parameter",
	"Wrong \"MaxCurrent\" parameter",
	"Wrong \"NumberOfCurrentPoints\" parameter",
	"Wrong \"MaxTime\" parameter",
	"Wrong \"NumberOfTimePoints\" parameter",
	"Wrong \"WaveformCurrent\" parameter",
	"Wrong \"LineZ\" parameter",
	"Wrong \"LineLength\" parameter",
	"Wrong \"LineDampingTau\" parameter",
	"Wrong \"ResonatorLocation\" parameter",
	"Wrong \"ResonatorFrequency\" parameter",
	"Wrong \"ResonatorR\" parameter",
	"Wrong \"ResonatorAlpha\" parameter",
	"Wrong \"ResonatorC\" parameter",
	"Wrong \"ResonatorL\" parameter",
	"Wrong \"LineNpoints\" parameter",
	"Error in Resonator Circuit definition",
	"Number of threads is too big",
	"Wrong \"JunctionLocation\" parameter"
};


static size_t NUMBER_OF_ERROR_MESSAGES = sizeof(ERROR_MESSAGES) / sizeof(*ERROR_MESSAGES);

enum struct  ERROR_MESSAGE_INDEX {
	_MinCurrent,
	_MaxCurrent,
	_NumberOfCurrentPoints,
	_MaxTime,
	_NumberOfTimePoints,
	_WaveformCurrent,
	_LineZ,
	_LineLength,
	_LineDampingTau,
	_ResonatorLocation,
	_ResonatorFrequency,
	_ResonatorR,
	_ResonatorAlpha,
	_ResonatorC,
	_ResonatorL,
	_LineNpoints,
	_ResonatorError,
	_NumberOfThreadsError,
	_JunctionLocation
};


#endif
size_t VerifyInputData(InputData *data, size_t **errors);
const char *GetErrorMessage(size_t index);
int ProcessResonators(InputData *data);

#endif
