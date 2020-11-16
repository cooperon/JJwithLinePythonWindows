#include "DataDefinitions.h"

int MainProg(InputData *inputData);
int ParametersCalculation(AuxiliaryData *auxiliaryData);
int NextStep(int k, AuxiliaryData *aData, ParallelData *cData);
void Derivative(double *function, int numberOfPoints, double step, double *derivative);

