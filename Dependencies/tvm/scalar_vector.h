
#ifndef TVM_SCALAR_VECTOR_
#define TVM_SCALAR_VECTOR_  

#include <new>
#include <utility>
#include <cstdint>

#include "concepts.h"
#include "hexprint.h"

// Written by Hans de Nivelle and Dina Muktubayeva 
// October-November 2022.

namespace tvm
{
   // This class is used only for determinizing sizes and offsets.
   // No instances are created. 

   template< typename S, typename V > 
   struct sv_size
   {
      size_t nrrefs;   
      size_t ss;    
      size_t cc;  
      S scal;  
      V v0; 

      sv_size( ) = delete;
      ~sv_size( ) = delete; 
         // To be sure that nobody will accidentily get the idea of
         // creating one. 
   };


   // We cannot have traditional constructors, because we need
   // to do the allocation by yourselves.
   // We have a destructor, but this destructor is incomplete, because 
   // it doesn't destroy the vector part. 

   template< typename S, typename V >
   struct scalar_vector
   {
      size_t nrrefs;   
      size_t ss;  
      size_t cc;  
      S scal;  

      using SV = sv_size< S, V > ;
      static constexpr size_t size0 = offsetof( SV, v0 );

      scalar_vector( const S& scal, size_t ss, size_t cc )
        : nrrefs(0), ss(ss), cc(cc), scal( scal ) 
      { }

      scalar_vector( S&& scal, size_t ss, size_t cc )
        : nrrefs(0), ss(ss), cc(cc), scal( std::move( scal ))
      { }

      V* begin( )
      {
         return & (( sv_size<S,V> * ) this ) -> v0;
      }

      const V* begin( ) const
      { 
         return & (( const sv_size<S,V> * ) this ) -> v0; 
      }

      V* end( )
      {
         return & (( sv_size<S,V> * ) this ) -> v0 + ss;
      }

      const V* end( ) const
      {
         return & (( const sv_size<S,V> * ) this ) -> v0 + ss;
      }

      size_t size( ) const { return ss; }
      size_t capacity( ) const { return cc; } 

      ~scalar_vector( ) = default;
      scalar_vector( const scalar_vector& ) = delete;
      scalar_vector& operator = ( const scalar_vector& ) = delete; 

   };

   // Allocate, but don't initialize:

   template< typename S, typename V > 
   scalar_vector<S,V> * alloc_scalar_vector( size_t cap )
   {
      size_t a = scalar_vector<S,V> :: size0 + cap * sizeof(V); 
      return ( scalar_vector<S,V> * ) operator new(a);  
   }

   // These functions should be viewed as constructors:

   template< typename S, typename V > 
   scalar_vector<S,V> * 
   constr_scalar_vector( const S& scal, size_t cap )
   {
      auto* alloc = alloc_scalar_vector<S,V> ( cap );
      new (alloc) scalar_vector<S,V> ( scal, 0, cap );
      return alloc; 
   }

   template< typename S, typename V, const_iterator<V> I > 
   scalar_vector<S,V> * 
   constr_scalar_vector( const S& scal, I begin, I end )
   {
      size_t cap = end - begin; 
      auto* alloc = alloc_scalar_vector<S,V> ( cap );

      new (alloc) scalar_vector<S,V> ( scal, cap, cap );
     
      V* to = alloc -> begin( );
      for( I from = begin; from != end; ++ from ) 
         new ( to ++ ) V( *from );

      return alloc;
   }

   template< typename S, typename V >
   scalar_vector<S,V> *
   constr_scalar_vector( S&& scal, size_t cap )
   {
      auto* alloc = alloc_scalar_vector<S,V> ( cap );
      new (alloc) scalar_vector<S,V> ( std::move( scal ), 0, cap );
      return alloc;
   }

   template< typename S, typename V, move_iterator<V> I >
   scalar_vector<S,V> *
   constr_scalar_vector( S&& scal, I begin, I end )
   {
      size_t cap = end - begin;
      auto* alloc = alloc_scalar_vector<S,V> ( cap );

      new (alloc) scalar_vector<S,V> ( std::move( scal ), cap, cap );

      V* to = alloc -> begin( );
      for( I from = begin; from != end; ++ from )
         new ( to ++ ) V( *from );

      return alloc;
   }


