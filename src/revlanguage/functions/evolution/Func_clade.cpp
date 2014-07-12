//
//  Func_exp.cpp
//  RevBayesCore
//
//  Created by Sebastian Hoehna on 8/7/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//


#include "Func_clade.h"
#include "ModelVector.h"
#include "RlClade.h"
#include "RlString.h"

using namespace RevLanguage;

/** default constructor */
Func_clade::Func_clade( void ) : Function( ) {
    
}


/** Clone object */
Func_clade* Func_clade::clone( void ) const {
    
    return new Func_clade( *this );
}


RevPtr<Variable> Func_clade::execute() {
    
    // now allocate a new Clade
    const std::vector<std::string>& n = static_cast<const ModelVector<RlString> &>( args[0].getVariable()->getRevObject() ).getValue();
    RevBayesCore::Clade *c = new RevBayesCore::Clade(n,0.0);
    
    return new Variable( new Clade(c) );
}


/* Get argument rules */
const ArgumentRules& Func_clade::getArgumentRules( void ) const {
    
    static ArgumentRules argumentRules = ArgumentRules();
    static bool          rulesSet = false;
    
    if ( !rulesSet ) 
    {
        
        argumentRules.push_back( new ArgumentRule( "taxa", true, ModelVector<RlString>::getClassTypeSpec() ) );
        
        rulesSet = true;
    }
    
    return argumentRules;
}


const std::string& Func_clade::getClassType(void) { 
    
    static std::string rbClassName = "Func_clade";
    
	return rbClassName; 
}

/* Get class type spec describing type of object */
const TypeSpec& Func_clade::getClassTypeSpec(void) { 
    
    static TypeSpec rbClass = TypeSpec( getClassType(), new TypeSpec( Function::getClassTypeSpec() ) );
    
	return rbClass; 
}


/* Get return type */
const TypeSpec& Func_clade::getReturnType( void ) const {
    
    static TypeSpec returnTypeSpec = Clade::getClassTypeSpec();
    
    return returnTypeSpec;
}


const TypeSpec& Func_clade::getTypeSpec( void ) const {
    
    static TypeSpec typeSpec = getClassTypeSpec();
    
    return typeSpec;
}
