#include "CeilFunction.h"
#include <cmath>

using namespace RevBayesCore;

CeilFunction::CeilFunction(const TypedDagNode<double> *x) : TypedFunction<long>( new long(0) ),
    a( x )
{
    addParameter( x );
    
}


CeilFunction* CeilFunction::clone( void ) const {

    return new CeilFunction(*this);

}


void CeilFunction::swapParameterInternal(const DagNode *oldP, const DagNode *newP)
{

    if (oldP == a) 
    {
        a = static_cast<const TypedDagNode<double>* >( newP );
    }
    
}

void CeilFunction::update( void )
{
    *value = int( ceil( a->getValue() ) );
}


