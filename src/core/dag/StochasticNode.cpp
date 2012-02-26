/**
 * @file
 * This file contains the implementation of StochasticNode, which is derived
 * from VariableNode. StochasticNode is used for DAG nodes holding stochastic
 * variables with an associated distribution object.
 *
 * @brief Implementation of StochasticNode
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-08-16, version 1.0
 * @extends DAGNode
 *
 * $Id$
 */

#include "RbBoolean.h"
#include "Distribution.h"
#include "DistributionDiscrete.h"
#include "RbException.h"
#include "RbUtil.h"
#include "StochasticNode.h"
#include "UserInterface.h"
#include "VectorString.h"
#include "Workspace.h"

#include <algorithm>
#include <cassert>
#include <cmath>


/** Constructor of empty StochasticNode */
StochasticNode::StochasticNode( void ) : VariableNode( ), clamped( false ), distribution( NULL ), type( INSTANTIATED ), needsRecalculation( true ), storedValue( NULL ) {
}


/** Constructor from distribution */
StochasticNode::StochasticNode( Distribution* dist ) : VariableNode( ), clamped( false ), distribution( dist ), type( INSTANTIATED ), needsRecalculation( true ), storedValue( NULL ) {
    
    /* Get distribution parameters */
    std::map<std::string, RbVariablePtr>& params = dist->getMembers();

    /* Check for cycles */
    std::list<DAGNode*> done;
    for ( std::map<std::string, RbVariablePtr>::iterator i = params.begin(); i != params.end(); i++ ) {
        done.clear();
        if ( i->second->getDagNode()->isParentInDAG( this, done ) )
            throw RbException( "Invalid assignment: cycles in the DAG" );
    }

    /* Set parent(s) and add myself as a child to these */
    for ( std::map<std::string, RbVariablePtr>::iterator i = params.begin(); i != params.end(); i++ ) {
        DAGNode* theParam = i->second->getDagNode();
        addParentNode( theParam );
        theParam->addChildNode(this);
    }

    /* We use a random draw as the initial value */
    value = distribution->rv().clone();
    if (value == NULL) {
        std::cerr << "Ooops, rv return NULL!\n";
    }
    
    /* Get initial probability */
    lnProb = calculateLnProbability();
    
}


/** Copy constructor */
StochasticNode::StochasticNode( const StochasticNode& x ) : VariableNode( x ) {

    /* Set distribution */
    distribution = x.distribution->clone();

    /* Get distribution parameters */
    std::map<std::string, RbVariablePtr>& params = distribution->getMembers();

    /* Set parent(s) and add myself as a child to these */
    for ( std::map<std::string, RbVariablePtr>::iterator i = params.begin(); i != params.end(); i++ ) {
        DAGNode* theParam = i->second->getDagNode();
        addParentNode( theParam );
        theParam->addChildNode(this);
    }

    clamped             = x.clamped;
    type                = x.type;
    needsRecalculation  = x.needsRecalculation;
    value               = x.value->clone();
    touched             = x.touched;
    if ( x.touched == true ) {
        storedValue     = x.storedValue->clone();
    } else
        storedValue     = NULL;
    
    lnProb              = x.lnProb;
    storedLnProb        = x.storedLnProb;
}


/** Destructor */
StochasticNode::~StochasticNode( void ) {

    /* Remove parents first */
    for ( std::set<DAGNode*>::iterator i = parents.begin(); i != parents.end(); i++ ) {
        DAGNode* node = *i;
        node->removeChildNode( this );
    }
    parents.clear();
    
    delete distribution;
    
    delete value;
    
    if (storedValue != NULL) {
        delete storedValue;
    }
}


/** Assignment operator */
StochasticNode& StochasticNode::operator=( const StochasticNode& x ) {

    if ( this != &x ) {
        
        /* Remove parents first */
        for ( std::set<DAGNode*>::iterator i = parents.begin(); i != parents.end(); i++ ) {
            DAGNode* node = *i;
            node->removeChildNode( this );
        }
        parents.clear();

        /* Set distribution */
        distribution = x.distribution->clone();

        /* Get distribution parameters */
        std::map<std::string, RbVariablePtr>& params = distribution->getMembers();

        /* Set parent(s) and add myself as a child to these */
        for ( std::map<std::string, RbVariablePtr>::iterator i = params.begin(); i != params.end(); i++ ) {
            DAGNode* theParam = i->second->getDagNode();
            addParentNode( theParam );
            theParam->addChildNode(this);
        }

        clamped             = x.clamped;
        type                = x.type;
        needsRecalculation  = x.needsRecalculation;
        
        factorRoot          = x.factorRoot;
        
        value               = x.value->clone();
        touched             = x.touched;
        storedValue         = x.storedValue->clone();
        lnProb              = x.lnProb;
        storedLnProb        = x.storedLnProb;
        
        // set the name
        name = x.name;
    }

    return ( *this );
}


