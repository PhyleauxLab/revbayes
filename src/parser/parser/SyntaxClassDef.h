/**
 * @file
 * This file contains the declaration of SyntaxClassDef, which is
 * used to hold the definitions of user-defined types.
 *
 * @brief Declaration of SyntaxClassDef
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 *
 * $Id$
 */

#ifndef SyntaxClassDef_H
#define SyntaxClassDef_H

#include "RbString.h"
#include "SyntaxElement.h"
#include "SyntaxFormal.h"
#include "SyntaxVariable.h"

#include <ostream>
#include <list>
#include <string>


const std::string SyntaxClassDef_name = "Class definition";

class SyntaxClassDef : public SyntaxElement {

    public:
                                    SyntaxClassDef(RbPtr<RbString>          name,
                                                   RbPtr<RbString>          base,
                                                   RbPtr<std::list<RbPtr<SyntaxElement> > > defs);    //!< Standard constructor
                                    SyntaxClassDef(const SyntaxClassDef& x);            //!< Copy constructor
	    virtual                    ~SyntaxClassDef();                                   //!< Destructor

        // Assignment operator
        SyntaxClassDef&             operator=(const SyntaxClassDef& x);                 //!< Assignment operator

        // Basic utility functions
        std::string                 briefInfo() const;                                  //!< Brief info about object
        SyntaxElement*              clone() const;                                      //!< Clone object
        const VectorString&         getClass(void) const;                               //!< Get class vector 
        const TypeSpec&             getTypeSpec(void) const;                            //!< Get language type of the object
        void                        print(std::ostream& o) const;                       //!< Print info about object

        // Regular functions
        RbPtr<Variable>             evaluateContent(RbPtr<Environment> env);            //!< Get semantic value

    protected:
        RbPtr<RbString>                             className;                                          //!< The name of the class
        RbPtr<RbString>                             baseClass;                                          //!< The name of the base class
        RbPtr<std::list<RbPtr<SyntaxElement> > >    definitions;                                        //!< Defined member variables and functions
    
    private:
        static const TypeSpec       typeSpec;
};

#endif

