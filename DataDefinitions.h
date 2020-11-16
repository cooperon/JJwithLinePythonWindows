#ifndef _DATA_DEFINITIONS_H
#define _DATA_DEFINITIONS_H

#include <Windows.h>
#include <limits.h>
#include <float.h>
#define _PI 3.141592653589793238462643
#define _2PI 6.283185307179586476925287
#define _PI_4 7.853981633974483e-01
#define _PI_2 1.5707963267948966192313
#define _TOL 1.e-10
#define _ROOT_STEP 0.1

#define _GHz_mV 483.6  /* GHz/mV */
#define _GAMMA_COEFF 4.2e-5   /* mA/K */
#define _NEP_COEFF 5.72e-13   /* (W*Hz^-0.5)/(mA^1.5*Ohm^0.5) */
#define _VN_COEFF 5.72e-10   /* (mV*s)^0.5 */
#define _CAPACITY_COEFF 0.329e3 /* fF*Ohm*mV */
#define _INDUCTANCE_COEFF 329.0 /* fH*mA */

#define _NUMBER_OF_INITIAL_PERIODS 100
#define _NUMBER_OF_INITIAL_TIME_POINTS 1000
#define _THETA0 0.0

#define _MIN_POINT_FOR_PERIOD 100
#define _V_MIN 1.e-2

typedef struct Resonator
{
	double location;
	unsigned int pointNumber;
	double frequency;
	double alpha;
	double C;
	double L;
	double R;
} Resonator;

typedef struct ResonatorValues
{
	double current;
	double fi;
} ResonatorValues;

typedef struct ResonatorCoeffs
{
	int resonatorPoint;
	double bmFiR;
	double bmIr;
	double irFiFiR;
	double irIr;
	double firIr;
} ResonatorCoeffs;


typedef struct InputData
{
	double minCurrent;
	double maxCurrent;
	unsigned int numberOfCurrentPoints;
	double maxTime;
	unsigned int numberOfTimePoints;
	char *waveformFile;
	double waveformCurrent;
	double lineZ;
	double lineLength;
	double lineDampingTau;
	unsigned int numberOfLinePoints;
	double junctionLocation;
	int numberOfThreads;
	int numberOfResonators;
	Resonator *resonators;
	double *voltage;
	double *current;
	double *rd;
	double *voltage0;
	double *x;
	double *waveform;
} InputData;





typedef struct AuxiliaryData
{
	InputData *inputData;
	double dt;
	int junctionPoint;
	unsigned int numberOfResonators;
	double *a;
	double *al;
	double *ar;
	double bFi;
	double bIminusI;
	ResonatorCoeffs *resonatorCoeffs;
	//	double bnFi;
	//	double bnI;
	//	double inDfi;
	//	double dIdFi;
	double bJJ;
	//	double b0I1;
	//	double b0Fi0;
	double iKiK;
	double ikDfi;
}AuxiliaryData;

typedef struct ParallelData
{
	AuxiliaryData *auxiliaryData;
	double *currents;
	double *voltages;
	int waveformIndex;
	double *fi;
	double *fiOld;
	double *lineCurrent;
	double hi;
	ResonatorValues *resonatorValues;
	int numberOfLinePoints;
	double *b;
	double *alpha;
	double *beta;
} ParallelData;

#endif


