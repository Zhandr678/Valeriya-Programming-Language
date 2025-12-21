
#ifndef TVM_FIELD_
#define TVM_FIELD_    

#include "unit.h"
#include "scalar.h"
#include "scalar_vector.h"

// This should be looked at, maybe we need 8 cases!

namespace tvm
{
   template< typename L, typename S, typename V >
   struct field
   {
      L loc;
      scalar_vector<S,V> * heap;

      ~field( ) = default; 
   };

   template< typename L, typename S >
   struct field<L,S,tvm::unit> 
   {
      L loc;
      scalar<S> * heap;

      ~field( ) = default; 
   };

   template< typename S, typename V >
   struct field<tvm::unit,S,V>
   {
      scalar_vector<S,V> * heap;

      ~field( ) = default; 
   };

   template< typename S >
   struct field<tvm::unit,S,tvm::unit>
   {
      scalar<S> * heap;

      ~field( ) = default;
   };

}

#endif 
 

