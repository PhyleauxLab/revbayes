#include "ArgumentRule.h"
#include "ConstructorFunction.h"
#include "RevObject.h"
#include "TypeSpec.h"

#include <sstream>

using namespace RevLanguage;

/** Constructor */
ConstructorFunction::ConstructorFunction( RevObject *obj ) : Procedure(),
    templateObject(obj)
{
    
    // Hack: we know that we will not own the argRules.
    argRules = &templateObject->getParameterRules();
}


/** Constructor */
ConstructorFunction::ConstructorFunction(const ConstructorFunction& obj) : Procedure(obj)
{
    
    templateObject = obj.templateObject->clone();
    
    // Hack: we know that we will not own the argRules.
    argRules = &templateObject->getParameterRules();
}


ConstructorFunction& ConstructorFunction::operator=(const ConstructorFunction &c) {
    
    if (this != &c) {
        Function::operator=(c);
        
        // delete the old object
        delete templateObject;
        
        // clone the new object
        templateObject = c.templateObject->clone();
        
        // Hack: we know that we will not own the argRules.
        argRules = &templateObject->getParameterRules();
    }
    
    return *this;
}


ConstructorFunction::~ConstructorFunction( void )
{
    
    delete templateObject;
}


/** Clone the object */
ConstructorFunction* ConstructorFunction::clone(void) const
{
    
    return new ConstructorFunction(*this);
}


/**
 * Execute function. We make a clone of our template object and set its
 * member variables. If a member variable is 'const', this is already
 * taken care of when the arguments are passed to the constructor
 * function, so we need not differentiate between 'const' and dynamic
 * member variables here. Member variables that are 'protected' cannot
 * be changed but they can be set during construction, so we need not
 * worry about the 'protected' modifier of a member variable rule.
 *
 * @todo This is the old code, which needs to be changed when the member
 *       variable code is revised.
 */
RevPtr<RevVariable> ConstructorFunction::execute( void )
{
    
    RevObject* copyObject = templateObject->clone();
    
    for ( size_t i = 0; i < args.size(); i++ )
    {
        
        if ( args[i].isConstant() )
        {
            copyObject->setConstParameter( args[i].getLabel(), RevPtr<const RevVariable>( (RevVariable*) args[i].getVariable() ) );
        }
        else
        {
            copyObject->setParameter( args[i].getLabel(), args[i].getReferenceVariable() );
        }
    }
    
    // now call the constructor for the internal object
    copyObject->constructInternalObject();
    
    return new RevVariable( copyObject );
}


/** Get argument rules */
const ArgumentRules& ConstructorFunction::getArgumentRules(void) const
{
    
    return *argRules;
}


/** Get Rev type of object */
const std::string& ConstructorFunction::getClassType(void)
{
    
    static std::string revType = "ConstructorFunction";
    
	return revType; 
}

/** Get class type spec describing type of object */
const TypeSpec& ConstructorFunction::getClassTypeSpec(void)
{
    
    static TypeSpec revTypeSpec = TypeSpec( getClassType(), new TypeSpec( Function::getClassTypeSpec() ) );
    
	return revTypeSpec; 
}


/**
 * Get the primary Rev name for this function.
 */
const std::string& ConstructorFunction::getFunctionName( void ) const
{
    
    return ( templateObject != NULL ? templateObject->getConstructorFunctionName() : "" );
}


/** Get type spec */
const TypeSpec& ConstructorFunction::getTypeSpec( void ) const
{
    
    static TypeSpec typeSpec = getClassTypeSpec();
    
    return typeSpec;
}


/** Get return type */
const TypeSpec& ConstructorFunction::getReturnType(void) const
{
    
    return templateObject->getTypeSpec();
}



