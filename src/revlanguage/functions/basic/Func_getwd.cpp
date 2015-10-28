#include "Argument.h"
#include "ArgumentRule.h"
#include "Func_getwd.h"
#include "RbException.h"
#include "RbSettings.h"
#include "RlString.h"
#include "RlUtils.h"
#include "TypeSpec.h"

#include <fstream>

using namespace RevLanguage;

/** Default constructor */
Func_getwd::Func_getwd( void ) : Procedure()
{
    
}


/** Clone object */
Func_getwd* Func_getwd::clone( void ) const
{
    
    return new Func_getwd( *this );
}


/** Execute function */
RevPtr<RevVariable> Func_getwd::execute( void )
{
    
    RbSettings& s = RbSettings::userSettings();
    const std::string& wd = s.getWorkingDirectory();
    
    RlString* type = new RlString( wd );
    
    return new RevVariable( type );
}


/** Get argument rules */
const ArgumentRules& Func_getwd::getArgumentRules( void ) const
{
    
    static ArgumentRules argumentRules = ArgumentRules();
    
    return argumentRules;
}


/** Get Rev type of object */
const std::string& Func_getwd::getClassType(void)
{
    
    static std::string revType = "Func_getwd";
    
	return revType;
}


/** Get class type spec describing type of object */
const TypeSpec& Func_getwd::getClassTypeSpec(void)
{
    
    static TypeSpec revTypeSpec = TypeSpec( getClassType(), new TypeSpec( Function::getClassTypeSpec() ) );
    
	return revTypeSpec;
}


/**
 * Get the primary Rev name for this function.
 */
const std::string& Func_getwd::getFunctionName( void ) const
{
    // create a static name variable that is the same for all instance of this class
    static std::string f_name = "getwd";
    
    return f_name;
}


/** Get type spec */
const TypeSpec& Func_getwd::getTypeSpec( void ) const
{
    
    static TypeSpec typeSpec = getClassTypeSpec();
    
    return typeSpec;
}


/** Get return type */
const TypeSpec& Func_getwd::getReturnType( void ) const
{
    
    static TypeSpec returnTypeSpec = RlString::getClassTypeSpec();
    
    return returnTypeSpec;
}

