#include "EssTest.h"

using namespace RevBayesCore;
using namespace std;


EssTest::EssTest(double t) : ConvergenceDiagnosticContinuous(),
    k( t )
{
    
}


bool EssTest::assessConvergence(const TraceNumeric& trace)
{
    
    // make mean invalid for recalculation
    trace.computeCorrelation();
    return  trace.getEss() > k;
}

