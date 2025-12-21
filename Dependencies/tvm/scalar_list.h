
#ifndef TVM_SCALAR_LIST_
#define TVM_SCALAR_LIST_  

#include <new>
#include <utility>
#include <cstdint>
#include <limits>

#include "concepts.h"
#include "hexprint.h"

// Written by Hans de Nivelle, September 2024. 

namespace tvm
{
   template< typename X > struct cell
   {
      X x;
      size_t prev;
      size_t next;

      cell( ) = delete;

      cell( const X& x ) 
         : x(x)  
      { }

      cell( X&& x )
         : x( std::move(x)) 
      { }

      void print( std::ostream& out ) const 
      {
         if constexpr( printable<X> ) 
            out << x; 
         else
            out << "(not printable)"; 
         out << "   [ " << prev << ", " << next << " ]";
      }

      cell( const cell<X> & c ) = default;
      cell( cell<X> && c ) = default;

      cell& operator = ( const cell<X> & ) = delete;

      ~cell( ) = default;
   };


   template< typename L > struct index
   {
      size_t i;

      // Default is null:

      index( ) noexcept
         : i( std::numeric_limits< size_t > :: max( ))
      { } 

      explicit index( size_t i ) noexcept
         : i(i) 
      { } 

      // Since we are a list index, we have no < and > .

      bool operator == ( index<L> ind ) const
      {
         return i == ind. i;
      }

      bool operator != ( index<L> ind ) const
      {
         return i != ind. i;
      }

   };

 
   // This class is used only for determinizing sizes and offsets.
   // No instances should be created. 

   template< typename S, typename L > 
   struct sl_size
   {
      size_t nrrefs;
      size_t ff;     
      size_t cc;
      index<L> uu;   
      size_t ss;  
      cell<S> scal;
      cell<L> r0;

      sl_size( ) = delete;
      ~sl_size( ) = delete; 
         // To be sure that nobody accidentily will get the idea of
         // creating one. We don't want to see these things in real.
   };

   // We cannot have traditional constructors, because we need
   // to do the allocation by yourselves.
   // We have a destructor, but this destructor is incomplete, because 
   // it doesn't destroy the repeated part. 

   template< typename S, typename L >
   struct scalar_list
   {
      size_t nrrefs;   
      size_t ff;     // Between ff and cc, the cells are unused.  
      size_t cc;     // cc is reserved capacity.
      index<L> uu;   // list of unused cells. These are unused cells below ff. 
                     // The prev fields are not used. 
      size_t ss;     // Current size of the list. 
      cell<S> scal;  
          // By putting a cell<S> here, instead of just S, we sneakily 
          // introduced the front and back pointers for the list of 
          // repeated elements. We use next for the front, and
          // prev for the back pointer.

      static constexpr size_t null = std::numeric_limits<size_t> :: max( );
         // We use maximal size_t for representing null-index.
         // 0 cannot be used because it could be an existing index.

      using SL = sl_size<S,L> ;
      static constexpr size_t size0 = offsetof( SL, r0 );

      template< std::convertible_to<S> S1 >
      scalar_list( S1&& scal, size_t front, size_t back,
                   size_t ff, size_t cc, index<L> uu, size_t ss ) 
        : nrrefs(0), ff( ff ), cc( cc ), uu( uu ), ss( ss ),
          scal( std::forward<S1> ( scal )) 
      { 
         this -> scal. prev = back;
         this -> scal. next = front;
      }

      cell<L> * 
      cells_begin( ) { return & (( sl_size<S,L> * ) this ) -> r0; }
 
      const cell<L> *
      cells_begin( ) const { return & (( sl_size<S,L> * ) this ) -> r0; }

      cell<L> *
      cells_end( ) { return & (( sl_size<S,L> * ) this ) -> r0 + ss; }
 
      const cell<L> *
      cells_end( ) const { return & (( sl_size<S,L> * ) this ) -> r0 + ss; }
 
      index<L> begin( ) const
      {
         return index<L> ( scal. next ); 
      }

      index<L> end( ) const
      {
         return index<L> ( scalar_list<S,L> :: null );
      }

      index<L> prev( index<L> ind ) const
      {
         if( ind.i == scalar_list<S,L> :: null )
            return index<L> ( scal. prev );
         else
            return index<L> ( cells_begin( )[ ind. i ]. prev );
      }

      index<L> next( index<L> ind ) const
      {
         return index<L> ( cells_begin( )[ ind. i ]. next );
      }

