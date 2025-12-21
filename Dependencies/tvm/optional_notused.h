
// This implementation is incorrect, because it does not align.
// Instead of repairing it, I decided not to use this class.


#ifndef TVM_OPTIONAL_
#define TVM_OPTIONAL_

#include <cstddef>
#include <concepts>

namespace tvm
{

   template< typename X >
   struct optional
   {
      static inline constexpr std::size_t size = sizeof( X );

      std::byte repr[ size ];

      optional( ) = default;
      ~optional( ) = default;
      optional( const optional& ) = delete; 
      void operator = ( const optional& ) = delete;

      void init( X&& x ) { new (repr) X( std::move(x)); } 
      void init( const X& x ) { new (repr) X(x); }
      void assign( X&& x ) { ((X*) repr ) -> operator = ( std::move(x)); } 
      void assign( const X& x ) { ((X*) repr ) -> operator = (x); } 

      const X& value( ) const { return * ( (X*) repr ); }

      void destroy( ) { ((X*) repr ) -> ~X( ); }
   };

}

#endif


