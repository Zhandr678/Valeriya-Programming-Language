
// A scalar is similar to a shared_ptr, but its value
// cannot be extended (inherited from).
// This makes it possible to store the reference counter
// and the value in the same memory segment.
// Written by Hans de Nivelle, December 2022.

#ifndef TVM_SCALAR_
#define TVM_SCALAR_    

#include <new>
#include <cstdint>

#include "hexprint.h" 
#include "concepts.h"

namespace tvm
{

   template< typename S > 
   struct scalar 
   {
      size_t nrrefs;
      S scal;  

      scalar( const S& scal )
        : nrrefs(0), scal( scal ) 
      { }

      scalar( S&& scal )
        : nrrefs(0), scal( std::move( scal ))
      { }

      scalar( const scalar& ) = default;
   };


   // As far as I see, we cannot have normal constructors,
   // and also no destructors.  

   template< typename S > 
   scalar<S> * constr_scalar( const S& scal )
   {
      return new scalar<S> ( scal ); 
   }

   template< typename S >
   scalar<S> * constr_scalar( S&& scal )
   {
      return new scalar<S> ( std::move( scal )); 
   }

   template< typename S >
   bool iswritable( const scalar<S> * sr ) 
   {
      return ( sr -> nrrefs ) == 1;
   }

   template< std::copy_constructible S > 
   scalar<S> * replacebywritable( scalar<S> * sr ) 
   {
      if( -- (  sr -> nrrefs ))
         return new scalar<S> ( sr -> scal );
      else
         return sr;
   }

   template< typename S >
   void printstate( const scalar<S> * sr, std::ostream& out ) 
   {
      auto s = (uintptr_t) sr; 
      hexprint( out, s, 2 ); 
      out << ", #" << sr -> nrrefs;
   }

   template< typename S >
   inline scalar<S> * 
   takeshare( scalar<S> * sr ) { ++ sr -> nrrefs; return sr; }     

   template< std::destructible S >
   void dropshare( scalar<S> * sr ) 
   {
      if( -- sr -> nrrefs == 0 )
      {
         delete sr;  
      }
   }

}

#endif