      cell<L> & cell_at( index<L> ind ) 
      {
         return cells_begin( )[ ind.i ];
      }

      const cell<L> & cell_at( index<L> ind ) const
      {
         return cells_begin( )[ ind.i ];
      }

      L& at( index<L> ind ) 
      {
         return cells_begin( )[ ind. i ]. x;
      }

      const L& at( index<L> ind ) const
      {
         return cells_begin( )[ ind. i ]. x;
      }

      size_t size( ) const { return ss; }
      size_t capacity( ) const { return cc; } 

      ~scalar_list( ) = default;
      scalar_list( const scalar_list& ) = delete;
      scalar_list& operator = ( const scalar_list& ) = delete; 
   };

   // Allocate, but don't initialize:

   template< typename S, typename L > 
   scalar_list<S,L> * alloc_scalar_list( size_t cap )
   {
      size_t a = scalar_list<S,L> :: size0 + cap * sizeof( cell<L> ); 
      return ( scalar_list<S,L> * ) operator new(a);  
   }

   // These functions should be viewed as constructors:

   template< typename S, typename L, std::convertible_to<S> S1 > 
   scalar_list<S,L> * 
   constr_scalar_list( S1&& scal, size_t cap )
   {
      auto* alloc = alloc_scalar_list<S,L> ( cap );
      constexpr size_t null = scalar_list<S,L> :: null; 

      new (alloc) scalar_list<S,L> ( std::forward<S1> ( scal ), 
                                     null, null, 0, cap, index<L>( ), 0 );
      return alloc; 
   }

   template< typename S, typename L, 
             std::convertible_to<S> S1, const_iterator<L> I > 
   scalar_list<S,L> * 
   constr_scalar_list( S1&& scal, I begin, I end )
   {
      size_t cap = end - begin; 
      constexpr size_t null = scalar_list<S,L> :: null;

      if( cap == 0 )
         return constr_scalar_list<S,L> ( scal, 0 );

      auto* alloc = alloc_scalar_list<S,L> ( cap );
      new (alloc) scalar_list<S,L> ( std::forward<S1> ( scal ), 
                                     0, cap - 1, cap, cap, index<L>( ), cap );
   
      auto to = alloc -> cells_begin( ); 
      size_t i = 0; 
      I from = begin; 

      while( from != end ) 
      { 
         new ( to ) cell<L> ( *from );

         if( i == 0 )
             to -> prev = null;
         else
             to -> prev = i - 1;

         if( i + 1 == cap )
            to -> next = null;
         else
            to -> next = i + 1;

         ++ to; ++ from; ++ i;
      }
      return alloc;
   }


   // This one should be viewed as copy constructor.
   // Make a copy, possibly with increased capacity. 

   template< typename S, typename L >
   scalar_list<S,L> *
   copy_scalar_list( const scalar_list<S,L> * sl, size_t cap )
   { 
      if( cap == 0 )
         throw std::runtime_error( "case not handled" );

      auto* alloc = alloc_scalar_list<S,L> ( cap );
      new (alloc) scalar_list<S,L> ( sl -> scal.x, 
                                     sl -> scal. next, sl -> scal. prev, 
                                     sl -> ff, cap, sl -> uu, sl -> ss );

      for( auto ind = sl -> begin( ); ind != sl -> end( ); 
           ind = sl -> next( ind ))
      {
         new (&alloc -> cell_at( ind )) cell<L> ( sl -> cell_at( ind ));
      }

      // We also copy the unused list:

      for( auto ind = sl -> uu; ind != sl -> end( );
           ind = sl -> next( ind ))
      {
         alloc -> cell_at( ind ). next = sl -> cell_at( ind ). next;
            // Note that the .x field is uninitialized.
            // We only care about the next field. 
      }

      return alloc;  
   }


   template< typename S, typename L >
   scalar_list<S,L> *
   move_scalar_list( scalar_list<S,L> * sl, size_t cap )
   {
      if( cap == 0 )
         throw std::runtime_error( "case not handled" );

      auto* alloc = alloc_scalar_list<S,L> ( cap );

      new (alloc) scalar_list<S,L> ( std::move( sl -> scal.x ),
                                     sl -> scal. next, sl -> scal. prev,
                                     sl -> ff, cap, sl -> uu, sl -> ss );

      for( auto ind = sl -> begin( ); ind != sl -> end( ); 
           ind = sl -> next( ind )) 
      {
         new (&alloc -> cell_at( ind )) cell<L> ( 
                                          std::move( sl -> cell_at( ind )));
      }

      // We also copy the unused list:

      for( auto ind = sl -> uu; ind != sl -> end( );
           ind = sl -> next( ind ))
      {
         alloc -> cell_at( ind ). next = sl -> cell_at( ind ). next;
            // Note that the .x field is uninitialized.
            // We only copy the next field.
      }

      return alloc;
   }