   template< typename S, typename V >
   inline bool iswritable( const scalar_vector<S,V> * sv ) 
   {
      return ( sv -> nrrefs ) == 1;
   }

   template< typename S, typename V > 
   scalar_vector<S,V> * replacebywritable( scalar_vector<S,V> * sv ) 
   {
      if( -- (  sv -> nrrefs ))
      {
         auto p = constr_scalar_vector<S,V> ( sv -> scal, 
                                              sv -> begin( ), sv -> end( ));
         return p;
      }
      else
         return sv;
   }


   template< typename S, typename V >
   void printstate( const scalar_vector<S,V> * sv, std::ostream& out ) 
   {
      auto s = (uintptr_t) sv; 
      hexprint( out, s, 2 ); 
      out << ", #" << sv -> nrrefs << ", ";
      out << ( sv -> ss ) << "/" << ( sv -> cc ); 
   }

   template< typename S, typename V >
   inline scalar_vector<S,V> * 
   takeshare( scalar_vector<S,V> * sv ) { ++ sv -> nrrefs; return sv; }     

   template< std::destructible S, std::destructible V >
   void dropshare( scalar_vector<S,V> * sv ) 
   {
      if( -- sv -> nrrefs == 0 )
      {
         for( auto d = sv -> begin( ); d != sv -> end( ); ++ d ) 
            d -> ~V( );

         sv -> ~scalar_vector<S,V> ( ); 

         void* del = (void*) sv; 
         operator delete( del ); 
      }
   }


   template< typename S, typename V, std::convertible_to<V> V1 > 
   scalar_vector<S,V> * push_back( scalar_vector<S,V> * sv, V1&& v )
   {

      if( sv -> nrrefs == 1 && sv -> ss < sv -> cc )
      {
         new ( sv->begin( ) + sv->ss ++ ) V( std::forward<V1> (v) );
         return sv;
      }

      // Otherwise, life is a bit harder, because we have to reallocate.
      // It is not certain that ( sv -> ss ) + 1 > ( sv -> cc ) because we may
      // be reallocating due to sharing. 

      // std::cout << "old: " << sv->ss << " " << sv->cc << "\n";

      size_t cc_new = sv -> cc;
      size_t ss_new = sv -> ss + 1;

      if( cc_new < ss_new ) 
         cc_new = ss_new << 1;

      // std::cout << "new: " << ss_new << " " << cc_new << "\n";

      auto* alloc = alloc_scalar_vector<S,V> ( cc_new );

      if( sv -> nrrefs == 1 )
      {
         // If sv is not sharing, we move from it: 

         new (alloc) scalar_vector<S,V> ( std::move( sv -> scal ), ss_new, cc_new );

         auto to = alloc -> begin( );
         for( auto from = sv -> begin( ); from != sv -> end( ); ++ from )
            new ( to ++ ) V( std::move( *from ));

         new( to ) V( std::forward<V1> (v) );
      }
      else
      {
         // If sv is sharing, we copy sv: 

         new (alloc) scalar_vector<S,V>( sv -> scal, ss_new, cc_new );

         auto to = alloc -> begin( );
         for( auto from = sv -> begin( ); from != sv -> end( ); ++ from ) 
            new ( to ++ ) V( *from ); 

         new( to ) V( std::forward<V1> (v) );
      }

      dropshare( sv ); 
      return takeshare( alloc );  
   }
  
    
   template< typename S, typename V > 
   scalar_vector<S,V> * pop_back( scalar_vector<S,V> * sv )
   {
      sv = replacebywritable( sv );
      takeshare( sv );

      -- sv -> ss; 
      ( sv->begin( ) + sv->ss ) -> ~V( );
      return sv;  
   }

   template< typename S, typename V >
   scalar_vector<S,V> * clear( scalar_vector<S,V> * sv )
   {
      if( sv -> nrrefs == 1 )
      {
         for( auto d = sv -> begin( ); d != sv -> end( ); ++ d )
            d -> ~V( );

         sv -> ss = 0;  
         return sv; 
      }
      else
      {
         -- ( sv -> nrrefs );  
                       // It cannot become zero, because we know it was > 1.  

         auto* alloc = alloc_scalar_vector<S,V> ( sv -> cc ); 
         new (alloc) scalar_vector<S,V>( sv -> scal, 0, sv -> cc ); 
         return takeshare( alloc ); 
      }
   }

}

#endif