/** Are any distribution params touched? Get distribution params and check if any one is touched */
bool StochasticNode::areDistributionParamsTouched( void ) const {

    std::map<std::string, RbVariablePtr>& params = distribution->getMembers();
    
    for ( std::map<std::string, RbVariablePtr>::iterator i = params.begin(); i != params.end(); i++ ) {
        
        DAGNode* theNode  = i->second->getDagNode();

        if ( !theNode->isTypeSpec( VariableNode::getClassTypeSpec() ) )
            continue;

        if ( static_cast<const VariableNode*>( (DAGNode*)theNode )->isTouched() )
            return true;
    }

    return false;
}


/** Get the conditional ln probability of the node; do not rely on stored values */
double StochasticNode::calculateLnProbability( void ) {
    
    if (factorRoot != NULL) {
        return factorRoot->calculateSummedLnProbability();
    }
    
    if (needsRecalculation) {
        if (type == INSTANTIATED) { // this should always be true
            lnProb = distribution->lnPdf( *value );
        }
        else {
            throw RbException("We are asked to calculate the summed ln probability but do not have the root of the factor set. Oh oh ...");
        }
    }
    
    return lnProb;
}


/** 
 * Calculate the ln-probability summed over all possible states.
 * This is the variable elimination algorithm. It only works if we can condition on only one parent being eliminated.
 */
double StochasticNode::calculateSummedLnProbability( void ) {
    
    
    if (needsRecalculation) {
        // initialize the probability
        double prob = 0.0;
            
        // we need to iterate over my states
        DistributionDiscrete* d = static_cast<DistributionDiscrete*>( distribution );
            
        // we ask for the state vector
        const std::vector<RbLanguageObject*>& states = d->getStateVector();
            
        // now we calculate the probabilities and likelihoods
        size_t i = 0;
        for (std::vector<RbLanguageObject*>::const_iterator state = states.begin(); state != states.end(); state++) {
            // I set my value so that my children can access if for recalculation
            RbLanguageObject* v = (*state);
            // we set the value here and do not call set value because we do not want that the memory of the old value gets freed
            value = v;
                
            size_t j = 0;
                
            // initialize the likelihood for this state
            double lnLikelihood = 0.0;
                
            // I need to ask for the likelihood of my children
            for (std::set<VariableNode*>::iterator child = children.begin(); child != children.end(); child++) {
                // only if the child has flag as changed I need to ask for the likelihood
                if ( (*child)->isTouched() ) {
                    likelihoods[i][j] = (*child)->calculateSummedLnProbability();
                }
                    
                // add the log-likelihood for this child
                lnLikelihood += likelihoods[i][j];
                
                // increment the child index
                j++;
            }
                
            // set the likelihood for this state to the probability array
            probabilities[i] = distribution->lnPdf( *value ) + lnLikelihood;
                
            prob += exp(probabilities[i]);
            
            // increment the state index
            i++;
        }
            
        lnProb = log(prob);
    }
        
    needsRecalculation = false;
    
    return lnProb;
}


/** Clamp the node to an observed value */
void StochasticNode::clamp( RbLanguageObject* observedVal ) {

    if ( touched )
        throw RbException( "Cannot clamp stochastic node in volatile state" );

    // touch for recalculation
    touch(); 
    
    // check for type conversion
    if (observedVal->isTypeSpec(distribution->getVariableType())) {
        if (value != NULL) {
            delete value;
        }
        value = observedVal;
        if (value == NULL) {
            std::cerr << "Ooops, observed value was NULL!\n";
        }
    }
    else if (observedVal->isConvertibleTo(distribution->getVariableType())) {
        value = static_cast<RbLanguageObject*>(observedVal->convertTo(distribution->getVariableType()) );
        if (value == NULL) {
            std::cerr << "Ooops, observed value was NULL!\n";
        }
        // we own the parameter so we need to delete it
        delete observedVal;
    }
    else {
        throw RbException("Cannot clamp stochastic node with value of type \"" + observedVal->getType() + "\" because the distribution requires a \"" + distribution->getVariableType().toString() + "\".");
    }

    clamped = true;
    lnProb  = calculateLnProbability();

}


