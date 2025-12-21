
#ifndef MAPH_LEXING_CONCEPTS_
#define MAPH_LEXING_CONCEPTS_  1

#include <concepts>

namespace lexing
{
   // The first argument is the main one:

   template< typename R, typename C >
   concept reader_concept =
      requires( R r, size_t i ) 
      { { r. has(i) } -> std::same_as< bool > ; 
        { r. peek(i) } -> std::convertible_to<C> ; 
      };

}

#endif

