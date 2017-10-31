#include "ArgumentRule.h"
#include "ConstantNode.h"
#include "Func_annotateTree.h"
#include "ModelVector.h"
#include "NexusWriter.h"
#include "Probability.h"
#include "RbException.h"
#include "RevNullObject.h"
#include "RlBranchLengthTree.h"
#include "RlString.h"
#include "RlTimeTree.h"
#include "RlTraceTree.h"
#include "RlUtils.h"
#include "StringUtilities.h"

#include <map>
#include <set>
#include <sstream>


using namespace RevLanguage;


/**
 * The clone function is a convenience function to create proper copies of inherited objected.
 * E.g. a.clone() will create a clone of the correct type even if 'a' is of derived type 'b'.
 *
 * \return A new copy of the process.
 */
Func_annotateTree* Func_annotateTree::clone( void ) const
{
    
    return new Func_annotateTree( *this );
}


/** Execute function */
RevPtr<RevVariable> Func_annotateTree::execute( void )
{
    size_t arg_index = 0;
    
    // get the input tree
    RevBayesCore::Tree* tree = static_cast<const Tree&>( this->args[arg_index++].getVariable()->getRevObject() ).getValue().clone();
    
    // get the  tree trace
    TraceTree& tt = static_cast<TraceTree&>( args[arg_index++].getVariable()->getRevObject() );
    
    // get the filename
    const std::string& filename = static_cast<const RlString&>( args[arg_index++].getVariable()->getRevObject() ).getValue();
    

    RevBayesCore::AnnotationReport report;

    report.ages      = static_cast<const RlBoolean &>( this->args[arg_index++].getVariable()->getRevObject() ).getValue();
    report.cc_ages   = static_cast<const RlBoolean &>( this->args[arg_index++].getVariable()->getRevObject() ).getValue();
    report.ccp       = static_cast<const RlBoolean &>( this->args[arg_index++].getVariable()->getRevObject() ).getValue();
    report.tree_ages = static_cast<const RlBoolean &>( this->args[arg_index++].getVariable()->getRevObject() ).getValue();
    report.hpd       = static_cast<const Probability &>(args[arg_index++].getVariable()->getRevObject()).getValue();
    report.mean      = static_cast<const RlBoolean &>( this->args[arg_index++].getVariable()->getRevObject() ).getValue();
    report.sa        = static_cast<const RlBoolean &>( this->args[arg_index++].getVariable()->getRevObject() ).getValue();
    
    // do not make a new tree summary object
    // this way we don't need to resummarize every time we annotate a tree
    // RevBayesCore::TreeSummary summary = RevBayesCore::TreeSummary( tt.getValue() );
    
    bool verbose = true;
    tt.getValue().annotateTree( *tree, report, verbose );
    
    // return the tree
    if ( filename != "" )
    {

        RevBayesCore::NexusWriter writer(filename);
        writer.openStream(false);
        
        std::vector<RevBayesCore::Taxon> taxa;
        tree->getRoot().getTaxa(taxa);
        RevBayesCore::Clade c( taxa );
        writer.writeNexusBlock(c);
        
        writer.writeNexusBlock(*tree);
        
        writer.closeStream();
        
    }
    
    Tree* t;
    if ( tt.getValue().isClock() )
    {
        t = new TimeTree( tree );
    }
    else
    {
        t = new BranchLengthTree( tree );
    }

    return new RevVariable( t );
}



/** Get argument rules */
const ArgumentRules& Func_annotateTree::getArgumentRules( void ) const
{
    
    static ArgumentRules argumentRules = ArgumentRules();
    static bool rules_set = false;
    
    if (!rules_set)
    {
        argumentRules.push_back( new ArgumentRule( "tree", Tree::getClassTypeSpec()        , "The input tree which will be annotated.", ArgumentRule::BY_VALUE, ArgumentRule::ANY ) );
        argumentRules.push_back( new ArgumentRule( "trace", TraceTree::getClassTypeSpec()   , "The sample trace.", ArgumentRule::BY_REFERENCE, ArgumentRule::ANY ) );
        argumentRules.push_back( new ArgumentRule( "file"     , RlString::getClassTypeSpec()    , "The name of the file where to store the tree.", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlString("")  ) );

        argumentRules.push_back( new ArgumentRule( "ages" , RlBoolean::getClassTypeSpec() , "Annotate node ages?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean(true) ) );
        argumentRules.push_back( new ArgumentRule( "ccAges" , RlBoolean::getClassTypeSpec() , "Annotate conditional clade ages?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean(false) ) );
        argumentRules.push_back( new ArgumentRule( "ccp" , RlBoolean::getClassTypeSpec() , "Annotate conditional clade probabilities?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean(false) ) );
        argumentRules.push_back( new ArgumentRule( "conditionalAges" , RlBoolean::getClassTypeSpec() , "Annotate node ages conditional on the input topology?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean(false) ) );
        argumentRules.push_back( new ArgumentRule( "hpd"   ,    Probability::getClassTypeSpec() , "The probability mass of the highest posterior density age interval.", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new Probability(0.95) ) );
        argumentRules.push_back( new ArgumentRule( "mean" , RlBoolean::getClassTypeSpec() , "Annotate node ages using the mean age instead of the median?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean(true) ) );
        argumentRules.push_back( new ArgumentRule( "sampledAncestors" , RlBoolean::getClassTypeSpec() , "Annotate sampled ancestor probs?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean(false) ) );

        rules_set = true;
    }
    
    return argumentRules;
}


/** Get Rev type of object */
const std::string& Func_annotateTree::getClassType(void)
{
    
    static std::string rev_type = "Func_annotateTree<" + Tree::getClassType() + ">";
    
    return rev_type;
}


/** Get class type spec describing type of object */
const TypeSpec& Func_annotateTree::getClassTypeSpec(void)
{
    
    static TypeSpec rev_type_spec = TypeSpec( getClassType(), new TypeSpec( Function::getClassTypeSpec() ) );
    
    return rev_type_spec;
}


/**
 * Get the primary Rev name for this function.
 */
std::string Func_annotateTree::getFunctionName( void ) const
{
    // create a name variable that is the same for all instance of this class
    std::string f_name = "annotateTree";
    
    return f_name;
}


/** Get type spec */
const TypeSpec& Func_annotateTree::getTypeSpec( void ) const
{
    
    static TypeSpec type_spec = getClassTypeSpec();
    
    return type_spec;
}


/** Get return type */
const TypeSpec& Func_annotateTree::getReturnType( void ) const
{
    
    static TypeSpec return_typeSpec = Tree::getClassTypeSpec();
    return return_typeSpec;
}