/** Clone this object */
StochasticNode* StochasticNode::clone( void ) const {

    return new StochasticNode( *this );
}


/** Clone the entire graph: clone children, swap parents */
DAGNode* StochasticNode::cloneDAG( std::map<const DAGNode*, RbDagNodePtr>& newNodes ) const {

    if ( newNodes.find( this ) != newNodes.end() )
        return ( newNodes[ this ] );

    /* Get pristine copy */
    StochasticNode* copy = new StochasticNode( );
    newNodes[ this ] = copy;
    
    /* Set the name so that the new node remains identifiable */
    copy->setName(name);

    /* Set the copy member variables */
    copy->distribution = distribution->clone();
    copy->clamped      = clamped;
    copy->touched      = touched;
    if (value != NULL) {
        copy->value    = value->clone();
    }
    if (storedValue == NULL)
        copy->storedValue = NULL;
    else {
        copy->storedValue = storedValue->clone();
    }
    copy->lnProb             = lnProb;
    copy->storedLnProb       = storedLnProb;
    copy->needsRecalculation = needsRecalculation;

    /* Set the copy params to their matches in the new DAG */
    std::map<std::string, RbVariablePtr>& params     = distribution->getMembers();
    
    // first we need to remove the copied params
//    copy->distribution->clear();
    
    for ( std::map<std::string, RbVariablePtr>::iterator i = params.begin(); i != params.end(); i++ ) {
        
        // clone the i-th member and get the clone back
        DAGNode* theParam = i->second->getDagNode();
        // if we already have cloned this parent (parameter), then we will get the previously created clone
        DAGNode* theParamClone = theParam->cloneDAG( newNodes );
        
        // set the clone of the member as the member of the clone
        // TODO: We should check that this does destroy the dependencies of the parameters.
        // Instead of creating a new variable we might need to get the pointer to the variable from somewhere.
        copy->distribution->setMember(i->first, new Variable( theParamClone) );

        copy->addParentNode( theParamClone );
        theParamClone->addChildNode( copy );
    }

    /* Make sure the children clone themselves */
    for( std::set<VariableNode*>::const_iterator i = children.begin(); i != children.end(); i++ ) {
        (*i)->cloneDAG( newNodes );
    }

    return copy;
}


/** Complete info about object */
std::string StochasticNode::debugInfo(void) const {
    
    std::ostringstream o;
    o << "StochasticNode:" << std::endl;
    o << "Clamped      = " << ( clamped ? "true" : "false" ) << std::endl;
    o << "Touched      = " << ( touched ? "true" : "false" ) << std::endl;
    o << "Distribution = ";
    distribution->printValue( o );
    o << std::endl;
    o << "Value        = ";
    value->printValue( o );
    o << std::endl;
    o << "Stored value = ";
    if ( storedValue == NULL )
        o << "NULL";
    else
        storedValue->printValue( o );
    
    return o.str();
}


/** Get class name of object */
const std::string& StochasticNode::getClassName(void) { 
    
    static std::string rbClassName = "Stochastic DAG node";
    
	return rbClassName; 
}

/** Get class type spec describing type of object */
const TypeSpec& StochasticNode::getClassTypeSpec(void) { 
    
    static TypeSpec rbClass = TypeSpec( getClassName(), new TypeSpec( VariableNode::getClassTypeSpec() ) );
    
	return rbClass; 
}

/** Get type spec */
const TypeSpec& StochasticNode::getTypeSpec( void ) const {
    
    static TypeSpec typeSpec = getClassTypeSpec();
    
    return typeSpec;
}


/** Get affected nodes: insert this node and only stop recursion here if instantiated, otherwise (if integrated over) we pass on the recursion to our children */
void StochasticNode::getAffected( std::set<StochasticNode* >& affected ) {
    
    /* If we have already touched this node, we are done; otherwise, get the affected children */
    //    if ( !touched ) {
    //        touched = true;
    affected.insert( this );
    
    // if this node is integrated out, then we need to add the children too
    if (type != INSTANTIATED) {
        for ( std::set<VariableNode*>::iterator i = children.begin(); i != children.end(); i++ ) {
            (*i)->getAffected( affected );
        }
    }
    //    }
}