   template< typename S, typename L >
   inline bool iswritable( const scalar_list<S,L> * sl ) 
   {
      return ( sl -> nrrefs ) == 1;
   }

   // We decrease the reference counter because we always pretend
   // that we are a fresh copy (a replacement): 

   template< typename S, typename L > 
   scalar_list<S,L> * replacebywritable( scalar_list<S,L> * sl ) 
   {
      if( -- ( sl -> nrrefs ))
         return copy_scalar_list( sl, sl -> size( )); 
      else
         return sl;
   }

   // The result is writable and has the required capacity.
   // We replace if either (1) we are shared, or (2) we don't have the
   // required capacity. We always pretend that we are fresh copy,
   // so the reference counter must be increased. 

   template< typename S, typename L >
   scalar_list<S,L> * 
   replacebywritablewithcap( scalar_list<S,L> * sl, size_t cap )
   {
      auto nrrefs = -- ( sl -> nrrefs );

      if( nrrefs || cap > sl -> capacity( ))
      {  
         // If capacity changes, it must be doubled at least: 

         if( cap > sl -> capacity( ))
         {
            if( cap < 2 * sl -> capacity( ))
               cap = 2 * sl -> capacity( );
         }

         if( !nrrefs )
         {
            auto cp = move_scalar_list( sl, cap );

            for( auto i = sl -> begin( ); i != sl -> end( ); i = sl -> next(i))
            {
               sl -> at(i). ~L( );
            }
            sl -> ~scalar_list<S,L> ( );

            void* del = (void*) sl;
            operator delete( del );
            return cp; 
         }
         else 
            return copy_scalar_list( sl, cap );  
      }  
      else
         return sl;
   }
 
   template< typename S, typename L >
   void printstate( const scalar_list<S,L> * sl, std::ostream& out ) 
   {
      auto s = (uintptr_t) sl; 
      hexprint( out, s, 2 ); 
      out << ", #" << sl -> nrrefs << ", ";
      out << ( sl -> ss ) << "/" << ( sl -> cc ); 
   }

   template< typename S, typename L >
   void checkinvariant( const scalar_list<S,L> * sl )
   {
      size_t truesize = 0; 
      for( auto p = sl -> begin( ); p != sl -> end( ); p = sl -> next(p) )
      {
         auto prev = sl -> prev(p); 
      
         if( p == sl -> begin( ) && prev != sl -> end( )) 
            throw std::runtime_error( "previous of front not null" );
         else
         {
            if( sl -> next( prev ) != p )
               throw std::runtime_error( "next(prev(p)) != p" );
         }
         ++ truesize; 
      } 

      // This test is probably useless, but it doesn't hurt. 
      // We go backwards to the list, and the size should be the same:

      size_t truesize2 = 0;
      for( auto p = sl -> end( ); p != sl -> begin( ); )
      {
         p = sl -> prev(p);
         ++ truesize2;
      }

      if( truesize != truesize2 )
         throw std::runtime_error( "forward length differs from backlength" );

      size_t truefree = 0; 
      for( auto f = sl -> uu; f != sl -> end( ); f = sl -> next(f))
      {
         if( f. i >= sl -> ff )
            throw std::runtime_error( "element in free list not a reuse" );

         ++ truefree;
      }

      if( truesize + truefree != sl -> ff )
         throw std::runtime_error( "free index (ff) is not correct" );

      if( sl -> ff > sl -> cc )
         throw std::runtime_error( "free index (ff) > capacity (cc)" ); 
   }


   template< typename S, typename L > 
   void show( const scalar_list<S,L> * sl, std::ostream& out )
   {
      auto s = (uintptr_t) sl;
      hexprint( out, s, 2 );
      out << ", nrrefs=" << sl -> nrrefs << ", scal=";
      sl -> scal. print( out ); out << "\n";
      out << "size = " << sl -> size( ) << "/" << sl -> capacity( ) << "\n";
      for( auto i = sl -> begin( ); i != sl -> end( ); i = sl -> next( i ))
      { 
         out << i.i << " : ";
         sl -> cell_at(i). print( out ); 
         out << "\n"; 
      }
   }


