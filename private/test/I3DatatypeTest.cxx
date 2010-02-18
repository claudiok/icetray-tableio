/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
 */

#include <I3Test.h>
#include "tableio/internals/I3Datatype.h"

TEST_GROUP(I3DatatypeTests)

TEST(signedness) {
    I3Datatype stype = I3DatatypeFromNativeType(int);
    I3Datatype utype = I3DatatypeFromNativeType(unsigned int);
    I3Datatype doubletype = I3DatatypeFromNativeType(double);
    
    ENSURE_EQUAL( stype.is_signed, true, "Signed types are detected");
    ENSURE_EQUAL( utype.is_signed, false, "Unsigned types are detected");
    
    ENSURE_EQUAL( doubletype.size, sizeof(double), "Sizes are correct" );
    
    ENSURE_EQUAL( (int)doubletype.kind, (int)I3Datatype::Float, "Floats are recognized");
    ENSURE( (int)doubletype.kind != (int)I3Datatype::Int, "Floats are recognized");
    ENSURE( (int)doubletype.kind != (int)I3Datatype::Bool, "Floats are recognized");
    ENSURE( (int)doubletype.kind != (int)I3Datatype::Enum, "Floats are recognized");
    
    ENSURE_EQUAL( stype.size, sizeof(int), "Sizes are correct" );
    
    ENSURE_EQUAL( (int)stype.kind, (int)I3Datatype::Int, "Ints are recognized");
    ENSURE( (int)stype.kind != (int)I3Datatype::Float,   "Ints are recognized");
    ENSURE( (int)stype.kind != (int)I3Datatype::Bool,    "Ints are recognized");
    ENSURE( (int)stype.kind != (int)I3Datatype::Enum,    "Ints are recognized");
    
    I3Datatype btype = I3DatatypeFromNativeType(bool);
    ENSURE_EQUAL( (int)btype.kind, (int)I3Datatype::Bool, "Bools are recognized");
    ENSURE_EQUAL( btype.size, sizeof(bool), "Sizes are correct" );
    
}