const Distribution& StochasticNode::getDistribution(void) const {
    return *distribution;
}


Distribution& StochasticNode::getDistribution(void) {
    return *distribution;
}


/** Get the ln probability ratio of this node */
double StochasticNode::getLnProbabilityRatio( void ) {
    
    if ( !isTouched() ) {

        return 0.0;
    }
    else {
//        assert( !areDistributionParamsTouched() );
        
        return calculateLnProbability() - storedLnProb;
    }
}


/** Get stored value */
const RbLanguageObject& StochasticNode::getStoredValue( void ) const {

    if ( !touched )
        return *value;

    return *storedValue;
}


/** Get const value; we always know our value. */
const RbLanguageObject& StochasticNode::getValue( void ) const {

    return *value;
}

/** Get non-const value; we always know our value. */
RbLanguageObject& StochasticNode::getValue( void ) {
    
    return *value;
}


///** Get value pointer; we always know our value. */
//const RbLanguageObject* StochasticNode::getValuePtr( void ) const {
//    
//    return value;
//}


/**
 * Is this node eliminated.
 * We can check that here by looking if the node is instantiated.
 */
bool StochasticNode::isEliminated( void ) const {
    return type != INSTANTIATED;
}


/**
 * Keep the current value of the node. 
 * At this point, we also need to make sure we update the stored ln probability.
 */
void StochasticNode::keepMe() {

    if ( touched ) {
        
        // delete the stored value
        if (storedValue != NULL) {
            delete storedValue;
        }
        storedValue = NULL;
        
        storedLnProb = 1.0E6;       // An almost impossible value for the density
        if (needsRecalculation) {
            lnProb = calculateLnProbability();
        }
    }

    touched = false;

}


/**
 * 
 */
void StochasticNode::likelihoodsNeedUpdates() {
    
}


/** Print struct for user */
void StochasticNode::printStruct( std::ostream& o ) const {

    o << "_Class        = " << getClassTypeSpec() << std::endl;
    o << "_Adress       = " << this << std::endl;
    o << "_valueType    = " << value->getType() << std::endl;
    o << "_distribution = ";
    distribution->printValue(o);
    o << std::endl;
    o << "_touched      = " << ( touched ? RbBoolean( true ) : RbBoolean( false ) ) << std::endl;
    o << "_clamped      = " << ( clamped ? RbBoolean( true ) : RbBoolean( false ) ) << std::endl;
    o << "_type         = ";
    if ( type == INSTANTIATED ) {
        o << "instantiated" << std::endl;
        o << "_value        = ";
        value->printValue(o);
        o << std::endl;
    }
    else if ( type == ELIMINATED ) {
        o << "eliminated" << std::endl;
    }
    else {
        o << "summed over" << std::endl;
    }
    
    // print the factor root if available
    if ( factorRoot != NULL ) {
        o << "_factor root  = " << factorRoot->getName() << std::endl;
        // only nodes which are not part of a eliminated subgraph have a probability
        o << "_lnProb       = " << lnProb << std::endl;
    }
    // if we are the factor root than we can print the likelihood of the eliminated subgraph.
    if (factorRoot == this) {
        o << "_lnLikelihood = " << lnProb << std::endl;
    }
    if ( touched && storedValue != NULL) {
        o << "_storedValue  = "; 
        storedValue->printValue(o);
        o << std::endl;
    }
    if ( touched )
        o << "_storedLnProb = " << storedLnProb << std::endl;    

    o << "_parents      = ";
    printParents(o);
    o << std::endl;

    o << "_children     = ";
    printChildren(o);
    o << std::endl;

    o << std::endl;
}


/** Print value for user */
void StochasticNode::printValue( std::ostream& o ) const {

    if ( touched )
        RBOUT( "Warning: Variable in touched state" );

    if (value == NULL) {
        o << "NULL";
    }
    else {
        value->printValue(o);
    }
}


