/**
 * @file
 * This file contains the declaration of VectorBoolean, a complex type
 * used to hold RbBoolean vectors.
 *
 * @brief Declaration of VectorBoolean
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes core development team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-12-04, version 1.0
 * @extends Vector
 *
 * $Id$
 */

#ifndef VectorBoolean_H
#define VectorBoolean_H

#include "AbstractVector.h"

#include <iostream>
#include <vector>


const std::string VectorBoolean_name = "RbBoolean vector";

class VectorBoolean : public AbstractVector {

    public:
    // Constructors and destructor
    VectorBoolean(void);                                            //!< Default constructor (empty vector)
    VectorBoolean(bool x);                                          //!< Construct vector with one bool x
    VectorBoolean(size_t n, bool x);                                //!< Construct vector with n bools x
    VectorBoolean(const std::vector<bool>& x);                      //!< Constructor from bool vector
    VectorBoolean(const std::vector<int>& x);                       //!< Constructor from int vector

    // Overloaded operators
    std::vector<bool>::reference        operator[](size_t i);                                           //!< Index op allowing change
    std::vector<bool>::const_reference  operator[](size_t i) const;                                     //!< Const index op
    bool                                operator==(const VectorBoolean& x) const;                       //!< Equals comparison
    bool                                operator!=(const VectorBoolean& x) const;                       //!< Not equals comparison
        
    // Basic utility functions
    VectorBoolean*                      clone(void) const;                                              //!< Clone object
    const VectorString&                 getClass(void) const;                                           //!< Get class
    const TypeSpec&                     getTypeSpec(void) const;                                        //!< Get language type of the object
    std::string                         richInfo(void) const;                                           //!< Complete info about object

    // Vector functions, including STL-like functions
    void                                clear(void);                                                    //!< Clear
    RbPtr<const RbObject>               getElement(size_t index) const;                                 //!< Get element
    RbPtr<RbObject>                     getElement(size_t index);                                       //!< Get element (non-const to return non-const element)
    std::vector<bool>                   getValue(void) const;                                           //!< Get value
    void                                pop_back(void);                                                 //!< Drop element at back
    void                                pop_front(void);                                                //!< Drop element from front
    void                                push_back(RbPtr<RbObject> x);                                   //!< Append element to end
    void                                push_back(bool x);                                              //!< Append element to end
    void                                push_front(RbPtr<RbObject> x);                                  //!< Add element in front
    void                                push_front(bool x);                                             //!< Add element in front
    void                                resize(size_t n);                                               //!< Resize to new AbstractVector of length n
    void                                setElement(const size_t index, RbPtr<RbLanguageObject> elem);         //!< Set element with type conversion
    void                                setValue(const VectorBoolean& x);                               //!< Set the value
    void                                setValue(const std::vector<bool>& x);                           //!< Set the value
    void                                sort(void);                                                     //!< sort the AbstractVector
    size_t                              size(void) const;                                               //!< get the number of elements in the AbstractVector
    void                                unique(void);                                                   //!< removes consecutive duplicates

    private:
    std::vector<bool>               elements;
    static const TypeSpec           typeSpec;
    
};

#endif

