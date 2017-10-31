#include "StationarityTest.h"
#include "DistributionNormal.h"

#include <cmath>

using namespace RevBayesCore;
using namespace std;


StationarityTest::StationarityTest(size_t b, double f) : ConvergenceDiagnosticContinuous(),
    nBlocks( b ),
    p( f )
{

}

bool StationarityTest::assessConvergence(const TraceNumeric& trace)
{
    // calculate the block size
    size_t blockSize = trace.size(true) / nBlocks;
    
    // use correction for multiple sampling
    double p_corrected = pow(1.0-p, 1.0/nBlocks);
    
    // get the mean for the trace
    trace.computeMean();
    double traceMean = trace.getMean();
    
    // get a mean and standard error for each block
    std::vector<double> blockMeans =  std::vector<double>(nBlocks,0.0);
    std::vector<double> blockSem =  std::vector<double>(nBlocks,0.0);
    for (size_t i=0; i<nBlocks; i++)
    {
        trace.computeCorrelation(i*blockSize+trace.getBurnin(),(i+1)*blockSize+trace.getBurnin());

        blockMeans[i]   = trace.getMean();
        blockSem[i]     = trace.getSem();
        
        // get the quantile of a normal with mu=0, var=sem and p=(1-p_corrected)/2
        double quantile = RbStatistics::Normal::quantile(0.0, blockSem[i], p_corrected);
        // check if the trace mean is outside this confidence interval
        if (blockMeans[i]-quantile > traceMean || blockMeans[i]+quantile < traceMean)
        {
            // the mean of the whole trace falls out of the confidence interval for this block and hence we cannot reject with p-confidence that the trace has not converged
            return false;
        }
    }
    
    return true;
}

bool StationarityTest::assessConvergence(const std::vector<TraceNumeric>& traces)
{
    
    // get number of chains
    size_t nChains = traces.size();
    
    // get the mean between all traces
    double total_mean = 0.0;
    size_t total_sample_size = 0.0;

    // get the mean and standard error for each chain
    std::vector<double> chainMeans =  std::vector<double>(nChains,0.0);
    std::vector<double> chainSem =  std::vector<double>(nChains,0.0);
    for (size_t i=0; i<nChains; i++)
    {
        traces[i].computeCorrelation();

        chainMeans[i] = traces[i].getMean();
        chainSem[i] = traces[i].getSem();

        total_mean += chainMeans[i]*traces[i].size(true);
        total_sample_size += traces[i].size(true);
    }
    
    total_mean /= double(total_sample_size);

    // use correction for multiple sampling
    double p_corrected = pow(1.0-p, 1.0/nChains);

    for (size_t i=0; i<nChains; i++)
    {
        // get the quantile of a normal with mu=0, var=sem and p=(1-p_corrected)/2
        double quantile = RbStatistics::Normal::quantile(0.0, chainSem[i], p_corrected);
        // check if the trace mean is outside this confidence interval
        if (chainMeans[i]-quantile > total_mean || chainMeans[i]+quantile < total_mean)
        {
            // the mean of the whole trace falls out of the confidence interval for this block and hence we cannot reject with p-confidence that the trace has not converged
            return false;
        }
    }
    
    return true;
}
