
// Written by Hans de Nivelle, October-December 2022.

#ifndef TVM_CONCEPTS_
#define TVM_CONCEPTS_  1 

#include <concepts>
#include <cstddef>
#include <iostream>

namespace tvm
{

   template< typename I, typename R >
   concept const_iterator =
      requires( I i ) { { *i } -> std::convertible_to< const R& > ; 
                        { ++i }; } &&
      requires( I i1, I i2 ) {{ i1 - i2 } -> std::convertible_to<size_t> ; 
                              { i1 != i2 } -> std::convertible_to<bool> ; }; 

   template< typename I, typename R >
   concept move_iterator =
      requires( I i ) { { *i } -> std::convertible_to< R&& > ;
                        { ++i }; } &&
      requires( I i1, I i2 ) {{ i1 - i2 } -> std::convertible_to<size_t> ;
                              { i1 != i2 } -> std::convertible_to<bool> ; };

   template< typename T >
   concept has_very_equal =
      requires( T t1, T t2 )
         { { very_equal( t1, t2 ) } -> std::convertible_to<bool> ; };

   template< typename T >
   concept has_very_equal_to =
      requires( T t1, T t2 )
         { { t1. very_equal_to( t2 ) } -> std::convertible_to<bool> ; };

   template< typename T >
   concept printable =
      requires( std::ostream out, T t )
         { { out << t } -> std::convertible_to< std::ostream& > ; }; 

}

#endif