/** Restore the old value of the node and tell affected */
void StochasticNode::restoreMe() {

    if ( touched ) {
        
        if (!clamped) {
            if (storedValue != NULL) {
                if (value != NULL) {
                    delete value;
                }
                value           = storedValue;
            
                // delete the stored value
                storedValue = NULL;
            }
        }
        
        lnProb          = storedLnProb;
        storedLnProb    = 1.0E6;    // An almost impossible value for the density
        
        needsRecalculation = false;
    }

    touched = false;
}


/*
 * Set whether the node needs to be instantiated or should be summed over.
 * During the MCMC run, an instantiated node has one specific value and hence needs moves attached to it.
 * If the node is summed over, no value will be associated with the node.
 * The node is in a sort of fuzzy state.
 */
void StochasticNode::setInstantiated(bool inst) {
    
    if (type != INSTANTIATED && inst) {
        // clear the probability and likelihood values
        probabilities.clear();
        likelihoods.clear();
        
//        value = distribution->rv().clone();
        
        type = INSTANTIATED;
        
        // if we still have parent which are eliminated we keep the factor root
        if ( factorRoot == this ) {
            factorRoot = NULL;
        }
        
        // TODO: I need to tell my children that I'm not eliminated anymore
        
        
        // flag for recalculation
        needsRecalculation = true;
        
    }
    else if ( type == INSTANTIATED && !inst) {
        // get the number of states
        DistributionDiscrete* discreteDist = static_cast<DistributionDiscrete*>( distribution );
        size_t nStates = discreteDist->getNumberOfStates();
        
        // resize the probability vector
        probabilities.resize( nStates );
        for (size_t i = 0; i < nStates; i++) {
            likelihoods.push_back( std::vector<double>(nStates) );
        }
        
//        delete value;
//        value = NULL;
        
        factorRoot = this;
        // how many parents do I have which are eliminated?
        size_t numEliminatedParents = 0;
        for (std::set<DAGNode*>::iterator i = parents.begin(); i != parents.end(); i++) {
            if ( (*i)->isEliminated() ) {
                factorRoot = static_cast<VariableNode*>( *i )->getFactorRoot();
                numEliminatedParents++;
            }
        }
        
        type = ELIMINATED;
        
        // flag for recalculation
        needsRecalculation = true;
    }
}


/**
 * Set value: same as clamp, but do not clamp. This function will
 * also be used by moves to propose a new value.
 */
void StochasticNode::setValue( RbLanguageObject* val ) {

    if (val == NULL) {
        std::cerr << "Ooops ..." << std::endl;
    }
    
    if ( clamped )
        throw RbException( "Cannot change value of clamped node" );

    // only if the node is instantiated (i.e. not summed out) setting the value should have an effect.
    // summed out nodes might set the value for internal computations.
    if ( type == INSTANTIATED ) {
        // touch the node (which will store the lnProb)
        touch();
    }
    
    // delete the stored value
    if (storedValue == NULL) {
        storedValue = value;
    }
    else {
        delete value;
    }
    

    // set the value
    value = val;
    if (value == NULL) {
        std::cerr << "Ooops, setting value to NULL!\n";
    }
}


/**
 * Swap parent node. Note that this function will be called by the tree plate
 * when the topology is changed. It is also used by the parser.
 * This function should be called from the mutateTo function, otherwise it
 * is dangerous because the distribution parameters will not be accommodated.
 */
void StochasticNode::swapParentNode(DAGNode* oldNode, DAGNode* newNode ) {

    if ( parents.find( oldNode ) == parents.end() )
        throw RbException( "Node is not a parent" );

    oldNode->removeChildNode( this );
    newNode->addChildNode   ( this );
    removeParentNode( oldNode );
    addParentNode( newNode );

    if ( clamped == false ) {

        touchMe();
        
        // We keep the current value; delegate moves can later change the value
        // if they are interested.
    }

    //for ( std::set<VariableNode*>::iterator i = children.begin(); i != children.end(); i++ )
    //    (*i)->touch();
}


/** touch this node for recalculation */
void StochasticNode::touchMe( void ) {
    
    if (!touched) {
        // Store the current lnProb 
        touched      = true;
    
        storedLnProb = lnProb;
        
        if ( type != INSTANTIATED ) {
            storedProbabilities = probabilities;
            storedLikelihoods   = likelihoods;
        }
    }
    
    needsRecalculation = true;
}

/** Unclamp the value; use the clamped value as initial value */
void StochasticNode::unclamp( void ) {

    clamped = false;
}

