#include "Move_EmpiricalTree.h"
#include "ArgumentRule.h"
#include "ArgumentRules.h"
#include "EmpiricalProposal.h"
#include "ModelVector.h"
#include "MetropolisHastingsMove.h"
#include "RbException.h"
#include "RealPos.h"
#include "RlTree.h"
#include "TypedDagNode.h"
#include "TypeSpec.h"


using namespace RevLanguage;


Move_EmpiricalTree::Move_EmpiricalTree() : Move()
{
    
}


/**
 * The clone function is a convenience function to create proper copies of inherited objected.
 * E.g. a.clone() will create a clone of the correct type even if 'a' is of derived type 'b'.
 *
 * \return A new copy of the process.
 */
Move_EmpiricalTree* Move_EmpiricalTree::clone(void) const
{
    
    return new Move_EmpiricalTree(*this);
}


void Move_EmpiricalTree::constructInternalObject( void )
{
    // we free the memory first
    delete value;
    
    // now allocate a new empirical tree move
    double w = static_cast<const RealPos &>( weight->getRevObject() ).getValue();
    
    RevBayesCore::TypedDagNode<RevBayesCore::Tree>* tmp = static_cast<const Tree &>( tree->getRevObject() ).getDagNode();
    RevBayesCore::StochasticNode<RevBayesCore::Tree> *t = static_cast<RevBayesCore::StochasticNode<RevBayesCore::Tree> *>( tmp );
    
    bool mh = static_cast<const RlBoolean &>( metropolisHastings->getRevObject() ).getValue();
    
    RevBayesCore::Proposal *p = new RevBayesCore::EmpiricalProposal<RevBayesCore::Tree>(t, mh);
    value = new RevBayesCore::MetropolisHastingsMove(p, w, false);
    
}


/** Get Rev type of object */
const std::string& Move_EmpiricalTree::getClassType(void)
{
    static std::string rev_type = "Move_EmpiricalTree";
    
    return rev_type;
}

/** Get class type spec describing type of object */
const TypeSpec& Move_EmpiricalTree::getClassTypeSpec(void)
{
    
    static TypeSpec rev_type_spec = TypeSpec( getClassType(), new TypeSpec( Move::getClassTypeSpec() ) );
    
    return rev_type_spec;
}


/**
 * Get the Rev name for the constructor function.
 *
 * \return Rev name of constructor function.
 */
std::string Move_EmpiricalTree::getMoveName( void ) const
{
    // create a constructor function name variable that is the same for all instance of this class
    std::string c_name = "EmpiricalTree";
    
    return c_name;
}


/** Return member rules (no members) */
const MemberRules& Move_EmpiricalTree::getParameterRules(void) const
{
    
    static MemberRules move_member_rules;
    static bool rules_set = false;
    
    if ( !rules_set )
    {
        move_member_rules.push_back( new ArgumentRule( "tree", Tree::getClassTypeSpec(), "The stochastic tree variable on which this moves operates.", ArgumentRule::BY_REFERENCE, ArgumentRule::STOCHASTIC ) );
        move_member_rules.push_back( new ArgumentRule( "metropolisHastings"   , RlBoolean::getClassTypeSpec(), "When propose a new tree from the empirical tree distribution, should we accept or reject this move based on the acceptance ratio (as a regular Metropolis-Hastings move so that we are approximating the joint posterior correctly through a valid MCMC) or should we simply always accept this move (so that we are effectively sampling every tree uniformly and combining the the estimates on each tree without weighting them according to the marginal likelihood of each tree)?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( true ) ) );

        /* Inherit weight from Move, put it after variable */
        const MemberRules& inheritedRules = Move::getParameterRules();
        move_member_rules.insert( move_member_rules.end(), inheritedRules.begin(), inheritedRules.end() );
        
        rules_set = true;
    }
    
    return move_member_rules;
}

/** Get type spec */
const TypeSpec& Move_EmpiricalTree::getTypeSpec( void ) const
{
    
    static TypeSpec type_spec = getClassTypeSpec();
    
    return type_spec;
}


/** Get type spec */
void Move_EmpiricalTree::printValue(std::ostream &o) const
{
    
    o << "Move_EmpiricalTree(";
    if (tree != NULL)
    {
        o << tree->getName();
    }
    else
    {
        o << "?";
    }
    o << ")";
}


/** Set a member variable */
void Move_EmpiricalTree::setConstParameter(const std::string& name, const RevPtr<const RevVariable> &var) {
    
    if ( name == "tree" )
    {
        tree = var;
    }
    else if ( name == "metropolisHastings" )
    {
        metropolisHastings = var;
    }
    else
    {
        Move::setConstParameter(name, var);
    }
}

