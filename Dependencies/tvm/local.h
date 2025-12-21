
// Simple functions for the handling of local data.
// Written by Hans de Nivelle, December 2022.

#ifndef TVM_LOCAL_
#define TVM_LOCAL_   1

#include <concepts>

namespace tvm
{
 
   template< std::copy_constructible X > 
   void init( X& var, X&& val ) { new (&var) X( std::move(val) ); } 

   template< std::move_constructible X > 
   void init( X& var, const X& val ) { new (&var) X(val); }

   template< typename X >
   void assign( X& var, const X& val ) { var = val; } 

   template< typename X >
   void assign( X& var, X&& val ) { var = std::move( val ); } 

   template< std::destructible X >
   void destroy( X& var ) { (&var ) -> ~X( ); }

}

#endif


