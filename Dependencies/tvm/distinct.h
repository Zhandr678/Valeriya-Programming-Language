
// Written by Hans de Nivelle, December 2022.
// This was called very_equal before.
// Now we have a predicate distinct, that can either be
// !very_equal( , ) , !very_equal_to( ), or just !=.
// 
// We had to choose how to deal with distinct:
// Make it a separate object (like std::less) for std::map,
// or use concepts. We chose for concepts, because
// the definition of very_equal is an essential part of the type.
// Otherwise the different distinct-definition would have to be 
// combined into the trees with all the types. 


#ifndef TVM_DISTINCT_
#define TVM_DISTINCT_  

#include "concepts.h"

namespace tvm
{
   template< has_very_equal_to T >
   bool distinct( const T& t1, const T& t2 )
      { return !t1. very_equal_to( t2 ); }

   template< has_very_equal T > 
   requires ( ! has_very_equal_to<T> ) 
   bool distinct( const T& t1, const T& t2 ) 
      { return !very_equal( t1, t2 ); } 

   template< std::equality_comparable T >
      requires ( ! has_very_equal<T> && ! has_very_equal_to<T> )
   bool distinct( const T& t1, const T& t2 ) 
      { return t1 != t2; } 
}

#endif