   template< typename S, typename L >
   inline scalar_list<S,L> * 
   takeshare( scalar_list<S,L> * sl ) { ++ sl -> nrrefs; return sl; }     

   template< std::destructible S, std::destructible L >
   void dropshare( scalar_list<S,L> * sl ) 
   {
      if( -- sl -> nrrefs == 0 )
      {
         for( auto i = sl -> begin( ); i != sl -> end( ); i = sl -> next(i))
            sl -> at(i). ~L( );

         sl -> ~scalar_list<S,L> ( ); 

         void* del = (void*) sl; 
         operator delete( del ); 
      }
   }


   // This function assumes that we have the capacity, and
   // that we are writable. We don't check!
   // Note that this is different from scalar_repeated.
   // I think that the approach here is the right one, because 
   // both scalar_repeated and scalar_vector give out non-const references
   // that can be assigned to. This means that responsibility of
   // checking nonsharedness already lies with the caller sometimes. 
   // In order to be consistent, we should always move it to the caller, 
   // also for push_back and pop_back. 

   template< typename S, typename L, std::convertible_to<L> L1 > 
   index<L> insert( scalar_list<S,L> * sl, index<L> ind, L1&& r )
   {
      // We need to find a free cell.
      // If we have place on the free list, we pick from the free
      // list:

      constexpr size_t null = scalar_list<S,L> :: null;

      index<L> fr; 

      // If we have an unused cell in the list, we take from there:

      if( sl -> uu != sl -> end( ))
      {
         fr = sl -> uu;
         sl -> uu = sl -> next( fr );
      } 
      else
      {
         fr = index<L> (( sl -> ff ) ++ );
      }
   
      new ( & sl -> at(fr) ) L( std::forward<L1> (r)); 

      std::cout << "free cell = " << fr.i << " : " << sl -> at( fr ) << "\n";

      // Are we iserting at the front?

      if( ind == sl -> begin( ))
      {
         sl -> scal. next = fr.i;
         sl -> cell_at(fr). prev = null;
      }
      else
      { 
         auto prev = sl -> prev( ind );
         sl -> cell_at( prev ). next = fr.i;
         sl -> cell_at( fr ). prev = prev.i; 
      }

      if( ind == sl -> end( ))
      {
         sl -> cell_at(fr). next = null; 
         sl -> scal. prev = fr.i; 
      }
      else
      {
         sl -> cell_at(fr). next = ind.i;
         sl -> cell_at(ind). prev = fr.i; 
      }

      ++ ( sl -> ss );   // Size got bigger.  

      return fr;
   }
 
   // We return the index to the next element:
 
   template< typename S, typename L >
   index<L> erase( scalar_list<S,L> * sl, index<L> ind )
   {
      sl -> at( ind ). ~L( );
     
      if( ind == sl -> begin( ))
      {
         sl -> scal. next = sl -> next( ind ).i;
      }
      else
      {
         auto prev = sl -> prev( ind );
         sl -> cell_at( prev ).next = sl -> next( ind ).i;
      }

      auto ret = sl -> next( ind );
      if( ret == sl -> end( ))
      {
         sl -> scal. prev = sl -> prev( ind ).i;
      }
      else
      {
         sl -> cell_at( ret ).prev = sl -> prev( ind ).i;
      }
   
      -- ( sl -> ss );     // Size decreased.

      // Put ind at the front of the free-list:

      sl -> cell_at( ind ). next = sl -> uu.i;
      sl -> uu = ind; 
 
      return ret;
   }

   // You have to make writable by yourself.
   // This is a different policy than in scalar_repeated! 

   template< typename S, typename L >
   void clear( scalar_list<S,L> * sl )
   {

#if 0
      if( sl -> nrrefs == 1 )
      {
         for( auto d = sl -> begin( ); d != sl -> end( ); ++ d )
            d -> ~L( );

         sl -> ss = 0;  
         return sl; 
      }
      else
      {
         -- ( sl -> nrrefs );  
                       // It cannot become zero, because we know it was > 1.  

         auto* alloc = allocate<S,L> ( sl -> cc ); 
         new (alloc) scalar_repeated<S,L>( sl -> scal, 0, sl -> cc ); 
         return takeshare( alloc ); 
      }
#endif
   }

}

#endif

